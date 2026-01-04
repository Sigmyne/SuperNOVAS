/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"

using namespace novas;

namespace supernovas {

/**
 * Instantiates an angle, to be represented as time on a 24 hour interval, from a canonical angle
 * in radians.
 *
 * @param radians   [rad] the angle in radians
 *
 * @sa TimeAngle(Angle&), TimeAngle(std::string&)
 */
TimeAngle::TimeAngle(double radians)
: Angle(radians) {
  if(_rad < 0.0) _rad += TWOPI;
}

/**
 * Instantiates an angle, to be represented as time on a 24 hour interval, from a HMS or decimal
 * string representation of time.
 *
 * @param str   A decimal or HMS representation of time, using spaces, colons, and/or the letters
 *              `h`, `m` (and optionally `s`) as separators.
 *
 * @sa TimeAngle(double), TimeAngle(Angle&)
 */
TimeAngle::TimeAngle(const std::string& str)
: Angle(novas_str_hours(str.c_str()) * Unit::hour_angle) {}

/**
 * Instantiates an angle, to be represented as time on a 24 hour interval, from an angle instance.
 *
 * @param angle   an angle to initialize with.
 */
TimeAngle::TimeAngle(const Angle& angle)
: Angle(angle) {}

/**
 * Returns this angle as hours on a 24h interval
 *
 * @return    [h] the angle as hours of time on the [0.0:24.0) interval.
 *
 * @sa minutes(), seconds(), Angle::rad(), Angle::deg(), Angle::arcmin(), Angle::arcsec()
 */
double TimeAngle::hours() const {
  return _rad / Unit::hour_angle;
}

/**
 * Returns this angle as minutes inside a 24h interval
 *
 * @return    [m] the angle as minutes of time on the [0.0:1440.0) interval.
 *
 * @sa hours(), seconds(), Angle::rad(), Angle::deg(), Angle::arcmin(), Angle::arcsec()
 */
double TimeAngle::minutes() const {
  return hours() * 60.0;
}

/**
 * Returns this angle as seconds on a 24h interval
 *
 * @return    [s] the angle as seconds of time on the [0.0:86400.0) interval.
 *
 * @sa hours(), minutes(), seconds(), Angle::rad(), Angle::deg(), Angle::arcmin(), Angle::arcsec()
 */
double TimeAngle::seconds() const {
  return hours() * 3600.0;
}

/**
 * Provides a DDD:MM:SS.SSS representation of the angle with the specified separators and
 * decimal places for the arc seconds.
 *
 * @param separator   the type of separators / unit markers used to distinguish the degree,
 *                    minute, and second components.
 * @param decimals    [0:9] the number of decimal places to print for the arc seconds
 *                    component.
 * @return            the strung representation of this angle in degrees.
 *
 * @sa Angle::to_string(enum novas_separator_type, int)
 */
std::string TimeAngle::to_string(enum novas_separator_type separator, int decimals) const {
  char s[100] = {'\0'};
  novas_print_hms(hours(), separator, decimals, s, sizeof(s));
  return std::string(s);
}

/**
 * Returns a new time-angle that is the sum of this rime-angle and the specified other time-angle.
 *
 * @param r   the other time-angle on the right-hand-side.
 * @return    the sum of this time-angle and the argument, as a new time-angle.
 *
 * @sa operator-(Angle&), operator+(Interval&)
 */
TimeAngle TimeAngle::operator+(const TimeAngle& r) const {
  return TimeAngle(rad() + r.rad());
}

/**
 * Returns a new time-angle that is the difference of this time-angle and the specified other
 * time-angle.
 *
 * @param r   the other time-angle on the right-hand-side.
 * @return    the difference of this time-angle and the argument, as a new time-angle.
 *
 * @sa operator+(Angle&), operator-(Interval&)
 */
TimeAngle TimeAngle::operator-(const TimeAngle& r) const {
  return TimeAngle(rad() - r.rad());
}

/**
 * Returns a new time-angle that is the sum of this time-angle and the specified time interval.
 *
 * @param offset   the time interval on the right-hand-side.
 * @return         the sum of this time-angle and the time interval, as a new time-angle.
 *
 * @sa operator-(Interval&), operator+(Angle&)
 */
TimeAngle TimeAngle::operator+(const Interval& offset) const {
  return TimeAngle(rad() + offset.hours() * Unit::hour_angle);
}

/**
 * Returns a new time-angle that is the difference of this time-angle and the specified time
 * interval.
 *
 * @param offset   the time interval on the right-hand-side.
 * @return         the difference of this time-angle and the time interval, as a new time-angle.
 *
 * @sa operator+(Interval&), operator-(Angle&)
 */
TimeAngle TimeAngle::operator-(const Interval& offset) const {
  return TimeAngle(rad() - offset.hours() * Unit::hour_angle);
}

/**
 * Returns a new time-angle given the specified time value in hours. The input time may be
 * any real value, but the returned time angle will be restricted to a single 24 hour interval.
 *
 * @param value   [h] the angle defined as hours of time.
 * @return        a new time-angle with the specified time defining the angle.
 *
 * @sa minutes(double), seconds(double)
 */
TimeAngle TimeAngle::hours(double value) {
  return TimeAngle(value * Unit::hour_angle);
}

/**
 * Returns a new time-angle given the specified time value in minutes. The input time may be
 * any real value, but the returned time angle will be restricted to a single 24 hour interval.
 *
 * @param value   [m] the angle defined as minutes of time.
 * @return        a new time-angle with the specified time defining the angle.
 *
 * @sa hours(double), seconds(double)
 */
TimeAngle TimeAngle::minutes(double value) {
  return TimeAngle(value / 60.0 * Unit::hour_angle);
}

/**
 * Returns a new time-angle given the specified time value in seconds. The input time may be
 * any real value, but the returned time angle will be restricted to a single 24 hour interval.
 *
 * @param value   [s] the angle defined as seconds of time.
 * @return        a new time-angle with the specified time defining the angle.
 *
 * @sa hours(double), minutes(double)
 */
TimeAngle TimeAngle::seconds(double value) {
  return TimeAngle(value / 3600.0 * Unit::hour_angle);
}

/**
 * Returns a reference to a statically defined time-angle that represents a zero angle.
 *
 * @return    a reference to the statically defined zero angle.
 */
const TimeAngle& TimeAngle::zero() {
  static const TimeAngle _zero = TimeAngle(0.0);
  return _zero;
}

/**
 * Returns a reference to a statically defined time angle corresponding to noon (12h).
 *
 * @return    a reference to the statically defined time-angle for noon (12h).
 */
const TimeAngle& TimeAngle::noon() {
  static const TimeAngle _12h = TimeAngle(12.0 * Unit::hour_angle);
  return _12h;
}

} // namespace supernovas
