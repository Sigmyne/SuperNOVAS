/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Galactic");

  int n = 0;

  if(!test.check("invalid lon", !Galactic(NAN, 30.0 * Unit::deg, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid lat", !Galactic(45.0 * Unit::deg, NAN, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid lat > 90", !Galactic(45.0 * Unit::deg, 91.0 * Unit::deg, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid dis", !Galactic(45.0 * Unit::deg, 30.0 * Unit::deg, NAN).is_valid())) n++;
  if(!test.check("invalid dis < 0", !Galactic(45.0 * Unit::deg, 30.0 * Unit::deg, -1.0).is_valid())) n++;

  Galactic x = Galactic::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("longitude() invalid", isnan(x.longitude().rad()))) n++;
  if(!test.check("latitude() invalid", isnan(x.latitude().rad()))) n++;
  if(!test.check("distance() invalid", isnan(x.distance().m()))) n++;

  Galactic a(45.0 * Unit::deg, 30.0 * Unit::deg, 10.0 * Unit::au);
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("longitude()", a.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", a.latitude().deg(), 30.0, 1e-14)) n++;
  if(!test.equals("distance()", a.distance().au(), 10.0)) n++;

  Galactic b(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  if(!test.check("is_valid()", b.is_valid())) n++;
  if(!test.equals("longitude()", b.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", b.latitude().deg(), 30.0, 1e-14)) n++;
  if(!test.equals("distance()", b.distance().au(), 10.0)) n++;

  Position xyz = a.xyz();
  if(!test.equals("xyz().x()", xyz.x(), 10.0 * Unit::au * cos(a.latitude().rad()) * cos(a.longitude().rad()))) n++;
  if(!test.equals("xyz().y()", xyz.y(), 10.0 * Unit::au * cos(a.latitude().rad()) * sin(a.longitude().rad()))) n++;
  if(!test.equals("xyz().z()", xyz.z(), 10.0 * Unit::au * sin(a.latitude().rad()))) n++;

  a.to_string();

  std::cout << "Galactic.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
