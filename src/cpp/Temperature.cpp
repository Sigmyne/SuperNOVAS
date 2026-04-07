/**
 * @file
 *
 * @date Created  on Oct 1, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only

#define KELVIN_0C         273.15  ///< [K] 0 celsius.
/// \endcond

#include "supernovas.h"


namespace supernovas {

/**
 * Instantiates a temperature object with the given temperature value in degrees Celsius.
 *
 * @param deg_C   [C] temperature value.
 *
 * @since 1.6
 * @sa celsius(), kelvin(), farenheit()
 */
Temperature::Temperature(double deg_C) : Scalar(KELVIN_0C + deg_C) {
  static const char *fn = "Temperature()";

  if(!is_valid())
    novas_trace_invalid(fn);
  else if(_value < 0.0) {
    novas_set_errno(EINVAL, fn, "input value is below 0K");
    _valid = false;
  }
}

/**
 * Returns the temperature value in degrees Celsius.
 *
 * @return    [C] The temperature value
 *
 * @since 1.6
 * @sa kelvin(), farenheit()
 */
double Temperature::celsius() const {
  return _value - KELVIN_0C;
}

/**
 * Returns the temperature value in degrees Kelvin.
 *
 * @return    [K] The temperature value
 *
 * @since 1.6
 * @sa celsius(), farenheit()
 */
double Temperature::kelvin() const {
  return _value;
}

/**
 * Returns the temperature value in degrees Farenheit.
 *
 * @return    [F] The temperature value
 *
 * @since 1.6
 * @sa celsius(), kelvin()
 */
double Temperature::farenheit() const {
  return 32.0 + 1.8 * celsius();
}

std::string Temperature::SI_unit() const {
  return "K";
}

/**
 * Returns a human-readable string representation of this temperature value.
 *
 * @param decimals  (optional) [0:16] decimal places to print (default: 1).
 * @return          a string with the human readable representation of this temperature.
 *
 * @since 1.6
 */
std::string Temperature::to_string(int decimals) const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "%.1f C", celsius());
  return std::string(s);
}

/**
 * Returns a new temperature object, with the specified temperature value defined in degrees
 * Celsius.
 *
 * @param value   [C] temperature value
 * @return        A new temperature object with the specified value.
 *
 * @since 1.6
 * @sa kelvin(), farenheit()
 */
Temperature Temperature::celsius(double value) {
  Temperature T(value);
  if(!T.is_valid())
    novas_trace_invalid("Temperature::celsius(double)");
  return T;
}

/**
 * Returns a new temperature object, with the specified temperature value defined in degrees
 * Kelvin.
 *
 * @param value   [K] temperature value
 * @return        A new temperature object with the specified value.
 *
 * @since 1.6
 * @sa celsius(), farenheit()
 */
Temperature Temperature::kelvin(double value) {
  Temperature T(value - KELVIN_0C);
  if(!T.is_valid())
    novas_trace_invalid("Temperature::kelvin(double)");
  return T;
}

/**
 * Returns a new temperature object, with the specified temperature value defined in degrees
 * Farenheit.
 *
 * @param value   [F] temperature value
 * @return        A new temperature object with the specified value.
 *
 * @since 1.6
 * @sa celisus(), kelvin()
 */
Temperature Temperature::farenheit(double value) {
  Temperature T((value - 32.0) / 1.8);
  if(!T.is_valid())
    novas_trace_invalid("Temperature::farenheit(double)");
  return T;
}

} // namespace supernovas
