/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"

using namespace novas;

namespace supernovas {

Galactic::Galactic(double longitude_rad, double latitude_rad, double distance_m)
: Spherical(longitude_rad, latitude_rad, distance_m) {}

Galactic::Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance)
: Spherical(longitude, latitude, distance) {}

Galactic::Galactic(const Position& pos)
: Spherical(pos.as_spherical()) {}

Equatorial Galactic::as_equatorial() const {
  double ra = 0.0, dec = 0.0;
  gal2equ(longitude().deg(), latitude().deg(), &ra, &dec);
  return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, EquatorialSystem::icrs(), distance().m());
}

Ecliptic Galactic::as_ecliptic() const {
  return as_equatorial().as_ecliptic();
}

const std::string Galactic::str(enum novas_separator_type separator, int decimals) const {
  return "GAL  " + Spherical::str(separator, decimals);
}

static const Galactic _invalid = Galactic(NAN, NAN, NAN);
const Galactic& Galactic::invalid() {
  return _invalid;
}

} // namespace supernovas

