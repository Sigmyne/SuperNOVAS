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

/**
 * Instantiates a temperature object with the given temperature value in degrees Celsius.
 *
 * @param deg_C   [C] temperature value.
 *
 * @sa celsius(double), kelvin(double), farenheit(double)
 */
Temperature::Temperature(double deg_C) : _deg_C(deg_C) {
  static const char *fn = "Temperature()";

  if(isnan(deg_C))
    novas::novas_error(0, EINVAL, fn, "input value is NAN");
  else if(kelvin() < 0.0)
    novas::novas_error(0, EINVAL, fn, "input value is below 0K");
  else
    _valid = true;
}

/**
 * Returns the temperature value in degrees Celsius.
 *
 * @return    [C] The temperature value
 *
 * @sa kelvin(), farenheit()
 */
double Temperature::celsius() const {
  return _deg_C;
}

/**
 * Returns the temperature value in degrees Kelvin.
 *
 * @return    [K] The temperature value
 *
 * @sa celsius(), farenheit()
 */
double Temperature::kelvin() const {
  return 273.15 + _deg_C;
}

/**
 * Returns the temperature value in degrees Farenheit.
 *
 * @return    [F] The temperature value
 *
 * @sa celsius(), kelvin()
 */
double Temperature::farenheit() const {
  return 32.0 + 1.8 * _deg_C;
}

/**
 * Returns a human-readable string representation of this temperature value.
 *
 * @return    a string with the human readable representation of this temperature.
 */
std::string Temperature::to_string() const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "%.1f C", _deg_C);
  return std::string(s);
}

/**
 * Returns a new temperature object, with the specified temperature value defined in degrees
 * Celsius.
 *
 * @param value   [C] temperature value
 * @return        A new temperature object with the specified value.
 *
 * @sa kelvin(double), farenheit(double)
 */
Temperature Temperature::celsius(double value) {
  return Temperature(value);
}

/**
 * Returns a new temperature object, with the specified temperature value defined in degrees
 * Kelvin.
 *
 * @param value   [K] temperature value
 * @return        A new temperature object with the specified value.
 *
 * @sa celsius(double), farenheit(double)
 */
Temperature Temperature::kelvin(double value) {
  return Temperature(value - 273.15);
}

/**
 * Returns a new temperature object, with the specified temperature value defined in degrees
 * Farenheit.
 *
 * @param value   [F] temperature value
 * @return        A new temperature object with the specified value.
 *
 * @sa celisus(double), kelvin(double)
 */
Temperature Temperature::farenheit(double value) {
  return Temperature((value - 32.0) / 1.8);
}

} // namespace supernovas
