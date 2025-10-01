/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"

namespace supernovas {

class Galactic : public Spherical {
public:
  Galactic(double longitude, double latitude, double distance = NOVAS_DEFAULT_DISTANCE)
  : Spherical(longitude, latitude, distance) {}

  Galactic(const Angle& longitude, const Angle& latitude, double distance = NOVAS_DEFAULT_DISTANCE)
  : Spherical(longitude, latitude, distance) {}

  Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance)
  : Spherical(longitude, latitude, distance) {}

  Galactic(const Position& pos)
  : Spherical(pos.as_spherical()) {}

  Equatorial as_equatorial() const {
    double ra = 0.0, dec = 0.0;
    gal2equ(_lon.deg(), _lat.deg(), &ra, &dec);
    return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, "ICRS", _distance.m());
  }

  Ecliptic as_ecliptic() const {
    return as_equatorial().as_ecliptic();
  }

  const std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const {
    return "GAL  " + Spherical::str(separator, decimals);
  }
};

} // namespace supernovas


