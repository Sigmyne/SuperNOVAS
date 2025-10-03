/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.hpp"

using namespace novas;

namespace supernovas {

Angle::Angle() : _rad(NAN) {}

Angle::Angle(double x) : _rad(remainder(x, TWOPI)) {}

Angle::Angle(const std::string& str) {
  _rad = novas_str_degrees(str.c_str()) * Unit::deg;
}

Angle operator+(const Angle& l, const Angle& r) {
  return Angle(l.rad() + r.rad());
}

Angle operator-(const Angle& l, const Angle& r) {
  return Angle(l.rad() - r.rad());
}

double Angle::rad() const {
  return _rad;
}

double Angle::deg() const {
  return rad() / Unit::deg;
}

double Angle::arcmin() const {
  return rad() / Unit::arcsec;
}

double Angle::arcsec() const {
  return rad() / Unit::arcsec;
}

double Angle::mas() const {
  return rad() / Unit::mas;
}

double Angle::uas() const {
  return rad() / Unit::uas;
}

double Angle::fraction() const {
  return _rad / TWOPI;
}

const std::string Angle::str(enum novas_separator_type separator, int decimals) const {
  char s[100] = {'\0'};
  novas_print_dms(deg(), separator, decimals, s, sizeof(s));
  return std::string(s);
}

} // namespace supernovas

