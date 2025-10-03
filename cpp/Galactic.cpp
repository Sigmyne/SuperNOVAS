/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace novas;

namespace supernovas {

Galactic::Galactic(double longitude, double latitude, double distance)
: Spherical(longitude, latitude, distance) {}

Galactic::Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance)
: Spherical(longitude, latitude, distance) {}

Galactic::Galactic(const Position& pos, const Distance& distance)
: Spherical(pos.as_spherical()) {}

Equatorial Galactic::as_equatorial() const {
  double ra = 0.0, dec = 0.0;
  gal2equ(_lon.deg(), _lat.deg(), &ra, &dec);
  return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, "ICRS", _distance.m());
}

Ecliptic Galactic::as_ecliptic() const {
  return as_equatorial().as_ecliptic();
}

const std::string Galactic::str(enum novas_separator_type separator, int decimals) const {
  return "GAL  " + Spherical::str(separator, decimals);
}

} // namespace supernovas

