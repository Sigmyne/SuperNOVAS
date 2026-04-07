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
  TestUtil test = TestUtil("Frame");

  int n = 0;

  GeocentricObserver gc = Observer::at_geocenter();

  Frame x = Frame::undefined();
  if(!test.check("invalid", !x.is_valid())) n++;
  if(!test.check("observer_ssb_position(invalid)", !x.observer_ssb_position().is_valid())) n++;
  if(!test.check("observer_ssb_velocity(invalid)", !x.observer_ssb_velocity().is_valid())) n++;
  if(!test.check("geometric_moon_elp2000(invalid)", !x.geometric_moon_elp2000().is_valid())) n++;
  if(!test.check("apparent_moon_elp2000(invalid)", !x.apparent_moon_elp2000().is_valid())) n++;

  if(!test.check("invalid observer", !Frame(Observer::undefined(), Time::j2000(), (enum novas_accuracy) -1).is_valid())) n++;
  if(!test.check("invalid time", !Frame(gc, Time::undefined(), (enum novas_accuracy) -1).is_valid())) n++;
  if(!test.check("invalid accuracy", !Frame(gc, Time::j2000(), (enum novas_accuracy) -1).is_valid())) n++;

  Frame a = Frame::reduced_accuracy(gc, Time::j2000());
  if(!test.equals("accuracy()", a.accuracy(), NOVAS_REDUCED_ACCURACY)) n++;
  if(!test.check("time()", a.time() == Time::j2000())) n++;
  if(!test.check("observer_ssb_position()", a.observer_ssb_position() == Position(a._novas_frame()->obs_pos, Unit::AU))) n++;
  if(!test.check("observer_ssb_velocity()", a.observer_ssb_velocity() == Velocity(a._novas_frame()->obs_vel, Unit::AU_per_day))) n++;
  if(!test.equals("observer() type", a.observer().type(), NOVAS_OBSERVER_AT_GEOCENTER)) n++;
  if(!test.equals("clock_skew()", a.clock_skew(NOVAS_TT), novas_clock_skew(a._novas_frame(), NOVAS_TT))) n++;
  if(!test.check("clock_skew(timescale invalid)", isnan(a.clock_skew((enum novas_timescale) -1)))) n++;
  if(!test.equals("to_string()", a.to_string(), "Frame for Geocentric Observer at 2000-01-01T11:58:55.816 UTC")) n++;

  a = a; // @suppress("Assignment to itself")
  if(!test.equals("self assign", a.to_string(), "Frame for Geocentric Observer at 2000-01-01T11:58:55.816 UTC")) n++;

  double mp[3] = {0.0}, mv[3] = {0.0};
  novas_moon_elp_posvel_fp(a._novas_frame(), 0.1, NOVAS_ICRS, mp, mv);
  Geometric mg = a.geometric_moon_elp2000(0.1);
  if(!test.check("geometric_moon_elp2000()", mg.is_valid())) n++;
  if(!test.check("geometric_moon_elp2000().position()", mg.position() == Position(mp, Unit::AU))) n++;
  if(!test.check("geometric_moon_elp2000().velocity()", mg.velocity() == Velocity(mv, Unit::AU_per_day))) n++;

  sky_pos mpos = {};
  novas_moon_elp_sky_pos_fp(a._novas_frame(), 0.1, NOVAS_TOD, &mpos);
  Apparent ma = a.apparent_moon_elp2000(0.1);
  if(!test.check("apparent_moon_elp2000()", ma.is_valid())) n++;
  if(!test.equals("apparent_moon_elp2000() ra", ma._sky_pos()->ra, mpos.ra, 1e-13)) n++;
  if(!test.equals("apparent_moon_elp2000() dec", ma._sky_pos()->dec, mpos.dec, 1e-12)) n++;
  if(!test.equals("apparent_moon_elp2000() dis", ma._sky_pos()->dis, mpos.dis, 1e-12)) n++;
  if(!test.equals("apparent_moon_elp2000() rv", ma._sky_pos()->rv, mpos.rv, 1e-9)) n++;
  if(!test.check("apparent_moon_elp2000() r_hat", Position(ma._sky_pos()->r_hat) == Position(mpos.r_hat))) n++;

  Frame b(gc, Time::j2000(), NOVAS_REDUCED_ACCURACY);
  if(!test.check("Frame(reduced accuracy).is_valid()", b.is_valid())) n++;

  b = Frame(gc, Time::j2000(), NOVAS_FULL_ACCURACY);
  if(!test.check("Frame(full accuracy).is_valid()", !b.is_valid())) n++;

  b = Frame(Observer::undefined(), Time::j2000());
  if(!test.check("Frame(obs invalid).is_valid()", !b.is_valid())) n++;

  b = Frame(gc, Time::undefined());
  if(!test.check("Frame(time invalid).is_valid()", !b.is_valid())) n++;

  Frame c = Frame::reduced_accuracy(Observer::undefined(), Time::j2000());
  if(!test.check("reduced_accuracy(invalid).is_valid()", !c.is_valid())) n++;

  EOP eop(32, 0.1, 0.2 * Unit::arcsec, 0.3 * Unit::arcsec);
  Site site(10.0 * Unit::deg, -20.0 * Unit::deg, 30.0 * Unit::m);
  GeodeticObserver go = Observer::on_earth(site, eop);

  if(!test.check("is_valid(geodetic)", Frame::reduced_accuracy(go, Time::j2000()).is_valid())) n++;


  test = TestUtil("Frame(geodetic)");

  Frame gf = Frame::reduced_accuracy(go, Time::j2000());
  if(!test.check("is_valid()", gf.is_valid())) n++;
  if(!test.check("observer().is_geodetic()", gf.observer().is_geodetic())) n++;
  if(!test.check("observer().is_geocentric()", !gf.observer().is_geocentric())) n++;
  if(!test.equals("to_string()", gf.to_string(), "Frame for GeodeticObserver at Site (E  10d 00m 00.000s, S  20d 00m 00.000s, altitude 30 m) at 2000-01-01T11:58:55.816 UTC")) n++;

  gf = Frame::reduced_accuracy(go, Time::undefined());
  if(!test.check("reduced_accuracy(invalid).is_valid()", !c.is_valid())) n++;

  gf = Frame(go, Time::j2000(), NOVAS_REDUCED_ACCURACY);
  if(!test.check("create(reduced accuracy).is_valid()", gf.is_valid())) n++;
  if(!test.check("observer().is_geodetic()", gf.observer().is_geodetic())) n++;
  if(!test.check("observer().is_geocentric()", !gf.observer().is_geocentric())) n++;

  gf = Frame(go, Time::j2000(), NOVAS_FULL_ACCURACY);
  if(!test.check("create(full accuracy).is_valid()", !gf.is_valid())) n++;

  std::cout << "Frame.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
