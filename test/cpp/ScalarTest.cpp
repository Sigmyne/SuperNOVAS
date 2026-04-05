/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

class ScalarTest : public Scalar {
public:
  ScalarTest(double value) : Scalar(value) {}

  std::string SI_unit() const override {
    return "blah";
  }
};

int main() {
  TestUtil test = TestUtil("Scalar");

  int n = 0;

  Interval a = Interval(3.31 * Unit::s);
  Scalar& s = a;

  if(!test.equals("SI_unit()", s.SI_unit(), "s")) n++;
  if(!test.equals("SI_value()", s.SI_value(), 3.31, 1e-15)) n++;
  if(!test.check("equals()", s.equals(Interval(3.30 * Unit::s), 0.02 * Unit::s))) n++;
  if(!test.check("!equals()", !s.equals(Interval(3.30 * Unit::s), 0.001 * Unit::s))) n++;
  if(!test.equals("to_string()", s.to_string(), "3.310 s")) n++;

  ScalarTest b = ScalarTest(3.14);
  if(!test.equals("to_string()", b.to_string(), "3.14 blah")) n++;

  std::cout << "Scalar.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
