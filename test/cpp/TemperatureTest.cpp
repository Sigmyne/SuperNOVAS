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

  Temperature x = Temperature::celsius(NAN);
  if(!test.check("is_valid(NAN)", !x.is_valid())) n++;
  if(!test.check("isnan(NAN)", isnan(x.celsius()))) n++;
  if(!test.check("!is_valid(-1 K)", !Temperature::kelvin(-1.0).is_valid())) n++;

  Temperature a = Temperature::celsius(45.0);
  if(!test.check("is_valid(45 C)", a.is_valid())) n++;
  if(!test.equals("celsius()", a.celsius(), 45.0)) n++;
  if(!test.equals("farenheit()", a.farenheit(), 45.0 * 1.8 + 32.0, 1e-15)) n++;
  if(!test.equals("kelvin()", a.kelvin(), 45.0 + 273.15, 1e-12)) n++;

  Temperature b = Temperature::farenheit(451.0);
  if(!test.equals("F(value)", b.farenheit(), 451.0)) n++;

  Temperature c = Temperature::kelvin(300.0);
  if(!test.equals("K(value)", c.kelvin(), 300.0)) n++;

  if(!test.equals("to_string()", a.to_string(), "45.0 C")) n++;

  std::cout << "Temperature.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
