/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "TestUtil.hpp"


int main() {
  TestUtil test = TestUtil("Velocity");

  int n = 0;

  Velocity x = Velocity::undefined();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("x() invalid", !x.x().is_valid())) n++;
  if(!test.check("y() invalid", !x.y().is_valid())) n++;
  if(!test.check("z() invalid", !x.z().is_valid())) n++;
  if(!test.check("inv() invalid", !x.inv().is_valid())) n++;
  if(!test.check("speed() invalid", !x.speed().is_valid())) n++;
  if(!test.check("travel() invalid", !x.travel(Interval::zero()).is_valid())) n++;
  if(!test.check("operator*() invalid", !(x * Interval::zero()).is_valid())) n++;
  if(!test.check("invalid.operator+()", !(x + Velocity::stationary()).is_valid())) n++;
  if(!test.check("invalid.operator-()", !(x - Velocity::stationary()).is_valid())) n++;

  if(!test.check("invalid x", !Velocity(NAN, 0.0, 0.0).is_valid())) n++;
  if(!test.check("invalid x", !Velocity(0.0, NAN, 0.0).is_valid())) n++;
  if(!test.check("invalid x", !Velocity(0.0, 0.0, NAN).is_valid())) n++;

  if(!test.check("is_valid(> c) invalid", !Velocity(Constant::c + 1.0, 0.0, 0.0).is_valid())) n++;

  Velocity z = Velocity::stationary();
  if(!test.check("is_valid() stationary", z.is_valid())) n++;
  if(!test.equals("x() stationary", z.x().m_per_s(), 0.0)) n++;
  if(!test.equals("y() stationary", z.y().m_per_s(), 0.0)) n++;
  if(!test.equals("z() statiunary", z.z().m_per_s(), 0.0)) n++;
  if(!test.check("is_zero(stationary)", z.is_zero())) n++;
  if(!test.check("operator+(invalid)", !(z + x).is_valid())) n++;
  if(!test.check("operator-(invalid)", !(z - x).is_valid())) n++;
  if(!test.check("travel(invalid interval)", !x.travel(Interval(NAN)).is_valid())) n++;

  Velocity a(-1.0 * Unit::km / Unit::s, 2.0 * Unit::km / Unit::s, -3.0 * Unit::km / Unit::s);
  if(!test.check("is_valid(-1 km/s, 2 km/s, -3 km/s)", a.is_valid())) n++;
  if(!test.equals("x()", a.x().km_per_s(), -1.0)) n++;
  if(!test.equals("y()", a.y().km_per_s(), 2.0)) n++;
  if(!test.equals("z()", a.z().km_per_s(), -3.0)) n++;
  if(!test.check("is_zero()", !a.is_zero())) n++;
  if(!test.equals("speed()", a.speed().km_per_s(), sqrt(14.0), 1e-14)) n++;
  if(!test.equals("travel()", a.travel(Interval(2.0)).distance().km(), 2.0 * sqrt(14.0), 1e-14)) n++;
  if(!test.equals("operator*(Interval)", (a * Interval(3.0)).distance().km(), 3.0 * sqrt(14.0), 1e-14)) n++;
  if(!test.equals("to_string()", a.to_string(), "Velocity (-1.000 km/s, 2.000 km/s, -3.000 km/s)")) n++;

  Velocity ai = a.inv();
  if(!test.equals("x() inv", ai.x().m_per_s(), -a.x().m_per_s())) n++;
  if(!test.equals("y() inv", ai.y().m_per_s(), -a.y().m_per_s())) n++;
  if(!test.equals("z() inv", ai.z().m_per_s(), -a.z().m_per_s())) n++;

  if(!test.equals("[0]", a[0], -1.0 * Unit::km / Unit::s)) n++;
  if(!test.equals("[1]", a[1], 2.0 * Unit::km / Unit::s)) n++;
  if(!test.equals("[2]", a[2], -3.0 * Unit::km / Unit::s)) n++;

  double p[3] = {-1.0, 2.0, -3.0};
  Velocity b(p, Unit::km / Unit::s);

  if(!test.check("equals()", a.equals(b, 1e-15 * Unit::km / Unit::s))) n++;
  if(!test.check("!equals()", !a.equals(ai, 1e-15 * Unit::km / Unit::s))) n++;

  if(!test.check("operator ==", a == b)) n++;
  if(!test.check("operator !=", a != ai)) n++;

  if(!test.equals("projection_on(x)", a.projection_on(Position(5.0, 0.0, 0.0)), a.x().m_per_s(), 1e-18)) n++;
  if(!test.equals("projection_on(y)", a.projection_on(Position(0.0, 5.0, 0.0)), a.y().m_per_s(), 1e-18)) n++;
  if(!test.equals("projection_on(z)", a.projection_on(Position(0.0, 0.0, 5.0)), a.z().m_per_s(), 1e-18)) n++;

  double l = a.abs();
  if(!test.equals("unit_vector().x()", a.unit_vector()[0], a.x().m_per_s() / l, 1e-15)) n++;
  if(!test.equals("unit_vector().y()", a.unit_vector()[1], a.y().m_per_s() / l, 1e-15)) n++;
  if(!test.equals("unit_vector().z()", a.unit_vector()[2], a.z().m_per_s() / l, 1e-15)) n++;

  if(!test.equals("x(a - b)", (a - b).x().m_per_s(), 0.0)) n++;
  if(!test.equals("y(a - b)", (a - b).y().m_per_s(), 0.0)) n++;
  if(!test.equals("z(a - b)", (a - b).z().m_per_s(), 0.0)) n++;


  if(!test.equals("x(a + b)", (a + b).x().au_per_day(), novas_add_vel(a.x().au_per_day(), b.x().au_per_day()), 1e-15)) n++;
  if(!test.equals("y(a + b)", (a + b).y().au_per_day(), novas_add_vel(a.y().au_per_day(), b.y().au_per_day()), 1e-15)) n++;
  if(!test.equals("z(a + b)", (a + b).z().au_per_day(), novas_add_vel(a.z().au_per_day(), b.z().au_per_day()), 1e-15)) n++;

  if(!test.equals("x(2 * a)", (2 * a)[0], -2.0 * Unit::km / Unit::s, 1e-14 * Unit::km / Unit::s)) n++;
  if(!test.equals("y(2 * a)", (2 * a)[1], 4.0 * Unit::km / Unit::s, 1e-14 * Unit::km / Unit::s)) n++;
  if(!test.equals("z(2 * a)", (2 * a)[2], -6.0 * Unit::km / Unit::s, 1e-14 * Unit::km / Unit::s)) n++;

  if(!test.equals("dot(b)", a.dot(b), a.abs() * b.abs(), 1e-15 * a.abs() * b.abs())) n++;

  std::cout << "Velocity.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
