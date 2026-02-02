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
  TestUtil test = TestUtil("Spherical");

  int n = 0;

  if(!test.check("invalid lon", !Spherical(NAN, 30.0 * Unit::deg).is_valid())) n++;
  if(!test.check("invalid lat", !Spherical(45.0 * Unit::deg, NAN).is_valid())) n++;
  if(!test.check("invalid lat > 90", !Spherical(45.0 * Unit::deg, 91.0 * Unit::deg).is_valid())) n++;

  Spherical a(45.0 * Unit::deg, 30.0 * Unit::deg);
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("longitude()", a.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", a.latitude().deg(), 30.0, 1e-14)) n++;

  Spherical b(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg));
  if(!test.check("is_valid()", b.is_valid())) n++;
  if(!test.equals("longitude()", b.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", b.latitude().deg(), 30.0, 1e-14)) n++;

  Spherical c("45:00 00.000", "30h 00m 00s");
  if(!test.check("is_valid()", c.is_valid())) n++;
  if(!test.equals("longitude()", c.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", c.latitude().deg(), 30.0, 1e-14)) n++;

  Position xyz = a.xyz(Distance(10.0 * Unit::au));
  if(!test.equals("xyz().x()", xyz.x(), 10.0 * Unit::au * cos(a.latitude().rad()) * cos(a.longitude().rad()))) n++;
  if(!test.equals("xyz().y()", xyz.y(), 10.0 * Unit::au * cos(a.latitude().rad()) * sin(a.longitude().rad()))) n++;
  if(!test.equals("xyz().z()", xyz.z(), 10.0 * Unit::au * sin(a.latitude().rad()))) n++;

  a.to_string();

  std::cout << "Spherical.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
