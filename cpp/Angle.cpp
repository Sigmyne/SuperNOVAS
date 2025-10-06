/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cmath>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

Angle::Angle(double x) : _rad(remainder(x, TWOPI)) {
  if(isnan(x))
    novas_error(0, EINVAL, "Angle(double)", "input angle is NAN");
  else
    _valid = true;
}

Angle::Angle(const std::string& str) {
  _rad = novas_str_degrees(str.c_str()) * Unit::deg;
  if(isnan(_rad))
    novas_error(0, EINVAL, "Angle(std::string)", "invalid input angle: %s", str.c_str());
}

Angle operator+(const Angle& l, const Angle& r) {
  return Angle(l.rad() + r.rad());
}

Angle operator-(const Angle& l, const Angle& r) {
  return Angle(l.rad() - r.rad());
}

bool Angle::is_equal(const Angle& angle, double precision) const {
  return fabs(remainder(_rad - angle._rad, Constant::twoPi)) < fabs(precision);
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

std::string Angle::str(enum novas_separator_type separator, int decimals) const {
  char s[100] = {'\0'};
  if(novas_print_dms(deg(), separator, decimals, s, sizeof(s)) != 0)
    novas_trace_invalid("Angle::str");
  return std::string(s);
}

} // namespace supernovas

