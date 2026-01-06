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
 *
 * @return    a new string with a brief description of this observer.
 */
std::string Observer::to_string() const {
  return "Observer type=%d" + std::to_string(_observer.where);
}

/**
 * Returns a new observer located at a fixed observing site.
 *
 * @param site    the observing site
 * @param eop     Earth Orientation Parameters (EOP) appropriate the time of observation, such as
 *                obtained from the IERS bulletins or data service.
 * @return        a new observer instance for the given observing site.
 *
 * @sa in_earth_orbit(), in_solar_system(), at_geocenter(), at_ssb()
 */
GeodeticObserver Observer::on_earth(const Site& site, const EOP& eop) {
  return GeodeticObserver(site, eop);
}

/**
 * Returns a new observer that is moving, at some velocitym relative to Earth's surface,
 * such as an airborne aircraft or balloon based observatory.
 *
 * @param geodetic    the momentary geodetic location of the observer.
 * @param vel         the momentary velocity of the observer with respect to the surface
 *                    (in the ITRS).
 * @param eop         Earth Orientation Parameters (EOP) appropriate around the time of
 *                    observation, such as obtained from the IERS bulletins or data service.
 * @return            a new observer instance for the given moving observer.
 *
 * @sa in_earth_orbit(), in_solar_system(), at_geocenter(), at_ssb()
 */
GeodeticObserver Observer::on_earth(const Site& geodetic, const Velocity& vel, const EOP& eop) {
  return GeodeticObserver(geodetic, vel, eop);
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
  return GeocentricObserver(pos, vel);
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
  return SolarSystemObserver(pos, vel);
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

static Observer _invalid = GeocentricObserver(Position::invalid(), Velocity::invalid());

/**
 * Returns a reference to a statically defined standard invalid observer. This invalid
 * observer may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid observer.
 */
const Observer& Observer::invalid() {
  return _invalid;
}


/**
 * Instantiates a new observer located at the geocenter.
 *
 */
GeocentricObserver::GeocentricObserver()
: Observer() {
  make_observer_at_geocenter(&_observer);
  _valid = true;
}

/**
 * Instantiates a new observer located (and moving) relative to the geocenter.
 *
 * @param pos       momentary position of the observer relative to the geocenter.
 * @param vel       momentary velocity of the observer relative to the geocenter.
 */
GeocentricObserver::GeocentricObserver(const Position& pos, const Velocity& vel)
: Observer() {
  static const char *fn = "GeocentricObserver()";

  make_observer_in_space(pos.scaled(1.0 / Unit::km)._array(), vel.scaled(Unit::sec / Unit::km)._array(), &_observer);

  if(!pos.is_valid())
    novas_error(0, EINVAL, fn, "input position contains NAN component(s).");
  else if(!vel.is_valid())
    novas_error(0, EINVAL, fn, "input velocity contains NAN component(s).");
  else
    _valid = true;
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
  return "Geocentric Observer";
}


/**
 * Instantiates a new stationary observer located at the Solar-System Barycenter (SSB).
 *
 */
SolarSystemObserver::SolarSystemObserver() : Observer() {
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
: Observer() {
  static const char *fn = "SolarSystemObserver()";

  make_solar_system_observer(pos.scaled(1.0 / Unit::au)._array(), vel.scaled(Unit::day / Unit::au)._array(), &_observer);

  if(!pos.is_valid())
    novas_error(0, EINVAL, fn, "input position contains NAN component(s).");
  else if(!vel.is_valid())
    novas_error(0, EINVAL, fn, "input velocity contains NAN component(s).");
  else
    _valid = true;
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


/**
 * Instantiates a new observer at a fixed location on Earth.
 *
 * @param site    the observing site
 * @param eop     Earth Orientation Parameters (EOP) appropriate around the time of observation,
 *                such as obtained from the IERS bulletins or data service.
 */
GeodeticObserver::GeodeticObserver(const Site& site, const EOP& eop)
: Observer(), _eop(eop) {
  static const char *fn = "GeodeticObserver()";

  make_observer_at_site(site._on_surface(), &_observer);

  if(!site.is_valid())
    novas_error(0, EINVAL, fn, "input site is invalid");
  else if(!eop.is_valid())
    novas_error(0, EINVAL, fn, "input EOP is invalid");
  else
    _valid = true;
}

/**
 * Instantiates a new observer that is moving relative to Earth's surface, such as an airborne
 * observer.
 *
 * @param site    the momentary geodetic location of the observer
 * @param vel     the momentaty velocity of the observer relative to Earth's surface (in the '
 *                ITRS),
 * @param eop     Earth Orientation Parameters (EOP) appropriate around the time of observation.
 */
GeodeticObserver::GeodeticObserver(const Site& site, const Velocity& vel, const EOP& eop)
: Observer(), _eop(eop) {
  static const char *fn = "GeodeticObserver()";

  make_airborne_observer(site._on_surface(), vel._array(), &_observer);

  if(!site.is_valid())
    novas_error(0, EINVAL, fn, "input site is invalid.");
  else if(!eop.is_valid())
    novas_error(0, EINVAL, fn, "input EOP is invalid");
  else if(!vel.is_valid())
    novas_error(0, EINVAL, fn, "input velocity contains NAN component(s).");
  else
    _valid = true;
}

bool GeodeticObserver::is_geodetic() const { return true; }

/**
 * Returns the fixed or momentary observing site for this observer.
 *
 * @return    the observing site (fixed or momnetary).
 */
Site GeodeticObserver::site() const {
  const on_surface *s = &_observer.on_surf;
  return Site(s->longitude * Unit::deg, s->latitude * Unit::deg, s->height);
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
  return "Geodetic Observer " + site().to_string();
}


} // namespace supernovas

