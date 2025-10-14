/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Distance");

  int n = 0;

  Distance x(NAN);
  if(!test.check("is_valid(NAN)", !x.is_valid())) n++;
  if(!test.check("isnan(NAN)", isnan(x.m()))) n++;

  if(!test.check("is_valid(at_Gpc())", Distance::at_Gpc().is_valid())) n++;
  if(!test.equals("at_Gpc()", Distance::at_Gpc().m(), Unit::Gpc, 1e-15)) n++;

  Distance a(1.0 * Unit::au);
  if(!test.check("is_valid(1 AU)", a.is_valid())) n++;
  if(!test.equals("m()", a.m(), Unit::au)) n++;
  if(!test.equals("km()", a.km(), Unit::au / Unit::km, 1e-4)) n++;
  if(!test.equals("au()", a.au(), 1.0)) n++;
  if(!test.equals("lyr()", a.lyr(), Unit::au / Unit::lyr, 1e-20)) n++;
  if(!test.equals("pc()", a.pc(), Unit::au / Unit::pc, 1e-20)) n++;
  if(!test.equals("kpc()", a.kpc(), Unit::au / Unit::kpc, 1e-23)) n++;
  if(!test.equals("Mpc()", a.Mpc(), Unit::au / Unit::Mpc, 1e-26)) n++;
  if(!test.equals("Gpc()", a.Gpc(), Unit::au / Unit::Gpc, 1e-29)) n++;

  Distance b(1.0 * Unit::pc);
  if(!test.equals("pc(1 pc)", b.pc(), 1.0)) n++;
  if(!test.equals("parallax(1 pc)", b.parallax().arcsec(), 1.0, 1e-15)) n++;

  Distance c = Distance::from_parallax(Angle(1.0 * Unit::mas));
  if(!test.check("is_valid(parallax = 1 mas)", c.is_valid())) n++;
  if(!test.equals("parallax(1 mas)", c.parallax().mas(), 1.0, 1e-15)) n++;

  Distance d(-Unit::kpc);
  if(!test.equals("kpc(-1 kpc)", d.kpc(), -1.0, 1e-15)) n++;
  if(!test.equals("abs(-1 kpc)", d.abs().kpc(), 1.0, 1e-15)) n++;

  Distance(10.0 * Unit::m).to_string();
  Distance(10.0 * Unit::km).to_string();
  Distance(1e4 * Unit::km).to_string();
  Distance(10.0 * Unit::au).to_string();
  Distance(10.0 * Unit::pc).to_string();
  Distance(10.0 * Unit::kpc).to_string();
  Distance(10.0 * Unit::Mpc).to_string();
  Distance(10.0 * Unit::Gpc).to_string();

  std::cout << "Distance.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
