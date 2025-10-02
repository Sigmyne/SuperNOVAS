/**
 * @file
 *
 * @date Created  on Oct 1, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace supernovas;


double Temperature::celsius() const {
  return _deg_C;
}

double Temperature::kelvin() const {
  return 273.15 + _deg_C;
}

double Temperature::farenheit() const {
  return 32.0 + 1.8 * _deg_C;
}

std::string Temperature::str() const {
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

