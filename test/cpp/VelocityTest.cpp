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

  Velocity x = Velocity::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("x() invalid", isnan(x.x()))) n++;
  if(!test.check("y() invalid", isnan(x.y()))) n++;
  if(!test.check("z() invalid", isnan(x.z()))) n++;

  if(!test.check("is_valid(> c) invalid", !Velocity(Constant::c + 1.0, 0.0).is_valid())) n++;

  Velocity z = Velocity::stationary();
  if(!test.check("is_valid() stationary", z.is_valid())) n++;
  if(!test.equals("x() stationary", z.x(), 0.0)) n++;
  if(!test.equals("y() stationary", z.y(), 0.0)) n++;
  if(!test.equals("z() statiunary", z.z(), 0.0)) n++;
  if(!test.check("is_zero(stationary)", z.is_zero())) n++;

  Velocity a(-1.0 * Unit::km / Unit::s, 2.0 * Unit::km / Unit::s, -3.0 * Unit::km / Unit::s);
  if(!test.check("is_valid(-1 km/s, 2 km/s, -3 km/s)", a.is_valid())) n++;
  if(!test.equals("x()", a.x(), -1.0 * Unit::km / Unit::s)) n++;
  if(!test.equals("y()", a.y(), 2.0 * Unit::km / Unit::s)) n++;
  if(!test.equals("z()", a.z(), -3.0 * Unit::km / Unit::s)) n++;
  if(!test.check("is_zero()", !a.is_zero())) n++;
  if(!test.equals("speed()", a.speed().km_per_s(), sqrt(14.0), 1e-14)) n++;
  if(!test.equals("travel()", a.travel(Interval(2.0)).distance().km(), 2.0 * sqrt(14.0), 1e-14)) n++;
  if(!test.equals("operator*(Interval)", (a * Interval(3.0)).distance().km(), 3.0 * sqrt(14.0), 1e-14)) n++;
  if(!test.equals("to_string()", a.to_string(), "Velocity (-1.000 km/s, 2.000 km/s, -3.000 km/s)")) n++;

  Velocity ai = a.inv();
  if(!test.equals("x() inv", ai.x(), -a.x())) n++;
  if(!test.equals("y() inv", ai.y(), -a.y())) n++;
  if(!test.equals("z() inv", ai.z(), -a.z())) n++;

  const double *pa = a._array();
  if(!test.equals("_array()[0]", pa[0], -1.0 * Unit::km / Unit::s)) n++;
  if(!test.equals("_array()[1]", pa[1], 2.0 * Unit::km / Unit::s)) n++;
  if(!test.equals("_array()[2]", pa[2], -3.0 * Unit::km / Unit::s)) n++;

  double p[3] = {-1.0, 2.0, -3.0};
  Velocity b(p, Unit::km / Unit::s);

  if(!test.check("equals()", a.equals(b, 1e-15 * Unit::km / Unit::s))) n++;
  if(!test.check("!equals()", !a.equals(ai, 1e-15 * Unit::km / Unit::s))) n++;

  if(!test.check("operator ==", a == b)) n++;
  if(!test.check("operator !=", a != ai)) n++;

  if(!test.equals("projection_on(x)", a.projection_on(Position(5.0, 0.0, 0.0)), a.x(), 1e-15 * Unit::km / Unit::s)) n++;
  if(!test.equals("projection_on(y)", a.projection_on(Position(0.0, 5.0, 0.0)), a.y(), 1e-15 * Unit::km / Unit::s)) n++;
  if(!test.equals("projection_on(z)", a.projection_on(Position(0.0, 0.0, 5.0)), a.z(), 1e-15 * Unit::km / Unit::s)) n++;

  double l = a.abs();
  if(!test.equals("unit_vector().x()", a.unit_vector().x(), a.x() / l, 1e-15)) n++;
  if(!test.equals("unit_vector().y()", a.unit_vector().y(), a.y() / l, 1e-15)) n++;
  if(!test.equals("unit_vector().z()", a.unit_vector().z(), a.z() / l, 1e-15)) n++;

  if(!test.equals("x(a - b)", (a - b).x(), 0.0)) n++;
  if(!test.equals("y(a - b)", (a - b).y(), 0.0)) n++;
  if(!test.equals("z(a - b)", (a - b).z(), 0.0)) n++;

  double auday = Unit::au / Unit::day;

  if(!test.equals("x(a + b)", (a + b).x() / auday, novas::novas_add_vel(a.x() / auday, b.x() / auday), 1e-15)) n++;
  if(!test.equals("y(a + b)", (a + b).y() / auday, novas::novas_add_vel(a.y() / auday, b.y() / auday), 1e-15)) n++;
  if(!test.equals("z(a + b)", (a + b).z() / auday, novas::novas_add_vel(a.z() / auday, b.z() / auday), 1e-15)) n++;

  if(!test.equals("x(2 * a)", (2 * a).x(), -2.0 * Unit::km / Unit::s, 1e-14 * Unit::km / Unit::s)) n++;
  if(!test.equals("y(2 * a)", (2 * a).y(), 4.0 * Unit::km / Unit::s, 1e-14 * Unit::km / Unit::s)) n++;
  if(!test.equals("z(2 * a)", (2 * a).z(), -6.0 * Unit::km / Unit::s, 1e-14 * Unit::km / Unit::s)) n++;

  if(!test.equals("dot(b)", a.dot(b), a.abs() * b.abs(), 1e-15 * a.abs() * b.abs())) n++;

  std::cout << "Velocity.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
