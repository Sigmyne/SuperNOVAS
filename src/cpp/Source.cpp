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
 * Returns the type of this source
 *
 * @return    the source type constant
 */
enum novas_object_type Source::type() const {
  return _object.type;
}

/**
 * Returns the apparent position of a source (if possible), or else an invalid position. After the
 * return, you should probably check for validity, e.g.:
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
 * @return          the apparent position of the source, which may be invalid if the position
 *                  cannot be determined.
 *
 * @sa Planet::approx_apparent_in(), geometric_in()
 */
Apparent Source::apparent_in(const Frame& frame) const {
  sky_pos pos = {};

  if(novas_sky_pos(&_object, frame._novas_frame(), NOVAS_TOD, &pos) == 0) {
    Apparent app = Apparent::from_tod_sky_pos(frame, &pos);
    if(app)
      return app;
  }

  novas_trace_invalid("Source::apparent_in()");
  return Apparent::undefined();
}

/**
 * Returns the geometric position of a source (if possible) relative to the observer, or else an
 * invalid position if the source could not be positioned in the observing frame. After the
 * return, you should probably check for validity:
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
 * @return          the geometric (3D) position and velocity of the source, which may be invalid
 *                  if the position cannot be determined.
 *
 * @sa apparent_in()
 */
Geometric Source::geometric_in(const Frame& frame, enum novas_reference_system system) const {
  double p[3] = {0.0}, v[3] = {0.0};

  if(novas_geom_posvel(&_object, frame._novas_frame(), NOVAS_TOD, p, v) == 0) {
    return Geometric(frame,
            Position(p[0] * Unit::au, p[1] * Unit::au, p[2] * Unit::au),
            Velocity(v[0] * Unit::au / Unit::day, v[1] * Unit::au / Unit::day, v[2] * Unit::au / Unit::day),
            system
    );
  }

  novas_trace_invalid("Source::geometric_in()");
  return Geometric::undefined();
}

/**
 * Returns the time when the source rises above the specified elevation next for an observer
 * located on or near Earth's surface, or else `std::nullopt` if the observer is not near Earth's
 * surface. The returned value may also be NAN if the source does not cross the specified
 * elevation theshold within a day of the specified time of observation.
 *
 * @param el        elevation threshold angle
 * @param frame     observing frame (observer location and the lower bound for the returned time).
 * @param ref       atmospheric refraction model to assume
 * @param weather   local weather parameters for the refraction calculation
 * @return          the next time the source rises above the specified elevation after the frame's
 *                  observing time. It may be NAN if the source does not cross (rises above or
 *                  sets below) the elevation threshold within a day of the specified time of
 *                  observation. For observers non near Earth's surface, `Time::undefined()` will
 *                  be returned.
 *
 * @sa sets_below(), transits_in()
 */
Time Source::rises_above(const Angle& el, const Frame &frame, RefractionModel ref, const Weather& weather) const {
  Time t(novas_rises_above(el.deg(), &_object, frame._novas_frame(), ref), frame.eop());
  if(!t.is_valid())
    novas_trace_invalid("Source::rises_above()");
  return t;
}

/**
 * Returns the time when the source transits for an observer located on or near Earth's surface,
 * or else `std::nullopt` if the observer is not near Earth's surface.
 *
 * @param frame     observing frame (observer location and the lower bound for the returned time).
 * @return          the next time the source transits after the frame's observing time, or else
 *                  `Time::undefined()` if the observer is not near Earth's surface .
 *
 * @sa sets_below(), transits_in()
 */
Time Source::transits_in(const Frame &frame) const {
  Time t(novas_transit_time(&_object, frame._novas_frame()), frame.eop());
  if(!t.is_valid())
    novas_trace_invalid("Source::transits_in()");
  return t;
}

/**
 * Returns the time when the source sets below the specified elevation next for an observer
 * located on or near Earth's surface, or else `std::nullopt` if the observer is not near Earth's
 * surface. The returned value may also be NAN if the source does not cross the specified
 * elevation theshold within a day of the specified time of observation.
 *
 * @param el        elevation threshold angle
 * @param frame     observing frame (observer location and the lower bound for the returned time).
 * @param ref       atmospheric refraction model to assume
 * @param weather   local weather parameters for the refraction calculation
 * @return          the next time the source sets the specified elevation after the frame's
 *                  observing time. It may be NAN if the source does not cross (rises above or
 *                  sets below) the elevation threshold within a day of the specified time of
 *                  observation. For observers not near Earth's surface, `Time::undefined()` will
 *                  be returned.
 *
 * @sa rises_above(), transits_in()
 */
Time Source::sets_below(const Angle& el, const Frame &frame, RefractionModel ref, const Weather& weather) const {
  Time t(novas_sets_below(el.deg(), &_object, frame._novas_frame(), ref), frame.eop());
  if(!t.is_valid())
    novas_trace_invalid("Source::sets_below()");
  return t;
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
 * @return                a new near-term equatorial trajectory for this source, for the observing
 *                        location, around the time of observation, if possible, or else
 *                        `EquatorialTrack::undefined()`.
 *
 * @sa horizontal_track()
 */
EquatorialTrack Source::equatorial_track(const Frame &frame, double range_seconds) const {
  novas_track track = {};
  novas_equ_track(_novas_object(), frame._novas_frame(), range_seconds, &track);
  EquatorialTrack et = EquatorialTrack::from_novas_track(Equinox::tod(frame.time()), &track, Interval(range_seconds));
  if(!et.is_valid())
    novas_trace_invalid("Source::equatorial_track()");
  return et;
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
 * @param range           time range for which to fit a quadratic time evolution to the R.A., Dec,
 *                        distance, and radial velocity coordinates.
 * @return                a new near-term equatorial trajectory for this source, for the observing
 *                        location, around the time of observation, if possible, or else
 *                        `EquatorialTrack::undefined()`.
 *
 * @sa horizontal_track()
 */
EquatorialTrack Source::equatorial_track(const Frame &frame, const Interval& range) const {
  return equatorial_track(frame, range.seconds());
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
 *                        location, around the time of observation, if possible, or else
 *                        `HorizontalTrack::undefined()`.
 *
 * @sa equatorial_track()
 */
HorizontalTrack Source::horizontal_track(const Frame &frame, RefractionModel ref, const Weather& weather) const {
  if(!frame.observer().is_geodetic()) {
    novas_set_errno(EINVAL, "Source::horizontal_track()", "input frame is not a geodetic observing frame");
    return HorizontalTrack::undefined();
  }

  novas_frame f = *frame._novas_frame();
  on_surface *s = &f.observer.on_surf;

  s->temperature = weather.temperature().celsius();
  s->pressure = weather.pressure().mbar();
  s->humidity = weather.humidity();

  novas_track track = {};
  novas_hor_track(_novas_object(), frame._novas_frame(), ref, &track);

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
  return Angle(novas_check_nan("Source::sun_angle()", novas_sun_angle(&_object, frame._novas_frame()) * Unit::deg));
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
  return Angle(novas_check_nan("Source::moon_angle()", novas_moon_angle(&_object, frame._novas_frame()) * Unit::deg));
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
  return Angle(novas_check_nan("Source::angle_to()", novas_object_sep(&_object, &source._object, frame._novas_frame()) * Unit::deg));
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
  // defaults...
  _object.type = NOVAS_CATALOG_OBJECT;
  _object.number = e._cat_entry()->starnumber;

  if(!e.is_valid())
      novas_set_errno(EINVAL, "CatalogSource()", "input catalog entry is invalid");
  else {
    make_cat_object_sys(e._cat_entry(), e.system().name().c_str(), &_object);
    _valid = true;
  }
}

/**
 * Returns a pointer to a newly allocated copy of this catalog source instance
 *
 * @return    pointer to new copy of this catalog source instance.
 */
const Source* CatalogSource::copy() const {
  return new CatalogSource(*this);
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
  return "CatalogSource " + std::string(c->starname) + " @ " + TimeAngle(c->ra * Unit::hour_angle).to_string() +
          " " + Angle(c->dec * Unit::deg).to_string() + " " + _cat.system().to_string();
}

/**
 * Returns the Solar-system barycentric position and velocity of this Solar-system source, or else
 * an undefined geometry if the source's place cannot be determined at the requested time or
 * accuracy.
 *
 * @param time        The astrometric time
 * @param accuracy    (optional) NOVAS_FULL_ACCURACY (default), or NOVAS_REDUCED_ACCURACY.
 * @return            The Solar-system barycentric geometric position and velocity of this source
 *                    at the specified time, as obtained from ephemeris lookup; or else an
 *                    undefined (invalid) object if the place ould not be determined.
 */
Geometric SolarSystemSource::barycentric_at(const Time& time, enum novas_accuracy accuracy) const {
  const double tdb[2] = { time.jd(NOVAS_TDB), 0.0 };
  double p[3] = {0.0}, v[3] = {0.0};

  if(ephemeris(tdb, _novas_object(), NOVAS_BARYCENTER, accuracy, p, v) != 0) {
    novas_trace_invalid("SolarSystemSource::ssb_position_at()");
    return Geometric::undefined();
  }

  return Geometric(Observer::at_ssb().frame_at(time, accuracy),
          Position(p, Unit::AU), Velocity(v, Unit::AU / Unit::day), NOVAS_ICRS);
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
  return novas_check_nan("SolarSystemSource::solar_illumination()", novas_solar_illum(&_object, frame._novas_frame()));
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
Coordinate SolarSystemSource::helio_distance(const Time& time) const {
  double d = novas_helio_dist(time.jd(NOVAS_TDB), &_object, NULL);
  novas_check_nan("SolarSystemSource::helio_distance()", d);
  return Coordinate(d * Unit::au);
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
ScalarVelocity SolarSystemSource::helio_rate(const Time& time) const {
  double v = NAN;
  novas_helio_dist(time.jd(NOVAS_TDB), &_object, &v);
  novas_check_nan("SolarSystemSource::helio_rate()", v);
  return ScalarVelocity(v * Unit::au / Unit::day);
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
  return novas_check_nan("SolarSystemSource::solar_power()", novas_solar_power(time.jd(NOVAS_TDB), &_object));
}

/**
 * Instantiates an undefined / invalid planet.
 *
 */
Planet::Planet() : SolarSystemSource() {
  _object.type = NOVAS_PLANET;
  _object.number = (enum novas_planet) -1;
}

/**
 * Instantiates a planet from its NOVAS ID number.
 *
 * @param number    the NOVAS ID number
 */
Planet::Planet(enum novas_planet number) : SolarSystemSource() {
  // defaults...
  _object.type = NOVAS_PLANET;
  _object.number = number;

  if(make_planet(number, &_object) != 0)
    novas_set_errno(EINVAL, "Planet::for_novas_id()", "no planet for NOVAS id number: %d", number);
  else
    _valid = true;
}

/**
 * Returns a pointer to a newly allocated copy of this planet instance
 *
 * @return    pointer to new copy of this planet instance.
 */
const Source *Planet::copy() const {
  return new Planet(*this);
}

/**
 * Returns a new planet corresponding to the specified NAIF ID, if possible, or else an invalid
 * Planet if the NAIF id does not belong to a major planet in the SuperNOVAS definition (which
 * includes the Sun, Moon, SSB, EMB, and Pluto system barycenter also). As such, you should
 * typically check the result, e.g. as:
 *
 * ```c++
 *   Planet p = Planet::for_naif_id(num);
 *   if(!p) {
 *     // Oops there is no planet for that NAIF ID...
 *     return;
 *   }
 * ```
 *
 * @param naif    the NAIF ID number of the planet
 * @return        the corresponding planet, or an invalid planet if the ID does not specify a planet
 *                type body.
 *
 * @sa for_name(), naif_id()
 */
Planet Planet::for_naif_id(long naif) {
  enum novas_planet num = naif_to_novas_planet(naif);
  if((unsigned) num >= NOVAS_PLANETS) {
    novas_set_errno(EINVAL, "Planet::for_naif_id()", "no planet with NAIF %d", naif);
    return Planet();
  }
  return Planet(num);
}

/**
 * Returns a new planet corresponding to the specified name (case insensitive), if possible, or
 * else an invalid Planet if the name does not correspond to a major planet in the SuperNOVAS
 * definition (which includes the Sun, Moon, SSB, EMB, and Pluto system barycenter also). As such,
 * you should typically check the result, e.g. as:
 *
 * ```c++
 *   Planet p = Planet::for_name(some_name);
 *   if(!p) {
 *     // Oops there is no planet for that name...
 *     return;
 *   }
 * ```
 *
 * @param name    the planet's name (includes Sun, Moon, SSB, EMB, and Pluto-Barycenter also).
 *                Case insensitive.
 * @return        the corresponding planet, or an invalid planet if the ID does not specify a
 *                planet type body.
 *
 * @sa for_naif_id()
 */
Planet Planet::for_name(const std::string& name) {
  enum novas_planet num = novas_planet_for_name(name.c_str());
  if((unsigned) num >= NOVAS_PLANETS) {
    novas_set_errno(EINVAL, "Planet::for_name()", "no planet with name '%s'", name.c_str());
    return Planet();
  }
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
Coordinate Planet::mean_radius() const {
  static const double r[] = NOVAS_PLANET_RADII_INIT;
  if(!is_valid())
    return Coordinate::undefined();

  return Coordinate(r[_object.number]);
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

/**
 * Return an approximate apparent location on sky for a major planet, Sun, Moon, Earth-Moon
 * Barycenter (EMB) -- typically to arcmin level accuracy -- using Keplerian orbital elements. The
 * returned position is antedated for light-travel time (for Solar-System bodies). It also applies
 * an appropriate aberration correction (but not gravitational deflection).
 *
 * The orbitals can provide planet positions to arcmin-level precision for the rocky inner
 * planets, and to a fraction of a degree precision for the gas and ice giants and Pluto. The
 * accuracies for Uranus, Neptune, and Pluto are significantly improved (to the arcmin level) if
 * used in the time range of 1800 AD to 2050 AD. For a more detailed summary of the typical
 * accuracies, see either of the top two references below.
 *
 * For accurate positions, you should use planetary ephemerides (such as the JPL ephemerides via
 * the CALCEPH or CSPICE plugins) and `novas_sky_pos()` instead.
 *
 * While this function is generally similar to creating an orbital object with an orbit
 * initialized with `novas_make_planet_orbit()` or `novas_make_moon_orbit()`, and then calling
 * `novas_sky_pos()`, there are a few important differences to note:
 *
 * <ol>
 *  <li>This function calculates Earth and Moon positions about the Keplerian orbital position
 *  of the Earth-Moon Barycenter (EMB). In contrast, `novas_make_planet_orbit()` does not provide
 *  orbitals for the Earth directly, and `make_moot_orbit()` references the Moon's orbital to
 *  the Earth position returned by the currently configured planet calculator function (see
 *  `set_planet_provider()`).</li>
 *  <li>This function ignores gravitational deflection. It makes little sense to bother about
 *  corrections that are orders of magnitude below the accuracy of the orbital positions
 *  obtained.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>E.M. Standish and J.G. Williams 1992.</li>
 *  <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M., and Chapront, J. 1983, Astronomy and Astrophysics (ISSN 0004-6361),
 *      vol. 124, no. 1, July 1983, p. 50-62.</li>
 * </ol>
 *
 * @param frame     The observing frame for which to calculate the apparent positions.
 * @return          approximate, orbital model based, apparent position for the given planet.
 *                  It may be invalid either if this planet or the frame argument is invalid.
 *
 * @sa approx_geometric_in()
 */
Apparent Planet::approx_apparent_in(const Frame& frame) const {
  sky_pos pos = {};
  novas_approx_sky_pos(novas_id(), frame._novas_frame(), NOVAS_TOD, &pos);

  Apparent a = Apparent::from_tod_sky_pos(frame, &pos);
  if(!a.is_valid())
    novas_trace_invalid("Source::apparent_in()");
  return a;
}

/**
 * Returns approximate eometric posiitions and velocities for a major planet, Sun, Moon, Earth-Moon
 * Barycenter (EMB) -- typically to arcmin level accuracy -- using Keplerian orbital elements.
 *
 * The orbitals can provide planet positions to arcmin-level precision for the rocky inner
 * planets, and to a fraction of a degree precision for the gas and ice giants and Pluto. The
 * accuracies for Uranus, Neptune, and Pluto are significantly improved (to the arcmin level) if
 * used in the time range of 1800 AD to 2050 AD. For a more detailed summary of the typical
 * accuracies, see either of the top two references below.
 *
 * For accurate positions, you should use planetary ephemerides (such as the JPL ephemerides via
 * the CALCEPH or CSPICE plugins) and `novas_sky_pos()` instead.
 *
 * While this function is generally similar to creating an orbital object with an orbit
 * initialized with `novas_make_planet_orbit()` or `novas_make_moon_orbit()`, and then calling
 * `novas_geom_posvel()`, with an important difference:
 *
 * <ol>
 *  <li>This function calculates Earth and Moon positions about the Keplerian orbital position
 *  of the Earth-Moon Barycenter (EMB). In contrast, `novas_make_planet_orbit()` does not provide
 *  orbitals for the Earth directly, and `make_moot_orbit()` references the Moon's orbital to
 *  the Earth position returned by the currently configured planet calculator function (see
 *  `set_planet_provider()`).</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>E.M. Standish and J.G. Williams 1992.</li>
 *  <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M., and Chapront, J. 1983, Astronomy and Astrophysics (ISSN 0004-6361),
 *      vol. 124, no. 1, July 1983, p. 50-62.</li>
 * </ol>
 *
 * @param frame   the observing frame for which to calculate geometric positions.
 * @return        approximate, orbital model based, geometric positions and velocities of this
 *                planet in the specified observing frame. It may be invalid either if this planet
 *                or the frame argument is invalid.
 *
 * @sa approx_apparent_in()
 */
Geometric Planet::approx_geometric_in(const Frame& frame) const {
  double p[3] = {0.0}, v[3] = {0.0};

  novas_approx_heliocentric(novas_id(), frame.jd(NOVAS_TDB), p, v);

  for(int i = 0; i < 3; i++) {
    const novas_frame *f = frame._novas_frame();
    p[i] += f->sun_pos[i];
    v[i] += f->sun_vel[i];
  }

  Geometric g(frame, Position(p, Unit::AU), Velocity(v, Unit::AU / Unit::day));
  if(!g.is_valid())
    novas_trace_invalid("Source::geometric_in()");
  return g;
}

/**
 * Returns the approximate Keplerian orbital parameters for this planet, calculated for the
 * specified time of reference.
 *
 * @param ref_time    Reference time for the Keplerian orbital parameters.
 * @return            the approximate Keplerain orbital of this planet around the specified
 *                    reference time.
 */
Orbital Planet::orbit(const Time& ref_time) const {
  novas_orbital orbit = {};
  if(novas_make_planet_orbit(novas_id(), ref_time.jd(NOVAS_TDB), &orbit) != 0)
    novas_trace_invalid("Planet::orbit()");
  return Orbital::from_novas_orbit(&orbit);
}

std::string Planet::to_string() const {
  return "Planet " + name();
}

/**
 * Returns the static reference to the planet Mercury.
 *
 * @return    the reference to the static instance of Mercury.
 */
const Planet& Planet::mercury() {
  static const Planet _mercury = Planet(NOVAS_MERCURY);
  return _mercury;
}

/**
 * Returns the static reference to the planet Venus.
 *
 * @return    the reference to the static instance of Venus.
 */
const Planet& Planet::venus() {
  static const Planet _venus = Planet(NOVAS_VENUS);
  return _venus;
}

/**
 * Returns the static reference to the planet Earth.
 *
 * @return    the reference to the static instance of Earth.
 *
 * @sa emb()
 */
const Planet& Planet::earth() {
  static const Planet _earth = Planet(NOVAS_EARTH);
  return _earth;
}

/**
 * Returns the static reference to the planet Mars.
 *
 * @return    the reference to the static instance of Mars.
 */
const Planet& Planet::mars() {
  static const Planet _mars = Planet(NOVAS_MARS);
  return _mars;
}

/**
 * Returns the static reference to the planet Jupiter.
 *
 * @return    the reference to the static instance of Jupiter.
 */
const Planet& Planet::jupiter() {
  static const Planet _jupiter = Planet(NOVAS_JUPITER);
  return _jupiter;
}

/**
 * Returns the static reference to the planet Saturn.
 *
 * @return    the reference to the static instance of Saturn.
 */
const Planet& Planet::saturn() {
  static const Planet _saturn = Planet(NOVAS_SATURN);
  return _saturn;
}

/**
 * Returns the static reference to the planet Uranus.
 *
 * @return    the reference to the static instance of Uranus.
 */
const Planet& Planet::uranus() {
  static const Planet _uranus = Planet(NOVAS_URANUS);
  return _uranus;
}

/**
 * Returns the static reference to the planet Neptune.
 *
 * @return    the reference to the static instance of Neptune.
 */
const Planet& Planet::neptune() {
  static const Planet _neptune = Planet(NOVAS_NEPTUNE);
  return _neptune;
}

/**
 * Returns the static reference to the planet Pluto.
 *
 * @return    the reference to the static instance of Pluto.
 *
 * @sa Barycenter::pluto_system()
 */
const Planet& Planet::pluto() {
  static const Planet _pluto = Planet(NOVAS_PLUTO);
  return _pluto;
}

/**
 * Returns the static reference to the Sun.
 *
 * @return    the reference to the static instance of the Sun.
 *
 * @sa ssb()
 */
const Planet& Planet::sun() {
  static const Planet _sun = Planet(NOVAS_SUN);
  return _sun;
}

/**
 * Returns the static reference to the Moon.
 *
 * @return    the reference to the static instance of the Moon.
 *
 * @sa emb()
 */
const Planet& Planet::moon() {
  static const Planet _moon = Planet(NOVAS_MOON);
  return _moon;
}

/**
 * Returns the static reference to the Solar-System Barycenter (SSB).
 *
 * @return    the reference to the static instance of the SSB.
 *
 * @sa sun()
 */
const Planet& Planet::ssb() {
  static const Planet _ssb = Planet(NOVAS_SSB);
  return _ssb;
}

/**
 * Returns the static reference to the Earth-Moon Barycenter (EMB) position.
 *
 * @return    the reference to the static instance of the EMB.
 *
 * @sa earth(), moon()
 */
const Planet& Planet::emb() {
  static const Planet _emb = Planet(NOVAS_EMB);
  return _emb;
}

/**
 * Returns the static reference to the Pluto system barycenter position.
 *
 * @return    the reference to the static instance of the Pluto system.
 *
 * @sa pluto()
 */
const Planet& Planet::pluto_system() {
  static const Planet _pluto_system = Planet(NOVAS_PLUTO_BARYCENTER);
  return _pluto_system;
}

/**
 * Instantiates a new Solar-system body whose positions are provided by ephemeris lookup.
 *
 * @param name      source name as defined in the ephemeris data (for name-based lookup).
 * @param number    (optional) source ID number in the ephemeris data (for id-based lookup;
 *                  default: -1).
 */
EphemerisSource::EphemerisSource(const std::string &name, long number) : SolarSystemSource() {
  // defaults...
  _object.type = NOVAS_EPHEM_OBJECT;
  _object.number = number;

  if(make_ephem_object(name.c_str(), number, &_object) != 0)
    novas_trace_invalid("EphemerisSource()");
  else
    _valid = true;
}

/**
 * Returns a pointer to a newly allocated copy of this Solar-system ephemeris source instance
 *
 * @return    pointer to new copy of this Solar-system ephemeris source instance.
 */
const Source *EphemerisSource::copy() const {
  return new EphemerisSource(*this);
}

/**
 * Returns the number designation of this ephemeris
 *
 * @return    the source ID number (for number ID based lookup).
 *
 * @sa name()
 */
long EphemerisSource::number() const {
  return _object.number;
}

/**
 * Returns a simple string representation of this ephemeris source, with the name and
 * number designations.
 *
 * @return      a string containing the name and number designations for this source.
 */
std::string EphemerisSource::to_string() const {
  return "EphemerisSource " + name() + " (nr. " + std::to_string(number()) + ")";
}


/**
 * Instantiates a new Solar-system source defined by Keplerian orbital elements.
 *
 * @param name    source name as desired by the user.
 * @param orbit   Keplerian orbital elements.
 */
OrbitalSource::OrbitalSource(const std::string& name, const Orbital& orbit) : SolarSystemSource() {
  static const char *fn = "OrbitalSource()";

  // defaults...
  _object.type = NOVAS_ORBITAL_OBJECT;
  _object.number = 0;

  if(make_orbital_object(name.c_str(), 0, orbit._novas_orbital(), &_object) != 0)
    novas_trace_invalid(fn);
  else if(!orbit.is_valid())
    novas_set_errno(EINVAL, fn, "input orbital is invalid");
  else
    _valid = true;
}

/**
 * Returns a pointer to a newly allocated copy of this Solar-system orbital source instance
 *
 * @return    pointer to new copy of this Solar-system orbital source instance.
 */
const Source *OrbitalSource::copy() const {
  return new OrbitalSource(*this);
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
  Orbital orbit = Orbital::from_novas_orbit(&_object.orbit);
  if(!orbit.is_valid())
    novas_trace_invalid("OrbitalSource::orbit()");
  return orbit;
}

std::string OrbitalSource::to_string() const {
  return "OrbitalSource " + name();
}

} // namespace supernovas

