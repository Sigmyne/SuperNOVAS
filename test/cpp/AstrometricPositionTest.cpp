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
  Equatorial los(12.345 * Unit::hour_angle, -23.456 * Unit::deg, Equinox::b1950());

  Observer xo = Observer::in_solar_system(Position::undefined(), Velocity::stationary());

  AstrometricPosition x = AstrometricPosition(Position::undefined(), frame);
  if(!test.check("is_valid(pos invalid)", !x.is_valid())) n++;
  if(!test.check("to_equatorial(pos invalid)", !x.to_equatorial().is_valid())) n++;
  if(!test.check("to_interferometric(pos invalid)", !x.to_interferometric(los).is_valid())) n++;

  x = AstrometricPosition(Position::origin(), Frame::undefined());
  if(!test.check("is_valid(frame invalid)", !x.is_valid())) n++;
  if(!test.check("obs_time(frame invalid)", !x.obs_time().is_valid())) n++;
  if(!test.check("referenced_to_ssb(frame invalid)", !x.referenced_to_ssb().is_valid())) n++;

  if(!test.check("is_valid(obs pos invalid)", !AstrometricPosition(Position::origin(), xo.reduced_accuracy_frame_at(Time::j2000())).is_valid())) n++;
  if(!test.check("is_valid(system invalid)", !AstrometricPosition(Position::origin(), frame, (enum novas_reference_system) -1).is_valid())) n++;

  Position p = Position(-1.123456789 * Unit::au, 2.123456789 * Unit::au, -3.123456789 * Unit::au);
  AstrometricPosition a(p, frame, NOVAS_MOD);
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.check(" == position", a == p)) n++;
  if(!test.equals("system_type()", a.system_type(), NOVAS_MOD)) n++;
  if(!test.check("reference()", a.reference() == frame.observer_ssb_position())) n++;
  if(!test.check("obs_time()", a.obs_time() == Time::b1950())) n++;
  if(!test.check("emit_time()", a.emit_time() == (Time::b1950() - (a.distance().m() / Constant::c)))) n++;
  if(!test.check("origin(0)", a.reference() == frame.observer_ssb_position())) n++;

  Velocity vrel(1.0 * Unit::km_per_s, -2.0 * Unit::km_per_s, 3.0 * Unit::km_per_s);
  Coordinate dist(Unit::Gpc);
  double uvw[3] = {0.0};
  novas_uvw(a.scaled(1.0 / Unit::AU)._array(), vrel.scaled(Unit::day / Unit::AU)._array(),
          los.xyz(dist).scaled(1.0 / Unit::AU)._array(), uvw);

  Interferometric u = a.to_interferometric(los, dist, vrel);
  if(!test.check("to_interferometric()", u.is_valid())) n++;
  if(!test.equals("to_interferometric() u", u[0], uvw[0], 1e-9)) n++;
  if(!test.equals("to_interferometric() v", u[1], uvw[1], 1e-9)) n++;
  if(!test.equals("to_interferometric() w", u[2], uvw[2], 1e-9)) n++;
  if(!test.check("to_interferometric(phase center invalid)", !a.to_interferometric(Equatorial::undefined()).is_valid())) n++;

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
  if(!test.check("to_equatorial()", a.to_equatorial().is_valid())) n++;
  if(!test.equals("to_equatorial().ra()", a.to_equatorial().ra().hours(), ra, 1e-13)) n++;
  if(!test.equals("to_equatorial().dec()", a.to_equatorial().dec().deg(), dec, 1e-12)) n++;


  if(!test.equals("to_string()", a.to_string(),
          "Position (-1.123 AU, 2.123 AU, -3.123 AU) at 1949-12-31T21:36:28.371 UTC from SSB Position (-0.181 AU, 0.889 AU, 0.385 AU)")) n++;

  std::cout << "AstrometricPosition.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
