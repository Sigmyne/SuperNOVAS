/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("EOP");

  int n = 0;

  EOP x = EOP::invalid();
  if(!test.check("invalid()", !x.is_valid())) n++;
  if(!test.check("invalid().dUT1()", !x.dUT1().is_valid())) n++;
  if(!test.check("invalid().xp()", !x.xp().is_valid())) n++;
  if(!test.check("invalid().yp()", !x.yp().is_valid())) n++;

  if(!test.check("invalid(dut1 = NAN)", !EOP(0, NAN, 0.0, 0.0).is_valid())) n++;
  if(!test.check("invalid(xp = NAN)", !EOP(0, 0.0, NAN, 0.0).is_valid())) n++;
  if(!test.check("invalid(yp = NAN)", !EOP(0, 0.0, 0.0, NAN).is_valid())) n++;

  EOP a(32, 0.1, 200.0 * Unit::mas, 300.0 * Unit::mas);
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("leap_seconds()", a.leap_seconds(), 32L)) n++;
  if(!test.equals("dUT1()", a.dUT1().seconds(), 0.1)) n++;
  if(!test.equals("xp()", a.xp().mas(), 200.0)) n++;
  if(!test.equals("yp()", a.yp().mas(), 300.0)) n++;

  EOP b(32, Interval(0.1 * Unit::sec), Angle(200.0 * Unit::mas), Angle(300.0 * Unit::mas));
  if(!test.check("is_valid()", b.is_valid())) n++;
  if(!test.equals("leap_seconds()", b.leap_seconds(), 32L)) n++;
  if(!test.equals("dUT1()", b.dUT1().seconds(), 0.1)) n++;
  if(!test.equals("xp()", b.xp().mas(), 200.0)) n++;
  if(!test.equals("yp()", b.yp().mas(), 300.0)) n++;

  double xp = 0.0, yp = 0.0, dt = 0.0;
  novas::novas_itrf_transform_eop(2000.0, 0.2, 0.3, 0.1, 2014, &xp, &yp, &dt);
  EOP c = a.itrf_transformed(2000, 2014);

  if(!test.equals("itrf_transformed().dUT1()", c.dUT1().seconds(), dt, 1e-14)) n++;
  if(!test.equals("itrf_transformed().xp()", c.xp().arcsec(), xp, 1e-14)) n++;
  if(!test.equals("itrf_transformed().yp()", c.yp().arcsec(), yp, 1e-14)) n++;

  c.to_string();

  std::cout << "EOP.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
