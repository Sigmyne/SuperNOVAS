/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Observer {
protected:
  observer _observer = {};

  Observer() {}

public:

  const observer * _novas_observer() const {
    return &_observer;
  }

  enum novas_observer_place type() const {
    return _observer.where;
  }

  static EarthBasedObserver on_earth(const Site& site) {
    return EarthBasedObserver(site);
  }

  static EarthBasedObserver on_earth(const Site& site, const Velocity& vel) {
    return EarthBasedObserver(site, vel);
  }

  static Observer in_earth_orbit(const Position& pos, const Velocity& vel) {
    Observer obs = Observer();
    make_observer_in_space(pos.scaled(1.0 / Unit::km)._array(), vel.scaled(Unit::sec / Unit::km)._array(), &obs._observer);
    return obs;
  }

  static Observer solar_system(const Position& pos, const Velocity& vel) {
    Observer obs = Observer();
    make_solar_system_observer(pos.scaled(1.0 / Unit::au)._array(), vel.scaled(Unit::day / Unit::au)._array(), &obs._observer);
    return obs;
  }

  static Observer at_geocenter() {
    Observer obs = Observer();
    make_observer_at_geocenter(&obs._observer);
    return obs;
  }

  static Observer at_ssb() {
    double zero[3] = {0.0};
    Observer obs = Observer();
    make_solar_system_observer(zero, zero, &obs._observer);
    return obs;
  }
};

class EarthBasedObserver : public Observer {
public:
  EarthBasedObserver(const Site& site)
  : Observer() {
    make_observer_at_site(site._on_surface(), &_observer);
  }

  EarthBasedObserver(const Site& site, const Velocity& vel)
  : Observer() {
    make_airborne_observer(site._on_surface(), vel._array(), &_observer);
  }

  Site site() const {
    const on_surface *s = &_observer.on_surf;
    return Site(s->longitude * Unit::deg, s->latitude * Unit::deg, s->height);
  }
};


} // namespace supernovas


