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

static double v_add(double v1, double v2) {
  v1 /= Constant::c;
  v2 /= Constant::c;
  return (v1 + v2) / (1.0 + v1 * v2) * Constant::c;
}

/**
 * Instantiates a new velocity vector from its _xyz_ components.
 *
 * @param x_ms    [m/s] _x_ component
 * @param y_ms    [m/s] _y_ component
 * @param z_ms    [m/s] _z_ component
 */
Velocity::Velocity(double x_ms, double y_ms, double z_ms)
: Vector(x_ms, y_ms, z_ms) {
  static const char *fn = "Velocity()";

  if(!_valid)
    novas::novas_trace_invalid(fn);
  else if(abs() > Constant::c) {
    novas::novas_set_errno(ERANGE, fn, "input velocity exceeds the speed of light");
    _valid = false;
  }
}

/**
 * Instantiates a new velocity vector from its _xyz_ components, specified in the given physical
 * unit.
 *
 * @param vel   The _xyz_ 3-vector defining the components
 * @param unit  (optional) the physical unit (e.g. Unit::kms, or Unit::au / Unit::day) in which
 *              the components were specified (default: Unit:m / Unit::s = 1.0).
 */
Velocity::Velocity(const double vel[3], double unit)
: Vector(vel[0] * unit, vel[1] * unit, vel[2] * unit) {}

/**
 * Checks if this velocity is the same as another, within the specified precision.
 *
 * @param v           the reference velocity
 * @param precision   the precision for testing equality (default: 1 mm/s).
 * @return            `true` if this velocity equals the argument within the specified
 *                    precision, or else `false`.
 *
 * @sa operator==(), operator!=()
 */
bool Velocity::equals(const Velocity& v, double precision) const {
  return Vector::equals(v, precision);
}

/**
 * Checks if this velocity is the same as another, within 1 mm/s.
 *
 * @param v   the reference velocity
 * @return    `true` if this velocity equals the argument within 1 mm/s, or else `false`.
 *
 * @sa equals(), operator!=()
 */
bool Velocity::operator==(const Velocity& v) const {
  return equals(v, Unit::mm / Unit::sec);
}

/**
 * Checks if this velocity is the same as another, within 1 mm/s.
 *
 * @param v   the reference velocity
 * @return    `true` if this velocity equals the argument within 1 mm/s, or else `false`.
 *
 * @sa operator==()
 */
bool Velocity::operator!=(const Velocity& v) const {
  return !equals(v, Unit::mm / Unit::sec);
}


/**
 * Returns the sum of this velocity and the specified other velocity. The sum is calculated using
 * the relativistic formula.
 *
 * @param r   the other velocity on the right-hand side.
 * @return    the sum of this velocity and the argument.
 *
 * @sa operator-()
 */
Velocity Velocity::operator+(const Velocity& r) const {
  return Velocity(v_add(x(), r.x()), v_add(y(), r.y()), v_add(z(), r.z()));
}

/**
 * Returns the difference of this velocity and the specified other velocity. The difference is
 * calculated using the relativistic formula.
 *
 * @param r   the other velocity on the right-hand side.
 * @return    the sum of this velocity and the argument.
 *
 * @sa operator+()
 */
Velocity Velocity::operator-(const Velocity& r) const {
  return Velocity(v_add(x(), -r.x()), v_add(y(), -r.y()), v_add(z(), -r.z()));
}

/**
 * Returns the speed (absolute value) of this velocity vector.
 *
 * @return    the speed (absolute value) of this velocity.
 */
Speed Velocity::speed() const {
  return Speed(abs());
}

/**
 * Returns the vector travelled in the given time interval at this velocity.
 *
 * @param t   time interval
 * @return    the change in position of an object travelling at this velocity in the
 *            specified time interval.
 *
 * @sa operator*(), Speed:travel()
 */
Position Velocity::travel(const Interval& t) const {
  return travel(t.seconds());
}

/**
 * Returns the vector travelled in the given time interval at this velocity.
 *
 * @param seconds [s] time interval
 * @return    the change in position of an object travelling at this velocity in the
 *            specified time interval.
 *
 * @sa operator*(), Speed:travel()
 */
Position Velocity::travel(double seconds) const {
  return Position(x() * seconds, y() * seconds, z() * seconds);
}

/**
 * Returns the vector travelled in the given time interval at this velocity.
 *
 * @param t   time interval
 * @return    the change in position of an object travelling at this velocity in the
 *            specified time interval.
 *
 * @sa travel(),  Speed:travel()
 */
Position Velocity::operator*(const Interval& t) const {
  return travel(t);
}


/**
 * Returns the velocity with the same magnitude as this, but moving in the opposite
 * direction.
 *
 * @return    the velocity in the opposite direction.
 */
Velocity Velocity::inv() const {
  return Velocity(-_component[0], -_component[1], -_component[2]);
}

/**
 * Returns a reference to the statically defined zero velocity of a stationary object.
 *
 * @return  the static reference to a stationary object with zero velocity
 */
const Velocity& Velocity::stationary() {
  static const Velocity _stationary = Velocity(0.0, 0.0, 0.0);
  return _stationary;
}

/**
 * Returns a string representation of this velocity with the requested decimal precision.
 *
 * @param decimals    (optional) the number of decimnal places to print for the components
 * @return            a new string with a representation of this velocity.
 */
std::string Velocity::to_string(int decimals) const {
  return "Velocity (" + Speed(x()).to_string(decimals) + ", " + Speed(y()).to_string(decimals) + ", " + Speed(z()).to_string(decimals) + ")";
}

/**
 * Returns a reference to the statically defined standard invalid velocity. This invalid
 * velocity may be used inside any object that is invalid itself.
 *
 * @return  the static reference to a standard invalid velocity
 */
const Velocity& Velocity::invalid() {
  static const Velocity _nan = Velocity(NAN, NAN, NAN);
  return _nan;
}

}
