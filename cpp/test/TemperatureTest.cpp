/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Temperature");

  int n = 0;

  Temperature x = Temperature::C(NAN);
  if(!test.check("is_valid(NAN)", !x.is_valid())) n++;
  if(!test.check("isnan(NAN)", isnan(x.C()))) n++;
  if(!test.check("!is_valid(-1 K)", !Temperature::K(-1.0).is_valid())) n++;

  Temperature a = Temperature::C(45.0);
  if(!test.check("is_valid(45 C)", a.is_valid())) n++;
  if(!test.equals("C()", a.C(), 45.0)) n++;
  if(!test.equals("F()", a.F(), 45.0 * 1.8 + 32.0, 1e-15)) n++;
  if(!test.equals("K()", a.K(), 45.0 + 273.15, 1e-12)) n++;

  Temperature b = Temperature::F(451.0);
  if(!test.equals("F(value)", b.F(), 451.0)) n++;

  Temperature c = Temperature::K(300.0);
  if(!test.equals("K(value)", c.K(), 300.0)) n++;

  c.to_string();

  std::cout << "Temperature.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
