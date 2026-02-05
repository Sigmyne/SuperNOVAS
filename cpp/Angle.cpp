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
Angle::Angle(double radians) : _rad(remainder(radians, Constant::two_pi)) {
  if(!isfinite(radians))
    novas_set_errno(EINVAL, "Angle(double)", "input angle is NAN or infinite");
  else
    _valid = true;
}

/**
 * Instantiates an angle from its string representation, either as decimal degrees or as
 * [-]DDD:MM:SS.SSS. Other than colons, the degrees, minutes, and seconds components may be
 * separated by the symbols d, m, or white spaces of cobination of the above. To learn more about
 * what exact representations are supported see novas:novas_str_degrees().
 *
 * @param str   a string specifying an angle either as decimal degrees or as DDD:MM:SS.SSS and
 *              variants thereof.
 *
 * @sa novas:novas_str_degrees()
 */
Angle::Angle(const std::string& str) {
  _rad = novas_str_degrees(str.c_str()) * Unit::deg;
  if(!isfinite(_rad))
    novas_set_errno(EINVAL, "Angle(std::string)", "invalid input angle: %s", str.c_str());
  else
    _valid = true;
}

/**
 * Returns a new angle that is the sum of this angle and the specified other angle.
 *
 * @param r   the other angle on the right-hand-side.
 * @return    the sum of this angle and the argument, as a new angle.
 *
 * @sa operator-()
 */
Angle Angle::operator+(const Angle& r) const {
  return Angle(rad() + r.rad());
}

/**
 * Returns a new angle that is the difference of this angle and the specified other angle.
 *
 * @param r   the other angle on the right-hand-side.
 * @return    the difference of this angle and the argument, as a new angle.
 *
 * @sa operator+()
 */
Angle Angle::operator-(const Angle& r) const {
  return Angle(rad() - r.rad());
}

/**
 * Checks if this angle instance equals another within the specified absolute precision.
 *
 * @param angle       another angle
 * @param precision   [rad] the abolute precision to use when testing for equality. If the two
 *                    angles are closer on the 2&pi; circle to each other than this precision,
 *                    then they are consideredc equal, otherwise not.
 * @return            `true` if the two angles match within the specified precision, or else
 *                    `false`.
 *
 * @sa operator==(), operator!=()
 */
bool Angle::equals(const Angle& angle, double precision) const {
  return fabs(remainder(_rad - angle._rad, Constant::two_pi)) < fabs(precision);
}

/**
 * Checks if this angle instance equals another within the 1 &mu;as.
 *
 * @param angle       another angle
 * @return            `true` if the two angles match within 1 &mu;as, or else `false`.
 *
 * @sa equals(), operator!=()
 */
bool Angle::operator==(const Angle& angle) const {
  return equals(angle);
}

/**
 * Checks if this angle instance differs from another by more than 1 &mu;as.
 *
 * @param angle       another angle
 * @return            `true` if the two angles differ by more than 1 &mu;as, or else `false`.
 *
 * @sa equals(), operator==()
 */
bool Angle::operator!=(const Angle& angle) const {
  return !equals(angle);
}

/**
 * Returns the angle in radians
 *
 * @return    [rad] the angle in radians, usually in the [-&pi;:&pi;) range, although subclasses
 *            may support different ranges for the returned value.
 *
 * @sa deg(), arcmin(), arcsec(), mas(), uas()
 */
double Angle::rad() const {
  return _rad;
}

/**
 * Returns the angle in degrees
 *
 * @return    [degrees] the angle in degrees, usually in the [-180:180) range, although subclasses
 *            may support different ranges for the returned value.
 *
 * @sa rad(), arcmin(), arcsec(), mas(), uas()
 */
double Angle::deg() const {
  return rad() / Unit::deg;
}

/**
 * Returns the angle in arc minutes
 *
 * @return    [arcmin] the angle in arc minutes, usually in the [-10800:10800) range, although
 *            subclasses may support different ranges for the returned value.
 *
 * @sa rad(), deg(), arcsec(), mas(), uas()
 */
double Angle::arcmin() const {
  return rad() / Unit::arcmin;
}

/**
 * Returns the angle in arc seconds
 *
 * @return    [arcsec] the angle in arc seconds, usually in the [-648000:648000) range, although
 *            subclasses may support different ranges for the returned value.
 *
 * @sa rad(), deg(), arcmin(), mas(), uas()
 */
double Angle::arcsec() const {
  return rad() / Unit::arcsec;
}

/**
 * Returns the angle in milliarcseconds.
 *
 * @return    [mas] the angle in milliarcseconds, usually in the [-6.48e8:6.48e8) range, although
 *            subclasses may support different ranges for the returned value.
 *
 * @sa rad(), deg(), arcmin(), arcsec(), uas()
 */
double Angle::mas() const {
  return rad() / Unit::mas;
}

/**
 * Returns the angle in microarcseconds.
 *
 * @return    [&mu;as] the angle in microarcseconds, usually in the [-6.48e11:6.48e11) range,
 *            although subclasses may support different ranges for the returned value.
 *
 * @sa rad(), deg(), arcmin(), arcsec(), mas()
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
  double f = _rad / Constant::two_pi;
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
 * @return            the strung representation of this angle in degrees.
 *
 * @sa TimeAngle::to_string()
 */
std::string Angle::to_string(enum novas_separator_type separator, int decimals) const {
  char s[100] = {'\0'};
  if(novas_print_dms(deg(), separator, decimals, s, sizeof(s)) != 0)
    novas_trace_invalid("Angle::str");
  // TODO what to return in case of error?
  return std::string(s);
}

/**
 * Returns a reference to a statically defined standard invalid angle. Sunch invalid angles may be
 * used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid angle.
 */
Angle& Angle::invalid() {
  static Angle _invalid(NAN);
  return _invalid;
}

} // namespace supernovas

