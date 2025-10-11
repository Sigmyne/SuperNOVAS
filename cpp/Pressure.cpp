/**
 * @file
 *
 * @date Created  on Oct 1, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

namespace supernovas {


Pressure::Pressure(double value) : _pascal(value) {
  static const char *fn = "Pressure()";

  if(isnan(value))
    novas::novas_error(0, EINVAL, fn, "input value is NAN");
  else if(value < 0.0)
    novas::novas_error(0, EINVAL, fn, "input value is negative");
  else
    _valid = true;
}


double Pressure::Pa() const {
  return _pascal;
}

double Pressure::hPa() const {
  return 0.01 * _pascal;
}

double Pressure::kPa() const {
  return 1e-3 * _pascal;
}

double Pressure::mbar() const {
  return _pascal / Unit::mbar;
}

double Pressure::bar() const {
  return _pascal / Unit::bar;
}

double Pressure::torr() const {
  return _pascal / Unit::torr;
}

double Pressure::atm() const {
  return _pascal / Unit::atm;
}

std::string Pressure::to_string() const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "%.1f mbar", _pascal / Unit::mbar);
  return std::string(s);
}

Pressure Pressure::Pa(double value) {
  return Pressure(value);
}

Pressure Pressure::hPa(double value) {
  return Pressure(100.0 * value);
}

Pressure Pressure::kPa(double value) {
  return Pressure(1000.0 * value);
}

Pressure Pressure::mbar(double value) {
  return Pressure(value * Unit::mbar);
}

Pressure Pressure::bar(double value) {
  return Pressure(value * Unit::bar);
}

Pressure Pressure::torr(double value) {
  return Pressure(value * Unit::torr);
}

Pressure Pressure::atm(double value) {
  return Pressure(value * Unit::atm);
}


}
