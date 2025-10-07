/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;


namespace supernovas {

void Equatorial::validate() {
  static const char *fn = "Equatorial()";

  if(!Spherical::is_valid())
     novas_trace_invalid(fn);

  else if(!_sys.is_valid()) {
    _valid = false;
    novas_error(0, EINVAL, fn, "Invalid catalog system: %s", _sys.str().c_str());
  }
}


Equatorial::Equatorial(double ra_rad, double dec_rad, const CatalogSystem &system, double distance_m)
: Spherical(ra_rad, dec_rad, distance_m), _sys(system) {
  validate();
}

Equatorial::Equatorial(const Angle& ra, const Angle& dec, const CatalogSystem &system, const Distance& distance)
: Spherical(ra, dec, distance), _sys(system) {
  validate();
}

Equatorial::Equatorial(const Position& pos, const CatalogSystem& system)
: Spherical(pos.as_spherical()), _sys(system) {
  validate();
}

const CatalogSystem& Equatorial::system() const {
  return _sys;
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
  return Ecliptic(longitude * Unit::deg, latitude * Unit::deg, _sys, _distance.m());
}

Galactic Equatorial::as_galactic() const {
  double longitude, latitude;
  equ2gal(ra().hours(), dec().deg(), &longitude, &latitude);
  return Galactic(longitude * Unit::deg, latitude * Unit::deg, _distance.m());
}

const std::string Equatorial::str(enum novas_separator_type separator, int decimals) const {
  return "EQU  " + ra().str(separator, (decimals > 1 ? decimals - 1 : decimals)) + "  " + _lat.str(separator, decimals) + "  " + _sys.str();
}

static const Equatorial _invalid = Equatorial(NAN, NAN);
const Equatorial& Equatorial::invalid() {
  return _invalid;
}

} // namespace supernovas


