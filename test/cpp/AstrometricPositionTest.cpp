/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

int main() {
  TestUtil test = TestUtil("AstrometricPosition");

  int n = 0;

  Observer obs = Observer::at_geocenter();
  Frame frame = obs.reduced_accuracy_frame_at(Time::b1950());

  Observer xo = Observer::in_solar_system(Position::undefined(), Velocity::stationary());

  AstrometricPosition x = AstrometricPosition(Position::undefined(), frame);
  if(!test.check("is_valid(pos invalid)", !x.is_valid())) n++;
  if(!test.check("as_equatorial(pos invalid)", !x.as_equatorial().is_valid())) n++;

  x = AstrometricPosition(Position::origin(), Frame::undefined());
  if(!test.check("is_valid(frame invalid)", !x.is_valid())) n++;
  if(!test.check("obs_time(frame invalid)", !x.obs_time().is_valid())) n++;
  if(!test.check("referenced_to_ssb(frame invalid)", !x.referenced_to_ssb().is_valid())) n++;

  if(!test.check("is_valid(obs pos invalid)", !AstrometricPosition(Position::origin(), xo.reduced_accuracy_frame_at(Time::j2000())).is_valid())) n++;
  if(!test.check("is_valid(system invalid)", !AstrometricPosition(Position::origin(), frame, (enum novas_reference_system) -1).is_valid())) n++;

  Position p = Position(-1.123456789 * Unit::au, 2.123456789 * Unit::au, -3.123456789 * Unit::au);
  AstrometricPosition a(p, frame);
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.check(" == position", a == p)) n++;
  if(!test.equals("system_type()", a.system_type(), NOVAS_TOD)) n++;
  if(!test.check("reference()", a.reference() == frame.observer_ssb_position())) n++;
  if(!test.check("obs_time()", a.obs_time() == Time::b1950())) n++;
  if(!test.check("emit_time()", a.emit_time() == (Time::b1950() - (a.distance().m() / Constant::c)))) n++;
  if(!test.check("origin(0)", a.reference() == frame.observer_ssb_position())) n++;

  Position r = Position(1.3 * Unit::AU, -2.2 * Unit::AU, 3.1 * Unit::AU);
  Observer o2 = Observer::in_solar_system(r, Velocity::stationary());
  AstrometricPosition b(p, o2.reduced_accuracy_frame_at(Time::b1950()));
  if(!test.check("reference()", b.reference() == r)) n++;

  if(!test.check("referenced_to()", a.referenced_to(r) == (p + frame.observer_ssb_position() - r))) n++;
  if(!test.check("referenced_to().reference()", a.referenced_to(r).reference() == r)) n++;
  if(!test.check("referenced_to(invalid pos)", !a.referenced_to(Position::undefined()).is_valid())) n++;

  if(!test.check("referenced_to_ssb()", b.referenced_to_ssb() == (p + r))) n++;
  if(!test.check("referenced_to_ssb().reference()", b.referenced_to_ssb().reference() == Position::origin())) n++;

  double ra = 0.0, dec = 0.0;
  vector2radec(a._array(), &ra, &dec);
  if(!test.check("as_equatorial()", a.as_equatorial().is_valid())) n++;
  if(!test.equals("as_equatorial().ra()", a.as_equatorial().ra().hours(), ra, 1e-13)) n++;
  if(!test.equals("as_equatorial().dec()", a.as_equatorial().dec().deg(), dec, 1e-12)) n++;

  if(!test.equals("to_string()", a.to_string(),
          "Position (-1.123 AU, 2.123 AU, -3.123 AU) at 1949-12-31T21:36:28.371 UTC from SSB Position (-0.181 AU, 0.889 AU, 0.385 AU)")) n++;

  std::cout << "AstrometricPosition.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
