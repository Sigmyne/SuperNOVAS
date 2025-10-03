/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


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
: _seconds(seconds), _scale(timescale) {}



enum novas_timescale Interval::timescale() const {
  return _scale;
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

TimeAngle operator+(const Interval& l, const TimeAngle& base) {
  return TimeAngle(l.seconds() + base.seconds());
}


} // namespace supernovas
