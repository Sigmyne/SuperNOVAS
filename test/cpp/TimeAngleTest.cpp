/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("TimeAngle");

  int n = 0;

  TimeAngle x(NAN);
  if(!test.check("is_valid(NAN)", !x.is_valid())) n++;
  if(!test.check("isnan(NAN)", isnan(x.rad()))) n++;

  TimeAngle a(45.0 * Unit::deg);
  if(!test.check("is_valid(45 deg)", a.is_valid())) n++;
  if(!test.equals("deg()", a.deg(), 45.0)) n++;
  if(!test.equals("rad()", a.rad(), M_PI / 4.0, 1e-12)) n++;
  if(!test.equals("arcmin()", a.arcmin(), 45.0 * 60.0, 1e-10)) n++;
  if(!test.equals("arcsec()", a.arcsec(), 45.0 * 3600.0, 1e-8)) n++;
  if(!test.equals("mas()", a.mas(), 45.0 * 3.6e6, 1e-5)) n++;
  if(!test.equals("uas()", a.uas(), 45.0 * 3.6e9, 1e-2)) n++;
  if(!test.equals("hours()", a.hours(), 3.0, 1e-14)) n++;
  if(!test.equals("minutes()", a.minutes(), 180.0, 1e-12)) n++;
  if(!test.equals("seconds()", a.seconds(), 3.0 * 3600.0, 1e-10)) n++;
  if(!test.equals("fraction()", a.fraction(), 1.0 / 8.0, 1e-15)) n++;

  TimeAngle b(30.0 * Unit::deg);
  if(!test.check("operator ==", a == a)) n++;
  if(!test.check("operator !=", a != b)) n++;

  if(!test.equals("operator +", (a + b).deg(), 75.0, 1e-12)) n++;
  if(!test.equals("operator -", (a - b).deg(), 15.0, 1e-12)) n++;

  TimeAngle c("11:59:59.999");
  if(!test.check("is_valid(str)", c.is_valid())) n++;
  if(!test.equals("equals(str)", c.hours(), 12.0, 1e-6)) n++;
  if(!test.equals("+ 90d", (c + Angle(90.0 * Unit::deg)).deg(), -90.0, 1e-5)) n++;
  if(!test.equals("- 90d", (c - Angle(90.0 * Unit::deg)).deg(), 90.0, 1e-5)) n++;
  if(!test.equals("+ 1h", (c + Interval(1.0 * Unit::hour)).hours(), 13.0, 1e-6)) n++;
  if(!test.equals("- 1h", (c - Interval(1.0 * Unit::hour)).hours(), 11.0, 1e-6)) n++;

  TimeAngle d("blah");
  if(!test.check("is_valid(blah)", !d.is_valid())) n++;
  if(!test.check("isnan(blah)", isnan(d.rad()))) n++;

  TimeAngle e(Angle(45.0 * Unit::deg));
  if(!test.check("copy(Angle).is_valid()", e.is_valid())) n++;
  if(!test.equals("cop(Angle)", e.deg(), 45.0)) n++;

  if(!test.equals("zero()", TimeAngle::zero().deg(), 0.0)) n++;
  if(!test.equals("noon()", TimeAngle::noon().deg(), 180.0)) n++;

  if(!test.equals("hours(x)", TimeAngle::hours(1.0).deg(), 15.0, 1e-14)) n++;
  if(!test.equals("minutes(x)", TimeAngle::minutes(1.0).arcmin(), 15.0, 1e-14)) n++;
  if(!test.equals("seconds(x)", TimeAngle::seconds(1.0).arcsec(), 15.0, 1e-14)) n++;

  c.to_string();

  std::cout << "TimeAngle.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
