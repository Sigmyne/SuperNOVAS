/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cstring>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {


Observer::Observer(enum novas_observer_place type, const Site& site, const Position& pos,
        const Velocity& vel) {
  _observer.where = type;
  _observer.on_surf = *site._on_surface();

  double pUnit = (type == NOVAS_SOLAR_SYSTEM_OBSERVER) ? Unit::AU : Unit::km;
  double vUnit = (type == NOVAS_SOLAR_SYSTEM_OBSERVER) ? Unit::AU / Unit::day : Unit::km / Unit::s;

  for(int i = 0; i < 3; i++) {
    _observer.near_earth.sc_pos[i] = pos._array()[i] / pUnit;
    _observer.near_earth.sc_vel[i] = vel._array()[i] / vUnit;
  }
};

/**
 * Returns a pointer to a newly allocated copy of this generic observer instance
 *
 * @return    pointer to new copy of this generic observer instance.
 */
const Observer *Observer::copy() const {
  return new Observer(*this);
}

/**
 * Checks if this observer is at a geodetic location, such as an observer at a fixed observatory
 * location, or an airborne observer.
 *
 * @return    `true` is this a geodetic observer on or near Earth's surface, otherwise `false`
 *
 * @sa is_geocentric()
 */
bool Observer::is_geodetic() const {
  return false;
}

/**
 * Checks if this is a fictious observer located at the geocenter.
 *
 * @return    `true` if this observer is located at the geocenter, otherwise `false`
 *
 * @sa is_geodetic()
 */
bool Observer::is_geocentric() const {
  return false;
}

/**
 * Returns the underlying NOVAS C `observer` data structure.
 *
 * @return    pointer to the underlying C `observer` data structure.
 */
const observer * Observer::_novas_observer() const {
  return &_observer;
}

/**
 * Returns the observer type.
 *
 * @return   the observer type constant for this observer
 */
enum novas_observer_place Observer::type() const {
  return _observer.where;
}

/**
 * Returns a string representation of this observer.
 *I am salivating at the thought of it. Tell Benjami
 * @return    a new string with a brief description of this observer.
 */
std::string Observer::to_string() const {
  return "Observer type " + std::to_string(_observer.where);
}

/**
 * Returns a new observer located at a fixed observing site.
 *
 * @param site    the observing site
 * @param eop     Earth Orientation Parameters (EOP) appropriate the time of observation, such as
 *                obtained from the IERS bulletins or data service.
 * @return        a new observer instance for the given observing site.
 *
 * @sa moving_on_earth(), in_earth_orbit(), in_solar_system(), at_geocenter(), at_ssb()
 */
GeodeticObserver Observer::on_earth(const Site& site, const EOP& eop) {
  GeodeticObserver o = GeodeticObserver(site, eop);
  if(!o.is_valid())
    novas_trace_invalid("Observer::on_earth");
  return o;
}

/**
 * Returns a new observer that is moving, at some velocitym relative to Earth's surface,
 * such as an airborne aircraft or balloon based observatory.
 *
 * @param geodetic    the momentary geodetic location of the observer.
 * @param itrs_vel    the momentary velocity of the observer with respect to the surface
 *                    (in ITRS).
 * @param eop         Earth Orientation Parameters (EOP) appropriate around the time of
 *                    observation, such as obtained from the IERS bulletins or data service.
 * @return            a new observer instance for the given moving observer.
 *
 * @sa on_earth(), in_earth_orbit(), in_solar_system(), at_geocenter(), at_ssb()
 */
GeodeticObserver Observer::moving_on_earth(const Site& geodetic, const Velocity& itrs_vel, const EOP& eop) {
  GeodeticObserver o = GeodeticObserver(geodetic, itrs_vel, eop);
  if(!o.is_valid())
    novas_trace_invalid("Observer::on_earth");
  return o;
}

/**
 * Instantiates a new observer that is moving relative to Earth's surface, such as an airborne
 * observer.
 *
 * @param site          the momentary geodetic location of the observer.
 * @param eop           Earth Orientation Parameters (EOP) appropriate around the time of
 *                      observation.
 * @param horizontal    momentary horizontal speed of moving observer.
 * @param direction     azimuthal direction of motion (from North, measured to the East).
 * @param vertical      (optional) momentary vertical speed of observer (default: 0).
 *
 * @sa Site::enu_to_itrf()
 */
GeodeticObserver Observer::moving_on_earth(const Site& site, const EOP& eop, const Speed& horizontal, const Angle& direction, const Speed& vertical) {
  GeodeticObserver o = GeodeticObserver(site, eop, horizontal, direction, vertical);
  if(!o.is_valid())
    novas_trace_invalid("Observer::on_earth");
  return o;
}

/**
 * Returns a new observer orbiting the Earth.
 *
 * @param pos       momentary position of the observer relative to the geocenter.
 * @param vel       momentary velocity of the observer relative to the geocenter.
 * @return          a new observer instance for the observer in Earth orbit.
 *
 * @sa on_earth(), in_solar_system(), at_geocenter(), at_ssb()
 */
GeocentricObserver Observer::in_earth_orbit(const Position& pos, const Velocity& vel) {
  GeocentricObserver o = GeocentricObserver(pos, vel);
  if(!o.is_valid())
    novas_trace_invalid("Observer::in_earth_orbit");
  return o;
}

/**
 * Returns a fictitious observer placed at the location of the geocenter.
 *
 * @return         a new fictitious observer located at the geocenter.
 *
 * @sa on_earth(), in_earth_orbit(), in_solar_system(), at_ssb()
 */
GeocentricObserver Observer::at_geocenter() {
  return GeocentricObserver();
}

/**
 * Returns a new observer in some Solar-system location.
 *
 * @param pos     momentary position of the observer relative to the Solar-system Barycenter
 *                (SSB).
 * @param vel     momentary velocity of the observer relative to the Solar-system Barycenter
 *                (SSB).
 * @return        a new observer instance for the given Solar-system location.
 *
 * @sa at_ssb(), at_geocenter(), on_earth(), in_earth_orbit()
 */
SolarSystemObserver Observer::in_solar_system(const Position& pos, const Velocity& vel) {
  SolarSystemObserver o = SolarSystemObserver(pos, vel);
  if(!o.is_valid())
    novas_trace_invalid("Observer::in_solar_system");
  return o;
}

/**
 * Returns a fictitious observer placed at the location of the Solar-System Barycenter (SSB).
 *
 * @return        a new fictitious observer located at the Solar-System Barycenter (SSB).
 *
 * @sa in_solar_system(), at_geocenter(), on_earth(), in_earth_orbit()
 */
SolarSystemObserver Observer::at_ssb() {
  return SolarSystemObserver();
}

/**
 * Returns a reference to a statically defined standard invalid observer. This invalid
 * observer may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid observer.
 */
const Observer &Observer::invalid() {
  static const Observer _invalid = Observer((enum novas_observer_place) -1, Site::invalid(), Position::invalid(), Velocity::invalid());
  return _invalid;
}

/**
 * Instantiates a new observer located at the geocenter.
 *
 */
GeocentricObserver::GeocentricObserver()
: Observer(NOVAS_OBSERVER_AT_GEOCENTER, Site::invalid()) {
  _valid = true;
}

/**
 * Instantiates a new observer located (and moving) relative to the geocenter.
 *
 * @param pos       momentary position of the observer relative to the geocenter.
 * @param vel       momentary velocity of the observer relative to the geocenter.
 */
GeocentricObserver::GeocentricObserver(const Position& pos, const Velocity& vel)
: Observer(NOVAS_OBSERVER_IN_EARTH_ORBIT, Site::invalid(), pos, vel) {
  static const char *fn = "GeocentricObserver()";

  if(!pos.is_valid())
    novas_set_errno(EINVAL, fn, "input position contains NAN component(s)");
  else if(!vel.is_valid())
    novas_set_errno(EINVAL, fn, "input velocity contains NAN component(s)");
  else
    _valid = true;
}

/**
 * Returns a pointer to a newly allocated copy of this geocentric observer instance.
 *
 * @return    pointer to new copy of this geocentric observer instance.
 */
const Observer *GeocentricObserver::copy() const {
  return new GeocentricObserver(*this);
}

bool GeocentricObserver::is_geocentric() const {
  return true;
}

/**
 * Returns the momentary geocentric position of this observer.
 *
 * @return    the momentary geocentric position
 *
 * @sa geocentric_velocity()
 */
Position GeocentricObserver::geocentric_position() const {
  return Position(_observer.near_earth.sc_pos, Unit::km);
}

/**
 * Returns the momentary geocentric velocity of this observer.
 *
 * @return    the momentary geocentric velocity
 *
 * @sa geocentric_position()
 */
Velocity GeocentricObserver::geocentric_velocity() const {
  return Velocity(_observer.near_earth.sc_vel, Unit::km / Unit::sec);
}

/**
 * Returns a string representation of this geocentric observer.
 *
 * @return    a string representation of this observer.
 */
std::string GeocentricObserver::to_string() const {
  Position pos = geocentric_position();
  Velocity vel = geocentric_velocity();

  std::string p = (pos.is_zero()) ? "" : " at " + pos.to_string();
  std::string v = (vel.is_zero()) ? "" : " moving at " + vel.to_string();

  return "Geocentric Observer" + p + v;
}


/**
 * Instantiates a new stationary observer located at the Solar-System Barycenter (SSB).
 *
 */
SolarSystemObserver::SolarSystemObserver()
: SolarSystemObserver(Position::origin(), Velocity::stationary()) {
  const double zero[3] = {0.0};
  make_solar_system_observer(zero, zero, &_observer);
  _valid = true;
}

/**
 * Instantiates a new observer in the Solar System.
 *
 * @param pos     momentary position of the observer relative to the Solar-system Barycenter
 *                (SSB).
 * @param vel     momentary velocity of the observer relative to the Solar-system Barycenter
 *                (SSB).
 */
SolarSystemObserver::SolarSystemObserver(const Position& pos, const Velocity& vel)
: Observer(NOVAS_SOLAR_SYSTEM_OBSERVER, Site::invalid(), pos, vel) {
  static const char *fn = "SolarSystemObserver()";

  make_solar_system_observer(pos.scaled(1.0 / Unit::au)._array(), vel.scaled(Unit::day / Unit::au)._array(), &_observer);

  if(!pos.is_valid())
    novas_set_errno(EINVAL, fn, "input position contains NAN component(s)");
  else if(!vel.is_valid())
    novas_set_errno(EINVAL, fn, "input velocity contains NAN component(s)");
  else
    _valid = true;
}

/**
 * Returns a pointer to a newly allocated copy of this observer instance at a Solar-system location.
 *
 * @return    pointer to new copy of thus Solar-system based observer instance.
 */
const Observer *SolarSystemObserver::copy() const {
  return new SolarSystemObserver(*this);
}

/**
 * Returns the momentary location of this observer relative to the Solar-System
 * Barycenter (SSB).
 *
 * @return      the momentary position of the observer relative to hte SSB.
 *
 * @sa ssb_velocity()
 */
Position SolarSystemObserver::ssb_position() const {
  return Position(_observer.near_earth.sc_pos, Unit::au);
}

/**
 * Returns the momentary velocity of this observer relative to the Solar-System
 * Barycenter (SSB).
 *
 * @return      the momentary velocity of the observer relative to hte SSB.
 *
 * @sa ssb_position()
 */
Velocity SolarSystemObserver::ssb_velocity() const {
  return Velocity(_observer.near_earth.sc_vel, Unit::au / Unit::day);
}

std::string SolarSystemObserver::to_string() const {
  Position pos = ssb_position();
  Velocity vel = ssb_velocity();

  std::string p = (pos.is_zero()) ? "SSB" : pos.to_string();
  std::string v = (vel.is_zero()) ? "" : " moving at SSB " + vel.to_string();

  return "SolarSystemObserver at " + p + v;
}


/**
 * Instantiates a new observer at a fixed location on Earth.
 *
 * @param site    the observing site
 * @param eop     Earth Orientation Parameters (EOP) appropriate around the time of observation,
 *                such as obtained from the IERS bulletins or data service.
 */
GeodeticObserver::GeodeticObserver(const Site& site, const EOP& eop)
: Observer(NOVAS_OBSERVER_ON_EARTH, site, Position::origin(), Velocity::stationary()), _eop(eop) {
  static const char *fn = "GeodeticObserver()";

  if(!site.is_valid())
    novas_set_errno(EINVAL, fn, "input site is invalid");
  else if(!eop.is_valid())
    novas_set_errno(EINVAL, fn, "input EOP is invalid");
  else
    _valid = true;
}

/**
 * Instantiates a new observer that is moving relative to Earth's surface, such as an airborne
 * observer.
 *
 * @param site    the momentary geodetic location of the observer
 * @param vel     the momentaty velocity of the observer relative to Earth's surface (in ITRS),
 * @param eop     Earth Orientation Parameters (EOP) appropriatI am salivating at the thought of it. Tell Benjamie around the time of observation.
 *
 * @sa Site::enu_to_itrf()
 */
GeodeticObserver::GeodeticObserver(const Site& site, const Velocity& vel, const EOP& eop)
: Observer(NOVAS_AIRBORNE_OBSERVER, site, Position::origin(), vel), _eop(eop) {
  static const char *fn = "GeodeticObserver()";

  make_airborne_observer(site._on_surface(), vel.scaled(Unit::s / Unit::km)._array(), &_observer);

  if(!site.is_valid())
    novas_set_errno(EINVAL, fn, "input site is invalid");
  else if(!eop.is_valid())
    novas_set_errno(EINVAL, fn, "input EOP is invalid");
  else if(!vel.is_valid())
    novas_set_errno(EINVAL, fn, "input velocity contains NAN component(s)");
  else
    _valid = true;
}

/**
 * Instantiates a new observer that is moving relative to Earth's surface, such as an airborne
 * observer.
 *
 * @param site          the momentary geodetic location of the observer.
 * @param eop           Earth Orientation Parameters (EOP) appropriate around the time of
 *                      observation.
 * @param horizontal    momentary horizontal speed of moving observer.
 * @param direction     azimuthal direction of motion (from North, measured to the East).
 * @param vertical      (optional) momentary vertical speed of observer (default: 0).
 *
 * @sa Site::enu_to_itrf()
 */
GeodeticObserver::GeodeticObserver(const Site& site, const EOP& eop, const Speed& horizontal, const Angle& direction, const Speed& vertical)
: GeodeticObserver(site, Velocity::stationary(), eop) {
  static const char *fn = "GeodeticObserver()";

  if(_valid)
    errno = 0;

  if(!horizontal.is_valid())
    novas_set_errno(EINVAL, fn, "input horizontal speed is invalid");
  if(!direction.is_valid())
    novas_set_errno(EINVAL, fn, "input azimuthal direction of motion is invalid");
  if(!vertical.is_valid())
    novas_set_errno(EINVAL, fn, "input vertical speed is invalid");

  _valid &= (errno == 0);

  double v[3] = {0.0};
  v[0] = horizontal.km_per_s() * sin(direction.rad());
  v[1] = horizontal.km_per_s() * cos(direction.rad());
  v[2] = vertical.km_per_s();

  novas_enu_to_itrs(v, site.longitude().deg(), site.latitude().deg(), _observer.near_earth.sc_vel);
}

/**
 * Returns a pointer to a newly allocated copy of this geodetic (Earth-based) observer instance.
 *
 * @return    pointer to new copy of this geodetic (Earth-based) observer instance.
 */
const Observer *GeodeticObserver::copy() const {
  return new GeodeticObserver(*this);
}


bool GeodeticObserver::is_geodetic() const { return true; }

/**
 * Returns the fixed or momentary observing site for this observer.
 *
 * @return    the observing site (fixed or momentary).
 *
 * @sa velocity()
 */
Site GeodeticObserver::site() const {
  const on_surface *s = &_observer.on_surf;
  return Site(s->longitude * Unit::deg, s->latitude * Unit::deg, s->height);
}

/**
 * Returns the surface velocity of a moving observer, such as an airborne or balloon borne
 * observatory.
 *
 * @return    the momentary ITRS surface velocity vector of the moving observer.
 *
 * @sa Site::itrs_to_enu()
 */
Velocity GeodeticObserver::itrs_velocity() const {
  return Velocity(_observer.near_earth.sc_vel, Unit::km / Unit::s);
}


/**
 * Returns the surface velocity of a moving observer, such as an airborne or balloon borne
 * observatory.
 *
 * @return    the momentary surface velocity vector of the moving observer in the East-North-Up
 *            (ENU) directions at the current location.
 *
 * @sa Site::itrs_to_enu()
 */
Velocity GeodeticObserver::enu_velocity() const {
  double v[3] = {0.0};
  novas_itrs_to_enu(_observer.near_earth.sc_vel, _observer.on_surf.longitude, _observer.on_surf.latitude, v);
  return Velocity(v, Unit::km / Unit::s);
}


/**
 * Earth Orientation Parameters (EOP) appropriate around the time of observation.
 *
 * @return    the Earth Orientation Parameters (EOP) defined for this observer.
 */
const EOP& GeodeticObserver::eop() const {
  return _eop;
}

/**
 * Returns a string representation of this Earth-based observer location.
 *
 * @return    a new string that describes this observer.
 */
std::string GeodeticObserver::to_string() const {
  Velocity vel = enu_velocity();

  std::string v = (vel.is_zero()) ? "" : " moving at ENU " + vel.to_string();

  return "GeodeticObserver at " + site().to_string() + v;
}


} // namespace supernovas

