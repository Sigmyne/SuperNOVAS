/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"

using namespace novas;

namespace supernovas {

TimeAngle::TimeAngle(double radians)
: Angle(radians) {
  if(_rad < 0.0) _rad += TWOPI;
}

TimeAngle::TimeAngle(const std::string& str)
: Angle(novas_str_hours(str.c_str()) * Unit::hour_angle) {}

TimeAngle::TimeAngle(const Angle& angle)
: Angle(angle) {}

double TimeAngle::hours() const {
  return _rad / Unit::hour_angle;
}

double TimeAngle::minutes() const {
  return hours() * 60.0;
}

double TimeAngle::seconds() const {
  return hours() * 3600.0;
}

std::string TimeAngle::to_string(enum novas_separator_type separator, int decimals) const {
  char s[100] = {'\0'};
  novas_print_hms(hours(), separator, decimals, s, sizeof(s));
  return std::string(s);
}

TimeAngle TimeAngle::operator+(const TimeAngle& r) const {
  return TimeAngle(rad() + r.rad());
}

TimeAngle TimeAngle::operator-(const TimeAngle& r) const {
  return TimeAngle(rad() - r.rad());
}

Angle TimeAngle::operator+(const Angle& angle) const {
  return Angle::operator+(angle);
}

Angle TimeAngle::operator-(const Angle& angle) const {
  return Angle::operator-(angle);
}

TimeAngle TimeAngle::operator+(const Interval& offset) const {
  return TimeAngle(rad() + offset.hours() * Unit::hour_angle);
}

TimeAngle TimeAngle::operator-(const Interval& offset) const {
  return TimeAngle(rad() - offset.hours() * Unit::hour_angle);
}

TimeAngle TimeAngle::hours(double value) {
  return TimeAngle(value * Unit::hour_angle);
}

TimeAngle TimeAngle::minutes(double value) {
  return TimeAngle(value / 60.0 * Unit::hour_angle);
}

TimeAngle TimeAngle::seconds(double value) {
  return TimeAngle(value / 3600.0 * Unit::hour_angle);
}

const TimeAngle& TimeAngle::zero() {
  static const TimeAngle _zero = TimeAngle(0.0);
  return _zero;
}

const TimeAngle& TimeAngle::noon() {
  static const TimeAngle _12h = TimeAngle(12.0 * Unit::hour_angle);
  return _12h;
}

} // namespace supernovas
