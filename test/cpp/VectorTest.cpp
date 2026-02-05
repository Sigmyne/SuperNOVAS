/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

int main() {
  TestUtil test = TestUtil("Vector");

  int n = 0;

  if(!test.equals("phi()", Position(1.0, 1.0, 0.0).phi().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("theta()", Position(1.0, 0.0, 2.0).theta().rad(), atan(0.5), 1e-14)) n++;
 
  Vector a = Position(-1.123456789 * Unit::au, 2.123456789 * Unit::au, -3.123456789 * Unit::au).scaled(1.0 / Unit::au);
  if(!test.equals("to_string(3)", a.to_string(3), "VEC (-1.123, 2.123, -3.123)")) n++;

  std::cout << "Vector.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
