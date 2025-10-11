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
    novas::novas_error(0, ERANGE, fn, "input velocity exceeds the speed of light: %s", to_string().c_str());
    _valid = false;
  }
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
  return Position(x() * seconds, y() * seconds, z() * seconds);
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

std::string Velocity::to_string() const {
  return "VEL ( " + Speed(x()).to_string() + ", " + Speed(y()).to_string() + ", " + Speed(z()).to_string() + ")";
}

Velocity operator+(const Velocity& l, const Velocity& r) {
  return Velocity(v_add(l.x(), r.x()), v_add(l.y(), r.y()), v_add(l.z(), r.z()));
}

Velocity operator-(const Velocity& l, const Velocity& r) {
  return Velocity(v_add(l.x(), -r.x()), v_add(l.y(), -r.y()), v_add(l.z(), -r.z()));
}

static const Velocity _nan = Velocity(NAN, NAN, NAN);
const Velocity& Velocity::invalid() {
  return _nan;
}

}
