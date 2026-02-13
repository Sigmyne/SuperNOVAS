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

using namespace novas;

namespace supernovas {

void EOP::validate() {
  static const char *fn = "EOP()";

  if(!isfinite(_dut1))
    novas_set_errno(EINVAL, fn, "input dUT1 is NAN or infinite");
  else if(fabs(_dut1) >= 1.0)
    novas_set_errno(EINVAL, fn, "input dUT1 is outside of legal (-1.0:1.0) range: %.3g", _dut1);
  else if(!_xp.is_valid())
    novas_set_errno(EINVAL, fn, "input xp is NAN");
  else if(!_yp.is_valid())
    novas_set_errno(EINVAL, fn, "input yp is NAN");
  else
    _valid = true;
}

/**
 * Instantiates new Earth Orientation Parameters (EOP) with the specified values.
 *
 * @param leap_seconds    [s] Leap seconds (TAI - UTC).
 * @param dut1_sec        [s] (optional) UT1 - UTC time difference (default: 0.0)
 * @param xp_rad          [rad] (optional) IERS _x_<sub>p</sub> pole offset (default: 0.0)
 * @param yp_rad          [rad] (optional) IERS _y_<sub>p</sub> pole offset (default: 0.0)
 */
EOP::EOP(int leap_seconds, double dut1_sec, double xp_rad, double yp_rad)
: _leap(leap_seconds), _xp(xp_rad), _yp(yp_rad), _dut1(dut1_sec) {
  validate();
}

/**
 * Instantiates new Earth Orientation Parameters (EOP) with the specified values.
 *
 * @param leap_seconds    [s] Leap seconds (TAI - UTC).
 * @param dut1            (optional) UT1 - UTC time difference (default: 0.0)
 * @param xp              (optional) IERS _x_<sub>p</sub> pole offset (default: 0.0)
 * @param yp              (optional) IERS _y_<sub>p</sub> pole offset (default: 0.0)
 */
EOP::EOP(int leap_seconds, const Interval& dut1, const Angle& xp, const Angle& yp)
: EOP(leap_seconds, dut1.seconds(), xp.rad(), yp.rad()) {
  validate();
}

/**
 * Checks if these Earth Orientation Parameters are the same as another, within
 * 1 &mu;s / 1 &mu;as accuracy.
 *
 * @param eop   another set of Earth Orientation Parameters
 * @return      `true` if this EOP matches the argument to 1 &mu;s / 1 &mu;as accuracy,
 *              otherwise `false`.
 *
 * @sa operator!=()
 */
bool EOP::operator==(const EOP& eop) const {
  if(_leap != eop._leap)
    return false;
  if(!(fabs(_dut1 - eop._dut1) < Unit::us))
    return false;
  if(!_xp.equals(eop._xp, Unit::uas))
    return false;
  if(!_yp.equals(eop._yp, Unit::uas))
    return false;
  return true;
}

/**
 * Checks if these Earth Orientation Parameters differ from another by more than
 * 1 &mu;s / 1 &mu;as.
 *
 * @param eop   another set of Earth Orientation Parameters
 * @return      `true` if this EOP differs from argument by more than 1 &mu;s / 1 &mu;as,
 *              otherwise `false`.
 *
 * @sa operator!=()
 */
bool EOP::operator!=(const EOP& eop) const {
  return !(*this == eop);
}

/**
 * Returns the leap seconds (TAI - UTC time difference) in seconds.
 *
 * @return    [s] the leap seconds (TAI - UTC).
 *
 * @sa dUT1()
 */
int EOP::leap_seconds() const {
  return _leap;
}

/**
 * Returns the IERS _x_ pole offset (_x_<sub>p</sub>) as an angle.
 *
 * @return    the reference to the _x_ pole offset angle, as stored internally.
 *
 * @sa yp()
 */
const Angle& EOP::xp() const {
  return _xp;
}

/**
 * Returns the IERS _y_ pole offset (_y_<sub>p</sub>) as an angle.
 *
 * @return    the reference to the _y_ pole offset angle, as stored internally.
 *
 * @sa xp()
 */
const Angle& EOP::yp() const {
  return _yp;
}

/**
 * Returns the UT1 - UTC time difference as a time interval.
 *
 * @return    a new time interval with the UT1 - UTC time difference.
 *
 * @sa leap_seconds()
 */
Interval EOP::dUT1() const {
  return Interval(_dut1);
}

/**
 * Transforms these Earth Orientation Parameters (EOP) to a different ITRF realization
 *
 * @param from_year   [yr] the ITRF realization of these EOP values.
 * @param to_year     [yr] the ITRF realization in which to return EOP
 * @return            the EOP values in the specified new ITRF realization.
 *
 * @sa Site::itrf_transformed()
 */
EOP EOP::itrf_transformed(int from_year, int to_year) const {
  double xp1, yp1, t1;
  novas_itrf_transform_eop(from_year, _xp.arcsec(), _yp.arcsec(), _dut1, to_year, &xp1, &yp1, &t1);
  return EOP(_leap, t1, xp1 * Unit::arcsec, yp1 * Unit::arcsec);
}

/**
 * Returns a string representation of these Earth Orientation Paramaters (EOP).
 *
 * @return    a new string with a representation of this EOP.
 */
std::string EOP::to_string() const {
  char sx[20] = {'\0'}, sy[20] = {'\0'}, st[20] = {'\0'};
  snprintf(st, sizeof(st), "dUT1 = %.6f s", _dut1);
  snprintf(sx, sizeof(sx), "xp = %.3f mas", _xp.mas());
  snprintf(sy, sizeof(sy), "yp = %.3f mas", _yp.mas());
  return "EOP (leap = " + std::to_string(_leap) + ", "   + st + ", " + sx + ", " + sy + ")";
}

/**
 * Returns a reference to a statically defined standard invalid EOP. This invalid EOP may be used
 * inside any object that is invalid itself.
 *
 * @return    a reference to a static standard EOP.
 */
const EOP& EOP::invalid() {
  static const EOP _invalid = EOP(0, NAN, NAN, NAN);
  return _invalid;
}

} // namespace supernovas
