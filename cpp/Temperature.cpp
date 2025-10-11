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

Temperature::Temperature(double deg_C) : _deg_C(deg_C) {
  static const char *fn = "Temperature()";

  if(isnan(deg_C))
    novas::novas_error(0, EINVAL, fn, "input value is NAN");
  else if(kelvin() < 0.0)
    novas::novas_error(0, EINVAL, fn, "input value is below 0K");
  else
    _valid = true;
}

double Temperature::celsius() const {
  return _deg_C;
}

double Temperature::kelvin() const {
  return 273.15 + _deg_C;
}

double Temperature::farenheit() const {
  return 32.0 + 1.8 * _deg_C;
}

std::string Temperature::to_string() const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "%.1f C", _deg_C);
  return std::string(s);
}

Temperature Temperature::celsius(double value) {
  return Temperature(value);
}

Temperature Temperature::kelvin(double value) {
  return Temperature(value - 273.15);
}

Temperature Temperature::farenheit(double value) {
  return Temperature((value - 32.0) / 1.8);
}

} // namespace supernovas
