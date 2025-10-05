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

bool Source::is_valid() const {
  return _object.type >= 0 && _object.type < NOVAS_OBJECT_TYPES;
}

const struct novas_object *Source::_novas_object() const {
  return &_object;
}

std::string Source::name() const {
  return std::string(_object.name);
}

Apparent Source::apparent(const Frame& frame, enum novas_reference_system system) const {
  sky_pos pos = {};

  if(novas_sky_pos(&_object, frame._novas_frame(), system, &pos) != 0) {
    novas_trace_nan("Source::apparent");
    return Apparent::invalid();
  }

  return Apparent(frame, pos, system);
}

Geometric Source::geometric(const Frame& frame, enum novas_reference_system system) const {
  double p[3] = {0.0}, v[3] = {0.0};

  if(novas_geom_posvel(&_object, frame._novas_frame(), system, p, v) != 0) {
    novas_trace_nan("Source::geometric");
    return Geometric::invalid();
  }

  return Geometric(frame, system,
          Position(p[0] * Unit::au, p[1] * Unit::au, p[2] * Unit::au),
          Velocity(v[0] * Unit::au / Unit::day, v[1] * Unit::au / Unit::day, v[2] * Unit::au / Unit::day)
  );
}

static EOP no_eop = EOP(0, 0.0, 0.0, 0.0);

static const EOP& extract_eop(const Frame &frame) {
  if(!frame.observer().is_geodetic())
    return no_eop;

  const GeodeticObserver& eobs = dynamic_cast<const GeodeticObserver&>(frame.observer());
  return eobs.eop();
}

Time Source::rises_above(double el, const Frame &frame, RefractionModel ref, const Weather& weather) const {
  if(ref)
    el = Horizontal(0.0, el * Unit::deg).to_unrefracted(frame, ref, weather).elevation().deg();
  Time t = Time(novas_rises_above(el / Unit::deg, &_object, frame._novas_frame(), NULL), extract_eop(frame));
  novas_check_nan("Source::rises_above", t.jd());
  return t;
}

Time Source::transits(const Frame &frame) const {
  return Time(
          novas_check_nan("Source::transits", novas_transit_time(&_object, frame._novas_frame())),
          extract_eop(frame));
}

Time Source::sets_below(double el, const Frame &frame, RefractionModel ref, const Weather& weather) const {
  if(ref)
    el = Horizontal(0.0, el * Unit::deg).to_unrefracted(frame, ref, weather).elevation().deg();
  return Time(
          novas_check_nan("Source::sets_below", novas_sets_below(el / Unit::deg, &_object, frame._novas_frame(), NULL)),
          extract_eop(frame));
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
  if(make_cat_object_sys(e._cat_entry(), e.system().name().c_str(), &_object) != 0)
    novas_trace("CatalogSource(cat_entry)", 0, 0);
}

bool CatalogSource::is_valid() const {
  return Source::is_valid() && _system.is_valid() && catalog_entry().is_valid();
}

const cat_entry * CatalogSource::_cat_entry() const {
  return &_object.star;
}

CatalogEntry CatalogSource::catalog_entry() const {
  return CatalogEntry(_object.star, std::string(_object.star.starname));
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
    novas_trace("Planet(number)", 0, 0);
}

Planet::Planet(const std::string& name) : SolarSystemSource() {
  if(make_planet(novas_planet_for_name(name.c_str()), &_object) != 0)
    novas_trace("Planet(name)", 0, 0);
}

bool Planet::is_valid() const {
  return Source::is_valid() && _object.number >= 0 && _object.number < NOVAS_PLANETS;
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
const Planet& ssb() {
  return _ssb;
}

static Planet _mercury = Planet(NOVAS_MERCURY);
const Planet& mercury() {
  return _mercury;
}

static Planet _venus = Planet(NOVAS_VENUS);
const Planet& venus() {
  return _venus;
}

static Planet _earth = Planet(NOVAS_EARTH);
const Planet& earth() {
  return _earth;
}

static Planet _mars = Planet(NOVAS_MARS);
const Planet& mars() {
  return _mars;
}

static Planet _jupiter = Planet(NOVAS_JUPITER);
const Planet& jupiter() {
  return _jupiter;
}

static Planet _saturn = Planet(NOVAS_SATURN);
const Planet& saturn() {
  return _saturn;
}

static Planet _uranus = Planet(NOVAS_URANUS);
const Planet& uranus() {
  return _uranus;
}

static Planet _neptune = Planet(NOVAS_NEPTUNE);
const Planet& neptune() {
  return _neptune;
}

static Planet _pluto = Planet(NOVAS_PLUTO);
const Planet& pluto() {
  return _pluto;
}

static Planet _sun = Planet(NOVAS_SUN);
const Planet sun() {
  return _sun;
}

static Planet _moon = Planet(NOVAS_MOON);
const Planet& moon() {
  return _moon;
}

static Planet _emb = Planet(NOVAS_EMB);
const Planet& emb() {
  return _emb;
}

static Planet _pluto_system = Planet(NOVAS_PLUTO_BARYCENTER);
const Planet& pluto_system() {
  return _pluto_system;
}




EphemerisSource::EphemerisSource(const std::string &name, long number) : SolarSystemSource() {
  if(make_ephem_object(name.c_str(), number, &_object) != 0)
    novas_trace("EphemerisSource(name, number)", 0, 0);
}


static bool is_valid_orbital_system(const novas_orbital_system *s) {
  if(s->center < 0 || s->center >= NOVAS_PLANETS)
    return false;
  if(s->plane < 0 || s->plane >= NOVAS_REFERENCE_PLANES)
    return false;
  if(s->type < 0 || s->type >= NOVAS_REFERENCE_SYSTEMS)
    return false;
  if(isnan(s->Omega))
    return false;
  if(isnan(s->obl))
    return false;
  return true;
}

static bool is_valid_orbit(const novas_orbital *o) {
  if(!(o->a > 0.0))
    return false;
  if(!(o->e >= 0.0))
    return false;
  if(!(o->apsis_period > 0.0))
    return false;
  if(!(o->node_period > 0.0))
    return false;
  if(isnan(o->jd_tdb))
    return false;
  if(isnan(o->M0))
    return false;
  if(isnan(o->omega))
    return false;
  if(isnan(o->Omega))
    return false;
  if(isnan(o->n))
    return false;
  if(isnan(o->i))
    return false;
  return is_valid_orbital_system(&o->system);
}


OrbitalSource::OrbitalSource(const std::string& name, long number, const novas_orbital *orbit) : SolarSystemSource() {
  static const char *fn = "OrbitalSource(name, number, orbit";

  if(!is_valid_orbit(orbit))
    novas_error(0, EINVAL, fn, "input orbit is invalid");

  if(make_orbital_object(name.c_str(), number, orbit, &_object) != 0)
    novas_trace(fn, 0, 0);
}

bool OrbitalSource::is_valid() const {
  return Source::is_valid() && is_valid_orbit(&_object.orbit);
}

Position OrbitalSource::orbital_position(const Time& time, enum novas_accuracy accuracy) const {
  static const char *fn = "OrbitalSource::orbital_position";
  double p[3] = {0.0};

  if(!is_valid()) {
    novas_trace_nan(fn);
    return Position::invalid();
  }

  if(novas_orbit_posvel(time.jd(), &_object.orbit, accuracy, p, NULL) != 0) {
    novas_trace_nan(fn);
    return Position::invalid();
  }

  return Position(p, Unit::au);
}

Velocity OrbitalSource::orbital_velocity(const Time& time, enum novas_accuracy accuracy) const {
  double v[3] = {0.0};

  if(!is_valid())
    return Velocity::invalid();

  if(novas_orbit_posvel(time.jd(), &_object.orbit, accuracy, NULL, v) != 0)
    return Velocity::invalid();

  return Velocity(v, Unit::au / Unit::day);
}

const novas_orbital * OrbitalSource::_novas_orbital() const {
  return &_object.orbit;
}

} // namespace supernovas

