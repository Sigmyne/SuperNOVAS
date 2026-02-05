/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

using namespace novas;

int main() {
  TestUtil test = TestUtil("Observer");

  int n = 0;


  EOP eop(37, 0.15, 0.2 * Unit::arcsec, -0.3 * Unit::arcsec);
  Site site(Angle(-2.0), Angle(1.0), Distance(75.0));

  if(!test.check("invalid Site", !Observer::on_earth(Site(NAN, NAN, NAN), eop).is_valid())) n++;
  if(!test.check("invalid Velocity", !Observer::on_earth(site, Velocity::invalid(), eop).is_valid())) n++;
  if(!test.check("invalid EOP", !Observer::on_earth(site, EOP::invalid()).is_valid())) n++;

  if(!test.check("invalid orbit Position", !Observer::in_earth_orbit(Position::invalid(), Velocity::stationary()).is_valid())) n++;
  if(!test.check("invalid orbit Velocity", !Observer::in_earth_orbit(Position::origin(), Velocity::invalid()).is_valid())) n++;

  if(!test.check("invalid ssb Position", !Observer::in_solar_system(Position::invalid(), Velocity::stationary()).is_valid())) n++;
  if(!test.check("invalid ssb Velocity", !Observer::in_solar_system(Position::origin(), Velocity::invalid()).is_valid())) n++;

  GeodeticObserver g1 = Observer::on_earth(site, eop);
  if(!test.check("is_valid(on_earth)", g1.is_valid())) n++;
  if(!test.equals("type(on_earth)", g1.type(), NOVAS_OBSERVER_ON_EARTH)) n++;
  if(!test.check("is_geodetic(on_earth)", g1.is_geodetic())) n++;
  if(!test.check("is_geocentric(on_earth)", !g1.is_geocentric())) n++;
  if(!test.check("site()", g1.site() == site)) n++;
  if(!test.check("velocity()", g1.itrs_velocity() == Velocity::stationary())) n++;
  if(!test.check("site()", g1.eop() == eop)) n++;

  Velocity v1 = Velocity(1.0, -2.0, 3.0);
  GeodeticObserver g2 = Observer::on_earth(site, v1, eop);
  if(!test.equals("type(moving)", g2.type(), NOVAS_AIRBORNE_OBSERVER)) n++;
  if(!test.check("itrs_velocity(moving)", g2.itrs_velocity() == v1)) n++;
  
  GeocentricObserver gc = Observer::at_geocenter();
  if(!test.check("is_valid(gc)", gc.is_valid())) n++;
  if(!test.equals("type(gc)", gc.type(), NOVAS_OBSERVER_AT_GEOCENTER)) n++;
  if(!test.check("is_geocentric(gc)", gc.is_geocentric())) n++;
  if(!test.check("is_geodetic(gc)", !gc.is_geodetic())) n++;
  if(!test.check("geocentric_position(gc)", gc.geocentric_position() == Position::origin())) n++;
  if(!test.check("geocentric_velocity(gc)", gc.geocentric_velocity() == Velocity::stationary())) n++;

  Position p1(10000.0 * Unit::km, 0.0, 0.0);
  GeocentricObserver o1 = Observer::in_earth_orbit(p1, v1);
  if(!test.check("is_valid(orbit)", o1.is_valid())) n++;
  if(!test.equals("type(orbit)", o1.type(), NOVAS_OBSERVER_IN_EARTH_ORBIT)) n++;
  if(!test.check("is_geocentric(orbit)", o1.is_geocentric())) n++;
  if(!test.check("is_geodetic(orbit)", !o1.is_geodetic())) n++;
  if(!test.check("geocentric_position(orbit)", o1.geocentric_position() == p1)) n++;
  if(!test.check("geocentric_velocity(orbit)", o1.geocentric_velocity() == v1)) n++;

  SolarSystemObserver ssb = Observer::at_ssb();
  if(!test.check("is_valid(ssb)", ssb.is_valid())) n++;
  if(!test.equals("type(ssb)", ssb.type(), NOVAS_SOLAR_SYSTEM_OBSERVER)) n++;
  if(!test.check("is_geocentric(ssb)", !ssb.is_geocentric())) n++;
  if(!test.check("is_geodetic(ssb)", !ssb.is_geodetic())) n++;

  Position p2(-1.1 * Unit::au, 2.2 * Unit::au, -3.3 * Unit::au);
  Velocity v2(1.0 * Unit::au / Unit::day, -2.0 * Unit::au / Unit::day, 3.0 * Unit::au / Unit::day);

  SolarSystemObserver s1 = Observer::in_solar_system(p2, v2);
  if(!test.check("is_valid(ss)", s1.is_valid())) n++;
  if(!test.equals("type(ss)", s1.type(), NOVAS_SOLAR_SYSTEM_OBSERVER)) n++;
  if(!test.check("is_geocentric(ss)", !s1.is_geocentric())) n++;
  if(!test.check("is_geodetic(ss)", !s1.is_geodetic())) n++;
  if(!test.check("ssb_position(ss)", s1.ssb_position() == p2)) n++;
  if(!test.check("ssb_velocity(ss)", s1.ssb_velocity() == v2)) n++;


  std::cout << "Observer.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
