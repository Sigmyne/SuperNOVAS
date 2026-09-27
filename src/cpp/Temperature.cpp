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
 * Checks if this pressure value is the same as another, within the specified precision.
 *
 * @param temp        Another temperature value.
 * @param precision   [K] (optional) Precision for the comparison (default: 1 mK).
 * @return            `true` if the two pressures agree within the specified precision, otherwise
 *                    `false`.
 *
 * @since 1.8
 *
 * @sa operator==(), operator!=()
 */
bool Temperature::equals(const Temperature& temp, double precision) const {
    return Scalar::equals(temp, precision);
}

/**
 * Checks if this pressure value is the same as another, within 1 mK.
 *
 * @param temp        Another temperature value.
 * @return            `true` if the two coordinates are effectively the same within 1 mK, otherwise
 *                    `false`.
 *
 * @since 1.8
 *
 * @sa equals(), operator!=()
 */
bool Temperature::operator==(const Temperature& temp) const {
  return equals(temp);
}

/**
 * Checks if this pressure value differs from another by more than 1 mK.
 *
 * @param temp        Another temperature value.
 * @return            `true` if the two coordinates differ by more than 1 mK, otherwise `false`.
 *
 * @since 1.8
 *
 * @sa operator==(), equals()
 */
bool Temperature::operator!=(const Temperature& temp) const {
  return !(*this == temp);
}

/**
 * Returns the temperature value in degrees Celsius.
 *
 * @return    [C] The temperature value
 *
 * @since 1.6
 * @sa kelvin(), fahrenheit()
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
 * @sa celsius(), fahrenheit()
 */
double Temperature::kelvin() const {
  return _value;
}

/**
 * Returns the temperature value in degrees Fahrenheit.
 *
 * @return    [F] The temperature value
 *
 * @since 1.6
 * @sa celsius(), kelvin()
 */
double Temperature::fahrenheit() const {
  return 32.0 + 1.8 * celsius();
}

std::string Temperature::SI_unit() const {
  return "K";
}

/**
 * Returns a human-readable string representation of this temperature value.
 *
 * @param decimals  (optional) [0:16] decimal places to print (default: 3).
 * @return          a string with the human readable representation of this temperature.
 *
 * @since 1.6
 */
std::string Temperature::to_string(int decimals) const {
  char s[40] = {'\0'};
  novas_print_decimal(celsius(), decimals, s, (int) sizeof(s));
  return std::string(s) + " C";
}

/**
 * Returns a new temperature object, with the specified temperature value defined in degrees
 * Celsius.
 *
 * @param value   [C] temperature value
 * @return        A new temperature object with the specified value.
 *
 * @since 1.6
 * @sa kelvin(), fahrenheit()
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
 * @sa celsius(), fahrenheit()
 */
Temperature Temperature::kelvin(double value) {
  Temperature T(value - KELVIN_0C);
  if(!T.is_valid())
    novas_trace_invalid("Temperature::kelvin(double)");
  return T;
}

/**
 * Returns a new temperature object, with the specified temperature value defined in degrees
 * Fahrenheit.
 *
 * @param value   [F] temperature value
 * @return        A new temperature object with the specified value.
 *
 * @since 1.6
 * @sa celisus(), kelvin()
 */
Temperature Temperature::fahrenheit(double value) {
  Temperature T((value - 32.0) / 1.8);
  if(!T.is_valid())
    novas_trace_invalid("Temperature::farenheit(double)");
  return T;
}

} // namespace supernovas
