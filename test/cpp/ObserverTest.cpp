/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>
#include <string.h>

#include "TestUtil.hpp"

using namespace novas;


int main() {
  TestUtil test = TestUtil("Observer");

  int n = 0;


  EOP eop(37, 0.15, 0.2 * Unit::arcsec, -0.3 * Unit::arcsec);
  Site site(Angle(-2.0), Angle(1.0), Distance(75.0));

  Observer x = Observer::invalid();
  const Observer *copy;

  if(!test.check("invalid()", !x.is_valid())) n++;
  if(!test.equals("invalid().type", (int) x.type(), -1)) n++;

  copy = x.copy();
  if(!test.check("invalid().copy()", !copy->is_valid())) n++;
  delete copy;

  if(!test.check("invalid Site", !Observer::on_earth(Site::invalid(), eop).is_valid())) n++;
  if(!test.check("invalid Site (moving)", !Observer::moving_on_earth(Site::invalid(), Velocity::stationary(), eop).is_valid())) n++;
  if(!test.check("invalid Site (moving ENU)", !Observer::moving_on_earth(Site::invalid(), eop, Speed(1.0), Angle(0.0)).is_valid())) n++;
  if(!test.check("invalid Velocity", !Observer::moving_on_earth(site, Velocity::invalid(), eop).is_valid())) n++;
  if(!test.check("invalid speed", !Observer::moving_on_earth(site, eop, Speed(NAN), Angle(0.0)).is_valid())) n++;
  if(!test.check("invalid direction", !Observer::moving_on_earth(site, eop, Speed(1.0), Angle(NAN)).is_valid())) n++;
  if(!test.check("invalid vertial speed", !Observer::moving_on_earth(site, eop, Speed(1.0), Angle(0.0), Speed(NAN)).is_valid())) n++;
  if(!test.check("invalid EOP", !Observer::on_earth(site, EOP::invalid()).is_valid())) n++;
  if(!test.check("invalid EOP (moving)", !Observer::moving_on_earth(site, Velocity::stationary(), EOP::invalid()).is_valid())) n++;

  if(!test.check("invalid orbit Position", !Observer::in_earth_orbit(Position::invalid(), Velocity::stationary()).is_valid())) n++;
  if(!test.check("invalid orbit Velocity", !Observer::in_earth_orbit(Position::origin(), Velocity::invalid()).is_valid())) n++;

  if(!test.check("invalid ssb Position", !Observer::in_solar_system(Position::invalid(), Velocity::stationary()).is_valid())) n++;
  if(!test.check("invalid ssb Velocity", !Observer::in_solar_system(Position::origin(), Velocity::invalid()).is_valid())) n++;

  if(!test.equals("to_string(base)", Observer::invalid().to_string(),
          "Observer type -1")) n++;

  GeodeticObserver g1 = Observer::on_earth(site, eop);
  if(!test.check("is_valid(on_earth)", g1.is_valid())) n++;
  if(!test.equals("type(on_earth)", g1.type(), NOVAS_OBSERVER_ON_EARTH)) n++;
  if(!test.check("is_geodetic(on_earth)", g1.is_geodetic())) n++;
  if(!test.check("is_geocentric(on_earth)", !g1.is_geocentric())) n++;
  if(!test.check("site()", g1.site() == site)) n++;
  if(!test.check("velocity()", g1.itrs_velocity() == Velocity::stationary())) n++;
  if(!test.check("site()", g1.eop() == eop)) n++;
  if(!test.equals("to_string(on_earth)", g1.to_string(),
          "GeodeticObserver at Site (W 114d 35m 29.612s, N  57d 17m 44.806s, altitude 75 m)")) n++;

  copy = g1.copy();
  if(!test.check("copy(on_earth)", memcmp(copy->_novas_observer(), g1._novas_observer(), sizeof(observer)) == 0)) n++;
  delete copy;

  const observer *o = g1._novas_observer();
  if(!test.check("_novas_observer(on_earth)", o != NULL && o->where == NOVAS_OBSERVER_ON_EARTH)) n++;

  Velocity v1 = Velocity(1.0, -2.0, 3.0);
  GeodeticObserver g2 = Observer::moving_on_earth(site, v1, eop);
  if(!test.equals("type(moving)", g2.type(), NOVAS_AIRBORNE_OBSERVER)) n++;
  if(!test.check("itrs_velocity(moving)", g2.itrs_velocity() == v1)) n++;
  if(!test.equals("to_string(moving)", g2.to_string(),
          "GeodeticObserver at Site (W 114d 35m 29.612s, N  57d 17m 44.806s, altitude 75 m) moving at ENU Velocity (0.002 km/s, 0.000 km/s, 0.003 km/s)")) n++;

  copy = g2.copy();
  if(!test.check("copy(moving)", memcmp(copy->_novas_observer(), g2._novas_observer(), sizeof(observer)) == 0)) n++;
  delete copy;

  double v_enu[3] = {1.0, -2.0, 3.0}, v_itrs[3] = {0.0};
  novas_enu_to_itrs(v_enu, site.longitude().deg(), site.latitude().deg(), v_itrs);
  GeodeticObserver g3 = Observer::moving_on_earth(site, eop, Speed(hypot(v_enu[0], v_enu[1]) * Unit::km / Unit::s), Angle(atan2(v_enu[0], v_enu[1])), Speed(3.0 * Unit::km / Unit::s));
  if(!test.equals("type(moving ENU)", g3.type(), NOVAS_AIRBORNE_OBSERVER)) n++;
  if(!test.check("enu_velocity(moving ENU)", g3.enu_velocity() == Velocity(v_enu, Unit::km / Unit::s))) n++;
  if(!test.check("itrs_velocity(moving ENU)", g3.itrs_velocity() == Velocity(v_itrs, Unit::km / Unit::s))) n++;
  if(!test.equals("to_string(moving ENU)", g3.to_string(),
          "GeodeticObserver at Site (W 114d 35m 29.612s, N  57d 17m 44.806s, altitude 75 m) moving at ENU Velocity (1.000 km/s, -2.000 km/s, 3.000 km/s)")) n++;

  GeocentricObserver gc = Observer::at_geocenter();
  if(!test.check("is_valid(gc)", gc.is_valid())) n++;
  if(!test.equals("type(gc)", gc.type(), NOVAS_OBSERVER_AT_GEOCENTER)) n++;
  if(!test.check("is_geocentric(gc)", gc.is_geocentric())) n++;
  if(!test.check("is_geodetic(gc)", !gc.is_geodetic())) n++;
  if(!test.check("geocentric_position(gc)", gc.geocentric_position() == Position::origin())) n++;
  if(!test.check("geocentric_velocity(gc)", gc.geocentric_velocity() == Velocity::stationary())) n++;
  if(!test.equals("to_string(gc)", gc.to_string(), "Geocentric Observer")) n++;


  copy = gc.copy();
  if(!test.check("copy(gc)", memcmp(copy->_novas_observer(), gc._novas_observer(), sizeof(observer)) == 0)) n++;
  delete copy;

  o = gc._novas_observer();
  if(!test.check("_novas_observer(gc)", o != NULL && o->where == NOVAS_OBSERVER_AT_GEOCENTER)) n++;

  Position p1(10000.0 * Unit::km, 0.0, 0.0);
  GeocentricObserver o1 = Observer::in_earth_orbit(p1, v1);
  if(!test.check("is_valid(orbit)", o1.is_valid())) n++;
  if(!test.equals("type(orbit)", o1.type(), NOVAS_OBSERVER_IN_EARTH_ORBIT)) n++;
  if(!test.check("is_geocentric(orbit)", o1.is_geocentric())) n++;
  if(!test.check("is_geodetic(orbit)", !o1.is_geodetic())) n++;
  if(!test.check("geocentric_position(orbit)", o1.geocentric_position() == p1)) n++;
  if(!test.check("geocentric_velocity(orbit)", o1.geocentric_velocity() == v1)) n++;
  if(!test.equals("to_string(orbit)", o1.to_string(), "Geocentric Observer at Position (10000.000 km, 0.000 m, 0.000 m) moving at Velocity (0.001 km/s, -0.002 km/s, 0.003 km/s)")) n++;

  o = o1._novas_observer();
  if(!test.check("_novas_observer(orbit)", o != NULL && o->where == NOVAS_OBSERVER_IN_EARTH_ORBIT)) n++;

  copy = o1.copy();
  if(!test.check("copy(orbit)", memcmp(copy->_novas_observer(), o1._novas_observer(), sizeof(observer)) == 0)) n++;
  delete copy;

  SolarSystemObserver ssb = Observer::at_ssb();
  if(!test.check("is_valid(ssb)", ssb.is_valid())) n++;
  if(!test.equals("type(ssb)", ssb.type(), NOVAS_SOLAR_SYSTEM_OBSERVER)) n++;
  if(!test.check("is_geocentric(ssb)", !ssb.is_geocentric())) n++;
  if(!test.check("is_geodetic(ssb)", !ssb.is_geodetic())) n++;
  if(!test.equals("to_string(ssb)", ssb.to_string(), "SolarSystemObserver at SSB")) n++;

  o = ssb._novas_observer();
  if(!test.check("_novas_observer(ssb)", o != NULL && o->where == NOVAS_SOLAR_SYSTEM_OBSERVER)) n++;

  Position p2(-1.1 * Unit::au, 2.2 * Unit::au, -3.3 * Unit::au);
  Velocity v2(1.0 * Unit::au / Unit::day, -2.0 * Unit::au / Unit::day, 3.0 * Unit::au / Unit::day);

  SolarSystemObserver s1 = Observer::in_solar_system(p2, v2);
  if(!test.check("is_valid(ss)", s1.is_valid())) n++;
  if(!test.equals("type(ss)", s1.type(), NOVAS_SOLAR_SYSTEM_OBSERVER)) n++;
  if(!test.check("is_geocentric(ss)", !s1.is_geocentric())) n++;
  if(!test.check("is_geodetic(ss)", !s1.is_geodetic())) n++;
  if(!test.check("ssb_position(ss)", s1.ssb_position() == p2)) n++;
  if(!test.check("ssb_velocity(ss)", s1.ssb_velocity() == v2)) n++;
  if(!test.equals("to_string(ss)", s1.to_string(),
          "SolarSystemObserver at Position (-1.100 AU, 2.200 AU, -3.300 AU) moving at SSB Velocity (1731.457 km/s, -3462.914 km/s, 5194.371 km/s)")) n++;

  o = s1._novas_observer();
  if(!test.check("_novas_observer(ss)", o != NULL && o->where == NOVAS_SOLAR_SYSTEM_OBSERVER)) n++;

  copy = s1.copy();
  if(!test.check("copy(ss)", memcmp(copy->_novas_observer(), s1._novas_observer(), sizeof(observer)) == 0)) n++;
  delete copy;

  std::cout << "Observer.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
