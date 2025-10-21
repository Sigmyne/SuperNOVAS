/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Ecliptic");

  int n = 0;

  if(!test.check("invalid lon", !Ecliptic::icrs(NAN, 30.0 * Unit::deg, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid lat", !Ecliptic::icrs(45.0 * Unit::deg, NAN, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid lat > 90", !Ecliptic::icrs(45.0 * Unit::deg, 91.0 * Unit::deg, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid dis", !Ecliptic::icrs(45.0 * Unit::deg, 30.0 * Unit::deg, NAN).is_valid())) n++;
  if(!test.check("invalid dis < 0", !Ecliptic::icrs(45.0 * Unit::deg, 30.0 * Unit::deg, -1.0).is_valid())) n++;

  Ecliptic x = Ecliptic::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("longitude() invalid", isnan(x.longitude().rad()))) n++;
  if(!test.check("latitude() invalid", isnan(x.latitude().rad()))) n++;
  if(!test.check("distance() invalid", isnan(x.distance().m()))) n++;

  Ecliptic a = Ecliptic::icrs(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("longitude()", a.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", a.latitude().deg(), 30.0, 1e-14)) n++;
  if(!test.equals("distance()", a.distance().au(), 10.0)) n++;

  Ecliptic b = Ecliptic::j2000(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  Ecliptic b1 =  b.to_icrs().to_j2000();
  if(!test.equals("to_icrs().to_j2000().longitude()", b1.longitude().deg(), b.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_j2000().latitude()", b1.latitude().deg(), b.latitude().deg(), 1e-12)) n++;

  Ecliptic c = Ecliptic::mod(Time::b1950(), Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  Ecliptic c1 =  c.to_icrs().to_mod(Time::b1950());
  if(!test.equals("to_icrs().to_mod().longitude()", c1.longitude().deg(), c.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_mod().latitude()", c1.latitude().deg(), c.latitude().deg(), 1e-12)) n++;

  Ecliptic d = Ecliptic::tod(Time::b1950(), Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  Ecliptic d1 =  d.to_icrs().to_tod(Time::b1950());
  if(!test.equals("to_icrs().to_tod().longitude()", d1.longitude().deg(), d.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_tod().latitude()", d1.latitude().deg(), d.latitude().deg(), 1e-12)) n++;

  Position xyz = a.xyz();
  if(!test.equals("xyz().x()", xyz.x(), 10.0 * Unit::au * cos(a.latitude().rad()) * cos(a.longitude().rad()))) n++;
  if(!test.equals("xyz().y()", xyz.y(), 10.0 * Unit::au * cos(a.latitude().rad()) * sin(a.longitude().rad()))) n++;
  if(!test.equals("xyz().z()", xyz.z(), 10.0 * Unit::au * sin(a.latitude().rad()))) n++;

  a.to_string();

  std::cout << "Ecliptic.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
