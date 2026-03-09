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
  TestUtil test = TestUtil("Geometric");

  int n = 0;

  Geometric x = Geometric::invalid();
  if(!test.check("invalid", !x.is_valid())) n++;
  if(!test.check("invalid frame", !x.frame().is_valid())) n++;
  if(!test.check("invalid pos", !x.position().is_valid())) n++;
  if(!test.check("invalid vel", !x.velocity().is_valid())) n++;
  if(!test.equals("invalid system_type()", x.system_type(), -1)) n++;
  if(!test.check("invalid to_system()", !x.to_icrs().is_valid())) n++;

  Frame frame = Observer::at_geocenter().reduced_accuracy_frame_at(Time::j2000());

  if(!test.check("invalid (pos invalid)", !Geometric(frame, Position::invalid(), Velocity::stationary()).is_valid())) n++;
  if(!test.check("invalid (vel invalid)", !Geometric(frame, Position::origin(), Velocity::invalid()).is_valid())) n++;
  if(!test.check("invalid (frame invalid)", !Geometric(Frame::invalid(), Position::origin(), Velocity::stationary()).is_valid())) n++;
  if(!test.check("invalid (system -1)", !Geometric(frame, Position::origin(), Velocity::stationary(), (enum novas_reference_system) -1).is_valid())) n++;

  Position pos(1.0 * Unit::pc, 2.0 * Unit::pc, 3.0 * Unit::pc);
  Velocity vel(-1.1 * Unit::km / Unit::s, -2.2 * Unit::km / Unit::s, -3.3 * Unit::km / Unit::s);

  Geometric a = frame.geometric(pos, vel);
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.check("position()", a.position() == pos)) n++;
  if(!test.check("velocity()", a.velocity() == vel)) n++;
  if(!test.equals("frame().observer().type()", a.frame().observer().type(), NOVAS_OBSERVER_AT_GEOCENTER)) n++;
  if(!test.check("frame().time()", a.frame().time() == frame.time())) n++;
  if(!test.equals("system_type()", a.system_type(), NOVAS_TOD)) n++;
  if(!test.check("equatorial()", a.equatorial().xyz(pos.distance()) == pos)) n++;
  if(!test.check("ecliptic()", a.ecliptic() == a.equatorial().to_ecliptic())) n++;
  if(!test.check("galactic()", a.galactic() == a.equatorial().to_galactic())) n++;

  Geometric a1 = (a >> NOVAS_ICRS);
  if(!test.equals("operator>>().system_type()", a1.system_type(), NOVAS_ICRS)) n++;
  if(!test.check("operator>>().position()", a1.position() == a.to_icrs().position())) n++;
  if(!test.check("operator>>().velocity()", a1.velocity() == a.to_icrs().velocity())) n++;

  double pos1[3] = {0.0}, vel1[3] = {0.0};
  tod_to_gcrs(frame.time().jd(NOVAS_TDB), frame.accuracy(), pos._array(), pos1);
  tod_to_gcrs(frame.time().jd(NOVAS_TDB), frame.accuracy(), vel._array(), vel1);

  if(!test.equals("to_icrs().system_type()", a1.system_type(), NOVAS_ICRS)) n++;
  if(!test.check("to_icrs().position()", a1.position() == Position(pos1))) n++;
  if(!test.check("to_icrs().velocity()", a1.velocity() == Velocity(vel1))) n++;

  Geometric a2 = a.to_j2000();
  tod_to_j2000(frame.time().jd(NOVAS_TDB), frame.accuracy(), pos._array(), pos1);
  tod_to_j2000(frame.time().jd(NOVAS_TDB), frame.accuracy(), vel._array(), vel1);

  if(!test.equals("to_j2000().system_type()", a2.system_type(), NOVAS_J2000)) n++;
  if(!test.check("to_j2000().position()", a2.position() == Position(pos1))) n++;
  if(!test.check("to_j2000().velocity()", a2.velocity() == Velocity(vel1))) n++;

  Geometric a3 = a.to_mod();
  nutation(frame.time().jd(NOVAS_TDB), NUTATE_TRUE_TO_MEAN, frame.accuracy(), pos._array(), pos1);
  nutation(frame.time().jd(NOVAS_TDB), NUTATE_TRUE_TO_MEAN, frame.accuracy(), vel._array(), vel1);

  if(!test.equals("to_mod().system_type()", a3.system_type(), NOVAS_MOD)) n++;
  if(!test.check("to_mod().position()", a3.position() == Position(pos1))) n++;
  if(!test.check("to_mod().velocity()", a3.velocity() == Velocity(vel1))) n++;

  Geometric a4 = a.to_cirs();
  tod_to_cirs(frame.time().jd(), frame.accuracy(), pos._array(), pos1);
  tod_to_cirs(frame.time().jd(), frame.accuracy(), vel._array(), vel1);

  if(!test.equals("to_cirs().system_type()", a4.system_type(), NOVAS_CIRS)) n++;
  if(!test.check("to_cirs().position()", a4.position() == Position(pos1))) n++;
  if(!test.check("to_cirs().velocity()", a4.velocity() == Velocity(vel1))) n++;

  Geometric a5 = a.to_tod();
  if(!test.equals("to_tod().system_type()", a5.system_type(), NOVAS_TOD)) n++;
  if(!test.check("to_tod().position()", a5.position() == a.position())) n++;
  if(!test.check("to_tod().velocity()", a5.velocity() == a.velocity())) n++;

  EOP eop(32, 0.1, 0.2 * Unit::arcsec, 0.3 * Unit::arcsec);
  Site site(25.0 * Unit::deg, -40.0 * Unit::deg, 600.0 * Unit::m);
  frame = Observer::on_earth(site, eop).reduced_accuracy_frame_at(Time::j2000());

  novas_transform T = {};

  novas_make_transform(frame._novas_frame(), NOVAS_TOD, NOVAS_TIRS, &T);
  novas_transform_vector(pos._array(), &T, pos1);
  novas_transform_vector(vel._array(), &T, vel1);

  Geometric b = frame.geometric(pos, vel);

  Geometric b1 = b.to_tirs();
  if(!test.equals("to_tirs().system_type()", b1.system_type(), NOVAS_TIRS)) n++;
  if(!test.check("to_tirs().position()", b1.position() == Position(pos1))) n++;
  if(!test.check("to_tirs().velocity()", b1.velocity() == Velocity(vel1))) n++;

  std::optional<Geometric> opt = a.to_itrs();
  if(!test.check("to_itrs(gc).has_value()", !opt.has_value())) n++;

  opt = b.to_itrs(eop);
  if(!test.check("to_itrs().has_value()", opt.has_value())) n++;
  else {
    Geometric b2 = opt.value();
    novas_frame f = {};

    novas_make_frame(NOVAS_REDUCED_ACCURACY, frame.observer()._novas_observer(), frame.time()._novas_timespec(), eop.xp().mas(), eop.yp().mas(), &f);
    novas_make_transform(&f, NOVAS_TOD, NOVAS_ITRS, &T);
    novas_transform_vector(pos._array(), &T, pos1);
    novas_transform_vector(vel._array(), &T, vel1);

    if(!test.equals("to_itrs().system_type()", b2.system_type(), NOVAS_ITRS)) n++;
    if(!test.check("to_itrs().position()", b2.position() == Position(pos1))) n++;
    if(!test.check("to_itrs().velocity()", b2.velocity() == Velocity(vel1))) n++;

    Geometric b3 = b2.to_itrs(eop).value();
    if(!test.equals("to_itrs(ITRS).system_type()", b3.system_type(), NOVAS_ITRS)) n++;
    if(!test.check("to_itrs(ITRS).position()", b3.position() == b2.position())) n++;
    if(!test.check("to_itrs(ITRS).velocity()", b3.velocity() == b2.velocity())) n++;
  }

  opt = b.to_system(NOVAS_ITRS);
  if(!test.check("to_itrs().has_value()", opt.has_value())) n++;
  else {
    Geometric b2 = opt.value();
    novas_frame f = {};

    novas_make_frame(NOVAS_REDUCED_ACCURACY, frame.observer()._novas_observer(), frame.time()._novas_timespec(), eop.xp().mas(), eop.yp().mas(), &f);
    novas_make_transform(&f, NOVAS_TOD, NOVAS_ITRS, &T);
    novas_transform_vector(pos._array(), &T, pos1);
    novas_transform_vector(vel._array(), &T, vel1);

    if(!test.equals("to_itrs(internal EOP).system_type()", b2.system_type(), NOVAS_ITRS)) n++;
    if(!test.check("to_itrs(internal EOP).position()", b2.position() == Position(pos1))) n++;
    if(!test.check("to_itrs(internal EOP).velocity()", b2.velocity() == Velocity(vel1))) n++;
  }

  // Spoof a full accuracy frame (normally we cannot create one by the rules without an ephemeris provider...)
  novas_frame *f1 = (novas_frame *) b.frame()._novas_frame();
  f1->accuracy = NOVAS_FULL_ACCURACY;
  opt = b.to_itrs();
  if(!test.check("to_itrs().has_value()", opt.has_value())) n++;
  else {
    Geometric b2 = opt.value();
    novas_frame f = {};

    double xp, yp;
    novas_diurnal_eop_at_time(frame.time()._novas_timespec(), &xp, &yp, NULL);

    novas_make_frame(NOVAS_REDUCED_ACCURACY, frame.observer()._novas_observer(), frame.time()._novas_timespec(),
            eop.xp().mas() + 1000.0 * xp, eop.yp().mas() + 1000.0 * yp, &f);

    f.accuracy = NOVAS_FULL_ACCURACY;

    novas_make_transform(&f, NOVAS_TOD, NOVAS_ITRS, &T);
    novas_transform_vector(pos._array(), &T, pos1);
    novas_transform_vector(vel._array(), &T, vel1);

    if(!test.equals("to_itrs(hp).system_type()", b2.system_type(), NOVAS_ITRS)) n++;
    if(!test.check("to_itrs(hp).position()", b2.position() == Position(pos1))) {
      std::cout << "### " << b2.position().to_string(9) << ", " << Position(pos1).to_string(9) << "\n";
      n++;
    }
    if(!test.check("to_itrs(hp).velocity()", b2.velocity() == Velocity(vel1))) n++;
  }

  std::cout << "Geometric.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
