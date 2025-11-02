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

  if(!test.check("invalid lon", !Ecliptic::icrs(NAN, 30.0 * Unit::deg, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid lat", !Ecliptic::icrs(45.0 * Unit::deg, NAN, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid lat > 90", !Ecliptic::icrs(45.0 * Unit::deg, 91.0 * Unit::deg, 10.0 * Unit::au).is_valid())) n++;
  if(!test.check("invalid dis", !Ecliptic::icrs(45.0 * Unit::deg, 30.0 * Unit::deg, NAN).is_valid())) n++;
  if(!test.check("invalid dis < 0", !Ecliptic::icrs(45.0 * Unit::deg, 30.0 * Unit::deg, -1.0).is_valid())) n++;

  Ecliptic x = Ecliptic::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("longitude() invalid", isnan(x.longitude().rad()))) n++;
  if(!test.check("latitude() invalid", isnan(x.latitude().rad()))) n++;
  if(!test.check("distance() invalid", isnan(x.distance().m()))) n++;
  if(!test.check("as_equatorial() invalid", !x.as_equatorial().is_valid())) n++;
  if(!test.check("as_galactic() invalid", !x.as_galactic().is_valid())) n++;

  Ecliptic a = Ecliptic::icrs(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("longitude()", a.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", a.latitude().deg(), 30.0, 1e-14)) n++;
  if(!test.equals("distance()", a.distance().au(), 10.0)) n++;
  if(!test.equals("equator(ICRS)", a.equator(), novas::NOVAS_GCRS_EQUATOR)) n++;
  if(!test.equals("jd(ICRS)", a.jd(), NOVAS_JD_J2000)) n++;
  if(!test.equals("mjd(ICRS)", a.mjd(), NOVAS_JD_J2000 - NOVAS_JD_MJD0)) n++;
  if(!test.check("to_icrs(ICRS)", a.to_icrs() == a)) n++;
  if(!test.equals("to_string(ICRS)", a.to_string(novas::NOVAS_SEP_COLONS), "ECL    45:00:00.000    30:00:00.000  ICRS")) n++;

  Ecliptic b = Ecliptic::j2000(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  Ecliptic b1 =  b.to_icrs().to_j2000();
  if(!test.equals("to_icrs().to_j2000().longitude()", b1.longitude().deg(), b.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs(J2000).to_j2000().latitude()", b1.latitude().deg(), b.latitude().deg(), 1e-12)) n++;
  if(!test.equals("equator(J2000)", b.equator(), novas::NOVAS_MEAN_EQUATOR)) n++;
  if(!test.equals("jd(J2000)", b.jd(), NOVAS_JD_J2000)) n++;
  if(!test.check("to_j2000(J2000)", b.to_j2000() == b)) n++;
  if(!test.check("to_mod(J2000)", b.to_mod(NOVAS_JD_J2000) == b)) n++;
  if(!test.equals("to_string(J2000)", b.to_string(novas::NOVAS_SEP_COLONS), "ECL    45:00:00.000    30:00:00.000  J2000")) n++;

  Ecliptic c = Ecliptic::mod(Time::b1950(), Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  Ecliptic c1 =  c.to_icrs().to_mod(Time::b1950());
  if(!test.equals("to_icrs().to_mod().longitude()", c1.longitude().deg(), c.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_mod().latitude()", c1.latitude().deg(), c.latitude().deg(), 1e-12)) n++;
  if(!test.equals("equator(B1950)", c.equator(), novas::NOVAS_MEAN_EQUATOR)) n++;
  if(!test.equals("jd(B1950)", c.jd(), NOVAS_JD_B1950)) n++;
  if(!test.check("to_mod(B1950)", c.to_mod(NOVAS_JD_B1950) == c)) n++;
  if(!test.equals("to_string(B1950)", c.to_string(novas::NOVAS_SEP_COLONS), "ECL    45:00:00.000    30:00:00.000  B1950")) n++;

  Ecliptic d = Ecliptic::tod(Time::b1900(), Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Distance(10.0 * Unit::au));
  Ecliptic d1 =  d.to_icrs().to_tod(Time::b1900());
  if(!test.equals("to_icrs().to_tod().longitude()", d1.longitude().deg(), d.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_tod().latitude()", d1.latitude().deg(), d.latitude().deg(), 1e-12)) n++;
  if(!test.equals("equator(TOD)", d.equator(), novas::NOVAS_TRUE_EQUATOR)) n++;
  if(!test.equals("jd(B1900)", d.jd(), NOVAS_JD_B1900)) n++;
  if(!test.check("to_tod(B1900)", d.to_tod(NOVAS_JD_B1900) == d)) n++;
  if(!test.equals("to_string(TOD B1900)", d.to_string(novas::NOVAS_SEP_COLONS), "ECL    45:00:00.000    30:00:00.000  TOD B1900")) n++;

  Ecliptic e = Ecliptic::icrs(Angle(20.0 * Unit::deg), Angle(15.0 * Unit::deg), Distance(10.0 * Unit::au));
  if(!test.equals("distance_to()", e.distance_to(a).deg(),
          novas::novas_sep(e.longitude().deg(), e.latitude().deg(), a.longitude().deg(), a.latitude().deg()), 0.1 * Unit::uas)) n++;

  double ra = 0.0, dec = 0.0;
  novas::ecl2equ(a.jd(), a.equator(), novas::NOVAS_FULL_ACCURACY, a.longitude().deg(), a.latitude().deg(), &ra, &dec);
  Equatorial eq0 = Equatorial(ra * Unit::hour_angle, dec * Unit::deg);
  if(!test.check("as_equatorial()", a.as_equatorial() == eq0)) {
    std::cout << "  " << a.as_equatorial().to_string(novas::NOVAS_SEP_COLONS, 6) << " != "
            <<  eq0.to_string(novas::NOVAS_SEP_COLONS, 6) << "\n";
    n++;
  }

  double blon = 0.0, blat = 0.0;
  novas::equ2gal(ra, dec, &blon, &blat);
  Galactic ga0 = Galactic(blon * Unit::deg, blat * Unit::deg);
  if(!test.check("as_ecliptic()", a.as_galactic() == ga0)) {
    std::cout << "  " << a.as_galactic().to_string(novas::NOVAS_SEP_COLONS, 8) << " != "
            <<  ga0.to_string(novas::NOVAS_SEP_COLONS, 8) << "\n";
    n++;
  }

  Position xyz = a.xyz();
  if(!test.equals("xyz().x()", xyz.x(), 10.0 * Unit::au * cos(a.latitude().rad()) * cos(a.longitude().rad()))) n++;
  if(!test.equals("xyz().y()", xyz.y(), 10.0 * Unit::au * cos(a.latitude().rad()) * sin(a.longitude().rad()))) n++;
  if(!test.equals("xyz().z()", xyz.z(), 10.0 * Unit::au * sin(a.latitude().rad()))) n++;

  Ecliptic f = Ecliptic(xyz, a.equator(), a.jd());
  if(!test.check("Ecliptic(xyz)", f == a)) n++;


  std::cout << "Ecliptic.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
