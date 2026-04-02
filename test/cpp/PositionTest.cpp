/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Position");

  int n = 0;

  Position x = Position::undefined();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("x() invalid", !x.x().is_valid())) n++;
  if(!test.check("y() invalid", !x.y().is_valid())) n++;
  if(!test.check("z() invalid", !x.z().is_valid())) n++;
  if(!test.check("operator+() invalid", !(x + Position::origin()).is_valid())) n++;
  if(!test.check("operator-() invalid", !(x - Position::origin()).is_valid())) n++;
  if(!test.check("inv() invalid", !x.inv().is_valid())) n++;
  if(!test.check("operator/() invalid", !(x / Interval(2.0)).is_valid())) n++;

  Position z = Position::origin();
  if(!test.check("is_valid() origin", z.is_valid())) n++;
  if(!test.equals("x() origin", z.x().m(), 0.0)) n++;
  if(!test.equals("y() origin", z.y().m(), 0.0)) n++;
  if(!test.equals("z() origin", z.z().m(), 0.0)) n++;
  if(!test.check("is_zero(origin)", z.is_zero())) n++;
  if(!test.check("operator+(invalid)", !(z + x).is_valid())) n++;
  if(!test.check("operator-(invalid)", !(z - x).is_valid())) n++;

  if(!test.check("invalid x", !Position(NAN, 0.0, 0.0).is_valid())) n++;
  if(!test.check("invalid x", !Position(0.0, NAN, 0.0).is_valid())) n++;
  if(!test.check("invalid x", !Position(0.0, 0.0, NAN).is_valid())) n++;

  Position a(-1.0 * Unit::au, 2.0 * Unit::au, -3.0 * Unit::au);
  if(!test.check("is_valid(-1 AU, 2 AU, -3 AU)", a.is_valid())) n++;
  if(!test.equals("x()", a.x().m(), -1.0 * Unit::au)) n++;
  if(!test.equals("y()", a.y().m(), 2.0 * Unit::au)) n++;
  if(!test.equals("z()", a.z().m(), -3.0 * Unit::au)) n++;
  if(!test.check("is_zero()", !a.is_zero())) n++;
  if(!test.equals("distance()", a.distance().au(), sqrt(14.0), 1e-14)) n++;
  if(!test.equals("to_string()", a.to_string(), "Position (-1.000 AU, 2.000 AU, -3.000 AU)")) n++;
  if(!test.check("operator/(Interval&)", (a / Interval(Unit::yr)) ==
          Velocity(-1.0 * Unit::au / Unit::yr, 2.0 * Unit::au / Unit::yr, -3.0 * Unit::au / Unit::yr))) n++;
  if(!test.check("operator/(interval 0)", !(a / Interval::zero()).is_valid())) n++;

  Frame frame = Observer::at_geocenter().reduced_accuracy_frame_at(Time::j2000());

  if(!test.check("as_astrometric(invalid)", !x.as_astrometric(frame).is_valid())) n++;
  if(!test.check("as_astrometric(frame invalid)", !a.as_astrometric(Frame::undefined()).is_valid())) n++;

  AstrometricPosition ap = a.as_astrometric(frame);
  if(!test.check("as_astrometric()", ap.is_valid())) n++;
  if(!test.check("as_astrometric() ==", ap == a)) n++;
  if(!test.check("as_astrometric().obs_time()", ap.obs_time() == Time::j2000())) n++;
  if(!test.check("as_astrometric().emit_time()", ap.emit_time() == (Time::j2000() - (a.distance().m() / Constant::c)))) n++;

  Position ai = a.inv();
  if(!test.equals("x() inv", ai.x().m(), -a.x().m())) n++;
  if(!test.equals("y() inv", ai.y().m(), -a.y().m())) n++;
  if(!test.equals("z() inv", ai.z().m(), -a.z().m())) n++;
  if(!test.check("operator!=() inv", (a != ai))) n++;
  if(!test.check("operator==() inv !", !(a == ai))) n++;

  if(!test.equals("[0]", a[0], -1.0 * Unit::au)) n++;
  if(!test.equals("[1]", a[1], 2.0 * Unit::au)) n++;
  if(!test.equals("[2]", a[2], -3.0 * Unit::au)) n++;

  double p[3] = {-1.0, 2.0, -3.0};
  Position b(p, Unit::au);

  if(!test.check("equals()", a.equals(b, 1e-15 * Unit::au))) n++;
  if(!test.check("!equals()", !a.equals(ai, 1e-15 * Unit::au))) n++;
  if(!test.check("operator==()", a == b)) n++;
  if(!test.check("operator!=() !", !(a != b))) n++;

  Position c(p, Unit::m);
  Position c1(-1.00001, 2.0001, -3.0001);

  if(!test.check("operator==() mm", c == c1)) n++;
  if(!test.check("operator!=() mm !", !(c != c1))) n++;

  if(!test.equals("projection_on(x)", a.projection_on(Position(5.0, 0.0, 0.0)), a.x().m(), 1e-15 * Unit::AU)) n++;
  if(!test.equals("projection_on(y)", a.projection_on(Position(0.0, 5.0, 0.0)), a.y().m(), 1e-15 * Unit::AU)) n++;
  if(!test.equals("projection_on(z)", a.projection_on(Position(0.0, 0.0, 5.0)), a.z().m(), 1e-15 * Unit::AU)) n++;

  double l = a.abs();
  if(!test.equals("unit_vector().x()", a.unit_vector()[0], a.x().m() / l, 1e-15)) n++;
  if(!test.equals("unit_vector().y()", a.unit_vector()[1], a.y().m() / l, 1e-15)) n++;
  if(!test.equals("unit_vector().z()", a.unit_vector()[2], a.z().m() / l, 1e-15)) n++;

  if(!test.equals("x(a - b)", (a - b).x().m(), 0.0)) n++;
  if(!test.equals("y(a - b)", (a - b).y().m(), 0.0)) n++;
  if(!test.equals("z(a - b)", (a - b).z().m(), 0.0)) n++;

  if(!test.equals("x(a + b)", (a + b).x().au(), -2.0, 1e-14)) n++;
  if(!test.equals("y(a + b)", (a + b).y().au(), 4.0, 1e-14)) n++;
  if(!test.equals("z(a + b)", (a + b).z().au(), -6.0, 1e-14)) n++;

  if(!test.equals("x(2 * a)", (2 * a)[0], -2.0 * Unit::au, 1e-14 * Unit::au)) n++;
  if(!test.equals("y(2 * a)", (2 * a)[1], 4.0 * Unit::au, 1e-14 * Unit::au)) n++;
  if(!test.equals("z(2 * a)", (2 * a)[2], -6.0 * Unit::au, 1e-14 * Unit::au)) n++;

  if(!test.equals("dot(b)", a.dot(b), a.abs() * b.abs())) n++;

  std::cout << "Position.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
