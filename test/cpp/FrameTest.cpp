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

  if(!test.check("invalid", !Frame::invalid().is_valid())) n++;
  if(!test.check("invalid observer", !Frame(Observer::invalid(), Time::j2000(), (enum novas_accuracy) -1).is_valid())) n++;
  if(!test.check("invalid time", !Frame(gc, Time::invalid(), (enum novas_accuracy) -1).is_valid())) n++;
  if(!test.check("invalid accuracy", !Frame(gc, Time::j2000(), (enum novas_accuracy) -1).is_valid())) n++;

  Frame a(gc, Time::j2000(), NOVAS_REDUCED_ACCURACY);
  if(!test.equals("accuracy()", a.accuracy(), NOVAS_REDUCED_ACCURACY)) n++;
  if(!test.check("time()", a.time() == Time::j2000())) n++;
  if(!test.equals("observer() type", a.observer().type(), NOVAS_OBSERVER_AT_GEOCENTER)) n++;
  if(!test.equals("clock_skew()", a.clock_skew(NOVAS_TT), novas_clock_skew(a._novas_frame(), NOVAS_TT))) n++;

  std::optional <Frame> b = Frame::create(gc, Time::j2000(), NOVAS_REDUCED_ACCURACY);
  if(!test.check("create().has_value()", b.has_value())) n++;

  std::optional <Frame> c = Frame::create(Observer::invalid(), Time::j2000(), NOVAS_REDUCED_ACCURACY);
  if(!test.check("create(invalid).has_value()", !c.has_value())) n++;

  EOP eop(32, 0.1, 0.2 * Unit::arcsec, 0.3 * Unit::arcsec);
  Site site(10.0 * Unit::deg, -20.0 * Unit::deg, 30.0 * Unit::m);
  Observer go = Observer::on_earth(site, eop);

  if(!test.check("is_valid(geodetic)", Frame(go, Time::j2000(), NOVAS_REDUCED_ACCURACY).is_valid())) n++;

  if(!test.equals("to_string()", a.to_string(), "Frame for Geocentric Observer at 2000-01-01T11:58:55.816 UTC")) n++;

  std::cout << "Frame.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
