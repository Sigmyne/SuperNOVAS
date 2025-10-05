/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace novas;


namespace supernovas {

Velocity::Velocity(double x_ms, double y_ms, double z_ms)
: Vector(x_ms, y_ms, z_ms) {}

Velocity::Velocity(const double vel[3], double unit)
: Vector(vel[0] * unit, vel[1] * unit, vel[2] * unit) {}


double Velocity::x_ms() const {
  return _component[0];
}

double Velocity::y_ms() const {
  return _component[1];
}

double Velocity::z_ms() const {
  return _component[2];
}

Speed Velocity::speed() const {
  return Speed(abs());
}

Speed Velocity::along(const Vector& v) const {
  return Speed(projection_on(v));
}

Position Velocity::travel(const Interval& t) const {
  return travel(t.seconds());
}

Position Velocity::travel(double seconds) const {
  return Position(x_ms() * seconds, y_ms() * seconds, z_ms() * seconds);
}

Velocity Velocity::inv() const {
  return Velocity(-_component[0], -_component[1], -_component[2]);
}

static const Velocity _stationary = Velocity(0.0, 0.0, 0.0);
const Velocity& Velocity::stationary() {
  return _stationary;
}

static double v_add(double v1, double v2) {
  v1 /= Constant::c;
  v2 /= Constant::c;
  return (v1 + v2) / (1.0 + v1 * v2) * Constant::c;
}

std::string Velocity::str() const {
  return "VEL ( " + Speed(x_ms()).str() + ", " + Speed(y_ms()).str() + ", " + Speed(z_ms()).str() + ")";
}

Velocity operator+(const Velocity& l, const Velocity& r) {
  return Velocity(v_add(l.x_ms(), r.x_ms()), v_add(l.y_ms(), r.y_ms()), v_add(l.z_ms(), r.z_ms()));
}

Velocity operator-(const Velocity& l, const Velocity& r) {
  return Velocity(v_add(l.x_ms(), -r.x_ms()), v_add(l.y_ms(), -r.y_ms()), v_add(l.z_ms(), -r.z_ms()));
}

static const Velocity _nan = Velocity(NAN, NAN, NAN);
const Velocity& invalid() {
  return _nan;
}

}
