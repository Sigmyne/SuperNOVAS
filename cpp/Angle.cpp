/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cmath>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

/**
 * Instantiates a new angle with the specified value in radians. You can use the Unit class to
 * convert angles expressed in other units to radians. For example, to set 15.0 degrees, you might
 * write:
 *
 * ```c
 *   Angle a(15.0 * Unit::deg);
 * ```
 *
 * @param radians     [rad] the initializing value.
 *
 * @sa TimeAngle, Unit
 */
Angle::Angle(double radians) : _rad(remainder(radians, TWOPI)) {
  if(isnan(radians))
    novas_error(0, EINVAL, "Angle(double)", "input angle is NAN");
  else
    _valid = true;
}

/**
 * Instantiates an angle from its string representation, either as decimal degrees or as
 * [-]DDD:MM:SS.SSS. Other than colons, the degrees, minutes, and seconds components may be
 * separated by the symbols d, m, or white spaces of cobination of the above. To learn more about
 * what exact representations are supported see novas:novas_str_degrees().
 *
 *
 * @param str   a string specifying an angle either as decimal degrees or as DDD:MM:SS.SSS and
 *              variants thereof.
 *
 * @sa novas:novas_str_degrees()
 */
Angle::Angle(const std::string& str) {
  _rad = novas_str_degrees(str.c_str()) * Unit::deg;
  if(isnan(_rad))
    novas_error(0, EINVAL, "Angle(std::string)", "invalid input angle: %s", str.c_str());
}


Angle operator+(const Angle& l, const Angle& r) {
  return Angle(l.rad() + r.rad());
}

Angle operator-(const Angle& l, const Angle& r) {
  return Angle(l.rad() - r.rad());
}

/**
 * Checks if this angle instance equals another within the specified absolute precision.
 *
 * @param angle       another angle
 * @param precision   [rad] the abolute precision to use when testing for equality. If the two
 *                    angles are closer on the 2&pi; circle to each other than this precision,
 *                    then they are consideredc equal, otherwise not.
 * @return            true if the two angles match within the specified precision, or else false.
 */
bool Angle::is_equal(const Angle& angle, double precision) const {
  return fabs(remainder(_rad - angle._rad, Constant::twoPi)) < fabs(precision);
}

/**
 * Returns the angle in radians
 *
 * @return    [rad] the angle in radians, usually in the [-&pi;:&pi;) range, although subclasses
 *            may support different ranges for the returned value.
 */
double Angle::rad() const {
  return _rad;
}

/**
 * Returns the angle in degrees
 *
 * @return    [degrees] the angle in degrees, usually in the [-180:180) range, although subclasses
 *            may support different ranges for the returned value.
 */
double Angle::deg() const {
  return rad() / Unit::deg;
}

/**
 * Returns the angle in arc minutes
 *
 * @return    [arcmin] the angle in arc minutes, usually in the [-10800:10800) range, although
 *            subclasses may support different ranges for the returned value.
 */
double Angle::arcmin() const {
  return rad() / Unit::arcsec;
}

/**
 * Returns the angle in arc seconds
 *
 * @return    [arcsec] the angle in arc seconds, usually in the [-648000:648000) range, although
 *            subclasses may support different ranges for the returned value.
 */
double Angle::arcsec() const {
  return rad() / Unit::arcsec;
}

/**
 * Returns the angle in milliarcseconds.
 *
 * @return    [mas] the angle in milliarcseconds, usually in the [-6.48e8:6.48e8) range, although
 *            subclasses may support different ranges for the returned value.
 */
double Angle::mas() const {
  return rad() / Unit::mas;
}

/**
 * Returns the angle in microarcseconds.
 *
 * @return    [&mu;as] the angle in microarcseconds, usually in the [-6.48e11:6.48e11) range,
 *            although subclasses may support different ranges for the returned value.
 */
double Angle::uas() const {
  return rad() / Unit::uas;
}

/**
 * Returns the angle as a fraction of the circle.
 *
 * @return    the angle as a fraction of the circle, usually in the [0:1) range.
 */
double Angle::fraction() const {
  double f = _rad / TWOPI;
  return f >= 0 ? f : 1.0 + f;
}

/**
 * Provides a DDD:MM:SS.SSS representation of the angle with the specified separators and
 * decimal places for the arc seconds.
 *
 * @param separator   the type of separators / unit markers used to distinguish the degree,
 *                    minute, and second components.
 * @param decimals    [0:9] the number of decimal places to print for the arc seconds
 *                    component.
 * @return
 */
std::string Angle::str(enum novas_separator_type separator, int decimals) const {
  char s[100] = {'\0'};
  if(novas_print_dms(deg(), separator, decimals, s, sizeof(s)) != 0)
    novas_trace_invalid("Angle::str");
  // TODO what to return in case of error?
  return std::string(s);
}

} // namespace supernovas

