/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"


namespace supernovas {

class TimeAngle : Angle {

public:

  TimeAngle(double x)
  : Angle(x) {
    if(_rad < 0.0) _rad += TWOPI;
  }

  TimeAngle(const std::string& str)
  : Angle(novas_str_hours(str.c_str()) * Unit::hourAngle) {}

  TimeAngle(const Angle& angle)
  : Angle(angle) {}

  TimeAngle operator+(const Interval& offset) const {
    return TimeAngle(_rad + offset.seconds());
  }

  TimeAngle operator-(const Interval& offset) const {
    return TimeAngle(_rad - offset.seconds());
  }

  double hours() const {
    return _rad / Unit::hourAngle;
  }

  double minutes() const {
    return hours() * 60.0;
  }

  double seconds() const {
    return hours() * 3600.0;
  }

  std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) {
    char s[100] = {'\0'};
    novas_print_hms(hours(), separator, decimals, s, sizeof(s));
    return std::string(s);
  }

};



} // namespace supernovas
