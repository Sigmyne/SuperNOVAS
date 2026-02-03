/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cmath>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

/// \cond PROTECTED
#define DTA               32.184        ///< [s] TT - TAI time difference
#define UNIX_UTC_J2000    946728000L    ///< 12:00, 1 Jan 2000 (UTC timescale)
/// \endcond

static double novas_era(long ijd, double fjd) {
  return novas::era((double) ijd, fjd);
}

#define E9                1000000000L   ///< 10<sup>9</sup> as an integer

using namespace novas;

namespace supernovas {


static bool is_valid_parms(double dUT1,  enum novas_timescale timescale) {
  static const char *fn = "Time()";

  if(!isfinite(dUT1))
    return novas_error(0, EINVAL, fn, "input dUT1 is NAN");
  else if(fabs(dUT1) > 1.0)
    return novas_error(0, EINVAL, fn, "input dUT1 exceeds +/- 1s limit: %g", dUT1);
  else if((unsigned) timescale >= NOVAS_TIMESCALES)
    return novas_error(0, EINVAL, fn, "invalid timescale: %d", timescale);
  else
    return 1;
}

/**
 * Instantiates an astrometric time instance with the specified time parameters.
 *
 * @param jd            [day] Julian date (in the selected timescale)
 * @param leap_seconds  [s] leap seconds, that is TAI - UTC (default: 0)
 * @param dUT1          [s] UT1 - UTC time difference, e.g. from the IERS Bulletins or service (default: 0.0).
 * @param timescale     (optional) Astronomical timescale (default: TT).
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(double jd, int leap_seconds, double dUT1, enum novas_timescale timescale) {
  novas_set_time(timescale, jd, leap_seconds, dUT1, &_ts);
  if(!isfinite(jd))
    novas_set_errno(EINVAL, "Time()", "input jd is NAN or infinite");
  else
    _valid = is_valid_parms(dUT1, timescale);
}

/**
 * Instantiates an astrometric time instance with the specified time parameters.
 *
 * @param jd            [day] Julian date (in the selected timescale)
 * @param eop           Earth Orientation Parameters (EOP) values, e.g. obtained from IERS.
 * @param timescale     (optional) Astronomical timescale (default: TT).
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(double jd, const EOP& eop, enum novas_timescale timescale)
: Time(jd, eop.leap_seconds(), eop.dUT1().seconds(), timescale) {}

/**
 * Instantiates an astrometric time instance with the specified time parameters.
 *
 *
 * @param ijd           [day] integer part of Julian date (in the selected timescale)
 * @param fjd           [day] fractional part of Julian date (in the selected timescale)
 * @param leap_seconds  [s] leap seconds, that is TAI - UTC (default: 0)
 * @param dUT1          [s] UT1 - UTC time difference, e.g. from the IERS Bulletins or service (default: 0.0).
 * @param timescale     (optional) Astronomical timescale (default: TT).
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(long ijd, double fjd, int leap_seconds, double dUT1, enum novas_timescale timescale) {
  novas_set_split_time(timescale, ijd, fjd, leap_seconds, dUT1, &_ts);

  if(!isfinite(fjd))
    novas_set_errno(EINVAL, "Time()", "input jd is NAN or infinite");
  else
    _valid = is_valid_parms(dUT1, timescale);
}

/**
 * Instantiates an astrometric time instance with the specified time parameters.
 *
 * @param ijd           [day] integer part of Julian date (in the selected timescale)
 * @param fjd           [day] fractional part of Julian date (in the selected timescale)
 * @param eop           Earth Orientation Parameters (EOP) values, e.g. obtained from IERS.
 * @param timescale     (optional) Astronomical timescale (default: TT).
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(long ijd, double fjd, const EOP& eop, enum novas_timescale timescale)
: Time(ijd, fjd, eop.leap_seconds(), eop.dUT1().seconds(), timescale) {}

/**
 * Instantiates an astrometric time instance with the specified time parameters.
 *
 * @param timestamp     A precision timestamp, such as an ISO 8601 timestamp.
 * @param leap_seconds  [s] leap seconds, that is TAI - UTC (default: 0)
 * @param dUT1          [s] UT1 - UTC time difference, e.g. from the IERS Bulletins or service (default: 0.0).
 * @param timescale     (optional) Astronomical timescale (default: TT).
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(const std::string& timestamp, int leap_seconds, double dUT1, enum novas_timescale timescale) {
  if(novas_set_str_time(timescale, timestamp.c_str(), leap_seconds, dUT1, &_ts) != 0)
    novas_trace_invalid("Time()");
  else
    _valid = is_valid_parms(dUT1, timescale);
}

/**
 * Instantiates an astrometric time instance with the specified time parameters.
 *
 * @param timestamp     A precision timestamp, such as an ISO 8601 timestamp.
 * @param eop           Earth Orientation Parameters (EOP) values, e.g. obtained from IERS.
 * @param timescale     (optional) Astronomical timescale (default: TT).
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(const std::string& timestamp, const EOP& eop, enum novas_timescale timescale)
: Time(timestamp, eop.leap_seconds(), eop.dUT1().seconds(), timescale) {}

/**
 * Instantiates an astrometric time instance with the specified time parameters.
 *
 * @param t             A precision POSIX standard UTC timestamp.
 * @param leap_seconds  [s] leap seconds, that is TAI - UTC (default: 0)
 * @param dUT1          [s] UT1 - UTC time difference, e.g. from the IERS Bulletins or service (default: 0.0).
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(const struct timespec *t, int leap_seconds, double dUT1) {
  if(!t)
    novas_set_errno(EINVAL, "Time()", "input timespec is NULL");
  else {
    novas_set_unix_time(t->tv_sec, t->tv_nsec, leap_seconds, dUT1, &_ts);
    _valid = is_valid_parms(dUT1, NOVAS_UTC);
  }
}

/**
 * Instantiates an astrometric time instance with the specified time parameters.
 *
 * @param t             A precision POSIX standard UTC timestamp.
 * @param eop           Earth Orientation Parameters (EOP) values, e.g. obtained from IERS.
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(const struct timespec *t, const EOP& eop)
: Time(t, eop.leap_seconds(), eop.dUT1().seconds()) {}

/**
 * Instantiates an astrometric time instance with the specified SuperNOVAS C astrometric
 * time specification data structure.
 *
 * @param t   pointer to the SuperNOVAS C astrometric time specification data structure.
 *
 * @sa now(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time::Time(const novas_timespec *t) {
  static const char *fn = "Time()";

  if(!t)
    novas_set_errno(EINVAL, fn, "input timespec is NULL");
  else if(!isfinite(t->fjd_tt))
    novas_set_errno(EINVAL, fn, "input t->fjd_tt is NAN or infinite");
  else if(!isfinite(t->ut1_to_tt))
    novas_set_errno(EINVAL, fn, "input t->ut1_to_tt is NAN or infinite");
  else if(!isfinite(t->tt2tdb))
    novas_set_errno(EINVAL, fn, "input t->tt2tdb is NANA or infinite");
  else
    _valid = true;

  if(_valid) _ts = *t;
}

/**
 * Returns an offset time. It assumes that the leap seconds and UT1 - UTC time difference of this
 * time instance is also valid for the offset time, which should be true typically for reasonably
 * small time offsets.
 *
 * @param seconds     [s] the offset interval
 * @return            a new time that is offset from this one by the specified interval in the
 *                    reverse direction (backwards in time).
 *
 * @sa oerator+(), shifted()
 */
Time Time::operator+(double seconds) const {
  return shifted(seconds);
}

/**
 * Returns an offset time in the backward direction. It assumes that the leap seconds and
 * UT1 - UTC time difference of this time instance is also valid for the offset time, which should
 * be true typically for reasonably small time offsets.
 *
 * @param offset    the offset interval (in backwards direction).
 * @return          a new time that is offset from this one by the specified interval.
 *
 * @sa oerator-(), shifted()
 */
Time Time::operator+(const Interval& offset) const {
  return shifted(offset);
}

/**
 * Returns an offset time in the backwards direction. It assumes that the leap seconds and
 * UT1 - UTC time difference of this time instance is also valid for the offset time, which should
 * be true typically for reasonably small time offsets.
 *
 * @param seconds     [s] the offset interval (backwards direction).
 * @return            a new time that is offset from this one by the specified interval in the
 *                    reverse direction (backwards in time).
 *
 * @sa oerator+(), shifted()
 */
Time Time::operator-(double seconds) const {
  return shifted(-seconds);
}

/**
 * Returns an offset time. It assumes that the leap seconds and UT1 - UTC time difference of this
 * time instance is also valid for the offset time, which should be true typically for reasonably
 * small time offsets.
 *
 * @param offset     the offset interval
 * @return           a new time that is offset from this one by the specified interval in the
 *                    reverse direction (backwards in time).
 *
 * @sa operator+(), shifted()
 */
Time Time::operator-(const Interval& offset) const {
  return shifted(offset.inv());
}

/**
 * Returns the difference of this time from the specified other time, in regular Earth-based
 * time measures (GPS, TAI, TT -- and also UTC assuming no change in leap seconds).
 *
 * @param r   the other time
 * @return    the difference between this time and the argument in regular Earth-based
 *            timescales.
 *
 * offset_from()
 */
Interval Time::operator-(const Time& r) const {
  return Interval(novas_diff_time(&_ts, &r._ts));
}

/**
 * Checks if this time is prior to the specified other time, at the full precision of the
 * representation.
 *
 * @param r   the other time
 * @return    `true` if this time instance is before the specified other time, or else
 *            `false`.
 *
 * @sa operator<=(), operator>(), operator>=()
 */
bool Time::operator<(const Time& r) const {
  return novas_diff_time(&_ts, &r._ts) < 0.0;
}

/**
 * Checks if this time is after to the specified other time, at the full precision of the
 * representation.
 *
 * @param r   the other time
 * @return    `true` if this time instance is after the specified other time, or else
 *            `false`.
 *
 * @sa operator>=(), operator<(), operator<=()
 */
bool Time::operator>(const Time& r) const {
  return novas_diff_time(&_ts, &r._ts) > 0.0;
}

/**
 * Checks if this time is the same as or before the specified other time, at the full precision of
 * the representation.
 *
 * @param r   the other time
 * @return    `true` if this time instance is the same as or before the specified other time, or
 *            else `false`.
 *
 * @sa operator<(), operator>=(), operator>()
 */
bool Time::operator<=(const Time& r) const {
  return novas_diff_time(&_ts, &r._ts) <= 0.0;
}

/**
 * Checks if this time is the same as or after the specified other time, at the full precision of
 * the representation.
 *
 * @param r   the other time
 * @return    `true` if this time instance is the same as or after the specified other time, or
 *            else `false`.
 *
 * @sa operator<(), operator>=(), operator>()
 */
bool Time::operator>=(const Time& r) const {
  return novas_diff_time(&_ts, &r._ts) >= 0.0;
}

/**
 * Checks if this time instance is the same as another time within the specified precision.
 *
 * @param time        the other time
 * @param precision   [s] (optional) The precision for the comparison (default: 1 &mus)
 * @return            `true` if the two times are equal within the specified precision, otherwise
 *                    `false`.
 *
 * @sa operator==(), operator!=()
 */
bool Time::equals(const Time& time, double precision) const {
  return fabs(novas_diff_time(&_ts, &time._ts)) <= fabs(precision);
}

/**
 * Checks if this time instance is the same as another time within the specified precision.
 *
 * @param time        the other time
 * @param precision   (optional) The precision for the comparison (default: 1 &mu;s).
 * @return            `true` if the two times are equal within the specified precision, otherwise
 *                    `false`.
 *
 * @sa operator==(), operator!=()
 */
bool Time::equals(const Time& time, const Interval& precision) const {
  return equals(time, precision.seconds());
}

/**
 * Checks if this time instance is the same as another time within 1 &mu;s.
 *
 * @param time        the other time
 * @return            `true` if the two times are equal within 1 &mu;s, otherwise `false`.
 *
 * @sa operator==(), operator!=()
 */
bool Time::operator==(const Time& time) const {
  return equals(time);
}

/**
 * Checks if this time instance differs from another time by more than 1 &mu;s.
 *
 * @param time        the other time
 * @return            `true` if the two times differ by more than 1 &mu;s.
 *
 * @sa operator==(), operator!=()
 */
bool Time::operator!=(const Time& time) const {
  return !equals(time);
}

/**
 * Returns the underlying C astrometric time specification data structure.
 *
 * @return    a pointer to the underlying C time specification data structure.
 */
const novas_timespec * Time::_novas_timespec() const {
  return &_ts;
}

/**
 * Returns the precise Julian Date of this time instance, in the specific timescale of choice.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            [day] the precise Julian date in the requested timescale.
 *
 * @sa jd_day(), mjd()
 */
double Time::jd(enum novas_timescale timescale) const {
  return novas_get_time(&_ts, timescale);
}

/**
 * Returns the integer Julian day of this time instance, in the specific timescale of choice.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            [day] the integer Julian day in the requested timescale.
 *
 * @sa jd_frac(), mjd_day(), mjd(), jd()
 */
long Time::jd_day(enum novas_timescale timescale) const {
  long ijd = 0;
  novas_get_split_time(&_ts, timescale, &ijd);
  return ijd;
}

/**
 * Returns the integer Modified Julian Day (MJD) of this time instance, in the specific timescale
 * of choice.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            [day] the integer Modified Julian Day (MJD) in the requested timescale.
 *
 * @sa jd_frac(), jd_day(), mjd(), jd()
 */
long Time::mjd_day(enum novas_timescale timescale) const {
  long ijd = 0;
  double fjd = novas_get_split_time(&_ts, timescale, &ijd);
  ijd -= 2400000L;
  return fjd >= 0.5 ? ijd + 1 : ijd;
}

/**
 * Returns the factional Julian Day (MJD) of this time instance, in the specific timescale
 * of choice. 0h in Julian dates is at noon.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            [day] the fractional part of the Julian Day in the requested timescale.
 *
 * @sa mjd_frac(), jd_day(), jd()
 */
double Time::jd_frac(enum novas_timescale timescale) const {
  return novas_get_split_time(&_ts, timescale, NULL);
}

/**
 * Returns the integer Modified Julian Day (MJD) of this time instance, in the specific timescale
 * of choice. 0h in MJD is at midnight.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            [day] the fractional part of the Julian Day in the requested timescale.
 *
 * @sa jd_frac(), mjd_day(), mjd()
 */
double Time::mjd_frac(enum novas_timescale timescale) const {
  double f = jd_frac(timescale);
  return f < 0.5 ? f + 0.5 : f - 0.5;
}

/**
 * Returns the precise Modified Julian Date (MJD) this time instance, in the specific timescale of
 * choice.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            [day] the precise Modifie Julian Date (MJD) in the requested timescale.
 *
 * @sa mjd_day(), jd()
 */
double Time::mjd(enum novas_timescale timescale) const {
  return (_ts.ijd_tt - (int) NOVAS_JD_MJD0) + _ts.fjd_tt - 0.5;
}

/**
 * Returns the precise UNIX time (UTC) corresponding to this time instance.
 *
 * @param[out] nanos    [ns] (optional) pointer to value in which to return the sub-second
 *                      component, or NULL if not required.
 * @return              [s] the integer UNIX seconds (UTC) of this time instance.
 */
time_t Time::unix_time(long *nanos) const {
  return novas_get_unix_time(&_ts, nanos);
}

/**
 * Returns the leap seconds for this time instance, as was provided during instantiation.
 *
 * @return      [s] the leap seconds (TAI - UTC).
 *
 * @sa dUT1()
 */
int Time::leap_seconds() const {
  return (_ts.ut1_to_tt - _ts.dut1 - DTA);
}

/**
 * Returns the UT1 - UTC time difference for this time instance, as was provided during
 * instantiation.
 *
 * @return      [s] The UT1 - UTC time difference.
 *
 * @sa leap_seconds()
 */
Interval Time::dUT1() const {
  return Interval(_ts.dut1);
}

/**
 * Returns the Julian coordinate epoch year of this time instance.
 *
 * @return      [yr] the Julian coordinate epoch (e.g. 2000.0 for J2000).
 */
double Time::epoch() const {
  return 2000.0 + (jd() - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

/**
 * Returns the Greenwich (Apparent) Sidereal Time (GST / GaST) for this time instance.
 *
 * @param accuracy      NOVAS_FULL_ACCURACY (0) for &mu;s-level precision or
 *                      NOVAS_REDUCED_ACCURACY (1) for &ms-level precison.
 * @return              the GST / GaST time-angle.
 *
 * @sa gmst(), lst(), era()
 */
TimeAngle Time::gst(enum novas_accuracy accuracy) const {
  return TimeAngle(novas_time_gst(&_ts, accuracy) * Unit::hour_angle);
}

/**
 * Returns the Greenwich Mean Sidereal Time (GMST) for this time instance.
 *
 * @return    the GMST time-angle
 *
 * @sa gst(), lst()
 */
TimeAngle Time::gmst() const {
  double jd_ut1 = novas_get_time(&_ts, NOVAS_UT1);
  return TimeAngle(novas_gmst(jd_ut1, _ts.ut1_to_tt) * Unit::hour_angle);
}

/**
 * Returns the Local (Apparent) Sidereal Time for this time instance.
 *
 * @param site        observing site
 * @param accuracy    NOVAS_FULL_ACCURACY (0) for &mu;s-level precision or
 *                    NOVAS_REDUCED_ACCURACY (1) for &ms-level precison.
 * @return            the LST / LaST time-angle.
 *
 * @sa gst(), era()
 */
TimeAngle Time::lst(const Site& site, enum novas_accuracy accuracy) const {
  return TimeAngle(novas_time_lst(&_ts, site.longitude().deg(), accuracy) * Unit::hour_angle);
}

/**
 * Returns the Earth Rotation Angle (ERA), which measures the angle between the Terrestrial
 * Intermediate Origin (TIO) and the Celestial Intermediate Origin (CIO) in the IAU 2000
 * convention.
 *
 * @return            The Earth Rotation Angle (ERA) as a time-angle.
 */
TimeAngle Time::era() const {
  return TimeAngle(novas_era(_ts.ijd_tt, _ts.fjd_tt) * Unit::deg);
}

/**
 * Returns the time of (calendar) day of this time instance in the timescale of choice.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            The time of day in the requested timescale.
 *
 * @sa jd_time_of_day()
 */
TimeAngle Time::time_of_day(enum novas_timescale timescale) const {
  return TimeAngle(TWOPI * remainder(novas_get_split_time(&_ts, timescale, NULL) - 0.5, 1.0));
}

/**
 * Returns the day of week index (1..7) for this time instance.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            The day-of-week index [1:7] in the same timescale as the input date. 1:Monday
 *                    ... 7:Sunday, or else 0 if the input Julian Date is NAN.
 */
int Time::day_of_week(enum novas_timescale timescale) const {
  return novas_day_of_week(jd(timescale));
}

/**
 * Returns the phase of the Moon at this time instance.
 *
 * @return      The Moon's phase, or more precisely the ecliptic longitude difference
 *              between the Sun and the Moon, as seen from the geocenter. 0&deg;: New Moon,
 *              90&deg;: 1st quarter, +/- 180&deg; Full Moon, -90&deg;: 3rd quarter, or NAN if the
 *              solution failed to converge (errno will be set to ECANCELED).
 *
 * @sa next_moon_phase()
 */
Angle Time::moon_phase() const {
  return Angle(novas_moon_phase(jd(NOVAS_TDB)) * Unit::deg);
}

/**
 * Returns the next time, after this time instance, when the Moon will be in the specified phase.
 *
 * @param phase   The Moon's phase. 0&deg;: New Moon, 90&deg;: 1st quarter, +/- 180&deg; Full Moon,
 *                -90&deg;: 3rd quarter.
 * @return        The next time the Moon will be in the specified phase.
 *
 * @sa moon_phase()
 */
Time Time::next_moon_phase(const Angle& phase) const {
  return Time(novas_next_moon_phase(jd(NOVAS_TDB), phase.deg()), leap_seconds(), dUT1().seconds(), NOVAS_TDB);
}

/**
 * Returns a string representation (a timestamp) of this time instance.
 *
 * @param timescale   (optional) the timescale in which to represent time (default: UTC).
 * @return            a new string containing the timestamp.
 *
 * @sa to_iso_timestamp(), to_epoch_string()
 */
std::string Time::to_string(enum novas_timescale timescale) const {
  char str[40] = {'\0'};
  novas_timestamp(&_ts, timescale, str, sizeof(str));
  return std::string(str);
}

/**
 * Returns an ISO 8601 timestamp for this time instance.
 *
 * @return            a new string containing the ISO 8601 timestamp.
 *
 * @sa to_string()
 */
std::string Time::to_iso_string() const {
  char str[40] = {'\0'};
  novas_iso_timestamp(&_ts, str, sizeof(str));
  return std::string(str);
}

/**
 * Returns the string representation of the Julian coordinate epoch corresponding to this timestamp
 *
 * @param decimals    (optional) number of decimal places to show on the year (default: 2).
 *
 * @return        a new string representing the Julian coordinate epoch of this time with the
 *                requested precision.
 */
std::string Time::to_epoch_string(int decimals) const {
  char fmt[20] = {'\0'}, s[40] = {'\0'};

  if(decimals < 0)
    decimals = 0;
  else if(decimals > 12)
    decimals = 12;

  snprintf(fmt, sizeof(fmt), "J%%.%df", decimals);
  snprintf(s, sizeof(s), fmt, epoch());

  return std::string(s);
}

/**
 * Returns a new astrometric time instance for a given Modified Julian Date (MJD), specified in the
 * timescale of choice.
 *
 * @param mjd           [day] Modified Julian Date, in the timescale of choice
 * @param leap_seconds  [s] leap seconds, that is TAI - UTC (default: 0)
 * @param dUT1          [s] UT1 - UTC time difference, e.g. from the IERS Bulletins or service (default: 0.0).
 * @param timescale     (optional) timescale in which MJD was specified (default: TT)
 * @return              a new astrometric time instance for the given MJD date.
 *
 * @sa Time(), now(), j2000(), b1950(), b1900(), hip()
 */
Time from_mjd(double mjd, int leap_seconds, double dUT1, enum novas::novas_timescale timescale) {
  return Time((int) NOVAS_JD_MJD0, mjd + 0.5, leap_seconds, dUT1, timescale);
}

/**
 * Returns a new astrometric time instance for a given Modified Julian Date (MJD), specified in the
 * timescale of choice.
 *
 * @param mjd           [day] Modified Julian Date, in the timescale of choice
 * @param eop           Earth Orientation Parameters (EOP) values, e.g. obtained from IERS.
 * @param timescale     (optional) timescale in which MJD was specified (default: TT)
 * @return              a new astrometric time instance for the given MJD date.
 *
 * @sa Time(), now(), j2000(), b1950(), b1900(), hip()
 */
Time from_mjd(double mjd, const EOP& eop, enum novas::novas_timescale timescale) {
  return Time((int) NOVAS_JD_MJD0, mjd + 0.5, eop, timescale);
}

/**
 * Resturns a new astrometric time instance for the current time. It is only as accurate as the
 * system clock, and its precision may be limited by the resolution of the system clock also. Be
 * sure that your computer is well synchronized to a trustworthy time server, preferably on a local
 * network, such as an ntp server with a GPS receiver.
 *
 * @param eop           Earth Orientation Parameters (EOP) values, e.g. obtained from IERS.
 * @return              a new astrometric time corresponding to the current system time.
 *
 * @sa Time(), from_mjd(), j2000(), b1950(), b1900(), hip()
 */
Time Time::now(const EOP& eop) {
  Time time = Time();
  novas_set_current_time(eop.leap_seconds(), eop.dUT1().seconds(), &time._ts);
  return time;
}

/**
 * Returns the difference of this time from the specified other time, in the specified timescale.
 * All timescales are supported.
 *
 * @param time        the other time
 * @param timescale   the timescale in which to return the result.
 * @return            the difference between this time and the argument time in the specified
 *                    timescale.
 *
 * operator-(), shifted()
 */
Interval Time::offset_from(const Time& time, enum novas_timescale timescale) {
  double dt = novas_diff_time_scale(&_ts, &time._ts, timescale);
  if(isnan(dt))
    novas_trace_invalid("Time::offset_from()");

  return Interval(dt);
}

/**
 * Returns an offset time. It assumes that the leap seconds and UT1 - UTC time difference of this
 * time instance is also valid for the offset time, which should be true typically for reasonably
 * small time offsets.
 *
 * @param seconds    [s] the offset interval in a regular Earth-based timescale (GPS, TAI, TT --
 *                   or UTC assuming no change in leap seconds).
 * @param timescale  (optional) timescale in which offset time is given (default: TT / TAI / GPS).
 * @return           a new time that is offset from this one by the specified interval in the
 *                   specified timescale.
 *
 * @sa oerator+(), offset_from()
 */
Time Time::shifted(double seconds, enum novas_timescale timescale) const {
  long ijd;
  double fjd = novas_get_split_time(&_ts, timescale, &ijd);
  novas_timespec ts1 = {};

  fjd += seconds / Unit::day;;

  novas_set_split_time(timescale, ijd, fjd, leap_seconds(), dUT1().seconds(), &ts1);
  return Time(&ts1);
}

/**
 * Returns an offset time. It assumes that the leap seconds and UT1 - UTC time difference of this
 * time instance is also valid for the offset time, which should be true typically for reasonably
 * small time offsets.
 *
 * @param offset     the offset interval in a regular Earth-based timescale (GPS, TAI, TT --
 *                   or UTC assuming no change in leap seconds).
 * @param timescale  (optional) timescale in which offset time is given (default: TT / TAI / GPS).
 * @return           a new time that is offset from this one by the specified interval in the
 *                   specified timescale
 *
 * @sa oerator+(), offset_from()
 */
Time Time::shifted(const Interval& offset, enum novas_timescale timescale) const {
  return shifted(offset.to_timescale(NOVAS_TT).seconds());
}

/**
 * Returns a calendar date, in the calendar of choice, and in the timescale of choice, for time
 * instance.
 *
 * @param calendar    (optional) Calendar in which to represent this time instance (default:
 *                    astronomical)
 * @param timescale   (optional) timescale in which MJD was specified (default: TT)
 * @return            the calendar date in the desired calendar and timescale of choice.
 */
CalendarDate Time::to_calendar_date(const Calendar& calendar, enum novas::novas_timescale timescale) const {
  return calendar.date(jd(timescale));
}

/**
 * Returns the astronomical calendar date instance for this astrometric time. The astronomical
 * calendar is the conventional calendar of date, which is the Gregorian calendar after the
 * Gregorian calendar reform if 1582, and the Roman calendar prior to that.
 *
 * @param timescale   timescale in which MJD was specified (default: TT)
 * @return            the astronomical calendar date in the timescale of choice.
 */
CalendarDate Time::to_calendar_date(enum novas::novas_timescale timescale) const {
  return to_calendar_date(Calendar::astronomical(), timescale);
}

/**
 * Returns the astrometric time for the J2000 epoch.
 *
 * @return    the astrometric time for J2000, that is 12:00:00 TT, 1 Jan 2000.
 *
 * @sa b1950, b1900(), hip()
 */
const Time& Time::j2000() {
  static const Time _j2000 = Time(NOVAS_JD_J2000, 32, 0.0, NOVAS_TT);
  return _j2000;
}

/**
 * Returns the astrometric time for the Hipparcos catalog epoch (which is J1991.25).
 *
 * @return    the astrometric time for the Hipparcos catalog epoch, that is J1991.25.
 *
 * @sa j2000(), b1950, b1900()
 */
const Time& Time::hip() {
  static const Time _hip = Time(NOVAS_JD_HIP, 26, 0.0, NOVAS_TT);
  return _hip;
}

/**
 * Returns the astrometric time for the B1950 epoch.
 *
 * @return    the astrometric time for B1950.
 *
 * @sa b1900(), j2000(), hip()
 */
const Time& Time::b1950() {
  static const Time _b1950 = Time(NOVAS_JD_B1950, 0, 0.0, NOVAS_TT);
  return _b1950;
}

/**
 * Returns the astrometric time for the B1900 epoch.
 *
 * @return    the astrometric time for B1900.
 *
 * @sa b1950(), j2000(), hip()
 */
const Time& Time::b1900() {
  static const Time _b1900 = Time(NOVAS_JD_B1900, 0, 0.0, NOVAS_TT);
  return _b1900;
}

/**
 * Returns a reference to a statically defined standard invalid astrometric time. This invalid
 * time may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid astrometric time.
 */
const Time& Time::invalid() {
  static const Time _invalid = Time(NAN, EOP::invalid(), NOVAS_TT);
  return _invalid;
}

} // namespace supernovas


