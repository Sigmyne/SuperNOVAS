/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"



namespace supernovas {

/**
 * Instantiates a (signed) scalar velocity (speed) with the specified S.I. value in m/s.
 *
 * @param m_per_s   [m/s] the speed
 *
 * @since 1.6
 * @sa from_redshift()
 */
ScalarVelocity::ScalarVelocity(double m_per_s) : Scalar(m_per_s) {
  static const char *fn = "ScalarVelocity()";

  if(!is_valid())
    novas_trace_invalid(fn);
  else if(fabs(m_per_s) > Constant::c) {
    novas_set_errno(ERANGE, fn, "input speed exceeds the speed of light: %g m/s", m_per_s);
    _valid = false;
  }
}

/**
 * Returns the signed scalar sum of this speed and the specified other speed, using the
 * relativistic formula for addition.
 *
 * @param r     the speed on the right-hand side
 * @return      the relativistic sum of this speed and the argument.
 *
 * @since 1.6
 * @sa operator-()
 */
ScalarVelocity ScalarVelocity::operator+(const ScalarVelocity& r) const {
  ScalarVelocity v((beta() + r.beta()) / (1 + beta() * r.beta()) * Constant::c);
  if(!v.is_valid())
    novas_trace_invalid("ScalarVelocity::operator+()");
  return v;
}

/**
 * Returns the signed scalar difference of this speed and the specified other speed, using the
 * relativistic formula for addition.
 *
 * @param r     the speed on the right-hand side
 * @return      the relativistic difference of this speed and the argument.
 *
 * @since 1.6
 * @sa operator+()
 */
ScalarVelocity ScalarVelocity::operator-(const ScalarVelocity& r) const {
  ScalarVelocity v((beta() - r.beta()) / (1 + beta() * r.beta()) * Constant::c);
  if(!v.is_valid())
    novas_trace_invalid("ScalarVelocity::operator-()");
  return v;
}

/**
 * Checks if this speed equals the specified other speed within the tolerance.
 *
 * @param speed       the reference speed
 * @param tolerance   [m/s] (optional) tolerance for the comparison (default: 0.001 m/s).
 * @return            `true` if this speed equals the argument within the tolerance, or else
 *                    `false`
 *
 * @since 1.6
 * @sa operator==(), operator!=()
 */
bool ScalarVelocity::equals(const ScalarVelocity& speed, double tolerance) const {
  return fabs(_value - speed._value) < fabs(tolerance);
}

/**
 * Checks if this speed equals the specified other speed within the tolerance.
 *
 * @param speed       the reference speed
 * @param tolerance   (optional) tolerance for the comparison (default: 0.001 m/s).
 * @return            `true` if this speed equals the argument within the tolerance, or else
 *                    `false`
 *
 * @since 1.6
 * @sa operator==(), operator!=()
 */
// cppcheck-suppress functionStatic
bool ScalarVelocity::equals(const ScalarVelocity& speed, const ScalarVelocity& tolerance) const {
  return equals(speed, tolerance.m_per_s());
}

/**
 * Checks if this speed equals the specified other speed within 1 mm/s.
 *
 * @param speed     the reference speed
 * @return          `true` if this speed equals the argument within 1 mm/s, or else `false`
 *
 * @since 1.6
 * @sa equals(), operator!=()
 */
bool ScalarVelocity::operator==(const ScalarVelocity& speed) const {
  return equals(speed);
}

/**
 * Checks if this speed differs from the specified other speed, by more than 1 mm/s.
 *
 * @param speed     the reference speed
 * @return          `true` if this speed differs from the argument by more than 1 mm/s, or else
 *                  `false`
 *
 * @since 1.6
 * @sa operator==()
 */
bool ScalarVelocity::operator!=(const ScalarVelocity& speed) const {
  return !equals(speed);
}

/**
 * Returns the unsigned magnitude of this (possibly signed) speed.
 *
 * @return    the absolute value of this (possibly signed) speed.
 *
 * @since 1.6
 */
ScalarVelocity ScalarVelocity::abs() const {
  ScalarVelocity v(fabs(_value));
  if(!v.is_valid())
    novas_trace_invalid("ScalarVelocity::abs()");
  return v;
}

/**
 * Returns this speed in m/s.
 *
 * @return    [m/s] the speed value.
 *
 * @since 1.6
 * @sa km_per_s(), au_per_day(), beta(), Gamma(), redshift()
 */
double ScalarVelocity::m_per_s() const {
  return _value;
}

/**
 * Returns this speed in km/s.
 *
 * @return    [km/s] the speed value.
 *
 * @since 1.6
 * @sa m_per_s(), au_per_day(), beta(), Gamma(), redshift()
 */
double ScalarVelocity::km_per_s() const {
  return 1e-3 * _value;
}

/**
 * Returns this speed in AU/day.
 *
 * @return    [AU/day] the speed value.
 *
 * @since 1.6
 * @sa m_per_s(), km_per_s(), beta(), Gamma(), redshift()
 */
double ScalarVelocity::au_per_day() const {
  return _value * Unit::day / Unit::au;
}

/**
 * Returns this speed (_v_) as &beta; = _v_ / _c_.
 *
 * @return    &beta; = _v_ / _c_
 *
 * @since 1.6
 * @sa m_per_s(), km_per_s(), au_per_day(), Gamma(), redshift()
 */
double ScalarVelocity::beta() const {
  return _value / Constant::c;
}

/**
 * Returns the relativistic boost parameter &Gamma; for this speed (_v_) as &Gamma; = &radic (1 - _v_<sup>2</sup> / _c_<sup>2</sup>).
 *
 * @return    the relativistic boost &Gamma; = &radic (1 - _v_<sup>2</sup> / _c_<sup>2</sup>)
 *
 * @since 1.6
 * @sa m_per_s(), km_per_s(), au_per_day(), redshift()
 */
double ScalarVelocity::Gamma() const {
  return 1.0 / sqrt(1.0 - beta() * beta());
}

/**
 * Returns this speed as a redshift measure.
 *
 * @return    the equivalent redshift measure _z_.
 *
 * @since 1.6
 * @sa m_per_s(), km_per_s(), au_per_day(), Gamma()
 */
double ScalarVelocity::redshift() const {
  return novas_check_nan("ScalarVelocity::redshift", novas_v2z(km_per_s()));
}

/**
 * Returns the distance travelled at this speed under the specified time interval.
 *
 * @param seconds   [s] the time interval
 * @return          the distance travelled
 *
 * @since 1.6
 * @sa operator*(), Velocity::travel()
 */
Coordinate ScalarVelocity::travel(double seconds) const {
  Coordinate x(_value * seconds);
  if(!x.is_valid())
    novas_trace_invalid("ScalarVelocity::travel()");
  return x;
}

/**
 * Returns the distance travelled at this speed under the specified time interval.
 *
 * @param time      the time interval
 * @return          the distance travelled
 *
 * @since 1.6
 * @sa operator*(), Velocity::travel()
 */
Coordinate ScalarVelocity::travel(const Interval& time) const {
  return travel(time.seconds());
}

/**
 * Returns the distance travelled at this speed under the specified time interval.
 *
 * @param time      the time interval
 * @return          the distance travelled
 *
 * @since 1.6
 * @sa travel()
 */
Coordinate ScalarVelocity::operator*(const Interval& time) const {
  Coordinate x = travel(time);
  if(!x.is_valid())
    novas_trace_invalid("ScalarVelocity::operator*()");
  return x;
}

std::string ScalarVelocity::SI_unit() const {
  return "m/s";
}

/**
 * Returns a string representation of this speed in km/s with the specified decimal places shown.
 *
 * @param decimals    (optional) the number of decimal places to print (default: 3)
 * @return            a new string containing a representation of this speed.
 *
 * @since 1.6
 */
std::string ScalarVelocity::to_string(int decimals) const {
  char fmt[20] = {'\0'}, s[40] = {'\0'};

  snprintf(fmt, sizeof(fmt), "%%.%df", decimals);
  snprintf(s, sizeof(s), fmt, km_per_s());

  return std::string(s) + " km/s";
}

/**
 * Returns a 3-dimensional velocity vector corresponding to this speed along the specified
 * direction.
 *
 * @param direction   a vector specifying the direction. Its magnitude is irrelevant.
 * @return            a new velocity vector, in the specified direction and with the magnitude of
 *                    this speed.
 *
 * @since 1.6
 */
Velocity ScalarVelocity::in_direction(const Vector& direction) const {
  Velocity v(direction._array(), _value / direction.abs());
  if(!v.is_valid())
    novas_trace_invalid("ScalarVelocity::to_direction()");
  return v;
}

/**
 * Instantiates a new speed instance from a given redshift value.
 *
 * @param z   the redshift value.
 * @return    a scalar velocity instance corresponding to the specified redshift value.
 *
 * @since 1.6
 */
ScalarVelocity ScalarVelocity::from_redshift(double z) {
  ScalarVelocity v(novas_z2v(z) * Unit::km_per_s);
  if(!v.is_valid())
    novas_trace_invalid("ScalarVelocity::from_redshift()");
  return v;
}

/**
 * Returns a reference to a statically defined zero speed of a stationary object.
 *
 * @return    a reference to a static instance of zero speed.
 *
 * @since 1.6
 */
const ScalarVelocity& ScalarVelocity::stationary() {
  static const ScalarVelocity _stationary = ScalarVelocity(0.0);
  return _stationary;
}

/**
 * Returns a reference to a static instance of an undefined / invalid scalar velocity.
 *
 * @return    a reference to a standard undefined scalar velocity instance.
 *
 * @since 1.6
 */
const ScalarVelocity& ScalarVelocity::undefined() {
  static const ScalarVelocity _undefined = ScalarVelocity();
  return _undefined;
}

} // namespace supernovas
