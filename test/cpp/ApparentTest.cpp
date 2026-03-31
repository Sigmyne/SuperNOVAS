/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>
#include <string.h>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Apparent");

  int n = 0;

  Apparent x = Apparent::undefined();
  if(!test.check("is_valid(invalid)", !x.is_valid())) n++;
  if(!test.check("frame(invalid)", !x.frame().is_valid())) n++;
  if(!test.check("to_horizontal(invalid)", !x.to_horizontal().is_valid())) n++;
  if(!test.check("xyz(invalid)", !x.xyz().is_valid())) n++;
  if(!test.check("radial_velocity(invalid)", !x.radial_velocity().is_valid())) n++;
  if(!test.check("distance(invalid)", !x.distance().is_valid())) n++;
  if(!test.check("equatorial(invalid)", !x.equatorial().is_valid())) n++;
  if(!test.check("to_cirs(invalid)", !x.cirs().is_valid())) n++;
  if(!test.check("ecliptic(invalid)", !x.ecliptic().is_valid())) n++;
  if(!test.check("galactic(invalid)", !x.galactic().is_valid())) n++;
  if(!test.check("astrometric_position(invalid)", !x.astrometric_position().is_valid())) n++;

  sky_pos p = {};
  p.ra = 3.0;
  p.dec = -15.0;
  radec2vector(p.ra, p.dec, 1.0, p.r_hat);
  p.rv = -133.0;
  p.dis = Unit::pc / Unit::au;

  if(!test.check("from_tod_sky_pos(invalid frame)", !Apparent::from_tod_sky_pos(Frame::undefined(), &p).is_valid())) n++;
  if(!test.check("from_cirs_sky_pos(invalid frame)", !Apparent::from_cirs_sky_pos(Frame::undefined(), &p).is_valid())) n++;

  EOP eop(32, 0.1, 0.2 * Unit::arcsec, 0.3 * Unit::arcsec);
  Frame frame = Observer::at_geocenter().reduced_accuracy_frame_at(Time::j2000());

  if(!test.check("from_tod_sky_pos(pos=NULL)", !Apparent::from_tod_sky_pos(frame, NULL).is_valid())) n++;
  if(!test.check("from_cirs_sky_pos(pos=NULL)", !Apparent::from_cirs_sky_pos(frame, NULL).is_valid())) n++;

  Apparent tod = Apparent::from_tod_sky_pos(frame, &p);
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
  if(!test.check("horizontal(gc)", !tod.to_horizontal().is_valid())) n++;
  if(!test.equals("to_string()", tod.to_string(), "Apparent EQU 03h 00m 00.0000s   -15d 00m 00.000s  TOD J2000 in Frame for Geocentric Observer at 2000-01-01T11:58:55.816 UTC")) n++;

  double v[3] = {0.0};
  novas_app_to_geom(frame._novas_frame(), NOVAS_TOD, p.ra, p.dec, p.dis, v);
  AstrometricPosition rp = tod.astrometric_position();
  if(!test.check("astrometric_position()", rp.is_valid())) n++;
  if(!test.check("astrometric_position() ==", rp == Position(v, Unit::AU))) n++;
  if(!test.check("astrometric_position(frame invalid)", !x.astrometric_position().is_valid())) n++;

  double ra_cirs = app_to_cirs_ra(frame.jd(), NOVAS_REDUCED_ACCURACY, p.ra);
  if(!test.check("to_cirs()", tod.cirs() == Equatorial(ra_cirs * Unit::hour_angle, p.dec * Unit::deg, Equinox::cirs(Time::j2000())))) n++;

  Apparent tod2 = Apparent::from_tod(Angle::undefined(), Angle(p.dec * Unit::deg), frame, ScalarVelocity(p.rv * Unit::km / Unit::s));
  if(!test.check("from_tod(invalid RA)", !tod2.is_valid())) n++;

  tod2 = Apparent::from_tod(Angle(p.ra * Unit::hour_angle), Angle(p.dec * Unit::deg), frame, ScalarVelocity(p.rv * Unit::km / Unit::s));
  if(!test.check("from_tod()", tod2.equatorial() == tod.equatorial())) n++;

  Apparent cirs = Apparent::from_cirs_sky_pos(frame, &p);
  double ra_tod = cirs_to_app_ra(frame.jd(), NOVAS_REDUCED_ACCURACY, p.ra);
  if(!test.check("to_cirs(CIRS)", cirs.cirs() == Equatorial(p.ra * Unit::hour_angle, p.dec * Unit::deg, Equinox::cirs(Time::j2000())))) n++;
  if(!test.check("equatorial(CIRS)", cirs.equatorial() == Equatorial(ra_tod * Unit::hour_angle, p.dec * Unit::deg, Equinox::tod(Time::j2000())))) n++;

  Apparent cirs2 = Apparent::from_cirs(Angle::undefined(), Angle(p.dec * Unit::deg), frame, ScalarVelocity(p.rv * Unit::km / Unit::s));
  if(!test.check("from_cirs(invalid RA)", !cirs2.is_valid())) n++;

  cirs2 = Apparent::from_cirs(Angle(p.ra * Unit::hour_angle), Angle(p.dec * Unit::deg), frame, ScalarVelocity(p.rv * Unit::km / Unit::s));
  if(!test.check("from_cirs()", cirs2.cirs() == cirs.cirs())) n++;

  if(!test.check("from_tod_sky_pos()", Apparent::from_tod_sky_pos(frame, &p).is_valid())) n++;

  sky_pos p1 = p; p1.ra = NAN;
  if(!test.check("invalid p.ra", !Apparent::from_tod_sky_pos(frame, &p1).is_valid())) n++;
  if(!test.check("to_horizontal(invalid RA)", !x.to_horizontal().is_valid())) n++;

  p1 = p; p1.dec = NAN;
  if(!test.check("invalid p.dec", !Apparent::from_tod_sky_pos(frame, &p1).is_valid())) n++;
  if(!test.check("to_horizontal(invalid Dec)", !x.to_horizontal().is_valid())) n++;

  p1 = p; p1.rv = NAN;
  if(!test.check("invalid p.rv", !Apparent::from_tod_sky_pos(frame, &p1).is_valid())) n++;

  p1 = p; p1.rv = Constant::c + 1.0;
  if(!test.check("from_to_sky_pos(p.rv > c)", !Apparent::from_tod_sky_pos(frame, &p1).is_valid())) n++;
  if(!test.check("from_cirs_sky_pos(p.rv > c)", !Apparent::from_cirs_sky_pos(frame, &p1).is_valid())) n++;

  p1 = p; p1.dis = NAN;
  if(!test.check("from_tod_sky_pos(invalid p.dis)", !Apparent::from_tod_sky_pos(frame, &p1).is_valid())) n++;
  if(!test.check("from_cirs_sky_pos(invalid p.dis)", !Apparent::from_cirs_sky_pos(frame, &p1).is_valid())) n++;

  Site site(-15.0 * Unit::deg, 42.0 * Unit::deg, 268.0 * Unit::m);
  frame = Observer::on_earth(site, eop).reduced_accuracy_frame_at(Time::j2000());

  double az = 0.0, el = 0.0;
  novas_app_to_hor(frame._novas_frame(), NOVAS_TOD, p.ra, p.dec, NULL, &az, &el);
  Horizontal opt = Apparent::from_tod_sky_pos(frame, &p).to_horizontal();

  if(!test.check("to_horizontal(site)", opt.is_valid())) n++;
  if(!test.equals("to_horizontal() az", opt.azimuth().deg(), az, 1e-13)) n++;
  if(!test.equals("to_horizontal() el", opt.elevation().deg(), el, 1e-13)) n++;

  p.ra = NAN;
  Apparent xra = Apparent::from_tod_sky_pos(frame, &p);
  if(!test.check("to_horizontal(invalid RA)", !xra.to_horizontal().is_valid())) n++;
  if(!test.check("astrometric_position(invalid RA)", !xra.astrometric_position().is_valid())) n++;


  std::cout << "Apparent.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
