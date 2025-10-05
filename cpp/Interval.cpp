/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"


using namespace novas;

namespace supernovas {

static Interval from_tt(double x, enum novas_timescale timescale) {
  switch(timescale) {
    case NOVAS_TCB: return Interval(x * (1.0 + Constant::L_B), timescale);
    case NOVAS_TCG: return Interval(x * (1.0 + Constant::L_G), timescale);
    default: return Interval(x, timescale);
  }
}

static double tt_seconds(const Interval& interval) {
  switch(interval.timescale()) {
    case NOVAS_TCB: return  interval.seconds() / (1.0 + Constant::L_B);
    case NOVAS_TCG: return  interval.seconds() / (1.0 + Constant::L_G);
    default: return interval.seconds();
  }
}


Interval::Interval(double seconds, enum novas_timescale timescale)
: _seconds(seconds), _scale(timescale) {
  static const char *fn = "Interval(seconds, timescale)";

  if(timescale < 0 || timescale >= NOVAS_TIMESCALES)
    novas_error(0, EINVAL, fn , "invalid timescale: %d", timescale);

  if(isnan(seconds))
    novas_error(0, EINVAL, fn , "input seconds is NAN");
}


TimeAngle Interval::operator+(const TimeAngle& base) const {
  return TimeAngle(_seconds + base.seconds());
}

Time Interval::operator+(const Time& base) const {
  EOP eop = EOP(base.leap_seconds(), base.dUT1().seconds(), 0.0, 0.0);
  return Time(base.jd() + days(), eop);
}

Distance Interval::operator*(const Speed& v) const {
  return v.travel(*this);
}

Position Interval::operator*(const Velocity& v) const {
  return v.travel(*this);
}

bool Interval::is_valid() const {
  return !isnan(_seconds);
}

bool Interval::is_equal(const Interval& interval, double precision) const {
  return fabs(_seconds - interval._seconds) < fabs(precision);
}

enum novas_timescale Interval::timescale() const {
  return _scale;
}

Interval Interval::inv() const {
  return Interval(-_seconds);
}

double Interval::milliseconds() const {
  return _seconds / Unit::ms;
}

double Interval::seconds() const {
  return _seconds;
};

double Interval::minutes() const {
  return _seconds / Unit::min;
}

double Interval::hours() const {
  return _seconds / Unit::hour;
}

double Interval::days() const {
  return _seconds / Unit::day;
}

double Interval::years() const {
  return _seconds / Unit::yr;
}

double Interval::julian_years() const {
  return _seconds / Unit::julianYear;
}

double Interval::julian_centuries() const {
  return _seconds / Unit::julianCentury;
}


Interval operator+(const Interval& l, const Interval& r) {
  return from_tt(tt_seconds(l) + tt_seconds(r), l.timescale());
}

Interval operator-(const Interval& l, const Interval& r) {
  return from_tt(tt_seconds(l) - tt_seconds(r), l.timescale());
}

} // namespace supernovas
