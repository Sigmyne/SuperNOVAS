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

void Weather::validate() {
  static const char *fn = "Weather()";

  errno = 0;

  if(!_temperature.is_valid())
    novas_set_errno(EINVAL, fn, "invalid temperature: %.6g C", _temperature.celsius());
  else if(!_pressure.is_valid())
    novas_set_errno(EINVAL, fn, "invalid pressure: %.6g Pa", _pressure.Pa());
  else if(!isfinite(_humidity) || _humidity < 0.0 || _humidity > 100.0)
    novas_set_errno(EINVAL, fn, "invalid humidity: %.6g %%", _humidity);

  _valid = (errno == 0);
}

/**
 * Instantiates a weather dataset with the specified parameters.
 *
 * @param T                 [C] outside air temperature
 * @param p                 [Pa] atmospheric pressure
 * @param humidity_percent  [%] relative humidity
 *
 * @since 1.6
 * @sa guess()
 */
Weather::Weather(const Temperature& T, const Pressure& p, double humidity_percent)
: _temperature(T), _pressure(p), _humidity(humidity_percent) {
  validate();
}

/**
 * Instantiates a weather dataset with the specified parameters.
 *
 * @param celsius           [C] ambient air temperature
 * @param pascal            [Pa] atmospheric pressure
 * @param humidity_percent  [%] relative humidity
 *
 * @since 1.6
 * @sa guess()
 */
Weather::Weather(double celsius, double pascal, double humidity_percent)
: _temperature(Temperature::celsius(celsius)), _pressure(Pressure::Pa(pascal)), _humidity(humidity_percent) {
  validate();
}

/**
 * Returns a reference to the temperature value in this weather dataset.
 *
 * @return    [C] outside air temperature
 *
 * @since 1.6
 * @sa pressure(), humidity(), humidity_fraction()
 */
const Temperature& Weather::temperature() const {
  return _temperature;
}

/**
 * Returns a reference to the the atmpspheric pressure value in this weather dataset.
 *
 * @return    [Pa] atmospheric pressure
 *
 * @since 1.6
 * @sa temperature(), humidity(), humidity_fraction()
 */
const Pressure& Weather::pressure() const {
  return _pressure;
}

/**
 * Returns the humidity value, as a percentage, from this weather dataset.
 *
 * @return    [%] relative humidity [0:100]
 *
 * @since 1.6
 * @sa humidity_fraction(), temperature(), pressure()
 */
double Weather::humidity() const {
  return _humidity;
}

/**
 * Returns the humidity value, as a fraction, from this weather dataset.
 *
 * @return    relative humidity [0.0:1.0]
 *
 * @since 1.6
 * @sa humidity(), temperature(), pressure()
 */
double Weather::humidity_fraction() const {
  return 0.01 * _humidity;
}

/**
 * Returns a string representation of this weather dataset.
 *
 * @return  A human-readable string representation of this weather data.
 *
 * @since 1.6
 */
std::string Weather::to_string() const {
  char sH[20] = {'\0'};
  snprintf(sH, sizeof(sH), "%.1f %%", humidity());
  return "Weather (T = " + _temperature.to_string() + ", p = " + _pressure.to_string() + ", h = " + std::string(sH) + ")";
}

/**
 * Returns a reference to a fixed standard weather instance (T = 10%deg;C, p = 1 atm, humidity =
 * 50%).
 *
 * @return    a static reference to a site-independent standard default weather.
 *
 * @since 1.6
 */
const Weather& Weather::standard() {
  static const Weather _standard(10.0, Unit::atm, 50.0);

  return _standard;
}

} // namespace supernovas
