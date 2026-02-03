/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Ecliptic");

  int n = 0;

  if(!test.check("invalid lon", !Ecliptic(NAN, 30.0 * Unit::deg, Equinox::icrs()).is_valid())) n++;
  if(!test.check("invalid lat", !Ecliptic(45.0 * Unit::deg, NAN, Equinox::icrs()).is_valid())) n++;
  if(!test.check("invalid lat > 90", !Ecliptic(45.0 * Unit::deg, 91.0 * Unit::deg, Equinox::icrs()).is_valid())) n++;

  Ecliptic x = Ecliptic::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("longitude() invalid", isnan(x.longitude().rad()))) n++;
  if(!test.check("latitude() invalid", isnan(x.latitude().rad()))) n++;
  if(!test.check("to_equatorial() invalid", !x.to_equatorial().is_valid())) n++;
  if(!test.check("to_galactic() invalid", !x.to_galactic().is_valid())) n++;

  Ecliptic a = Ecliptic(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Equinox::icrs());
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("longitude()", a.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", a.latitude().deg(), 30.0, 1e-14)) n++;
  if(!test.equals("equator(ICRS)", a.equator_type(), novas::NOVAS_GCRS_EQUATOR)) n++;
  if(!test.equals("jd(ICRS)", a.jd(), NOVAS_JD_J2000)) n++;
  if(!test.equals("mjd(ICRS)", a.mjd(), NOVAS_JD_J2000 - NOVAS_JD_MJD0)) n++;
  if(!test.check("to_icrs(ICRS)", a.to_icrs() == a)) n++;
  if(!test.equals("to_string(ICRS)", a.to_string(novas::NOVAS_SEP_COLONS), "ECL    45:00:00.000    30:00:00.000  ICRS")) n++;

  Ecliptic a1 = Ecliptic("45:00 00.000", "30d 00m00s", Equinox::icrs());
  if(!test.check("is_valid() Ecliptic(string)", a1.is_valid())) n++;
  if(!test.check("equals(Angle) Ecliptic(string)", a1.equals(a, Angle(1e-15)))) n++;
  if(!test.check("equals(reciprocal)", a.equals(a1, 1e-15))) n++;
  if(!test.check("a1 == a", (a1 == a))) n++;
  if(!test.check("!(a1 != a)", !(a1 != a))) n++;

  Ecliptic b = Ecliptic(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Equinox::j2000());
  Ecliptic b1 =  b.to_icrs().to_j2000();
  if(!test.equals("to_icrs().to_j2000().longitude()", b1.longitude().deg(), b.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs(J2000).to_j2000().latitude()", b1.latitude().deg(), b.latitude().deg(), 1e-12)) n++;
  if(!test.equals("equator(J2000)", b.equator_type(), novas::NOVAS_MEAN_EQUATOR)) n++;
  if(!test.equals("jd(J2000)", b.jd(), NOVAS_JD_J2000)) n++;
  if(!test.check("to_j2000(J2000)", b.to_j2000() == b)) n++;
  if(!test.check("to_mod(J2000)", b.to_mod(NOVAS_JD_J2000) == b)) n++;
  if(!test.check("operator >> (B1950)", (b >> Equinox::b1950()) == b.to_mod(NOVAS_JD_B1950))) n++;
  if(!test.equals("to_string(J2000)", b.to_string(novas::NOVAS_SEP_COLONS), "ECL    45:00:00.000    30:00:00.000  J2000")) n++;

  Ecliptic c = Ecliptic(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Equinox::b1950());
  Ecliptic c1 =  c.to_icrs().to_mod(Time::b1950());
  if(!test.equals("to_icrs().to_mod().longitude()", c1.longitude().deg(), c.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_mod().latitude()", c1.latitude().deg(), c.latitude().deg(), 1e-12)) n++;
  if(!test.equals("equator(B1950)", c.equator_type(), novas::NOVAS_MEAN_EQUATOR)) n++;
  if(!test.equals("jd(B1950)", c.jd(), NOVAS_JD_B1950)) n++;
  if(!test.check("to_mod(B1950)", c.to_mod(NOVAS_JD_B1950) == c)) n++;
  if(!test.equals("to_string(B1950)", c.to_string(novas::NOVAS_SEP_COLONS), "ECL    45:00:00.000    30:00:00.000  B1950")) n++;

  Ecliptic d = Ecliptic(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Equinox::tod(Time::b1900()));
  Ecliptic d1 =  d.to_icrs().to_tod(Time::b1900());
  if(!test.equals("to_icrs().to_tod().longitude()", d1.longitude().deg(), d.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_tod().latitude()", d1.latitude().deg(), d.latitude().deg(), 1e-12)) n++;
  if(!test.equals("equator(TOD)", d.equator_type(), novas::NOVAS_TRUE_EQUATOR)) n++;
  if(!test.equals("jd(B1900)", d.jd(), NOVAS_JD_B1900)) n++;
  if(!test.check("to_tod(B1900)", d.to_tod(NOVAS_JD_B1900) == d)) n++;
  if(!test.equals("to_string(TOD B1900)", d.to_string(novas::NOVAS_SEP_COLONS), "ECL    45:00:00.000    30:00:00.000  TOD B1900")) n++;

  Ecliptic e = Ecliptic(Angle(20.0 * Unit::deg), Angle(15.0 * Unit::deg), Equinox::icrs());
  if(!test.equals("distance_to()", e.distance_to(a).deg(),
          novas::novas_sep(e.longitude().deg(), e.latitude().deg(), a.longitude().deg(), a.latitude().deg()), 0.1 * Unit::uas)) n++;

  double ra = 0.0, dec = 0.0;
  novas::ecl2equ(a.jd(), a.equator_type(), novas::NOVAS_FULL_ACCURACY, a.longitude().deg(), a.latitude().deg(), &ra, &dec);
  Equatorial eq0 = Equatorial(ra * Unit::hour_angle, dec * Unit::deg);
  if(!test.check("to_equatorial()", a.to_equatorial() == eq0)) {
    std::cout << "  " << a.to_equatorial().to_string(novas::NOVAS_SEP_COLONS, 6) << " != "
            <<  eq0.to_string(novas::NOVAS_SEP_COLONS, 6) << "\n";
    n++;
  }

  double blon = 0.0, blat = 0.0;
  novas::equ2gal(ra, dec, &blon, &blat);
  Galactic ga0 = Galactic(blon * Unit::deg, blat * Unit::deg);
  if(!test.check("to_galactic()", a.to_galactic() == ga0)) {
    std::cout << "  " << a.to_galactic().to_string(novas::NOVAS_SEP_COLONS, 8) << " != "
            <<  ga0.to_string(novas::NOVAS_SEP_COLONS, 8) << "\n";
    n++;
  }

  Position xyz = a.xyz(Distance(10.0 * Unit::au));
  if(!test.equals("xyz().x()", xyz.x(), 10.0 * Unit::au * cos(a.latitude().rad()) * cos(a.longitude().rad()))) n++;
  if(!test.equals("xyz().y()", xyz.y(), 10.0 * Unit::au * cos(a.latitude().rad()) * sin(a.longitude().rad()))) n++;
  if(!test.equals("xyz().z()", xyz.z(), 10.0 * Unit::au * sin(a.latitude().rad()))) n++;

  Ecliptic f = Ecliptic(xyz, Equinox::icrs());
  if(!test.check("Ecliptic(xyz)", f == a)) n++;

  std::cout << "Ecliptic.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
