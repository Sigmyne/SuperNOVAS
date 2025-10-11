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
#define DTA         32.184        ///< [s] TT - TAI time difference
/// \endcond

static double novas_era(long ijd, double fjd) {
  return novas::era((double) ijd, fjd);
}

#define E9      1000000000L

using namespace novas;

namespace supernovas {

bool Time::is_valid_parms(double dUT1,  enum novas_timescale timescale) const {
  static const char *fn = "Time()";

  if(isnan(dUT1))
    return novas_error(0, EINVAL, fn, "input dUT1 is NAN");
  else if(fabs(dUT1) > 1.0)
    return novas_error(0, EINVAL, fn, "input dUT1 exceeds +/- 1s limit: %g", dUT1);
  else if(timescale < 0 || timescale >= NOVAS_TIMESCALES)
    return novas_error(0, EINVAL, fn, "invalid timescale: %d", timescale);
  else
    return 1;
}

Time::Time(double jd, int leap_seconds, double dUT1, enum novas_timescale timescale) {
  static const char *fn = "Time()";

  novas_set_time(timescale, jd, leap_seconds, dUT1, &_ts);

  if(isnan(jd))
    novas_error(0, EINVAL, fn, "input jd is NAN");
  else
    _valid = is_valid_parms(dUT1, timescale);

}

Time::Time(double jd, const EOP& eop, enum novas_timescale timescale)
: Time(jd, eop.leap_seconds(), eop.dUT1(), timescale) {}

Time::Time(long ijd, double fjd, int leap_seconds, double dUT1, enum novas_timescale timescale) {
  static const char *fn = "Time()";

  novas_set_split_time(timescale, ijd, fjd, leap_seconds, dUT1, &_ts);

  if(isnan(fjd))
    novas_error(0, EINVAL, fn, "input jd is NAN");
  else
    _valid = is_valid_parms(dUT1, timescale);

}

Time::Time(long ijd, double fjd, const EOP& eop, enum novas_timescale timescale)
: Time(ijd, fjd, eop.leap_seconds(), eop.dUT1(), timescale) {}

Time::Time(const std::string& timestamp, int leap_seconds, double dUT1, enum novas_timescale timescale) {
  static const char *fn = "Time()";

  if(novas_set_str_time(timescale, timestamp.c_str(), leap_seconds, dUT1, &_ts) != 0)
    novas_trace_invalid(fn);
  else
    _valid = is_valid_parms(dUT1, timescale);
}

Time::Time(const std::string& timestamp, const EOP& eop, enum novas_timescale timescale)
: Time(timestamp, eop.leap_seconds(), eop.dUT1(), timescale) {}

Time::Time(const struct timespec t, int leap_seconds, double dUT1) {
  _valid = is_valid_parms(dUT1, NOVAS_UTC);
  novas_set_unix_time(t.tv_sec, t.tv_nsec, leap_seconds, dUT1, &_ts);
}

Time::Time(const struct timespec t, const EOP& eop)
: Time(t, eop.leap_seconds(), eop.dUT1()) {}

Time::Time(const novas_timespec *t) : _ts(*t) {}

Interval Time::operator-(const Time& r) const {
  return Interval(novas_diff_time(&_ts, &r._ts));
}

bool Time::operator<(const Time& r) const {
  return novas_diff_time(&_ts, &r._ts) < 0.0;
}

bool Time::operator>(const Time& r) const {
  return novas_diff_time(&_ts, &r._ts) > 0.0;
}

bool Time::operator<=(const Time& r) const {
  return novas_diff_time(&_ts, &r._ts) <= 0.0;
}

bool Time::operator>=(const Time& r) const {
  return novas_diff_time(&_ts, &r._ts) >= 0.0;
}

bool Time::equals(const Time& time, double precision) const {
  return fabs(novas_diff_time(&_ts, &time._ts)) <= fabs(precision);
}

bool Time::equals(const Time& time, const Interval& interval) const {
  return equals(time, interval.seconds());
}

const novas_timespec * Time::_novas_timespec() const {
  return &_ts;
}

double Time::jd(enum novas_timescale timescale) const {
  return novas_get_time(&_ts, timescale);
}

int Time::leap_seconds() const {
  return (int) round(_ts.ut1_to_tt - DTA);
}

Interval Time::dUT1() const {
  return Interval(remainder(_ts.ut1_to_tt - DTA, 1.0));
}

double Time::epoch() const {
  return 2000.0 + (jd() - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

TimeAngle Time::gst(enum novas_accuracy accuracy) const {
  return TimeAngle(novas_time_gst(&_ts, accuracy) * Unit::hourAngle);
}

TimeAngle Time::gmst() const {
  double jd_ut1 = novas_get_time(&_ts, NOVAS_UT1);
  return TimeAngle(novas_gmst(jd_ut1, _ts.ut1_to_tt) * Unit::hourAngle);
}

TimeAngle Time::lst(const Site& site, enum novas_accuracy accuracy) const {
  return TimeAngle(novas_time_lst(&_ts, site.longitude().deg(), accuracy) * Unit::hourAngle);
}

TimeAngle Time::era() const {
  return TimeAngle(novas_era(_ts.ijd_tt, _ts.fjd_tt) * Unit::deg);
}

TimeAngle Time::time_of_day(enum novas_timescale timescale) const {
  return TimeAngle(TWOPI * novas_get_split_time(&_ts, timescale, NULL));
}

int Time::day_of_week(enum novas_timescale timescale) const {
  return novas_day_of_week(jd(timescale));
}

std::string Time::to_string(enum novas_timescale timescale) const {
  char str[40] = {'\0'};
  novas_timestamp(&_ts, timescale, str, sizeof(str));
  return std::string(str);
}

std::string Time::to_iso_string() const {
  char str[40] = {'\0'};
  novas_iso_timestamp(&_ts, str, sizeof(str));
  return std::string(str);
}

std::string Time::to_epoch_string() const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "J%.2f", epoch());
  return std::string(s);
}

Time Time::now(const EOP& eop) {
  Time time = Time();
  novas_set_current_time(eop.leap_seconds(), eop.dUT1(), &time._ts);
  return time;
}

Time Time::shifted(double seconds) const {
  struct novas_timespec ts = _ts;
  double djd = seconds / Unit::day;
  long idjd = (long) floor(djd);
  ts.ijd_tt += idjd;
  ts.fjd_tt += (djd - idjd);
  if(ts.fjd_tt > 1.0) {
    ts.ijd_tt++;
    ts.fjd_tt -= 1.0;
  }
  return Time(&ts);
}

Time Time::shifted(Interval offset) const {
  return shifted(offset.seconds());
}

Time operator+(const Time& l, const Interval& r) {
  return l.shifted(r);
}

Time operator-(const Time& l, const Interval& r) {
  return l.shifted(r.inv());
}

static const EOP _eop_j2000 = EOP(32, 0.0, 0.0, 0.0);
static const Time _j2000 = Time(NOVAS_JD_J2000, _eop_j2000, NOVAS_TT);
const Time& Time::j2000() {
  return _j2000;
}

static const EOP _eop_hip = EOP(26, 0.0, 0.0, 0.0);
static const Time _hip = Time(NOVAS_JD_HIP, _eop_hip, NOVAS_TT);
const Time& Time::hip() {
  return _hip;
}

static const EOP _eop_0 = EOP(0, 0.0, 0.0, 0.0);
static const Time _b1950 = Time(NOVAS_JD_B1950, _eop_0, NOVAS_TT);
const Time& Time::b1950() {
  return _b1950;
}

static const Time _b1900 = Time(NOVAS_JD_B1900, _eop_0, NOVAS_TT);
const Time& Time::b1900() {
  return _b1900;
}


static const Time _invalid = Time(NAN, EOP::invalid(), NOVAS_TT);
const Time& Time::invalid() {
  return _invalid;
}

} // namespace supernovas


