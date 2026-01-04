/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("EquinoxTest");

  int n = 0;

  Equinox x = Equinox::invalid();
  if(!test.check("invalid()", !x.is_valid())) n++;
  if(!test.check("invalid().is_icrs()", !x.is_icrs())) n++;
  if(!test.check("invalid().is_mod()", !x.is_mod())) n++;
  if(!test.check("invalid().is_true()", !x.is_true())) n++;
  if(!test.check("invalid().jd() ? NAN", isnan(x.jd()))) n++;
  if(!test.check("invalid().epoch() ? NAN", isnan(x.epoch()))) n++;
  if(!test.equals("invalid().system()", x.reference_system(), -1)) n++;



  std::cout << "EquinoxTest.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
