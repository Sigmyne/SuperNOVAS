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
  TestUtil test = TestUtil("Interval");

  int n = 0;

  Interval x(NAN);
  if(!test.check("is_valid(NAN)", !x.is_valid())) n++;
  if(!test.check("isnan(NAN)", isnan(x.seconds()))) n++;

  if(!test.check("is_valid(zero)", Interval::zero().is_valid())) n++;
  if(!test.equals("zero()", Interval::zero().minutes(), 0.0)) n++;

  if(!test.check("is_valid(timescale -1)", !Interval(1.0, (enum novas_timescale) -1).is_valid())) n++;

  Interval a(1.0 * Unit::min);
  if(!test.check("is_valid(1 min)", a.is_valid())) n++;
  if(!test.equals("hours()", a.hours(), 1.0 / 60.0, 1e-16)) n++;
  if(!test.equals("minutes)", a.minutes(), 1.0)) n++;
  if(!test.equals("seconds()", a.seconds(), 60.0, 1e-14)) n++;
  if(!test.equals("milliseconds()", a.milliseconds(), 60000.0, 1e-11)) n++;
  if(!test.equals("arcsec()", a.days(), Unit::min / Unit::day, 1e-18)) n++;
  if(!test.equals("weeks()", a.weeks(), Unit::min / Unit::week, 1e-19)) n++;
  if(!test.equals("years()", a.years(), Unit::min / Unit::yr, 1e-10)) n++;
  if(!test.equals("julian_years()", a.julian_years(), Unit::min / Unit::julian_year, 1e-22)) n++;
  if(!test.equals("julian_centuries()", a.julian_centuries(), Unit::min / Unit::julian_century, 1e-23)) n++;
  if(!test.equals("timescale()", (long) a.timescale(), (long) NOVAS_TT)) n++;
  if(!test.equals("operator - (tt)", (a - a).seconds(), 0.0, 1e-16)) n++;

  if(!test.equals("inv()", a.inv().minutes(), -1.0)) n++;

  if(!test.equals("operator * (Speed)", (a * Speed(1.0)).m(), 60.0, 1e-12)) n++;
  if(!test.equals("operator * (Velocity)", (a * Velocity(1.0, -1.0, 0.0)).distance().m(), sqrt(2.0) * 60.0)) n++;

  Interval b(59.0 * Unit::sec);
  if(!test.check("operator ==", a == a)) n++;
  if(!test.check("operator !=", a != b)) n++;

  if(!test.equals("operator +", (a + b).seconds(), 119.0, 1e-12)) n++;
  if(!test.equals("operator -", (a - b).seconds(), 1.0, 1e-12)) n++;

  Interval c(1000.0 * Unit::sec, NOVAS_TCG);
  if(!test.equals("tcg:seconds()", c.seconds(), 1000.0)) n++;
  if(!test.equals("tcg:to_tt:seconds()", c.to_timescale(NOVAS_TT).seconds() * (1.0 + Constant::L_G), 1000.0, 1e-16)) n++;
  if(!test.equals("operator - (tcg)", (c - c).seconds(), 0.0, 1e-16)) n++;

  Interval d(1000.0 * Unit::sec, NOVAS_TCB);
  if(!test.equals("tcb:seconds()", d.seconds(), 1000.0)) n++;
  if(!test.equals("tcb:to_tt:seconds()", d.to_timescale(NOVAS_TT).seconds() * (1.0 + Constant::L_B), 1000.0, 1e-16)) n++;
  if(!test.equals("operator - (tcb)", (d - d).seconds(), 0.0, 1e-16)) n++;

  if(!test.differs("operator - (tcb / tcg)", (d - c).seconds(), 0.0, 1e-15)) n++;

  //b.to_string(); TODO

  std::cout << "Interval.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
