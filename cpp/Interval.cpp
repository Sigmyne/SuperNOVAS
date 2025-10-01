/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"


namespace supernovas {
class Interval {
private:

  double tt() const {
    switch(_scale) {
      case NOVAS_TCB: return  _seconds / (1.0 + Constant::L_B);
      case NOVAS_TCG: return  _seconds / (1.0 + Constant::L_G);
      default: return _seconds;
    }
  }

  double from_tt(double x) const {
    switch(_scale) {
      case NOVAS_TCB: return x * (1.0 + Constant::L_B);
      case NOVAS_TCG: return x * (1.0 + Constant::L_G);
      default: return x;
    }
  }

protected:
  double _seconds;
  enum novas_timescale _scale;

public:

  Interval(double seconds, enum novas_timescale timescale = NOVAS_TT)
  : _seconds(seconds), _scale(timescale) {}

  Interval operator+(const Interval& r) const {
    return Interval(from_tt(tt() + r.tt()), _scale);
  }

  Interval operator-(const Interval& r) const {
    return Interval(from_tt(tt() - r.tt()), _scale);
  }

  TimeAngle operator+(const TimeAngle& base) const {
    return TimeAngle(_seconds + base.seconds());
  }

  enum novas_timescale timescale() const {
    return _scale;
  }

  double milliseconds() const {
    return _seconds / Unit::ms;
  }

  double seconds() const {
    return _seconds;
  };

  double minutes() const {
    return _seconds / Unit::min;
  }

  double hours() const {
    return _seconds / Unit::hour;
  }

  double days() const {
    return _seconds / Unit::day;
  }

  double years() const {
    return _seconds / Unit::yr;
  }

  double julian_years() const {
    return _seconds / Unit::julianYear;
  }

  double julian_centuries() const {
    return _seconds / Unit::julianCentury;
  }

};

} // namespace supernovas
