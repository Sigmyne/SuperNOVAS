/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"

namespace supernovas {

class Time {
protected:
  novas_timespec _ts = {};

  Time();

public:

  Time(double jd, const EOP& eop, enum novas_timescale timescale = NOVAS_TT) {
    novas_set_time(timescale, jd, eop.leap_seconds(), eop.dUT1(), &_ts);
  }

  Time(const std::string& timestamp, const EOP& eop, enum novas_timescale timescale = NOVAS_UTC) {
    novas_set_str_time(timescale, timestamp.c_str(), eop.leap_seconds(), eop.dUT1(), &_ts);
  }

  Time(const struct timespec *t, const EOP& eop) {
    novas_set_unix_time(t->tv_sec, t->tv_nsec, eop.leap_seconds(), eop.dUT1(), &_ts);
  }

  Time(const novas_timespec *t) : _ts(*t) {}

  Interval operator-(const Time &other) const {
    return Interval(novas_diff_time(&_ts, &other._ts), NOVAS_TT);
  }

  const novas_timespec * _novas_timespec() const {
    return &_ts;
  }

  double jd(enum novas_timescale timescale = NOVAS_TT) const {
    return novas_get_time(&_ts, timescale);
  }

  double epoch() const {
    return 2000.0 + (jd() - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
  }

  TimeAngle time_of_day(enum novas_timescale timescale = NOVAS_TT) const {
    return TimeAngle(TWOPI * novas_get_split_time(&_ts, timescale, NULL));
  }

  double day_of_week(enum novas_timescale timescale = NOVAS_TT) const {
    return novas_day_of_week(jd(timescale));
  }

  std::string str(enum novas_timescale timescale = NOVAS_UTC) const {
    char str[40] = {'\0'};
    novas_timestamp(&_ts, timescale, str, sizeof(str));
    return std::string(str);
  }

  std::string iso_str() const {
    char str[40] = {'\0'};
    novas_iso_timestamp(&_ts, str, sizeof(str));
    return std::string(str);
  }

  std::string epoch_str() const {
    char s[40] = {'\0'};
    snprintf(s, sizeof(s), "J%.2f", epoch());
    return std::string(s);
  }

  static Time now(const EOP& eop) {
    Time time = Time();
    novas_set_current_time(eop.leap_seconds(), eop.dUT1(), &time._ts);
    return time;
  }


};


} // namespace supernovas

