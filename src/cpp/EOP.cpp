/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <iostream>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

namespace supernovas {

void EOP::validate() {
  static const char *fn = "EOP()";

  if(_leap <= NOVAS_INVALID_LEAP)
    novas_set_errno(EINVAL, fn, "input leap is invalid");
  else if(!isfinite(_dut1))
    novas_set_errno(EINVAL, fn, "input dUT1 is NAN or infinite");
  else if(!_xp.is_valid())
    novas_set_errno(EINVAL, fn, "input xp is NAN");
  else if(!_yp.is_valid())
    novas_set_errno(EINVAL, fn, "input yp is NAN");
  else
    _valid = true;
}

/**
 * Instantiates new Earth Orientation Parameters (EOP) with the specified mean (interpolated)
 * values.
 *
 * @param leap_seconds    [s] Leap seconds (TAI - UTC).
 * @param dut1_sec        [s] (optional) UT1 - UTC time difference (default: 0.0)
 * @param xp_rad          [rad] (optional) IERS _x_<sub>p</sub> mean (interpolated) pole offset
 *                        in the ITRS _x_ direction. (default: 0.0)
 * @param yp_rad          [rad] (optional) IERS _y_<sub>p</sub> mean (interpolated) pole offset
 *                        in the ITRS _y_ direction. (default: 0.0)
 *
 * @since 1.6
 */
EOP::EOP(int leap_seconds, double dut1_sec, double xp_rad, double yp_rad)
: _leap(leap_seconds), _xp(xp_rad), _yp(yp_rad), _dut1(dut1_sec) {
  validate();
}

/**
 * Instantiates new Earth Orientation Parameters (EOP) with the specified mean (interpolated)
 * values.
 *
 * @param leap_seconds    [s] Leap seconds (TAI - UTC).
 * @param dut1            (optional) UT1 - UTC time difference (default: 0.0)
 * @param xp              (optional) IERS _x_<sub>p</sub> mean (interpolated) pole offset
 *                        in the ITRS _X_ direction (default: 0.0)
 * @param yp              (optional) IERS _y_<sub>p</sub> mean (interpolated) pole offset
 *                        in the ITRS _y_ direction (default: 0.0)
 *
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
 * @sa leap_seconds()
 */
Interval EOP::dUT1() const {
  Interval dt(_dut1);
  if(!dt.is_valid())
    novas_trace_invalid("EOP::dUT1()");
  return dt;
}

/**
 * Transforms these Earth Orientation Parameters (EOP) to a different ITRF realization
 *
 * @param from_year   [yr] the ITRF realization of these EOP values.
 * @param to_year     [yr] the ITRF realization in which to return EOP
 * @return            the EOP values in the specified new ITRF realization.
 *
 * @since 1.6
 * @sa Site::itrf_transformed()
 */
EOP EOP::itrf_transformed(int from_year, int to_year) const {
  double xp1, yp1, t1;
  novas_itrf_transform_eop(from_year, _xp.arcsec(), _yp.arcsec(), _dut1, to_year, &xp1, &yp1, &t1);

  EOP eop(_leap, t1, xp1 * Unit::arcsec, yp1 * Unit::arcsec);
  if(!eop.is_valid())
    novas_trace_invalid("EOP::itrf_transformed()");
  return eop;
}

/**
 * Returns a string representation of these Earth Orientation Paramaters (EOP).
 *
 * @return    a new string with a representation of this EOP.
 *
 * @since 1.6
 */
std::string EOP::to_string() const {
  char sx[20] = {'\0'}, sy[20] = {'\0'}, st[20] = {'\0'};
  snprintf(st, sizeof(st), "dUT1 = %.6f s", _dut1);
  snprintf(sx, sizeof(sx), "xp = %.3f mas", _xp.mas());
  snprintf(sy, sizeof(sy), "yp = %.3f mas", _yp.mas());
  return "EOP (leap = " + std::to_string(_leap) + ", "   + st + ", " + sx + ", " + sy + ")";
}

/**
 * Obtains cubic spline interpolated  Earth Orientation Parameters (EOP) from the International
 * Earth Rotation and Reference Systems Service (IERS).
 *
 * NOTES:
 *
 * - The IERS service may be offline, slow to respond, and the format of the data served may
 *   change. Therefore, you should avoid using this method in critical applications or when
 *   responsiveness is important. In such cases you are likely better off obtaining EOP from
 *   data stored locally, such as on the file-system or in a local database.
 *
 * - The returned data does not include diurnal variations for ocean tides and libration.
 *   These are added automatically in the constructors of Time and Frame as neeed.
 *
 * - If __SuperNOVAS__ was built withut cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *   configuration options), then this function will return an invalid / undefined EOP (`errno`
 *   set to `ENOSYS`).
 *
 * @param jd              [day] Julian Date (preferably UTC-based).
 * @param timeout_millis  [ms] (optional) HTTP connection timeout, or &lt;=0 to leave unchanged.
 * @return        EOP obtained from IERS or else an invalid EOP if there was an error (errno will
 *                indicate the type of error).
 *
 * @since 1.7
 *
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa fetch_for_mjd(), fetch_for(), fetch_current(), novas_fetch_eop()
 */
EOP EOP::fetch_for_jd(double jd, long timeout_millis) {
  static const char *fn = "EOP::fetch_for_jd()";

  novas_eop eop = {};
  if(novas_fetch_eop(jd, timeout_millis, &eop) != 0) {
    novas_trace_invalid(fn);
    return EOP::undefined();
  }

  EOP e(eop.leap, eop.dut1, eop.xp * Unit::arcsec, eop.yp * Unit::arcsec);
  if(!e.is_valid())
    novas_trace_invalid(fn);

  return e;
}

/**
 * Obtains cubic spline interpolated Earth Orientation Parameters (EOP) from the International
 * Earth Rotation and Reference Systems Service (IERS).
 *
 * NOTES:
 *
 * - The IERS service may be offline, slow to respond, and the format of the data served may
 *   change. Therefore, you should avoid using this method in critical applications or when
 *   responsiveness is important. In such cases you are likely better off obtaining EOP from
 *   data stored locally, such as on the file-system or in a local database.
 *
 * - The returned data does not include diurnal variations for ocean tides and libration.
 *   These are added automatically in the constructors of Time and Frame as neeed.
 *
 * - If __SuperNOVAS__ was built withut cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *   configuration options), then this function will return an invalid / undefined EOP (`errno`
 *   set to `ENOSYS`).
 *
 * @param mjd             [day] Modified Julian Date (preferably UTC-based).
 * @param timeout_millis  [ms] (optional) HTTP connection timeout, or &lt;=0 to leave unchanged.
 * @return        EOP obtained from IERS or else an invalid EOP if there was an error (errno will
 *                indicate the type of error).
 *
 * @since 1.7
 *
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa fetch_for_jd(), fetch_for(), fetch_current(), novas_fetch_eop()
 */
EOP EOP::fetch_for_mjd(double mjd, long timeout_millis) {
  EOP e = fetch_for_jd(NOVAS_JD_MJD0 + mjd, timeout_millis);
  if(!e.is_valid())
    novas_trace_invalid("EOP::fetch_for_mjd()");
  return e;
}

/**
 * Obtains cubic spline interpolated Earth Orientation Parameters (EOP) from the International
 * Earth Rotation and Reference Systems Service (IERS).
 *
 * NOTES:
 *
 * - The IERS service may be offline, slow to respond, and the format of the data served may
 *   change. Therefore, you should avoid using this method in critical applications or when
 *   responsiveness is important. In such cases you are likely better off obtaining EOP from
 *   data stored locally, such as on the file-system or in a local database.
 *
 * - The returned data does not include diurnal variations for ocean tides and libration.
 *   These are added automatically in the constructors of Time and Frame as neeed.
 *
 * - If __SuperNOVAS__ was built withut cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *   configuration options), then this function will return an invalid / undefined EOP (`errno`
 *   set to `ENOSYS`).
 *
 * @param time            UNIX time for which to try get EOP.
 * @param timeout_millis  [ms] (optional) HTTP connection timeout, or &lt;=0 to leave unchanged.
 * @return        EOP obtained from IERS or else an invalid EOP if there was an error (errno will
 *                indicate the type of error).
 *
 * @since 1.7
 *
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa fetch_for_jd(), fetch_for_mjd(), fetch_current(), novas_fetch_eop()
 */
EOP EOP::fetch_for(const time_t time, long timeout_millis) {
  EOP e = fetch_for_jd(NOVAS_JD_J2000 + (time - UNIX_SECONDS_0UTC_1JAN2000) / 86400.0, timeout_millis);
  if(!e.is_valid())
    novas_trace_invalid("EOP::fetch_for()");
  return e;
}

/**
 * Obtains cubic spline interpolated  Earth Orientation Parameters (EOP) from the International
 * Earth Rotation and Reference Systems Service (IERS).
 *
 * NOTES:
 *
 * - The IERS service may be offline, slow to respond, and the format of the data served may
 *   change. Therefore, you should avoid using this method in critical applications or when
 *   responsiveness is important. In such cases you are likely better off obtaining EOP from
 *   data stored locally, such as on the file-system or in a local database.
 *
 * - The returned data does not include diurnal variations for ocean tides and libration.
 *   These are added automatically in the constructors of Time and Frame as neeed.
 *
 * - If __SuperNOVAS__ was built withut cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *   configuration options), then this function will return an invalid / undefined EOP (`errno`
 *   set to `ENOSYS`).
 *
 * @param date            Calendar date for which to try get EOP.
 * @param timeout_millis  [ms] (optional) HTTP connection timeout, or &lt;=0 to leave unchanged.
 * @return        EOP obtained from IERS or else an invalid EOP if there was an error (errno will
 *                indicate the type of error).
 *
 * @since 1.7
 *
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa fetch_for_jd(), fetch_for_mjd(), fetch_current(), novas_fetch_eop()
 */
EOP EOP::fetch_for(const CalendarDate& date, long timeout_millis) {
  return fetch_for(date.unix_time(), timeout_millis);
}

/**
 * Obtains cubic spline interpolated Earth Orientation Parameters (EOP) from the International
 * Earth Rotation and Reference Systems Service (IERS) for the current time instant, or an offset
 * time from the current time.
 *
 * NOTES:
 *
 * - The IERS service may be offline, slow to respond, and the format of the data served may
 *   change. Therefore, you should avoid using this method in critical applications or when
 *   responsiveness is important. In such cases you are likely better off obtaining EOP from
 *   data stored locally, such as on the file-system or in a local database.
 *
 * - The returned data does not include diurnal variations for ocean tides and libration.
 *   These are added automatically in the constructors of Time and Frame as neeed.
 *
 * - If __SuperNOVAS__ was built withut cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *    configuration options), then this function will return an error (`errno` set to `ENOSYS`).
 *
 * - If __SuperNOVAS__ was built with the `WITHOUT_SYSTEM_CLOCK` preprocessor flag (or with the
 *    `WITHOUT_LIBC` build configuration option), then this function will always return an invalid
 *    / undefined EOP (`errno` set to `ENOSYS`). You may want to set the time explicitly with one
 *    of the constructors instead.
 *
 * @param offset          [s] (optional) time offset from current time (default: 0.0).
 * @param timeout_millis  [ms] (optional) HTTP connection timeout, or &lt;=0 to leave unchanged.
 * @return        Current EOP obtained from IERS or else an invalid EOP if there was an error
 *                (errno will indicate the type of error).
 *
 * @since 1.7
 *
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa fetch_for(), fetch_for_jd(), fetch_for_mjd(), novas_fetch_eop()
 */
EOP EOP::fetch_current(double offset, long timeout_millis) {
  EOP eop = fetch_for(time(NULL) + (time_t) floor(offset + 0.5), timeout_millis);
  if(!eop.is_valid())
    novas_trace_invalid("EOP::fetch_current()");
  return eop;
}

/**
 * Obtains cubic spline interpolated Earth Orientation Parameters (EOP) from the International
 * Earth Rotation and Reference Systems Service (IERS) for the current time instant, or an offset
 * time from the current time.
 *
 * NOTES:
 *
 * - The IERS service may be offline, slow to respond, and the format of the data served may
 *   change. Therefore, you should avoid using this method in critical applications or when
 *   responsiveness is important. In such cases you are likely better off obtaining EOP from
 *   data stored locally, such as on the file-system or in a local database.
 *
 * - The returned data does not include diurnal variations for ocean tides and libration.
 *   These are added automatically in the constructors of Time and Frame as neeed.
 *
 * - If __SuperNOVAS__ was built withut cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *    configuration options), then this function will return an invalid / undefined EOP (`errno`
 *    set to `ENOSYS`).
 *
 * - If __SuperNOVAS__ was built with the `WITHOUT_SYSTEM_CLOCK` preprocessor flag (or with the
 *    `WITHOUT_LIBC` build configuration option), then this function will always return an invalid
 *    / undefined EOP (`errno` set to `ENOSYS`). You may want to set the time explicitly with one
 *    of the constructors instead.
 *
 * @param offset          (optional) time offset from current time (default: 0.0).
 * @param timeout_millis  [ms] (optional) HTTP connection timeout, or &lt;=0 to leave unchanged.
 * @return        Current EOP obtained from IERS or else an invalid EOP if there was an error
 *                (errno will indicate the type of error).
 *
 * @since 1.7
 *
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa fetch_for(), fetch_for_jd(), fetch_for_mjd(), novas_fetch_eop()
 */
EOP EOP::fetch_current(const Interval& offset, long timeout_millis) {
  return fetch_current(offset.seconds(), timeout_millis);
}

/**
 * Returns a reference to a statically defined standard invalid EOP. This invalid EOP may be used
 * inside any object that is invalid itself.
 *
 * @return    a reference to a static standard EOP.
 *
 * @since 1.6
 */
const EOP& EOP::undefined() {
  static const EOP _invalid = EOP();
  return _invalid;
}


} // namespace supernovas
