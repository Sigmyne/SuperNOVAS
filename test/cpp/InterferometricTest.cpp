/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

int main() {
  TestUtil test = TestUtil("Interferometric");

  int n = 0;

  Interferometric x = Interferometric::undefined();
  if(!test.check("invalid", !x.is_valid())) n++;
  if(!test.check("invalid[0]", isnan(x[0]))) n++;
  if(!test.check("invalid[1]", isnan(x[1]))) n++;
  if(!test.check("invalid[2]", isnan(x[2]))) n++;
  if(!test.check("invalid ==", !(x == x))) n++;
  if(!test.check("invalid !=", x != x)) n++;

  if(!test.check("invalid u", !Interferometric(NAN, 0.0, 0.0).is_valid())) n++;
  if(!test.check("invalid v", !Interferometric(0.0, NAN, 0.0).is_valid())) n++;
  if(!test.check("invalid w", !Interferometric(0.0, 0.0, NAN).is_valid())) n++;

  Interferometric a = Interferometric(Coordinate(100.1), Coordinate(-200.1), Coordinate(300.1));
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("[0]", a[0], 100.1, 1e-12)) n++;
  if(!test.equals("[1]", a[1], -200.1, 1e-12)) n++;
  if(!test.equals("[2]", a[2], 300.1, 1e-12)) n++;
  if(!test.equals("u()", a.u().m(), 100.1, 1e-12)) n++;
  if(!test.equals("v()", a.v().m(), -200.1, 1e-12)) n++;
  if(!test.equals("w()", a.w().m(), 300.1, 1e-12)) n++;
  if(!test.check("operator+(invalid)", !(a + x).is_valid())) n++;
  if(!test.check("operator-(invalid)", !(a - x).is_valid())) n++;
  if(!test.check("invalid operator+()", !(x + a).is_valid())) n++;
  if(!test.check("invalid operator-()", !(x - a).is_valid())) n++;
  if(!test.check("geometric_delay()", a.geometric_delay() == Interval(-a[2] / Constant::c))) n++;
  if(!test.equals("to_string()", a.to_string(), "u = 100.100000 m, v = -200.100000 m, delay = -1.001026 us")) n++;

  Interferometric b = Interferometric(0.1, -0.1, 0.1);
  if(!test.check("(a + b).is_valid()", (a + b).is_valid())) n++;
  if(!test.check("(a + b) ==", (a + b) == Interferometric(100.2, -200.2, 300.2))) n++;
  if(!test.check("(a + b) != a", (a + b) != a)) n++;
  if(!test.check("(a + b).equals()", (a + b).equals(a, 0.2))) n++;

  if(!test.check("(a - b).is_valid()", (a - b).is_valid())) n++;
  if(!test.check("(a - b) ==", (a - b) == Interferometric(100.0, -200.0, 300.0))) n++;
  if(!test.check("(a - b) != a", (a - b) != a)) n++;
  if(!test.check("(a + b).equals()", (a - b).equals(a, 0.2))) n++;

  Interferometric c = Interferometric(Coordinate(100.1), Coordinate(-200.1), Interval(1.0 * Unit::us));
  if(!test.equals("geometric_delay(dt)", c.geometric_delay().seconds(), 1.0e-6, 1e-12)) n++;
  if(!test.equals("w(dt)", c.w().m(), -Unit::us * Constant::c, 1e-12)) n++;

  std::cout << "Interferometric.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
