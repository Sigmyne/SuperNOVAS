/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Angle {
protected:
  double _rad;

protected:
  Angle() : _rad(NAN) {}

public:

  Angle(double x) : _rad(remainder(x, TWOPI)) {}

  Angle(const std::string& str) {
    _rad = novas_str_degrees(str.c_str()) * Unit::deg;
  }

  Angle operator+(const Angle& r) const {
    return Angle(rad() + r.rad());
  }

  Angle operator-(const Angle& r) const {
    return Angle(rad() - r.rad());
  }

  double rad() const {
    return _rad;
  }

  double deg() const {
    return rad() / Unit::deg;
  }

  double arcmin() const {
    return rad() / Unit::arcsec;
  }

  double arcsec() const {
    return rad() / Unit::arcsec;
  }

  double mas() const {
    return rad() / Unit::mas;
  }

  double uas() const {
    return rad() / Unit::uas;
  }

  double fraction() {
    return _rad / TWOPI;
  }

  const std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const {
    char s[100] = {'\0'};
    novas_print_dms(deg(), separator, decimals, s, sizeof(s));
    return std::string(s);
  }
};


} // namespace supernovas
