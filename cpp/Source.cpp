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

const struct novas_object *Source::_novas_object() const {
  return &_object;
}

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
 *  - For Solar system sources:
 *     * SuperNOVAS may not have a planet or ephemeris provider function configured for the given
 *       accuracy.
 *     * The planet or ephemeris provider function does not have data for the source, or planet at
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
 * @param system    (optional) The coordinate reference system in which the apparent coordinates
 *                  should be calculated (default is NOVAS_TOD for true-of-date coordinates).
 * @return the apparent position of the source, or else an invalid position (with NAN values).
 *
 * @sa geometric()
 */
Apparent Source::apparent(const Frame& frame, enum novas_reference_system system) const {
  sky_pos pos = {};

  if(novas_sky_pos(&_object, frame._novas_frame(), system, &pos) != 0) {
    novas_trace_invalid("Source::apparent");
    return Apparent::invalid();
  }

  return Apparent::from_sky_pos(pos, frame, system).value();
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
 *  - For Solar system sources:
 *     * SuperNOVAS may not have a planet or ephemeris provider function configured for the given
 *       accuracy.
 *     * The planet or ephemeris provider function does not have data for the source, or planet at
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
 * @param system    (optional) The coordinate reference system in which the apparent coordinates
 *                  should be calculated (default is NOVAS_TOD for true-of-date coordinates).
 * @return the geometric (3D) position and velocity of the source, or else an invalid position
 *         (with NAN values).
 *
 * @sa apparent(), Frame::ephemeris_position(), Frame::ephemeris_velocity()
 */
Geometric Source::geometric(const Frame& frame, enum novas_reference_system system) const {
  double p[3] = {0.0}, v[3] = {0.0};

  if(novas_geom_posvel(&_object, frame._novas_frame(), system, p, v) != 0) {
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

std::optional<Time> Source::transits(const Frame &frame) const {
  static const char *fn = "Source::transits";

  if(frame.observer().is_geodetic())
    return Time(
          novas_check_nan("Source::transits", novas_transit_time(&_object, frame._novas_frame())),
          extract_eop(frame));

  novas_error(0, ENOSYS, fn, "Cannot calculate transit time for a non-geodetic observer.");
  return std::nullopt;
}

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

Angle Source::sun_angle(const Frame& frame) const {
  return Angle(novas_check_nan("Source::sun_angle", novas_sun_angle(&_object, frame._novas_frame()) * Unit::deg));
}

Angle Source::moon_angle(const Frame& frame) const {
  return Angle(novas_check_nan("Source::moon_angle", novas_moon_angle(&_object, frame._novas_frame()) * Unit::deg));
}

Angle Source::angle_to(const Source& source, const Frame& frame) const {
  return Angle(novas_check_nan("Source::angle_to", novas_object_sep(&_object, &source._object, frame._novas_frame()) * Unit::deg));
}


void Source::set_case_sensitive(bool value) {
  novas_case_sensitive(value);
}






CatalogSource::CatalogSource(const CatalogEntry& e)
: Source(), _system(e.system()) {
  static const char *fn = "CatalogSource()";

  if(make_cat_object_sys(e._cat_entry(), e.system().name().c_str(), &_object) != 0)
    novas_trace_invalid(fn);
  else if(!e.is_valid())
    novas_error(0, EINVAL, fn, "input catalog entry is invalid");
  else
    _valid = true;
}

const cat_entry * CatalogSource::_cat_entry() const {
  return &_object.star;
}

CatalogEntry CatalogSource::catalog_entry() const {
  return CatalogEntry(_object.star, _system);
}




double SolarSystemSource::solar_illumination(const Frame& frame) const {
  return novas_check_nan("SolarSystemSource::solar_illumination", novas_solar_illum(&_object, frame._novas_frame()));
}

double SolarSystemSource::helio_distance(const Time& time, double *rate) const {
  return novas_check_nan("SolarSystemSource::helio_distance", novas_helio_dist(time.jd(NOVAS_TDB), &_object, rate));
}

double SolarSystemSource::solar_power(const Time& time) const {
  return novas_check_nan("SolarSystemSource::solar_power", novas_solar_power(time.jd(NOVAS_TDB), &_object));
}


Planet::Planet(enum novas_planet number) : SolarSystemSource() {
  if(make_planet(number, &_object) != 0)
    novas_error(0, EINVAL, "Planet::for_novas_id", "no planet for NOVAS id number: %d", number);
  else
    _valid = true;
}

std::optional<Planet> Planet::for_naif_id(long naif) {
  enum novas_planet num = naif_to_novas_planet(naif);
  if((int) num < 0)
    return std::nullopt;
  return Planet(num);
}

std::optional<Planet> Planet::for_name(const std::string& name) {
  enum novas_planet num = novas_planet_for_name(name.c_str());
  if((int) num < 0)
    return std::nullopt;
  return Planet(num);
}

enum novas_planet Planet::novas_id() const {
  return (enum novas_planet) _object.number;
}

int Planet::naif_id() const {
  return novas_to_naif_planet(novas_id());
}

int Planet::de_number() const {
  return novas_to_dexxx_planet(novas_id());
}

double Planet::mean_radius() const {
  static double r[] = NOVAS_PLANET_RADII_INIT;
  if(!is_valid())
    return NAN;

  return r[_object.number];
}

double Planet::mass() const {
  static double r[] = NOVAS_RMASS_INIT;
  if(!is_valid())
    return NAN;

  return Constant::M_sun / r[_object.number];
}


static Planet _ssb = Planet(NOVAS_SSB);
const Planet& Planet::ssb() {
  return _ssb;
}

static Planet _mercury = Planet(NOVAS_MERCURY);
const Planet& Planet::mercury() {
  return _mercury;
}

static Planet _venus = Planet(NOVAS_VENUS);
const Planet& Planet::venus() {
  return _venus;
}

static Planet _earth = Planet(NOVAS_EARTH);
const Planet& Planet::earth() {
  return _earth;
}

static Planet _mars = Planet(NOVAS_MARS);
const Planet& Planet::mars() {
  return _mars;
}

static Planet _jupiter = Planet(NOVAS_JUPITER);
const Planet& Planet::jupiter() {
  return _jupiter;
}

static Planet _saturn = Planet(NOVAS_SATURN);
const Planet& Planet::saturn() {
  return _saturn;
}

static Planet _uranus = Planet(NOVAS_URANUS);
const Planet& Planet::uranus() {
  return _uranus;
}

static Planet _neptune = Planet(NOVAS_NEPTUNE);
const Planet& Planet::neptune() {
  return _neptune;
}

static Planet _pluto = Planet(NOVAS_PLUTO);
const Planet& Planet::pluto() {
  return _pluto;
}

static Planet _sun = Planet(NOVAS_SUN);
const Planet& Planet::sun() {
  return _sun;
}

static Planet _moon = Planet(NOVAS_MOON);
const Planet& Planet::moon() {
  return _moon;
}

static Planet _emb = Planet(NOVAS_EMB);
const Planet& Planet::emb() {
  return _emb;
}

static Planet _pluto_system = Planet(NOVAS_PLUTO_BARYCENTER);
const Planet& Planet::pluto_system() {
  return _pluto_system;
}




EphemerisSource::EphemerisSource(const std::string &name, long number) : SolarSystemSource() {
  if(make_ephem_object(name.c_str(), number, &_object) != 0)
    novas_trace("EphemerisSource(name, number)", 0, 0);
}


static bool is_valid_orbital_system(const novas_orbital_system *s) {
  static const char *fn = "OrbitalSource::from:orbit";

  if(s->center < 0 || s->center >= NOVAS_PLANETS)
    return novas_error(0, EINVAL, fn, "orbital system center planet is invalid: %d", s->center);
  if(s->plane < 0 || s->plane >= NOVAS_REFERENCE_PLANES)
    return novas_error(0, EINVAL, fn, "orbital system plane is invalid: %d", s->plane);
  if(s->type < 0 || s->type >= NOVAS_REFERENCE_SYSTEMS)
    return novas_error(0, EINVAL, fn, "orbital system type is invalid: %d", s->type);
  if(isnan(s->Omega))
    return novas_error(0, EINVAL, fn, "orbital system Omega is NAN");
  if(isnan(s->obl))
    return novas_error(0, EINVAL, fn, "orbital system obliquity is NAN");
  return true;
}

static bool is_valid_orbit(const novas_orbital *o) {
  static const char *fn = "OrbitalSource::from_orbit";

  if(!(o->a > 0.0))
    return novas_error(0, EINVAL, fn, "orbital system semi-major axis is invalid: %g AU", o->a);
  if(!(o->e >= 0.0))
    return novas_error(0, EINVAL, fn, "orbital system eccentricity is invalid: %g", o->e);
  if(!(o->apsis_period > 0.0))
    return novas_error(0, EINVAL, fn, "orbital system apsis_period is invalid: %g days", o->apsis_period);
  if(!(o->node_period > 0.0))
    return novas_error(0, EINVAL, fn, "orbital system node_period is invalid: %g days", o->node_period);
  if(isnan(o->jd_tdb))
    return novas_error(0, EINVAL, fn, "orbital system reference JD date is NAN");
  if(isnan(o->M0))
    return novas_error(0, EINVAL, fn, "orbital system M0 is NAN");
  if(isnan(o->omega))
    return novas_error(0, EINVAL, fn, "orbital system omega is NAN");
  if(isnan(o->Omega))
    return novas_error(0, EINVAL, fn, "orbital system Omega is NAN");
  if(isnan(o->n))
    return novas_error(0, EINVAL, fn, "orbital system mean daily motion is NAN");
  if(isnan(o->i))
    return novas_error(0, EINVAL, fn, "orbital system inclination is NAN");
  return is_valid_orbital_system(&o->system);
}


OrbitalSource::OrbitalSource(const std::string& name, long number, const novas_orbital orbit) : SolarSystemSource() {
  _valid = true;
}

std::optional<OrbitalSource> OrbitalSource::from_orbit(const std::string& name, long number, const novas_orbital orbit) {
  if(!is_valid_orbit(&orbit))
    return std::nullopt;

  return OrbitalSource(name, number, orbit);
}

Position OrbitalSource::orbital_position(const Time& time, enum novas_accuracy accuracy) const {
  double p[3] = {0.0};

  if(novas_orbit_posvel(time.jd(), &_object.orbit, accuracy, p, NULL) != 0) {
    novas_trace_invalid("OrbitalSource::orbital_position");
    return Position::invalid();
  }

  return Position(p, Unit::au);
}

Velocity OrbitalSource::orbital_velocity(const Time& time, enum novas_accuracy accuracy) const {
  double v[3] = {0.0};

  if(novas_orbit_posvel(time.jd(), &_object.orbit, accuracy, NULL, v) != 0) {
    novas_trace_invalid("OrbitalSource::orbital_velocity");
    return Velocity::invalid();
  }

  return Velocity(v, Unit::au / Unit::day);
}

const novas_orbital * OrbitalSource::_novas_orbital() const {
  return &_object.orbit;
}

} // namespace supernovas

