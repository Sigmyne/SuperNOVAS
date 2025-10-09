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

Weather::Weather(const Temperature& T, const Pressure& p, double humidity_percent)
: _temperature(T), _pressure(p), _humidity(humidity_percent) {
  validate();
}

Weather::Weather(double celsius, double pascal, double humidity_percent)
: _temperature(Temperature::celsius(celsius)), _pressure(Pressure::Pa(pascal)), _humidity(humidity_percent) {
  validate();
}


const Temperature& Weather::temperature() const {
  return _temperature;
}

const Pressure& Weather::pressure() const {
  return _pressure;
}

double Weather::humidity() const {
  return _humidity;
}

double Weather::humidity_fraction() const {
  return 0.01 * _humidity;
}

std::string Weather::str() const {
  char sT[20] = {'\0'}, sP[20] = {'\0'}, sH[20] = {'\0'};
  snprintf(sT, sizeof(sT), "T = %.2f C", temperature().celsius());
  snprintf(sP, sizeof(sT), "p = %.2f mbar", pressure().mbar());
  snprintf(sH, sizeof(sT), "h = %.1f %%", humidity());
  return "Weather ( " + std::string(sT) + ", " + std::string(sP) + ", " + std::string(sH) + " )";
}

Weather Weather::guess(const Site& site) {
  on_surface s = *site._on_surface();
  novas_set_default_weather(&s);
  return Weather(s.temperature, s.pressure * Unit::mbar, s.humidity);
}

} // namespace supernovas
