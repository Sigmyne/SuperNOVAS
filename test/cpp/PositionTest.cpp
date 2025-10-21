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

  Position x = Position::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("x() invalid", isnan(x.x()))) n++;
  if(!test.check("y() invalid", isnan(x.y()))) n++;
  if(!test.check("z() invalid", isnan(x.z()))) n++;

  Position a(-1.0 * Unit::au, 2.0 * Unit::au, -3.0 * Unit::au);
  if(!test.check("is_valid(-1 AU, 2 AU, -3 AU)", a.is_valid())) n++;
  if(!test.equals("x()", a.x(), -1.0 * Unit::au)) n++;
  if(!test.equals("y()", a.y(), 2.0 * Unit::au)) n++;
  if(!test.equals("z()", a.z(), -3.0 * Unit::au)) n++;
  if(!test.equals("length()", a.distance().au(), sqrt(14.0), 1e-14)) n++;

  const double *pa = a._array();
  if(!test.equals("_array()[0]", pa[0], -1.0 * Unit::au)) n++;
  if(!test.equals("_array()[1]", pa[1], 2.0 * Unit::au)) n++;
  if(!test.equals("_array()[2]", pa[2], -3.0 * Unit::au)) n++;

  double p[3] = {-1.0, 2.0, -3.0};
  Position b(p, Unit::au);
  if(!test.equals("x(a - b)", (a - b).x(), 0.0)) n++;
  if(!test.equals("y(a - b)", (a - b).y(), 0.0)) n++;
  if(!test.equals("z(a - b)", (a - b).z(), 0.0)) n++;

  if(!test.equals("x(a + b)", (a + b).x(), -2.0 * Unit::au, 1e-14 * Unit::au)) n++;
  if(!test.equals("y(a + b)", (a + b).y(), 4.0 * Unit::au, 1e-14 * Unit::au)) n++;
  if(!test.equals("z(a + b)", (a + b).z(), -6.0 * Unit::au, 1e-14 * Unit::au)) n++;

  if(!test.equals("x(2 * a)", (2 * a).x(), -2.0 * Unit::au, 1e-14 * Unit::au)) n++;
  if(!test.equals("y(2 * a)", (2 * a).y(), 4.0 * Unit::au, 1e-14 * Unit::au)) n++;
  if(!test.equals("z(2 * a)", (2 * a).z(), -6.0 * Unit::au, 1e-14 * Unit::au)) n++;

  if(!test.equals("dot(b)", a.dot(b), a.abs() * b.abs())) n++;

  a.to_string();

  std::cout << "Position.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
