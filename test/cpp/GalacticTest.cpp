/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Galactic");

  int n = 0;

  if(!test.check("invalid lon", !Galactic(NAN, 30.0 * Unit::deg).is_valid())) n++;
  if(!test.check("invalid lat", !Galactic(45.0 * Unit::deg, NAN).is_valid())) n++;
  if(!test.check("invalid lat > 90", !Galactic(45.0 * Unit::deg, 91.0 * Unit::deg).is_valid())) n++;

  Galactic x = Galactic::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("longitude() invalid", isnan(x.longitude().rad()))) n++;
  if(!test.check("latitude() invalid", isnan(x.latitude().rad()))) n++;
  if(!test.check("as_equatorial() invalid", !x.to_equatorial().is_valid())) n++;
  if(!test.check("as_ecliptic() invalid", !x.to_ecliptic().is_valid())) n++;

  Galactic a(45.0 * Unit::deg, 30.0 * Unit::deg);
  if(!test.check("is_valid(double)", a.is_valid())) n++;
  if(!test.equals("longitude(double)", a.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude(double)", a.latitude().deg(), 30.0, 1e-14)) n++;

  Galactic a1("45d00:00.000", "+30 00m 00");
  if(!test.check("is_valid(string)", a1.is_valid())) n++;
  if(!test.check("is_equals(string)", a1.equals(a))) n++;
  if(!test.check("operator ==", (a1 == a))) n++;

  Galactic a2("45d00:00", "+30 00m 00.001");
  if(!test.check("is_valid(default distance)", a2.is_valid())) n++;
  if(!test.check("equals(2 mas)", a2.equals(a, Angle(2.0 * Unit::mas)))) n++;
  if(!test.check("operator == (!)", !(a2 == a))) n++;
  if(!test.check("operator !=", (a2 != a))) n++;

  double ra = 0.0, dec = 0.0;
  novas::gal2equ(a.longitude().deg(), a.latitude().deg(), &ra, &dec);
  Equatorial eq0 = Equatorial(ra * Unit::hour_angle, dec * Unit::deg);
  if(!test.check("as_equatorial()", a.to_equatorial() == eq0)) {
    std::cout << "  " << a.to_equatorial().to_string(novas::NOVAS_SEP_COLONS, 6) << " != "
            <<  eq0.to_string(novas::NOVAS_SEP_COLONS, 6) << "\n";
    n++;
  }

  double elon = 0.0, elat = 0.0;
  novas::equ2ecl(NOVAS_JD_J2000, novas::NOVAS_GCRS_EQUATOR, novas::NOVAS_FULL_ACCURACY, ra, dec, &elon, &elat);
  Ecliptic ec0 = Ecliptic(elon * Unit::deg, elat * Unit::deg);
  if(!test.check("as_ecliptic()", a.to_ecliptic() == ec0)) {
    std::cout << "  " << a.to_ecliptic().to_string(novas::NOVAS_SEP_COLONS, 8) << " != "
            <<  ec0.to_string(novas::NOVAS_SEP_COLONS, 8) << "\n";
    n++;
  }

  Galactic b(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg));
  if(!test.check("is_valid()", b.is_valid())) n++;
  if(!test.equals("longitude()", b.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", b.latitude().deg(), 30.0, 1e-14)) n++;

  Galactic c = Galactic(Angle(20.0 * Unit::deg), Angle(15.0 * Unit::deg));
  if(!test.equals("distance_to()", c.distance_to(a).deg(),
          novas::novas_sep(c.longitude().deg(), c.latitude().deg(), a.longitude().deg(), a.latitude().deg()), 0.1 * Unit::uas)) n++;

  Position xyz = a.xyz(Distance(10.0 * Unit::au));
  if(!test.equals("xyz().x()", xyz.x(), 10.0 * Unit::au * cos(a.latitude().rad()) * cos(a.longitude().rad()))) n++;
  if(!test.equals("xyz().y()", xyz.y(), 10.0 * Unit::au * cos(a.latitude().rad()) * sin(a.longitude().rad()))) n++;
  if(!test.equals("xyz().z()", xyz.z(), 10.0 * Unit::au * sin(a.latitude().rad()))) n++;

  Galactic d = Galactic(xyz);
  if(!test.check("Galactic(xyz)", d == a)) n++;

  a.to_string();

  std::cout << "Galactic.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
