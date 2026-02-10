/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>
#include <string.h>

#include "TestUtil.hpp"

using namespace novas;

int main() {
  TestUtil test = TestUtil("Apparent");

  int n = 0;

  Apparent x = Apparent::invalid();
  if(!test.check("invalid", !x.is_valid())) n++;
  if(!test.check("invalid frame", !x.frame().is_valid())) n++;
  if(!test.check("invalid to_horizontal()", !x.to_horizontal().has_value())) n++;

  sky_pos p = {};
  p.ra = 3.0;
  p.dec = -15.0;
  radec2vector(p.ra, p.dec, 1.0, p.r_hat);
  p.rv = -133.0;
  p.dis = Unit::pc / Unit::au;

  EOP eop(32, 0.1, 0.2 * Unit::arcsec, 0.3 * Unit::arcsec);
  Frame frame(Observer::at_geocenter(), Time::j2000(), NOVAS_REDUCED_ACCURACY);

  Apparent tod = Apparent::from_tod_sky_pos(p, frame);
  if(!test.check("equatorial()", tod.equatorial() == Equatorial(p.ra * Unit::hour_angle, p.dec * Unit::deg, Equinox::tod(Time::j2000())))) n++;
  if(!test.equals("frame().observer()", tod.frame().observer().type(), NOVAS_OBSERVER_AT_GEOCENTER)) n++;
  if(!test.check("frame().time()", tod.frame().time() == Time::j2000())) n++;
  if(!test.equals("frame().accuracy()", tod.frame().accuracy(), NOVAS_REDUCED_ACCURACY)) n++;
  if(!test.equals("_sky_pos().ra", tod._sky_pos()->ra, p.ra, 1e-14)) n++;
  if(!test.equals("_sky_pos().dec", tod._sky_pos()->dec, p.dec, 1e-13)) n++;
  if(!test.check("_sky_pos().r_hat", Position(tod._sky_pos()->r_hat, Unit::AU) == Position(p.r_hat, Unit::AU))) n++;
  if(!test.equals("_sky_pos().rv", tod._sky_pos()->rv, p.rv, 1e-13)) n++;
  if(!test.equals("_sky_pos().dis", tod._sky_pos()->dis, p.dis, 1e-12)) n++;
  if(!test.check("xyz()", tod.xyz() == Position(p.r_hat, p.dis * Unit::AU))) n++;
  if(!test.equals("radial_velocity()", tod.radial_velocity().km_per_s(), p.rv, 1e-15)) n++;
  if(!test.equals("redshift()", tod.redshift(), novas_v2z(p.rv), 1e-18)) n++;
  if(!test.equals("distance()", tod.distance().au(), p.dis, 1e-12)) n++;
  if(!test.check("ecliptic()", tod.ecliptic() == tod.equatorial().to_ecliptic())) n++;
  if(!test.check("galactic()", tod.galactic() == tod.equatorial().to_galactic())) n++;
  if(!test.check("horizontal(gc)", !tod.to_horizontal().has_value())) n++;

  double ra_cirs = app_to_cirs_ra(frame.time().jd(), NOVAS_REDUCED_ACCURACY, p.ra);
  if(!test.check("cirs()", tod.cirs() == Equatorial(ra_cirs * Unit::hour_angle, p.dec * Unit::deg, Equinox::cirs(Time::j2000())))) n++;

  Apparent tod2 = Apparent::tod(Angle(p.ra * Unit::hour_angle), Angle(p.dec * Unit::deg), frame, Speed(p.rv * Unit::km / Unit::s));
  if(!test.check("tod(Angle...)", tod2.equatorial() == tod.equatorial())) n++;

  Apparent cirs = Apparent::from_cirs_sky_pos(p, frame);
  double ra_tod = cirs_to_app_ra(frame.time().jd(), NOVAS_REDUCED_ACCURACY, p.ra);
  if(!test.check("cirs(CIRS)", cirs.cirs() == Equatorial(p.ra * Unit::hour_angle, p.dec * Unit::deg, Equinox::cirs(Time::j2000())))) n++;
  if(!test.check("equatorial(CIRS)", cirs.equatorial() == Equatorial(ra_tod * Unit::hour_angle, p.dec * Unit::deg, Equinox::tod(Time::j2000())))) n++;

  Apparent cirs2 = Apparent::cirs(Angle(p.ra * Unit::hour_angle), Angle(p.dec * Unit::deg), frame, Speed(p.rv * Unit::km / Unit::s));
  if(!test.check("cirs(Angle...)", cirs2.cirs() == cirs.cirs())) n++;

  Apparent::from_tod_sky_pos(p, frame);

  sky_pos p1 = p; p1.ra = NAN;
  if(!test.check("invalid p.ra", !Apparent::from_tod_sky_pos(p1, frame).is_valid())) n++;

  p1 = p; p1.dec = NAN;
  if(!test.check("invalid p.dec", !Apparent::from_tod_sky_pos(p1, frame).is_valid())) n++;

  p1 = p; p1.rv = NAN;
  if(!test.check("invalid p.rv", !Apparent::from_tod_sky_pos(p1, frame).is_valid())) n++;

  p1 = p; p1.rv = Constant::c + 1.0;
  if(!test.check("invalid p.rv > c", !Apparent::from_tod_sky_pos(p1, frame).is_valid())) n++;

  p1 = p; p1.dis = NAN;
  if(!test.check("invalid p.dis", !Apparent::from_tod_sky_pos(p1, frame).is_valid())) n++;

  Site site(-15.0 * Unit::deg, 42.0 * Unit::deg, 268.0 * Unit::m);
  frame = Frame(Observer::on_earth(site, eop), Time::j2000(), NOVAS_REDUCED_ACCURACY);

  double az = 0.0, el = 0.0;
  novas_app_to_hor(frame._novas_frame(), NOVAS_TOD, p.ra, p.dec, NULL, &az, &el);
  std::optional<Horizontal> opt = Apparent::from_tod_sky_pos(p, frame).to_horizontal();

  if(!test.check("to_horizontal(site)", opt.has_value())) n++;
  if(!test.equals("to_horizontal() az", opt.value().azimuth().deg(), az, 1e-13)) n++;
  if(!test.equals("to_horizontal() el", opt.value().elevation().deg(), el, 1e-13)) n++;


  std::cout << "Apparent.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
