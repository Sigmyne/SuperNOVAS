/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Angle");

  int n = 0;

  Angle x(NAN);
  if(!test.check("is_valid(NAN)", !x.is_valid())) n++;
  if(!test.check("isnan(NAN)", isnan(x.rad()))) n++;

  Angle a(45.0 * Unit::deg);
  if(!test.check("is_valid(45 deg)", a.is_valid())) n++;
  if(!test.equals("deg()", a.deg(), 45.0)) n++;
  if(!test.equals("rad()", a.rad(), M_PI / 4.0, 1e-12)) n++;
  if(!test.equals("arcmin()", a.arcmin(), 45.0 * 60.0, 1e-10)) n++;
  if(!test.equals("arcsec()", a.arcsec(), 45.0 * 3600.0, 1e-8)) n++;
  if(!test.equals("mas()", a.mas(), 45.0 * 3.6e6, 1e-5)) n++;
  if(!test.equals("uas()", a.uas(), 45.0 * 3.6e9, 1e-2)) n++;
  if(!test.equals("fraction()", a.fraction(), 1.0 / 8.0, 1e-15)) n++;

  Angle b(30.0 * Unit::deg);
  if(!test.check("operator ==", a == a)) n++;
  if(!test.check("operator !=", a != b)) n++;

  if(!test.equals("operator +", (a + b).deg(), 75.0, 1e-12)) n++;
  if(!test.equals("operator -", (a - b).deg(), 15.0, 1e-12)) n++;

  Angle c("-179:59:59.999");
  if(!test.check("is_valid(str)", c.is_valid())) n++;
  if(!test.equals("equals(str)", c.deg(), -180.0, 1e-6)) n++;

  Angle d("blah");
  if(!test.check("is_valid(blah)", !d.is_valid())) n++;
  if(!test.check("isnan(blah)", isnan(d.rad()))) n++;

  c.to_string();

  std::cout << "Angle.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
