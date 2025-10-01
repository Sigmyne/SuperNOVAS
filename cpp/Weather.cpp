/**
 * @file
 *
 * @date Created  on Oct 1, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"


namespace supernovas {

class Weather {
private:
  Temperature _temperature;
  Pressure _pressure;
  double _humidity;

public:

  Weather(const Temperature& T, const Pressure& p, double humidity_percent)
  : _temperature(T), _pressure(p), _humidity(humidity_percent) {}

  Weather(double celsius, double pascal, double humidity_percent)
  : _temperature(Temperature::celsius(celsius)), _pressure(Pressure::Pa(pascal)), _humidity(humidity_percent) {}

  const Temperature& temperature() const {
    return _temperature;
  }

  const Pressure& pressure() const {
    return _pressure;
  }

  double humidity() const {
    return _humidity;
  }

  double humidity_fraction() const {
    return 0.01 * _humidity;
  }

  static Weather guess(const Site& site) {
    on_surface s = *site._on_surface();
    novas_set_default_weather(&s);
    return Weather(s.temperature, s.pressure * Unit::mbar, s.humidity);
  }

};

} // namespace supernovas
