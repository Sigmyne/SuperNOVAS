/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace novas;

namespace supernovas {

Ecliptic::Ecliptic(double longitude, double latitude, const std::string& system, double distance)
: Spherical(longitude, latitude, distance), _sys(system) {}

Ecliptic::Ecliptic(const Angle& ra, const Angle& dec, const CatalogSystem &system, const Distance& distance)
: Spherical(ra, dec, distance), _sys(system) {}

Ecliptic::Ecliptic(const Position& pos, const CatalogSystem& system)
: Spherical(pos.as_spherical()), _sys(system) {}

bool Ecliptic::is_valid() const {
  return Spherical::is_valid() && _sys.is_valid();
}

const CatalogSystem& Ecliptic::system() const {
  return _sys;
}

Equatorial Ecliptic::as_equatorial() const {
  double ra = 0.0, dec = 0.0;
  ecl2equ(_sys.jd(), NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, _lon.deg(), _lat.deg(), &ra, &dec);
  return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, _sys.name(), _distance.m());
}

Galactic Ecliptic::as_galactic() const {
  return as_equatorial().as_galactic();
}

const std::string Ecliptic::str(enum novas_separator_type separator, int decimals) const {
  return "ECL  " + Spherical::str(separator, decimals) + "  " + _sys.str();
}

static const Ecliptic _invalid = Ecliptic(NAN, NAN, NAN);
const Ecliptic& Ecliptic::invalid() {
  return _invalid;
}

} // namespace supernovas


