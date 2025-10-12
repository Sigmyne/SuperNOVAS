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


const observer * Observer::_novas_observer() const {
  return &_observer;
}

enum novas_observer_place Observer::type() const {
  return _observer.where;
}

std::string Observer::to_string() const {
  return "Observer type=%d" + std::to_string(_observer.where);
}

GeodeticObserver Observer::on_earth(const Site& site, const EOP& eop) {
  return GeodeticObserver(site, eop);
}

GeodeticObserver Observer::on_earth(const Site& geodetic, const Velocity& vel, const EOP& eop) {
  return GeodeticObserver(geodetic, vel, eop);
}

GeocentricObserver Observer::in_earth_orbit(const Position& pos, const Velocity& vel) {
  return GeocentricObserver(pos, vel);
}

GeocentricObserver Observer::at_geocenter() {
  return GeocentricObserver();
}

SolarSystemObserver Observer::in_solar_system(const Position& pos, const Velocity& vel) {
  return SolarSystemObserver(pos, vel);
}

SolarSystemObserver Observer::at_ssb() {
  return SolarSystemObserver();
}

static Observer _invalid = GeocentricObserver(Position::invalid(), Velocity::invalid());

const Observer& Observer::invalid() {
  return _invalid;
}


GeocentricObserver::GeocentricObserver()
: Observer() {
  make_observer_at_geocenter(&_observer);
  _valid = true;
}

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

Position GeocentricObserver::geocentric_position() const {
  return Position(_observer.near_earth.sc_pos, Unit::km);
}

Velocity GeocentricObserver::geocentric_velocity() const {
  return Velocity(_observer.near_earth.sc_vel, Unit::km / Unit::sec);
}

std::string GeocentricObserver::to_string() const {
  return "Geocentric Observer";
}




SolarSystemObserver::SolarSystemObserver() : Observer() {
  double zero[3] = {};
  make_solar_system_observer(zero, zero, &_observer);
  _valid = true;
}

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

Position SolarSystemObserver::ssb_position() const {
  return Position(_observer.near_earth.sc_pos, Unit::au);
}

Velocity SolarSystemObserver::ssb_velocity() const {
  return Velocity(_observer.near_earth.sc_vel, Unit::au / Unit::day);
}


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

Site GeodeticObserver::site() const {
  const on_surface *s = &_observer.on_surf;
  return Site(s->longitude * Unit::deg, s->latitude * Unit::deg, s->height);
}

const EOP& GeodeticObserver::eop() const {
  return _eop;
}

std::string GeodeticObserver::to_string() const {
  return "Geodetic Observer " + site().to_string();
}


} // namespace supernovas

