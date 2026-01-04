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

using namespace novas;

namespace supernovas {

void Weather::validate() {
  static const char *fn = "Weather()";

  if(!_temperature.is_valid())
    novas_error(0, EINVAL, fn, "invalid temperature: %.6g C", _temperature.celsius());
  else if(!_pressure.is_valid())
    novas_error(0, EINVAL, fn, "invalid pressure: %.6g Pa", _pressure.Pa());
  else if(isnan(_humidity) || _humidity < 0.0 || _humidity > 100.0)
    novas_error(0, EINVAL, fn, "invalid humidity: %.6g %%", _humidity);
  else
    _valid = true;
}

/**
 * Instantiates a weather dataset with the specified parameters.
 *
 * @param T                 [C] outside air temperature
 * @param p                 [Pa] atmospheric pressure
 * @param humidity_percent  [%] relative humidity
 *
 * @sa Weather(double, double, double)
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
 * @sa Weather(Temperature&, Pressure&, double)
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
 * @sa humidity(), temperature(), pressure()
 */
double Weather::humidity_fraction() const {
  return 0.01 * _humidity;
}

/**
 * Returns a string representation of this weather dataset.
 *
 * @return  A human-readable string representation of this weather data.
 */
std::string Weather::to_string() const {
  char sH[20] = {'\0'};
  snprintf(sH, sizeof(sH), "%.1f %%", humidity());
  return "Weather (T = " + _temperature.to_string() + ", p = " + _pressure.to_string() + ", h = " + std::string(sH) + ")";
}

/**
 * Returns a new weather dataset populated with typical annual average weather parameters for the
 * given observing site, based on a simple global weather model. This provides a very crude
 * educated guess for the weather at the site. For any precision application (e.g. refraction
 * calculation) you should always use measured weather values instead.
 *
 * @param site    The observing site
 * @return        The typical annual average weather at the site, based on a simple global weather
 *                model.
 *
 * @sa Weather(Temperature&, Pressure&, double), Weather(double, double, double)
 */
Weather Weather::guess(const Site& site) {
  on_surface s = *site._on_surface();
  novas_set_default_weather(&s);
  return Weather(s.temperature, s.pressure * Unit::mbar, s.humidity);
}

} // namespace supernovas
