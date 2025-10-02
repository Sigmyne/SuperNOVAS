/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace supernovas;


const observer * Observer::_novas_observer() const {
  return &_observer;
}

enum novas_observer_place Observer::type() const {
  return _observer.where;
}

bool Observer::is_geodetic() const {
  return false;
}

GeodeticObserver Observer::on_earth(const Site& site, const EOP& eop) {
  return GeodeticObserver(site, eop);
}

GeodeticObserver Observer::on_earth(const Site& geodetic, const Velocity& vel, const EOP& eop) {
  return GeodeticObserver(geodetic, vel, eop);
}

Observer Observer::in_earth_orbit(const Position& pos, const Velocity& vel) {
  Observer obs = Observer();
  make_observer_in_space(pos.scaled(1.0 / Unit::km)._array(), vel.scaled(Unit::sec / Unit::km)._array(), &obs._observer);
  return obs;
}

Observer Observer::in_solar_system(const Position& pos, const Velocity& vel) {
  Observer obs = Observer();
  make_solar_system_observer(pos.scaled(1.0 / Unit::au)._array(), vel.scaled(Unit::day / Unit::au)._array(), &obs._observer);
  return obs;
}

Observer Observer::at_geocenter() {
  Observer obs = Observer();
  make_observer_at_geocenter(&obs._observer);
  return obs;
}

Observer Observer::at_ssb() {
  double zero[3] = {0.0};
  Observer obs = Observer();
  make_solar_system_observer(zero, zero, &obs._observer);
  return obs;
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



