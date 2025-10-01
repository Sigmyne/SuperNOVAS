/**
 * @file
 *
 * @date Created  on Oct 1, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Temperature {
private:
  double _deg_C;

  Temperature(double deg_C) : _deg_C(deg_C) {}

public:
  double celsius() const {
    return _deg_C;
  }

  double kelvin() const {
    return 273.15 + _deg_C;
  }

  double farenheit() const {
    return 32.0 + 1.8 * _deg_C;
  }

  std::string str() const {
    char s[40] = {'\0'};
    snprintf(s, sizeof(s), "%.1f C", _deg_C);
    return std::string(s);
  }


  static Temperature celsius(double value) {
    return Temperature(value);
  }

  static Temperature kelvin(double value) {
    return Temperature(value - 273.15);
  }

  static Temperature farenheit(double value) {
    return Temperature((value - 32.0) / 1.8);
  }
};

} // namespace supernovas
