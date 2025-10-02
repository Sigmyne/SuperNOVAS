/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"

using namespace supernovas;


Time::Time(double jd, const EOP& eop, enum novas_timescale timescale) {
  novas_set_time(timescale, jd, eop.leap_seconds(), eop.dUT1(), &_ts);
}

Time::Time(const std::string& timestamp, const EOP& eop, enum novas_timescale timescale) {
  novas_set_str_time(timescale, timestamp.c_str(), eop.leap_seconds(), eop.dUT1(), &_ts);
}

Time::Time(const struct timespec *t, const EOP& eop) {
  novas_set_unix_time(t->tv_sec, t->tv_nsec, eop.leap_seconds(), eop.dUT1(), &_ts);
}

Time::Time(const novas_timespec *t) : _ts(*t) {}

const novas_timespec * Time::_novas_timespec() const {
  return &_ts;
}

double Time::jd(enum novas_timescale timescale) const {
  return novas_get_time(&_ts, timescale);
}

double Time::epoch() const {
  return 2000.0 + (jd() - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

TimeAngle Time::time_of_day(enum novas_timescale timescale) const {
  return TimeAngle(TWOPI * novas_get_split_time(&_ts, timescale, NULL));
}

int Time::day_of_week(enum novas_timescale timescale) const {
  return novas_day_of_week(jd(timescale));
}

std::string Time::str(enum novas_timescale timescale) const {
  char str[40] = {'\0'};
  novas_timestamp(&_ts, timescale, str, sizeof(str));
  return std::string(str);
}

std::string Time::iso_str() const {
  char str[40] = {'\0'};
  novas_iso_timestamp(&_ts, str, sizeof(str));
  return std::string(str);
}

std::string Time::epoch_str() const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "J%.2f", epoch());
  return std::string(s);
}

Time Time::now(const EOP& eop) {
  Time time = Time();
  novas_set_current_time(eop.leap_seconds(), eop.dUT1(), &time._ts);
  return time;
}

Interval operator-(const Time& l, const Time &r) {
  return Interval(novas_diff_time(l._novas_timespec(), r._novas_timespec()), NOVAS_TT);
}


