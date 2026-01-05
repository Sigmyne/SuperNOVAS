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

using namespace novas;

namespace supernovas {

/**
 * Instantiates a (signed) scalar velocity (speed) with the specified S.I. value in m/s.
 *
 * @param m_per_s   [m/s] the speed
 *
 * @sa from_redshift()
 */
Speed::Speed(double m_per_s) : _ms(m_per_s) {
  if(isnan(m_per_s))
    novas_error(0, EINVAL, "Speed(double)", "input value is NAN");
  else if(fabs(m_per_s) > Constant::c)
    novas_error(0, ERANGE, "Speed(double)", "input speed exceeds the speed of light: %g m/s", m_per_s);
  else
    _valid = true;
}

/**
 * Instantiates a (signed) scalar velocity (speed) given the distance travelled in the specified
 * time interval.
 *
 * @param d     distance travelled
 * @param time  time interval
 *
 * @sa from_redshift()
 */
Speed::Speed(const Distance& d, const Interval& time) : _ms(d.m() / time.seconds()) {}

/**
 * Returns the signed scalar sum of this speed and the specified other speed, using the
 * relativistic formula for addition.
 *
 * @param r     the speed on the right-hand side
 * @return      the relativistic sum of this speed and the argument.
 *
 * @sa operator-()
 */
Speed Speed::operator+(const Speed& r) const {
  return Speed((beta() + r.beta()) / (1 + beta() * r.beta()) * Constant::c);
}

/**
 * Returns the signed scalar difference of this speed and the specified other speed, using the
 * relativistic formula for addition.
 *
 * @param r     the speed on the right-hand side
 * @return      the relativistic difference of this speed and the argument.
 *
 * @sa operator+()
 */
Speed Speed::operator-(const Speed& r) const {
  return Speed((beta() - r.beta()) / (1 + beta() * r.beta()) * Constant::c);
}

/**
 * Checks if this speed equals the specified other speed within the tolerance.
 *
 * @param speed       the reference speed
 * @param tolerance   [m/s] (optional) tolerance for the comparison (default: 0.001 m/s).
 * @return            `true` if this speed equals the argument within the tolerance, or else
 *                    `false`
 *
 * @sa operator==(), operator!=()
 */
bool Speed::equals(const Speed& speed, double tolerance) const {
  return fabs(_ms - speed._ms) < fabs(tolerance);
}

/**
 * Checks if this speed equals the specified other speed within the tolerance.
 *
 * @param speed       the reference speed
 * @param tolerance   (optional) tolerance for the comparison (default: 0.001 m/s).
 * @return            `true` if this speed equals the argument within the tolerance, or else
 *                    `false`
 *
 * @sa operator==(), operator!=()
 */
bool Speed::equals(const Speed& speed, const Speed& tolerance) const {
  return equals(speed, tolerance.m_per_s());
}

/**
 * Checks if this speed equals the specified other speed within 1 mm/s.
 *
 * @param speed     the reference speed
 * @return          `true` if this speed equals the argument within 1 mm/s, or else `false`
 *
 * @sa equals(), operator!=()
 */
bool Speed::operator==(const Speed& speed) const {
  return equals(speed);
}

/**
 * Checks if this speed differs from the specified other speed, by more than 1 mm/s.
 *
 * @param speed     the reference speed
 * @return          `true` if this speed differs from the argument by more than 1 mm/s, or else
 *                  `false`
 *
 * @sa operator==()
 */
bool Speed::operator!=(const Speed& speed) const {
  return !equals(speed);
}

/**
 * Returns the unsigned magnitude of this (possibly signed) speed.
 *
 * @return    the absolute value of this (possibly signed) speed.
 */
Speed Speed::abs() const {
  return Speed(fabs(_ms));
}

/**
 * Returns this speed in m/s.
 *
 * @return    [m/s] the speed value.
 *
 * @sa km_per_s(), au_per_day(), beta(), Gamma(), redshift()
 */
double Speed::m_per_s() const {
  return _ms;
}

/**
 * Returns this speed in km/s.
 *
 * @return    [km/s] the speed value.
 *
 * @sa m_per_s(), au_per_day(), beta(), Gamma(), redshift()
 */
double Speed::km_per_s() const {
  return 1e-3 * _ms;
}

/**
 * Returns this speed in AU/day.
 *
 * @return    [AU/day] the speed value.
 *
 * @sa m_per_s(), km_per_s(), beta(), Gamma(), redshift()
 */
double Speed::au_per_day() const {
  return _ms * Unit::day / Unit::au;
}

/**
 * Returns this speed (_v_) as &beta; = _v_ / _c_.
 *
 * @return    &beta; = _v_ / _c_
 *
 * @sa m_per_s(), km_per_s(), au_per_day(), Gamma(), redshift()
 */
double Speed::beta() const {
  return _ms / Constant::c;
}

/**
 * Returns the relativistic boost parameter &Gamma; for this speed (_v_) as &Gamma; = &radic (1 - _v_<sup>2</sup> / _c_<sup>2</sup>).
 *
 * @return    the relativistic boost &Gamma; = &radic (1 - _v_<sup>2</sup> / _c_<sup>2</sup>)
 *
 * @sa m_per_s(), km_per_s(), au_per_day(), redshift()
 */
double Speed::Gamma() const {
  return 1.0 / sqrt(1.0 - beta() * beta());
}

/**
 * Returns this speed as a redshift measure.
 *
 * @return    the equivalent redshift measure _z_.
 *
 * @sa m_per_s(), km_per_s(), au_per_day(), Gamma()
 */
double Speed::redshift() const {
  return novas_v2z(km_per_s());
}

/**
 * Returns the distance travelled at this speed under the specified time interval.
 *
 * @param seconds   [s] the time interval
 * @return          the distance travelled
 *
 * @sa operator*(), Velocity::travel()
 */
Distance Speed::travel(double seconds) const {
  return Distance(_ms * seconds);
}

/**
 * Returns the distance travelled at this speed under the specified time interval.
 *
 * @param time      the time interval
 * @return          the distance travelled
 *
 * @sa operator*(), Velocity::travel()
 */
Distance Speed::travel(const Interval& time) const {
  return travel(time.seconds());
}

/**
 * Returns the distance travelled at this speed under the specified time interval.
 *
 * @param time      the time interval
 * @return          the distance travelled
 *
 * @sa travel()
 */
Distance Speed::operator*(const Interval& time) const {
  return travel(time);
}

/**
 * Returns a string representation of this speed in km/s with the specified decimal places shown.
 *
 * @param decimals    (optional) the number of decimal places to print (default: 3)
 * @return            a new string containing a representation of this speed.
 */
std::string Speed::to_string(int decimals) const {
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
 */
Velocity Speed::in_direction(const Vector& direction) const {
  return Velocity(direction._array(), _ms / direction.abs());
}

/**
 * Instantiates a new speed instance from a given redshift value.
 *
 * @param z   the redshift value.
 * @return    a scalar velocity instance corresponding to the specified redshift value.
 */
Speed Speed::from_redshift(double z) {
  return Speed(novas_z2v(z) * Unit::km / Unit::sec);
}

/**
 * Returns a reference to a statically defined zero speed of a stationary object.
 *
 * @return    a reference to a static instance of zero speed.
 */
const Speed& Speed::stationary() {
  static const Speed _stationary = Speed(0.0);
  return _stationary;
}

} // namespace supernovas
