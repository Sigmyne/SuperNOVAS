/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

using namespace novas;

int main() {
  TestUtil test = TestUtil("Equatorial");

  int n = 0;

  if(!test.check("invalid lon", !Equatorial(NAN, 30.0 * Unit::deg, Equinox::icrs()).is_valid())) n++;
  if(!test.check("invalid lat", !Equatorial(45.0 * Unit::deg, NAN, Equinox::icrs()).is_valid())) n++;
  if(!test.check("invalid lat > 90", !Equatorial(45.0 * Unit::deg, 91.0 * Unit::deg, Equinox::icrs()).is_valid())) n++;
  if(!test.check("invalid sys", !Equatorial(45.0 * Unit::deg, 30.0 * Unit::deg, Equinox::invalid()).is_valid())) n++;

  Equatorial x = Equatorial::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("longitude() invalid", isnan(x.longitude().rad()))) n++;
  if(!test.check("latitude() invalid", isnan(x.latitude().rad()))) n++;
  if(!test.check("to_ecliptic() invalid", !x.to_ecliptic().is_valid())) n++;
  if(!test.check("to_galactic() invalid", !x.to_galactic().is_valid())) n++;
  if(!test.check("to_icrs() invalid", !x.to_icrs().is_valid())) n++;

  Equatorial a = Equatorial(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Equinox::icrs());
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("longitude()", a.longitude().deg(), 45.0, 1e-14)) n++;
  if(!test.equals("latitude()", a.latitude().deg(), 30.0, 1e-14)) n++;
  if(!test.equals("reference_system(ICRS)", a.reference_system(), novas::NOVAS_ICRS)) n++;
  if(!test.equals("jd(ICRS)", a.system().jd(), NOVAS_JD_J2000)) n++;
  if(!test.check("to_icrs(ICRS)", a.to_icrs() == a)) n++;
  if(!test.equals("to_string(ICRS)", a.to_string(NOVAS_SEP_COLONS), "EQU 03:00:00.0000    30:00:00.000  ICRS")) n++;
  if(!test.check("to_system(invalid)", !a.to_system(Equinox::invalid()).is_valid())) n++;

  double p0[3] = {0.0}, z[3] = {0.0};
  Position pos(z, Unit::pc);
  double *p1 = (double *) pos._array();

  radec2vector(a.ra().hours(), a.dec().deg(), Unit::au, p0);

  Equatorial a1 = a.to_j2000();
  gcrs_to_j2000(p0, p1);
  if(!test.equals("to_j2000() sys", a1.reference_system(), NOVAS_J2000)) n++;
  if(!test.check("to_j2000() pos", pos == a1.xyz(Distance(Unit::AU)))) {
    std::cout << "### " << pos.to_string(12) << ", " << a1.xyz(Distance(Unit::pc)).to_string(12) << "\n";
    n++;
  }

  a1 = a.to_mod(Time::b1950());
  gcrs_to_mod(NOVAS_JD_B1950, p0, p1);
  if(!test.equals("to_mod(B1950) sys", a1.reference_system(), NOVAS_MOD)) n++;
  if(!test.check("to_mod(B1950) pos", pos == a1.xyz(Distance(Unit::AU)))) n++;
  if(!test.check("to_mod(B1950).to_icrs()", a1.to_icrs() == a)) n++;

  a1 = a.to_mod_at_besselian_epoch(1950.0);
  if(!test.equals("to_mod(B1950) sys", a1.reference_system(), NOVAS_MOD)) n++;
  if(!test.check("to_mod(B1950) pos", pos == a1.xyz(Distance(Unit::AU)))) n++;

  a1 = a.to_cirs(Time::hip());
  gcrs_to_cirs(NOVAS_JD_HIP, NOVAS_FULL_ACCURACY, p0, p1);
  if(!test.equals("to_cirs(HIP) sys", a1.reference_system(), NOVAS_CIRS)) n++;
  if(!test.check("to_cirs(HIP) pos", pos == a1.xyz(Distance(Unit::AU)))) n++;
  if(!test.check("to_cirs(HIP).to_icrs()", a1.to_icrs() == a)) n++;


  a1 = a.to_tod(Time::b1950());
  gcrs_to_tod(NOVAS_JD_B1950, NOVAS_FULL_ACCURACY, p0, p1);
  if(!test.equals("to_tod(B1950) sys", a1.reference_system(), NOVAS_TOD)) n++;
  if(!test.check("to_tod(B1950) pos", pos == a1.xyz(Distance(Unit::AU)))) n++;
  if(!test.check("to_tod(B1950).to_icrs()", a1.to_icrs() == a)) n++;

  a1 = a.to_hip();
  gcrs_to_mod(NOVAS_JD_HIP, p0, p1);
  if(!test.equals("to_hip() sys", a1.reference_system(), NOVAS_MOD)) n++;
  if(!test.check("to_hip() pos", pos == a1.xyz(Distance(Unit::AU)))) n++;
  if(!test.check("to_hip().to_icrs()", a1.to_icrs() == a)) n++;


  a1 = Equatorial("03:00 00.000", "30d 00m00s", Equinox::icrs());
  if(!test.check("is_valid() Equatorial(string)", a1.is_valid())) n++;
  if(!test.check("equals(Angle) Equatorial(string)", a1.equals(a, Angle(1e-15)))) n++;
  if(!test.check("equals(reciprocal)", a.equals(a1, 1e-15))) n++;
  if(!test.check("a1 == a", (a1 == a))) n++;
  if(!test.check("!(a1 != a)", !(a1 != a))) n++;

  Equatorial b = Equatorial(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Equinox::j2000());
  Equatorial b1 =  b.to_icrs().to_j2000();
  if(!test.equals("to_icrs().to_j2000().longitude()", b1.longitude().deg(), b.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs(J2000).to_j2000().latitude()", b1.latitude().deg(), b.latitude().deg(), 1e-12)) n++;
  if(!test.equals("reference_system(J2000)", b.reference_system(), NOVAS_J2000)) n++;
  if(!test.equals("jd(J2000)", b.system().jd(), NOVAS_JD_J2000)) n++;
  if(!test.check("to_j2000(J2000)", b.to_j2000() == b)) n++;
  if(!test.check("to_mod(J2000)", b.to_mod(NOVAS_JD_J2000) == b)) n++;
  if(!test.check("operator >> (B1950)", (b >> Equinox::b1950()) == b.to_mod(NOVAS_JD_B1950))) n++;
  if(!test.equals("to_string(J2000)", b.to_string(novas::NOVAS_SEP_COLONS), "EQU 03:00:00.0000    30:00:00.000  J2000")) n++;

  Equatorial c = Equatorial(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Equinox::b1950());
  Equatorial c1 =  c.to_icrs().to_mod(Time::b1950());
  if(!test.equals("to_icrs().to_mod().longitude()", c1.longitude().deg(), c.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_mod().latitude()", c1.latitude().deg(), c.latitude().deg(), 1e-12)) n++;
  if(!test.equals("reference_system(B1950)", c.reference_system(), NOVAS_MOD)) n++;
  if(!test.equals("jd(B1950)", c.system().jd(), NOVAS_JD_B1950)) n++;
  if(!test.check("to_mod(B1950)", c.to_mod(NOVAS_JD_B1950) == c)) n++;
  if(!test.equals("to_string(B1950)", c.to_string(novas::NOVAS_SEP_COLONS), "EQU 03:00:00.0000    30:00:00.000  B1950")) n++;

  Equatorial d = Equatorial(Angle(45.0 * Unit::deg), Angle(30.0 * Unit::deg), Equinox::tod(Time::b1900()));
  Equatorial d1 =  d.to_icrs().to_tod(Time::b1900());
  if(!test.equals("to_icrs().to_tod().longitude()", d1.longitude().deg(), d.longitude().deg(), 1e-12)) n++;
  if(!test.equals("to_icrs().to_tod().latitude()", d1.latitude().deg(), d.latitude().deg(), 1e-12)) n++;
  if(!test.equals("reference_system(TOD)", d.reference_system(), NOVAS_TRUE_EQUATOR)) n++;
  if(!test.equals("jd(B1900)", d.system().jd(), NOVAS_JD_B1900)) n++;
  if(!test.check("to_tod(B1900)", d.to_tod(NOVAS_JD_B1900) == d)) n++;
  if(!test.equals("to_string(TOD B1900)", d.to_string(NOVAS_SEP_COLONS), "EQU 03:00:00.0000    30:00:00.000  TOD J1900.001")) n++;

  Equatorial e = Equatorial(Angle(20.0 * Unit::deg), Angle(15.0 * Unit::deg), Equinox::icrs());
  if(!test.equals("distance_to()", e.distance_to(a).deg(),
          novas::novas_sep(e.longitude().deg(), e.latitude().deg(), a.longitude().deg(), a.latitude().deg()), 0.1 * Unit::uas)) n++;

  double lon = 0.0, lat = 0.0;
  novas::equ2ecl(a.system().jd(), a.system().equator_type(), novas::NOVAS_FULL_ACCURACY, a.ra().hours(), a.dec().deg(), &lon, &lat);
  Ecliptic ec0 = Ecliptic(lon * Unit::deg, lat * Unit::deg);
  if(!test.check("to_ecliptic()", a.to_ecliptic() == ec0)) {
    std::cout << "  " << a.to_ecliptic().to_string(novas::NOVAS_SEP_COLONS, 6) << " != "
            <<  ec0.to_string(novas::NOVAS_SEP_COLONS, 6) << "\n";
    n++;
  }

  double blon = 0.0, blat = 0.0;
  novas::equ2gal(a.ra().hours(), a.dec().deg(), &blon, &blat);
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

  Equatorial f = Equatorial(xyz, Equinox::icrs());
  if(!test.check("Equatorial(xyz)", f == a)) n++;

  std::cout << "Equatorial.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
