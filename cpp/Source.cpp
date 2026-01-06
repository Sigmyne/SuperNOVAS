/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

/**
 * Returns a pointer to the NOVAS C `novas_object` data structure that stores data internally.
 *
 * @return    a pointer to the underlying NOVAS C `novas_object` data structure.
 */
const struct novas_object *Source::_novas_object() const {
  return &_object;
}

/**
 * Returns the name given to this source at instantiation. It may be lower-case unless the
 * `set_case_sensitive(true)` was called before instantiating the source.
 *
 * @return    the given source name.
 *
 * @sa set_case_sensitive()
 */
std::string Source::name() const {
  return std::string(_object.name);
}

/**
 * Returns the apparent position of a source (if possible), or else an invalid position. After the
 * return, you should probably check for validity:
 *
 * ```c
 *   Apparent app = my_source.apparent(frame);
 *   if(!app.is_valid()) {
 *     // We could not obtain apparent coordinates for some reason.
 *     ...
 *   }
 * ```
 *
 * There are multiple reasons why we might not be able to calculate valid apparent positions, such
 * as:
 *
 *  - the frame itself may be invalid.
 *  - the system parameter may be outside of the enum range
 *  - for Solar system sources:
 *     * SuperNOVAS may not have a planet or ephemeris provider function configured for the given
 *       accuracy.
 *     * the planet or ephemeris provider function does not have data for the source, or planet at
 *       the orbital center (for orbital sources), for the requested time of observation.
 *
 * The apparent position of a source is where it appears to the observer on the celestial sphere.
 * As such it is mainly a direction on sky, which is corrected for light-travel time (i.e. where
 * the source was at the time light originated from the Solar-system body, or the differential
 * light-travel time between the Solar-system barycenter and the observer location for sidereal
 * sources).
 *
 * Unlike geometric positions, the apparent location is also corrected for the observer's motion
 * (aberration), as well as gravitational deflection around the major Solar-system bodies.
 *
 * @param frame     observer frame, which defines the observer location and the time of
 *                  observation, as well as the accuracy requirement.
 * @return the apparent position of the source, or else an invalid position (with NAN values).
 *
 * @sa geometric()
 */
Apparent Source::apparent(const Frame& frame) const {
  sky_pos pos = {};

  if(novas_sky_pos(&_object, frame._novas_frame(), NOVAS_TOD, &pos) != 0) {
    novas_trace_invalid("Source::apparent");
    return Apparent::invalid();
  }

  return Apparent::from_tod_sky_pos(pos, frame);
}

/**
 * Returns the geometric position of a source (if possible), or else an invalid position. After
 * the return, you should probably check for validity:
 *
 * ```c
 *   Geometric geom = my_source.geometric(frame);
 *   if(!geom.is_valid()) {
 *     // We could not obtain geometric positions for some reason.
 *     ...
 *   }
 * ```
 * There are multiple reasons why we might not be able to calculate valid geometric positions,
 * such as:
 *
 *  - the frame itself may be invalid.
 *  - the system parameter may be outside of the enum range
 *  - for Solar system sources:
 *     * SuperNOVAS may not have a planet or ephemeris provider function configured for the given
 *       accuracy.
 *     * the planet or ephemeris provider function does not have data for the source, or planet at
 *       the orbital center (for orbital sources), for the requested time of observation.
 *
 * A geometric position is the 3D location, relative to the observer location, where the light
 * originated from the source before being detected by the observer at the time of observation. As
 * such, geometric positions are necessarily antedated for light travel time (for Solar-system
 * sources) or corrected for the differential light-travel between the Solar-system barycenter
 * and the observer location (for sidereal sources).
 *
 * In other words, geometric positions are not the same as ephemeris positions for the equivalent
 * time for Solar-system bodies. Rather, geometric positions match the ephemeris positions for
 * an earlier time, when the observed light originated from the source.
 *
 *
 *
 * @param frame     observer frame, which defines the observer location and the time of
 *                  observation, as well as the accuracy requirement.
 * @param system    coordinate reference system in which to return positions and velocities.
 * @return the geometric (3D) position and velocity of the source, or else an invalid position
 *         (with NAN values).
 *
 * @sa apparent(), Frame::planet_position(), Frame::planet_velocity()
 */
Geometric Source::geometric(const Frame& frame, enum novas_reference_system system) const {
  double p[3] = {0.0}, v[3] = {0.0};

  if(novas_geom_posvel(&_object, frame._novas_frame(), NOVAS_TOD, p, v) != 0) {
    novas_trace_invalid("Source::geometric");
    return Geometric::invalid();
  }

  return Geometric(
          Position(p[0] * Unit::au, p[1] * Unit::au, p[2] * Unit::au),
          Velocity(v[0] * Unit::au / Unit::day, v[1] * Unit::au / Unit::day, v[2] * Unit::au / Unit::day),
          frame, system
  );
}

static const EOP& extract_eop(const Frame &frame) {
  const GeodeticObserver& eobs = dynamic_cast<const GeodeticObserver&>(frame.observer());
  return eobs.eop();
}

/**
 * Returns the time when the source rises above the specified elevation next for an observer
 * located on or near Earth's surface, or else `std::nullopt` if the observer is not near Earth's
 * surface. The returned value may also be NAN if the source does not cross the specified
 * elevation theshold within a day of the specified time of observation.
 *
 * @param el        [rad] elevation threshold angle
 * @param frame     observing frame (observer location and the lower bound for the returned time).
 * @param ref       atmospheric refraction model to assume
 * @param weather   local weather parameters for the refraction calculation
 * @return          the next time the source rises above the specified elevation after the frame's
 *                  observing time. It may be NAN if the source does not cross (rises above or
 *                  sets below) the elevation threshold within a day of the specified time of
 *                  observation. For observers non near Earth's surface, `std::nullopt` will be
 *                  returned.
 *
 * @sa sets_below(), transits()
 */
std::optional<Time> Source::rises_above(double el, const Frame &frame, RefractionModel ref, const Weather& weather) const {
  static const char *fn = "Source::rises_above";

  if(frame.observer().is_geodetic()) {
    if(ref)
      el = Horizontal(0.0, el * Unit::deg).to_unrefracted(frame, ref, weather).elevation().deg();

    Time t = Time(novas_check_nan(fn, novas_rises_above(el / Unit::deg, &_object, frame._novas_frame(), NULL)), extract_eop(frame));
    return t;
  }

  novas_error(0, ENOSYS, fn, "Cannot calculate rises time for a non-geodetic observer.");
  return std::nullopt;
}

/**
 * Returns the time when the source transits for an observer located on or near Earth's surface,
 * or else `std::nullopt` if the observer is not near Earth's surface.
 *
 * @param frame     observing frame (observer location and the lower bound for the returned time).
 * @return          the next time the source transits after the frame's observing time, or else
 *                  `std::nullopt` if the observer is not near Earth's surface .
 *
 * @sa sets_below(), transits()
 */
std::optional<Time> Source::transits(const Frame &frame) const {
  static const char *fn = "Source::transits";

  if(frame.observer().is_geodetic())
    return Time(
          novas_check_nan("Source::transits", novas_transit_time(&_object, frame._novas_frame())),
          extract_eop(frame));

  novas_error(0, ENOSYS, fn, "Cannot calculate transit time for a non-geodetic observer.");
  return std::nullopt;
}

/**
 * Returns the time when the source sets below the specified elevation next for an observer
 * located on or near Earth's surface, or else `std::nullopt` if the observer is not near Earth's
 * surface. The returned value may also be NAN if the source does not cross the specified
 * elevation theshold within a day of the specified time of observation.
 *
 * @param el        [rad] elevation threshold angle
 * @param frame     observing frame (observer location and the lower bound for the returned time).
 * @param ref       atmospheric refraction model to assume
 * @param weather   local weather parameters for the refraction calculation
 * @return          the next time the source sets the specified elevation after the frame's
 *                  observing time. It may be NAN if the source does not cross (rises above or
 *                  sets below) the elevation threshold within a day of the specified time of
 *                  observation. For observers not near Earth's surface, `std::nullopt` will be
 *                  returned.
 *
 * @sa rises_above(), transits()
 */
std::optional<Time> Source::sets_below(double el, const Frame &frame, RefractionModel ref, const Weather& weather) const {
  static const char *fn = "Source::sets_below";

  if(frame.observer().is_geodetic()) {
    if(ref)
      el = Horizontal(0.0, el * Unit::deg).to_unrefracted(frame, ref, weather).elevation().deg();
    return Time(
          novas_check_nan("Source::sets_below", novas_sets_below(el / Unit::deg, &_object, frame._novas_frame(), NULL)),
          extract_eop(frame));
  }

  novas_error(0, ENOSYS, fn, "Cannot calculate transit time for a non-geodetic observer.");
  return std::nullopt;
}

/**
 * Returns the short-term equatorial trajectory of this source on the observer's sky, which can be used for
 * extrapolating its apparent position in the near-term to avoid the repeated full-fledged position
 * calculation, which may be expensive. The equatorial trajectory may also be used to provide telescope
 * motor control parameters (position, tracking velocity, and acceleration) for equatorial telescope drive
 * systems.
 *
 * In case positions cannot be calculated for this source (e.g. because you do not have an ephemeris provider
 * configured, or there is no ephemeris data available), then `std::nullopt` is returned instead.
 *
 * @param frame           observing frame (observer location and time of observation)
 * @param range_seconds   [s] time range for which to fit a quadratic time evolution to the R.A., Dec,
 *                        distance, and radial velocity coordinates.
 * @return                a new near-term eqautorial trajectory for this source, for the observing
 *                        location, around the time of observation, if possible, or else `std::nullopt`.
 *
 * @sa horizontal_track()
 */
std::optional<EquatorialTrack> Source::equatorial_track(const Frame &frame, double range_seconds) const {
  novas_track track = {};

  if(novas_equ_track(_novas_object(), frame._novas_frame(), range_seconds, &track) != 0) {
    novas_trace_invalid("Source::equatorial_track");
    return std::nullopt;
  }

  return EquatorialTrack::from_novas_track(Equinox::tod(frame.time().jd()), &track, Interval(range_seconds));
}

/**
 * Returns the short-term horizontal trajectory of this source on the observer's sky, which can be used for
 * extrapolating its apparent position in the near-term to avoid the repeated full-fledged position
 * calculation, which may be expensive. The horizontal trajectory may also be used to provide telescope
 * motor control parameters (position, tracking velocity, and acceleration) for horizontal telescope drive
 * systems.
 *
 * If the observer is not located on or near Earth's surface, horizontal coordinates are not defined, and
 * so `std::nullopt` will be retuirned instead. Also, in case positions cannot be calculated for this source
 * (e.g. because you do not have an ephemeris provider configured, or there is no ephemeris data available),
 * then `std::nullopt` will be returned also.
 *
 * @param frame           observing frame (observer location and time of observation)
 * @param ref             atmospheric refraction model to use for refraction correction.
 * @param weather         local weather parameters for the refraction calculation.
 * @return                a new near-term horizontal trajectory for this source, for the observing
 *                        location, around the time of observation, if possible, or else `std::nullopt`.
 *
 * @sa equatorial_track()
 */
std::optional<HorizontalTrack> Source::horizontal_track(const Frame &frame, novas::RefractionModel ref, const Weather& weather) const {
  static const char *fn = "Source::horizontal_track";

  novas_track track = {};

  if(!frame.observer().is_geodetic()) {
    novas_error(0, EINVAL, fn, "input frame is not a geodetic observing frame");
    return std::nullopt;
  }

  novas_frame f = *frame._novas_frame();
  on_surface *s = &f.observer.on_surf;

  s->temperature = weather.temperature().celsius();
  s->pressure = weather.pressure().mbar();
  s->humidity = weather.humidity();

  if(novas_hor_track(_novas_object(), frame._novas_frame(), ref, &track) != 0) {
    novas_trace_invalid(fn);
    return std::nullopt;
  }

  return HorizontalTrack::from_novas_track(&track, Interval(1.0 * Unit::min));
}

/**
 * Returns the angular separation of this source from the Sun, for the given observer location and
 * time of observation.
 *
 * @param frame     observing frame (observer location and time of observation)
 * @return          the Sun's distance from the source.
 *
 * @sa moon_angle(), angle_to()
 */
Angle Source::sun_angle(const Frame& frame) const {
  return Angle(novas_check_nan("Source::sun_angle", novas_sun_angle(&_object, frame._novas_frame()) * Unit::deg));
}

/**
 * Returns the angular separation of this source from the Moon, for the given observer location and
 * time of observation.
 *
 * @param frame     observing frame (observer location and time of observation)
 * @return          the Moon's distance from the source.
 *
 * @sa sun_angle(), angle_to()
 */
Angle Source::moon_angle(const Frame& frame) const {
  return Angle(novas_check_nan("Source::moon_angle", novas_moon_angle(&_object, frame._novas_frame()) * Unit::deg));
}

/**
 * Returns the angular separation of this source from another source, for the given observer
 * location and  time of observation.
 *
 * @param source    the other source.
 * @param frame     observing frame (observer location and time of observation)
 * @return          the distance between this source and the specified other source.
 *
 * @sa sun_angle(), moon_angle()
 */
Angle Source::angle_to(const Source& source, const Frame& frame) const {
  return Angle(novas_check_nan("Source::angle_to", novas_object_sep(&_object, &source._object, frame._novas_frame()) * Unit::deg));
}


std::string Source::to_string() const {
  return "Source type " + std::to_string(_object.type);
}

/**
 * Enables or disabled case-sensitive treatment of source names. It only affect sources that are
 * instantiated after the change has been made.
 *
 * @param value     `true` to enable case sensitive processing of name for newly defined sources
 *                  or else `false` to convert all future source names to lower-case for
 *                  case-insensitive processing.
 */
void Source::set_case_sensitive(bool value) {
  novas_case_sensitive(value);
}





/**
 * Instantiates a new catalog source, from its catalog definition. ICRS coordinates are calculated
 * for all catalog entries, regardless of what catalog system they were defined it. As such, it
 * is important that for catalog entries that are not defined in ICRS or the J2000 catalog system,
 * you set proper motion as appropriate, such that they may be 'moved' into the J2000 epoch for
 * proper ICRS coordinates.
 *
 * @param e     the catalog entry
 */
CatalogSource::CatalogSource(const CatalogEntry& e)
: Source(), _cat(e) {
  static const char *fn = "CatalogSource()";

  if(make_cat_object_sys(e._cat_entry(), e.system().name().c_str(), &_object) != 0)
    novas_trace_invalid(fn);
  else if(!e.is_valid())
    novas_error(0, EINVAL, fn, "input catalog entry is invalid");
  else
    _valid = true;
}

/**
 * Returns the catalog entry stored internally.
 *
 * @return    a reference to the internal catalog entry.
 */
const CatalogEntry& CatalogSource::catalog_entry() const {
  return _cat;
}

/**
 * Returns a string representation of this catalog source.
 *
 * @return    a string representation of this catalog source
 */
std::string CatalogSource::to_string() const {
  const cat_entry *c = _cat._cat_entry();
  return "CatalogSource: " + std::string(c->starname) + " @ " + TimeAngle(c->ra * Unit::hour_angle).to_string() +
          " " + Angle(c->dec * Unit::deg).to_string() + " " + _cat.system().to_string();
}



/**
 * Returns the fraction [0.0:1.0] of the Solar-system source that appears illuminated by the Sun
 * when viewed from a given observing frame, assuming that the source has a spheroidal shape.
 *
 * @param frame   observing frame (observer location and time of observation)
 * @return        the fraction [0.0:1.0] that appears illuminated by the Sun from the observer's
 *                point of view.
 *
 * @sa solar_power()
 */
double SolarSystemSource::solar_illumination(const Frame& frame) const {
  return novas_check_nan("SolarSystemSource::solar_illumination", novas_solar_illum(&_object, frame._novas_frame()));
}

/**
 * Returns the heliocentric distance of a Solar-system source at the specified time of
 * observation.
 *
 * @param time        astrometric time of observation
 * @return            heliocentric distance of source at the specified time
 *
 * @sa helio_rate(), solar_power()
 */
Distance SolarSystemSource::helio_distance(const Time& time) const {
  double d = novas_helio_dist(time.jd(NOVAS_TDB), &_object, NULL);
  novas_check_nan("SolarSystemSource::helio_distance", d);
  return Distance(d * Unit::au);
}

/**
 * Returns the heliocentric rate of recession of a Solar-system source at the specified time of
 * observation.
 *
 * @param time        astrometric time of observation
 * @return            rate of recession from the Sun at the specified time
 *
 * @sa helio_distance()
 */
Speed SolarSystemSource::helio_rate(const Time& time) const {
  double r = NAN;
  novas_helio_dist(time.jd(NOVAS_TDB), &_object, &r);
  novas_check_nan("SolarSystemSource::helio_distance", r);
  return Speed(r * Unit::au / Unit::day);
}

/**
 * Returns the typical incident Solar power on the illuminated side of this Solar-system object.
 * The actual Solar power may vary due to fluctuations of the Solar output.
 *
 *
 * @param time    astrometric time of observation.
 * @return        [W/m<sup>2</sup>] Typical incident Solar power.
 *
 * @sa helio_distance(), solar_illumination()
 */
double SolarSystemSource::solar_power(const Time& time) const {
  return novas_check_nan("SolarSystemSource::solar_power", novas_solar_power(time.jd(NOVAS_TDB), &_object));
}



/**
 * Instantiates a planet from its NOVAS ID number.
 *
 * @param number    the NOVAS ID number
 */
Planet::Planet(enum novas_planet number) : SolarSystemSource() {
  if(make_planet(number, &_object) != 0)
    novas_error(0, EINVAL, "Planet::for_novas_id", "no planet for NOVAS id number: %d", number);
  else
    _valid = true;
}

/**
 * Returns a new planet corresponding to the specified NAIF ID, if possible, or else `std::nullopt`
 * if the NAIF id does not belong to a major planet in the SuperNOVAS definition (which includes
 * the Sun, Moon, SSB, EMB, and Pluto system barycenter also).
 *
 * @param naif    the NAIF ID number of the planet
 * @return        the corresponding planet, or `std::nullopt` if the ID does not specify a planet
 *                type body.
 *
 * @sa for_name(), naif_id()
 */
std::optional<Planet> Planet::for_naif_id(long naif) {
  enum novas_planet num = naif_to_novas_planet(naif);
  if((unsigned) num >= NOVAS_PLANETS)
    return std::nullopt;
  return Planet(num);
}

/**
 * Returns a new planet corresponding to the specified name (case insensitive), if possible, or
 * else `std::nullopt` if the name does not correspond to a major planet in the SuperNOVAS
 * definition (which includes the Sun, Moon, SSB, EMB, and Pluto system barycenter also).
 *
 * @param name    the planet's name (includes Sun, Moon, SSB, EMB, and Pluto-Barycenter also).
 *                Case insensitive.
 * @return        the corresponding planet, or `std::nullopt` if the ID does not specify a planet
 *                type body.
 *
 * @sa for_naif_id()
 */
std::optional<Planet> Planet::for_name(const std::string& name) {
  enum novas_planet num = novas_planet_for_name(name.c_str());
  if((unsigned) num >= NOVAS_PLANETS)
    return std::nullopt;
  return Planet(num);
}

/**
 * Returns the (Super)NOVAS ID of this planet (or planet type body in the SuperNOVAS sense).
 *
 * @return      the (Super)NOVAS ID of this planet .
 */
enum novas_planet Planet::novas_id() const {
  return (enum novas_planet) _object.number;
}

/**
 * Returns the NAIF ID number for this planet (or planet type body in the SuperNOVAS sense).
 *
 * @return    the NAIF id number of this planet.
 */
int Planet::naif_id() const {
  return novas_to_naif_planet(novas_id());
}

/**
 * Returns the ID number of for this planet (or planet type body in the SuperNOVAS sense) in
 * the JPL DExxx (e.g. DE441)  planetary ephemeris data files. For some planets, the DExxx
 * files contain data for the planet's center, while for others it is for the barycenter
 * of the planetary system.
 *
 * @return    The ID number of this planet in the JPL DExxx planetary ephemeris files.
 */
int Planet::de_number() const {
  return novas_to_dexxx_planet(novas_id());
}

/**
 * Returns the mean radius (average of the equatorial and polar radii) of this planet (or planet
 * type body in the SuperNOVAS sense)
 *
 * @return      the mean radius of this planet, or 0.0 this 'planet' does not denote a physical
 *              body (such as barycenters), or else a NAN distance if this planet is itself
 *              invalid.
 *
 * @sa mass()
 */
Distance Planet::mean_radius() const {
  static const double r[] = NOVAS_PLANET_RADII_INIT;
  if(!is_valid())
    return Distance(NAN);

  return Distance(r[_object.number]);
}

/**
 * Returns the mean radius (average of the equatorial and polar radii) of this planet (or planet
 * type body in the SuperNOVAS sense)
 *
 * @return      [kg] the mass of this planet, or 0.0 this 'planet' does not denote a physical
 *              body (such as barycenters), or else NAN if this planet is itself invalid.
 *
 * @sa mean_radius()
 */
double Planet::mass() const {
  static const double r[] = NOVAS_RMASS_INIT;
  if(!is_valid())
    return NAN;

  return Constant::M_sun / r[_object.number];
}

std::string Planet::to_string() const {
  return "Planet " + name();
}


static Planet _ssb = Planet(NOVAS_SSB);

/**
 * Returns the static reference to the Solar-System Barycenter (SSB).
 *
 * @return    the reference to the static instance of the SSB.
 *
 * @sa sun()
 */
const Planet& Planet::ssb() {
  return _ssb;
}

static Planet _mercury = Planet(NOVAS_MERCURY);

/**
 * Returns the static reference to the planet Mercury.
 *
 * @return    the reference to the static instance of Mercury.
 */
const Planet& Planet::mercury() {
  return _mercury;
}

static Planet _venus = Planet(NOVAS_VENUS);

/**
 * Returns the static reference to the planet Venus.
 *
 * @return    the reference to the static instance of Venus.
 */
const Planet& Planet::venus() {
  return _venus;
}

static Planet _earth = Planet(NOVAS_EARTH);

/**
 * Returns the static reference to the planet Earth.
 *
 * @return    the reference to the static instance of Earth.
 *
 * @sa emb()
 */
const Planet& Planet::earth() {
  return _earth;
}

static Planet _mars = Planet(NOVAS_MARS);

/**
 * Returns the static reference to the planet Mars.
 *
 * @return    the reference to the static instance of Mars.
 */
const Planet& Planet::mars() {
  return _mars;
}

static Planet _jupiter = Planet(NOVAS_JUPITER);

/**
 * Returns the static reference to the planet Jupiter.
 *
 * @return    the reference to the static instance of Jupiter.
 */
const Planet& Planet::jupiter() {
  return _jupiter;
}

static Planet _saturn = Planet(NOVAS_SATURN);

/**
 * Returns the static reference to the planet Saturn.
 *
 * @return    the reference to the static instance of Saturn.
 */
const Planet& Planet::saturn() {
  return _saturn;
}

static Planet _uranus = Planet(NOVAS_URANUS);

/**
 * Returns the static reference to the planet Uranus.
 *
 * @return    the reference to the static instance of Uranus.
 */
const Planet& Planet::uranus() {
  return _uranus;
}

static Planet _neptune = Planet(NOVAS_NEPTUNE);

/**
 * Returns the static reference to the planet Neptune.
 *
 * @return    the reference to the static instance of Neptune.
 */
const Planet& Planet::neptune() {
  return _neptune;
}

static Planet _pluto = Planet(NOVAS_PLUTO);

/**
 * Returns the static reference to the planet Pluto.
 *
 * @return    the reference to the static instance of Pluto.
 *
 * @sa pluto_system()
 */
const Planet& Planet::pluto() {
  return _pluto;
}

static Planet _sun = Planet(NOVAS_SUN);

/**
 * Returns the static reference to the Sun.
 *
 * @return    the reference to the static instance of the Sun.
 *
 * @sa ssb()
 */
const Planet& Planet::sun() {
  return _sun;
}

static Planet _moon = Planet(NOVAS_MOON);

/**
 * Returns the static reference to the Moon.
 *
 * @return    the reference to the static instance of the Moon.
 *
 * @sa emb()
 */
const Planet& Planet::moon() {
  return _moon;
}

static Planet _emb = Planet(NOVAS_EMB);

/**
 * Returns the static reference to the Earth-Moon Barycenter (EMB) position.
 *
 * @return    the reference to the static instance of the EMB.
 *
 * @sa earth(), moon()
 */
const Planet& Planet::emb() {
  return _emb;
}

static Planet _pluto_system = Planet(NOVAS_PLUTO_BARYCENTER);

/**
 * Returns the static reference to the Pluto system barycenter position.
 *
 * @return    the reference to the static instance of the Pluto system.
 *
 * @sa pluto()
 */
const Planet& Planet::pluto_system() {
  return _pluto_system;
}

/**
 * Instantiates a new Solar-system body whose positions are provided by ephemeris lookup.
 *
 * @param name      source name as defined in the ephemeris data (for name-based lookup).
 * @param number    source ID number in the ephemeris data (for id-based lookup).
 */
EphemerisSource::EphemerisSource(const std::string &name, long number) : SolarSystemSource() {
  if(make_ephem_object(name.c_str(), number, &_object) != 0)
    novas_trace("EphemerisSource(name, number)", 0, 0);
}

std::string EphemerisSource::to_string() const {
  return "EphemerisSource " + name();
}


/**
 * Instantiates a new Solar-system source defined by Keplerian orbital elements.
 *
 * @param name    source name as desired by the user.
 * @param number  ID number (or 0 if not needed by / known to the user).
 * @param orbit   Keplerian orbital elements.
 */
OrbitalSource::OrbitalSource(const std::string& name, long number, const Orbital& orbit) : SolarSystemSource() {
  static const char *fn = "OrbitalSource()";

  if(make_orbital_object(name.c_str(), number, orbit._novas_orbital(), &_object) != 0)
    novas_trace_invalid(fn);
  else if(!orbit.is_valid())
    novas_error(0, EINVAL, fn, "input orbital is invalid");
  else
    _valid = true;
}

/**
 * Returns the underlying C orbital elements data structure for this source.
 *
 * @return    the underlying C orbital elements data structure.
 */
const novas_orbital * OrbitalSource::_novas_orbital() const {
  return &_object.orbit;
}

/**
 * Returns the Keplerian orbital parameters of this source.
 *
 * @return    the Keplerian orbital parameters.
 */
Orbital OrbitalSource::orbital() const {
  return Orbital::from_novas_orbit(&_object.orbit);
}

std::string OrbitalSource::to_string() const {
  return "OrbitalSource " + name();
}

} // namespace supernovas

