/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"


namespace supernovas {

Velocity::Velocity(double x_ms, double y_ms, double z_ms)
: Vector(x_ms, y_ms, z_ms) {
  static const char *fn = "Velocity()";

  if(!_valid)
    novas::novas_trace_invalid(fn);
  else if(abs() > Constant::c) {
    novas::novas_error(0, ERANGE, fn, "input velocity exceeds the speed of light: %s", str().c_str());
    _valid = false;
  }
}

Velocity::Velocity(const double vel[3], double unit)
: Vector(vel[0] * unit, vel[1] * unit, vel[2] * unit) {}


double Velocity::x_m_per_s() const {
  return _component[0];
}

double Velocity::y_m_per_s() const {
  return _component[1];
}

double Velocity::z_m_per_s() const {
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
  return Position(x_m_per_s() * seconds, y_m_per_s() * seconds, z_m_per_s() * seconds);
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
  return "VEL ( " + Speed(x_m_per_s()).str() + ", " + Speed(y_m_per_s()).str() + ", " + Speed(z_m_per_s()).str() + ")";
}

Velocity operator+(const Velocity& l, const Velocity& r) {
  return Velocity(v_add(l.x_m_per_s(), r.x_m_per_s()), v_add(l.y_m_per_s(), r.y_m_per_s()), v_add(l.z_m_per_s(), r.z_m_per_s()));
}

Velocity operator-(const Velocity& l, const Velocity& r) {
  return Velocity(v_add(l.x_m_per_s(), -r.x_m_per_s()), v_add(l.y_m_per_s(), -r.y_m_per_s()), v_add(l.z_m_per_s(), -r.z_m_per_s()));
}

static const Velocity _nan = Velocity(NAN, NAN, NAN);
const Velocity& Velocity::invalid() {
  return _nan;
}

}
