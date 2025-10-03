/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


extern "C" {
#include "novas.h"
}

static double novas_era(long ijd, double fjd) {
  return era((double) ijd, fjd);
}


#include "supernovas.h"

#define E9      1000000000L


using namespace supernovas;



Time::Time(double jd, const EOP& eop, enum novas_timescale timescale) {
  novas_set_time(timescale, jd, eop.leap_seconds(), eop.dUT1(), &_ts);
}

Time::Time(long ijd, double fjd, const EOP& eop, enum novas_timescale timescale) {
  novas_set_split_time(timescale, ijd, fjd, eop.leap_seconds(), eop.dUT1(), &_ts);
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

TimeAngle Time::gst(enum novas_accuracy accuracy) const {
  return TimeAngle(novas_time_gst(&_ts, accuracy) * Unit::hourAngle);
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

Interval operator-(const Time& l, const Time &r) {
  return Interval(novas_diff_time(l._novas_timespec(), r._novas_timespec()), NOVAS_TT);
}


