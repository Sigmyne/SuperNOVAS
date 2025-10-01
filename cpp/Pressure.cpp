/**
 * @file
 *
 * @date Created  on Oct 1, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Pressure {
private:
  double _pascal;

  Pressure(double value) : _pascal(value) {}

public:
  double Pa() const {
    return _pascal;
  }

  double hPa() const {
    return 0.01 * _pascal;
  }

  double kPa() const {
    return 1e-3 * _pascal;
  }

  double mbar() const {
    return _pascal / Unit::mbar;
  }

  double bar() const {
    return _pascal / Unit::bar;
  }

  double torr() const {
    return _pascal / Unit::torr;
  }

  std::string str() const {
    char s[40] = {'\0'};
    snprintf(s, sizeof(s), "%.1f mbar", _pascal / Unit::mbar);
    return std::string(s);
  }

  static Pressure Pa(double value) {
    return Pressure(value);
  }

  static Pressure hPa(double value) {
    return Pressure(100.0 * value);
  }

  static Pressure kPa(double value) {
    return Pressure(1000.0 * value);
  }

  static Pressure mbar(double value) {
    return Pressure(value * Unit::mbar);
  }

  static Pressure bar(double value) {
    return Pressure(value * Unit::bar);
  }

  static Pressure torr(double value) {
    return Pressure(value * Unit::torr);
  }


};

} // namespace supernovas
