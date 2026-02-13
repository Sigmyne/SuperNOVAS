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
  TestUtil test = TestUtil("Horizontal");

  int n = 0;

  if(!test.check("invalid lon", !Horizontal(NAN, 30.0 * Unit::deg).is_valid())) n++;
  if(!test.check("invalid lat", !Horizontal(45.0 * Unit::deg, NAN).is_valid())) n++;
  if(!test.check("invalid lat > 90", !Horizontal(45.0 * Unit::deg, 91.0 * Unit::deg).is_valid())) n++;

  Horizontal x = Horizontal::invalid();
  if(!test.check("is_valid() invalid", !x.is_valid())) n++;
  if(!test.check("longitude() invalid", isnan(x.longitude().rad()))) n++;
  if(!test.check("latitude() invalid", isnan(x.latitude().rad()))) n++;

  Horizontal a(-20.0 * Unit::deg, -30.0 * Unit::deg);
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("azimuth()", a.azimuth().deg(), -20.0, 1e-14)) n++;
  if(!test.equals("elevation()", a.elevation().deg(), -30.0, 1e-14)) n++;
  if(!test.equals("zenith_angle()", a.zenith_angle().deg(), 120.0, 1e-13)) n++;
  if(!test.equals("to_string()", a.to_string(), "HOR  -20d 00m 00.000s  -30d 00m 00.000s")) n++;

  Site site(15.0 * Unit::deg, -42.0 * Unit::deg, 1.5 * Unit::km);
  Horizontal a1 = a.to_refracted(novas_standard_refraction,  Weather::guess(site));
  on_surface s = *site._on_surface();
  novas_set_default_weather(&s);

  double r = novas_standard_refraction(NAN, &s, NOVAS_REFRACT_ASTROMETRIC, a.elevation().deg());
  if(!test.equals("to_refracted() az", a1.azimuth().deg(), a.azimuth().deg(), 1e-6)) n++;
  if(!test.equals("to_refracted() el", a1.elevation().deg(), a.elevation().deg() + r, 1e-6)) n++;

  Horizontal a2 = a1.to_unrefracted(novas_standard_refraction,  Weather::guess(site));
  r = novas_standard_refraction(NAN, &s, NOVAS_REFRACT_OBSERVED, a1.elevation().deg());
  if(!test.equals("to_refracted() az", a2.azimuth().deg(), a.azimuth().deg(), 1e-6)) n++;
  if(!test.equals("to_refracted() el", a2.elevation().deg(), a.elevation().deg(), 1e-6)) n++;

  if(!test.check("to_refracted(NULL)", a.to_refracted(NULL) == a)) n++;
  if(!test.check("to_unrefracted(NULL)", a.to_unrefracted(NULL) == a)) n++;

  std::optional<Apparent> app = a.to_apparent(Frame(Observer::at_geocenter(), Time::j2000(), NOVAS_REDUCED_ACCURACY),
          Speed(Unit::km / Unit::s), Distance(Unit::pc));
  if(!test.check("to_apparent(geocentric)", !app.has_value())) n++;

  app = a.to_apparent(Frame::invalid(), Speed(Unit::km / Unit::s), Distance(Unit::pc));
  if(!test.check("to_apparent(Frame invalid).has_value()", !app.has_value())) n++;

  EOP eop(32, 0.1, 0.2 * Unit::arcsec, 0.3 * Unit::arcsec);
  Frame frame(Observer::on_earth(site, eop), Time::j2000(), NOVAS_REDUCED_ACCURACY);

  app = a.to_apparent(frame, Speed(Unit::km / Unit::s), Distance(Unit::pc));
  if(!test.check("to_apparent().has_value()", app.has_value())) n++;

  Apparent tod = app.value();
  sky_pos p = {};
  novas_hor_to_app(frame._novas_frame(), a.azimuth().deg(), a.elevation().deg(), NULL, NOVAS_TOD, &p.ra, &p.dec);
  if(!test.equals("to_apparent() R.A.", tod.equatorial().ra().hours(), p.ra, 1e-10)) n++;
  if(!test.equals("to_apparent() Dec", tod.equatorial().dec().deg(), p.dec, 1e-9)) n++;

  Horizontal b(a);
  if(!test.check("operator==()", b == a)) n++;
  if(!test.check("operator!=() !", !(b != a))) n++;

  Horizontal c(Angle(10.0 * Unit::deg), Angle(-20.0 * Unit::deg));
  if(!test.equals("azimuth(Angle)", c.azimuth().deg(), 10.0, 1e-14)) n++;
  if(!test.equals("elevation(Angle)", c.elevation().deg(), -20.0, 1e-14)) n++;
  if(!test.check("operator==() !", !(c == a))) n++;
  if(!test.check("operator!=()", c != a)) n++;

  if(!test.equals("zenith_angle()", c.distance_to(a).deg(), novas_sep(c.azimuth().deg(), c.elevation().deg(), a.azimuth().deg(), a.elevation().deg()), 1e-13)) n++;

  Horizontal d("-20h 00m 00.000s", "-30 00 00");
  if(!test.check("operator==(string)", d == a)) n++;
  if(!test.check("equals()", d.equals(a, Angle(Unit::uas)))) n++;

  std::cout << "Horizontal.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
