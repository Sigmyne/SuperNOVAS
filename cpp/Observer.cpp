/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace novas;


namespace supernovas {

const observer * Observer::_novas_observer() const {
  return &_observer;
}

enum novas_observer_place Observer::type() const {
  return _observer.where;
}

bool Observer::is_geodetic() const {
  return false;
}

bool Observer::is_geocentric() const {
  return false;
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



GeocentricObserver::GeocentricObserver()
: Observer() {
  make_observer_at_geocenter(&_observer);
}

GeocentricObserver::GeocentricObserver(const Position& pos, const Velocity& vel)
: Observer() {
  make_observer_in_space(pos.scaled(1.0 / Unit::km)._array(), vel.scaled(Unit::sec / Unit::km)._array(), &_observer);
}

Position GeocentricObserver::geocetric_position() const {
  return Position(_observer.near_earth.sc_pos, Unit::km);
}

Velocity GeocentricObserver::geocentric_velocity() const {
  return Velocity(_observer.near_earth.sc_vel, Unit::km / Unit::sec);
}

bool GeocentricObserver::is_geocentric() const {
  return true;
}


SolarSystemObserver::SolarSystemObserver() : Observer() {
  double zero[3] = {};
  make_solar_system_observer(zero, zero, &_observer);
}

SolarSystemObserver::SolarSystemObserver(const Position& pos, const Velocity& vel)
: Observer() {
  make_solar_system_observer(pos.scaled(1.0 / Unit::au)._array(), vel.scaled(Unit::day / Unit::au)._array(), &_observer);
}

Position SolarSystemObserver::ssb_position() const {
  return Position(_observer.near_earth.sc_pos, Unit::au);
}

Velocity SolarSystemObserver::ssb_velocity() const {
  return Velocity(_observer.near_earth.sc_vel, Unit::au / Unit::day);
}




GeodeticObserver::GeodeticObserver(const Site& site, const EOP& eop)
: Observer(), _eop(eop) {
  make_observer_at_site(site._on_surface(), &_observer);
}

GeodeticObserver::GeodeticObserver(const Site& site, const Velocity& vel, const EOP& eop)
: Observer(), _eop(eop) {
  make_airborne_observer(site._on_surface(), vel._array(), &_observer);
}

bool GeodeticObserver::is_geodetic() const {
  return true;
}

Site GeodeticObserver::site() const {
  const on_surface *s = &_observer.on_surf;
  return Site(s->longitude * Unit::deg, s->latitude * Unit::deg, s->height);
}

const EOP& GeodeticObserver::eop() const {
  return _eop;
}


} // namespace supernovas

