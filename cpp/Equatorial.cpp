/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"

using namespace novas;


namespace supernovas {

Equatorial::Equatorial(double ra, double dec, const std::string& system, double distance)
: Spherical(ra, dec, distance), _sys(system) {}

Equatorial::Equatorial(const Angle& ra, const Angle& dec, const CatalogSystem &system, const Distance& distance)
: Spherical(ra, dec, distance), _sys(system) {}

Equatorial::Equatorial(const Position& pos, const CatalogSystem& system)
: Spherical(pos.as_spherical()), _sys(system) {}

const CatalogSystem& Equatorial::system() const {
  return _sys;
}

bool Equatorial::is_valid() const {
  return Spherical::is_valid() && _sys.is_valid();
}

TimeAngle Equatorial::ra() const {
  return TimeAngle(longitude().rad());
}

const Angle& Equatorial::dec() const {
  return latitude();
}

Ecliptic Equatorial::as_ecliptic() const {
  double longitude, latitude;
  equ2ecl(_sys.jd(), NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, ra().hours(), dec().deg(), &longitude, &latitude);
  return Ecliptic(longitude * Unit::deg, latitude * Unit::deg, _sys.name(), _distance.m());
}

Galactic Equatorial::as_galactic() const {
  double longitude, latitude;
  equ2gal(ra().hours(), dec().deg(), &longitude, &latitude);
  return Galactic(longitude * Unit::deg, latitude * Unit::deg, _distance.m());
}

const std::string Equatorial::str(enum novas_separator_type separator, int decimals) const {
  return "EQU  " + ra().str(separator, (decimals > 1 ? decimals - 1 : decimals)) + "  " + _lat.str(separator, decimals) + "  " + _sys.str();
}

static const Equatorial _invalid = Equatorial(NAN, NAN, NAN);
const Equatorial& Equatorial::invalid() {
  return _invalid;
}

} // namespace supernovas


