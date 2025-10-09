/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cstring>

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

Equatorial Equatorial::at_jd(long jd_tt) const {
  return to_system(CatalogSystem::at_julian_date(jd_tt));
}

Equatorial Equatorial::at_time(const Time& time) const {
  return at_jd(time.jd());
}

Equatorial Equatorial::to_system(const CatalogSystem& system) const {
  if(_sys == system)
    return Equatorial(*this);
  if(_sys.is_icrs() && system.is_icrs())
    return Equatorial(ra(), dec(), system, distance());

  double p[3] = {'\0'};
  radec2vector(ra().hours(), dec().deg(), 1.0, p);

  if(_sys.is_icrs())
    gcrs_to_j2000(p, p);

  if(_sys.jd() != system.jd())
    precession(_sys.jd(), p, system.jd(), p);

  if(system.is_icrs())
    j2000_to_gcrs(p, p);

  double r = 0.0, d = 0.0;
  vector2radec(p, &r, &d);
  return Equatorial(r * Unit::hourAngle, d * Unit::deg, system, distance().m());
}

Equatorial Equatorial::to_icrs() const {
  return to_system(CatalogSystem::icrs());
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
  Equatorial icrs = to_icrs();
  double longitude, latitude;
  equ2gal(icrs.ra().hours(), icrs.dec().deg(), &longitude, &latitude);
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


