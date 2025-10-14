/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Pressure");

  int n = 0;

  Pressure x = Pressure::Pa(NAN);
  if(!test.check("is_valid(NAN)", !x.is_valid())) n++;
  if(!test.check("isnan(NAN)", isnan(x.Pa()))) n++;
  if(!test.check("!is_valid(-1 Pa)", !Pressure::Pa(-1.0).is_valid())) n++;

  Pressure a = Pressure::Pa(1e5);
  if(!test.check("is_valid(100 hPa)", a.is_valid())) n++;
  if(!test.equals("Pa()", a.Pa(), 1e5)) n++;
  if(!test.equals("hPa()", a.hPa(), 1000.0, 1e-12)) n++;
  if(!test.equals("kPa()", a.kPa(), 100.0, 1e-13)) n++;
  if(!test.equals("mbar()", a.mbar(), 1000.0, 1e-12)) n++;
  if(!test.equals("bar()", a.bar(), 1.0, 1e-15)) n++;
  if(!test.equals("atm()", a.atm(), 1e5 / Unit::atm, 1e-15)) n++;
  if(!test.equals("torr()", a.torr(), 1e5 / Unit::torr, 1e-15)) n++;

  if(!test.equals("hPA(x)", Pressure::hPa(99.0).hPa(), 99.0, 1e-13)) n++;
  if(!test.equals("kPA(x)", Pressure::kPa(99.0).kPa(), 99.0, 1e-13)) n++;
  if(!test.equals("mbar(x)", Pressure::mbar(99.0).mbar(), 99.0, 1e-13)) n++;
  if(!test.equals("bar(x)", Pressure::bar(99.0).bar(), 99.0, 1e-13)) n++;
  if(!test.equals("atm(x)", Pressure::atm(99.0).atm(), 99.0, 1e-13)) n++;
  if(!test.equals("torr(x)", Pressure::torr(99.0).torr(), 99.0, 1e-13)) n++;

  a.to_string();

  std::cout << "Pressure.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
