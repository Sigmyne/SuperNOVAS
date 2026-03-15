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
  TestUtil test = TestUtil("Frame");

  int n = 0;

  GeocentricObserver gc = Observer::at_geocenter();

  if(!test.check("invalid", !Frame::undefined().is_valid())) n++;
  if(!test.check("invalid observer", !Frame::create(Observer::undefined(), Time::j2000(), (enum novas_accuracy) -1).has_value())) n++;
  if(!test.check("invalid time", !Frame::create(gc, Time::undefined(), (enum novas_accuracy) -1).has_value())) n++;
  if(!test.check("invalid accuracy", !Frame::create(gc, Time::j2000(), (enum novas_accuracy) -1).has_value())) n++;

  Frame a = Frame::reduced_accuracy(gc, Time::j2000());
  if(!test.equals("accuracy()", a.accuracy(), NOVAS_REDUCED_ACCURACY)) n++;
  if(!test.check("time()", a.time() == Time::j2000())) n++;
  if(!test.equals("observer() type", a.observer().type(), NOVAS_OBSERVER_AT_GEOCENTER)) n++;
  if(!test.equals("clock_skew()", a.clock_skew(NOVAS_TT), novas_clock_skew(a._novas_frame(), NOVAS_TT))) n++;
  if(!test.equals("to_string()", a.to_string(), "Frame for Geocentric Observer at 2000-01-01T11:58:55.816 UTC")) n++;

  std::optional <Frame> b = Frame::create(gc, Time::j2000(), NOVAS_REDUCED_ACCURACY);
  if(!test.check("create(reduced accuracy).has_value()", b.has_value())) n++;

  b = Frame::create(gc, Time::j2000(), NOVAS_FULL_ACCURACY);
  if(!test.check("create(full accuracy).has_value()", !b.has_value())) n++;

  b = Frame::create(Observer::undefined(), Time::j2000());
  if(!test.check("create(obs invalid).has_value()", !b.has_value())) n++;

  b = Frame::create(gc, Time::undefined());
  if(!test.check("create(time invalid).has_value()", !b.has_value())) n++;

  std::optional <Frame> c = Frame::reduced_accuracy(Observer::undefined(), Time::j2000());
  if(!test.check("reduced_accuracy(invalid).has_value()", c.has_value())) n++;

  EOP eop(32, 0.1, 0.2 * Unit::arcsec, 0.3 * Unit::arcsec);
  Site site(10.0 * Unit::deg, -20.0 * Unit::deg, 30.0 * Unit::m);
  GeodeticObserver go = Observer::on_earth(site, eop);

  if(!test.check("is_valid(geodetic)", Frame::reduced_accuracy(go, Time::j2000()).is_valid())) n++;


  test = TestUtil("GeodeticFrame");

  GeodeticFrame gf = GeodeticFrame::reduced_accuracy(go, Time::j2000());
  if(!test.check("is_valid()", gf.is_valid())) n++;
  if(!test.check("observer().is_geodetic()", gf.observer().is_geodetic())) n++;
  if(!test.check("observer().is_geocentric()", !gf.observer().is_geocentric())) n++;
  if(!test.equals("to_string()", gf.to_string(), "Geodetic Frame for GeodeticObserver at Site (E  10d 00m 00.000s, S  20d 00m 00.000s, altitude 30 m) at 2000-01-01T11:58:55.816 UTC")) n++;

  gf = GeodeticFrame::reduced_accuracy(go, Time::undefined());
  if(!test.check("reduced_accuracy(invalid).has_value()", c.has_value())) n++;

  std::optional <GeodeticFrame> opt = GeodeticFrame::create(go, Time::j2000(), NOVAS_REDUCED_ACCURACY);
  if(!test.check("create(reduced accuracy).has_value()", opt.has_value())) n++;
  gf = opt.value();
  if(!test.check("observer().is_geodetic()", gf.observer().is_geodetic())) n++;
  if(!test.check("observer().is_geocentric()", !gf.observer().is_geocentric())) n++;

  opt = GeodeticFrame::create(go, Time::j2000(), NOVAS_FULL_ACCURACY);
  if(!test.check("create(full accuracy).has_value()", !opt.has_value())) n++;

  std::cout << "Frame.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
