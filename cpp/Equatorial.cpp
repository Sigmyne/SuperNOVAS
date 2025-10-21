/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cstring>
#include <iostream>

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
    novas_error(0, EINVAL, fn, "Invalid equatorial system: %s", _sys.to_string().c_str());
  }
}


Equatorial::Equatorial(double ra_rad, double dec_rad, const EquatorialSystem &system, double distance_m)
: Spherical(ra_rad, dec_rad, distance_m), _sys(system) {
  validate();
}

Equatorial::Equatorial(const Angle& ra, const Angle& dec, const EquatorialSystem &system, const Distance& distance)
: Spherical(ra, dec, distance), _sys(system) {
  validate();
}

Equatorial::Equatorial(const Position& pos, const EquatorialSystem& system)
: Spherical(pos.as_spherical()), _sys(system) {
  validate();
}

const EquatorialSystem& Equatorial::system() const {
  return _sys;
}

enum novas::novas_reference_system Equatorial::reference_system() const {
  return _sys.reference_system();
}

Equatorial Equatorial::to_system(const EquatorialSystem& system) const {
  if(_sys == system)
    return Equatorial(*this);
  if(_sys.is_icrs() && system.is_icrs())
    return Equatorial(ra(), dec(), system, distance());

  double p[3] = {'\0'};
  radec2vector(ra().hours(), dec().deg(), 1.0, p);

  // Convert to ICRS...
  switch(_sys.reference_system()) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      break;
    case NOVAS_MOD:
      mod_to_gcrs(_sys.jd(), p, p);
      break;
    case NOVAS_CIRS:
      cirs_to_gcrs(_sys.jd(), NOVAS_FULL_ACCURACY, p, p);
      break;
    case NOVAS_TOD:
      tod_to_gcrs(_sys.jd(), NOVAS_FULL_ACCURACY, p, p);
      break;
    default:
      p[0] = p[1] = p[2] = NAN;
  }

  // Convert from ICRS to output system...
  switch(system.reference_system()) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      break;
    case NOVAS_MOD:
      gcrs_to_mod(system.jd(), p, p);
      break;
    case NOVAS_TOD:
      gcrs_to_tod(system.jd(), NOVAS_FULL_ACCURACY, p, p);
      break;
    case NOVAS_CIRS:
      gcrs_to_cirs(system.jd(), NOVAS_FULL_ACCURACY, p, p);
      break;
    default:
      p[0] = p[1] = p[2] = NAN;
  }

  double r = 0.0, d = 0.0;
  vector2radec(p, &r, &d);
  return Equatorial(r * Unit::hour_angle, d * Unit::deg, system, distance().m());
}

Equatorial Equatorial::to_icrs() const {
  return to_system(EquatorialSystem::icrs());
}

Equatorial Equatorial::to_j2000() const {
  return to_system(EquatorialSystem::j2000());
}

Equatorial Equatorial::to_hip() const {
  return to_system(EquatorialSystem::mod(NOVAS_JD_HIP));
}

Equatorial Equatorial::to_mod(double jd_tdb) const {
  return to_system(EquatorialSystem::mod(jd_tdb));
}

Equatorial Equatorial::to_mod(const Time& time) const {
  return to_mod(time.jd(novas::NOVAS_TDB));
}

Equatorial Equatorial::to_mod_at_besselian_epoch(double year) const {
  return to_system(EquatorialSystem::mod_at_besselian_epoch(year));
}

Equatorial Equatorial::to_tod(double jd_tdb) const {
  return to_system(EquatorialSystem::tod(jd_tdb));
}

Equatorial Equatorial::to_tod(const Time& time) const {
  return to_tod(time.jd(novas::NOVAS_TDB));
}

Equatorial Equatorial::to_cirs(double jd_tdb) const {
  return to_system(EquatorialSystem::cirs(jd_tdb));
}

Equatorial Equatorial::to_cirs(const Time& time) const {
  return to_cirs(time.jd(novas::NOVAS_TDB));
}

TimeAngle Equatorial::ra() const {
  return TimeAngle(longitude().rad());
}

const Angle& Equatorial::dec() const {
  return latitude();
}

Ecliptic Equatorial::as_ecliptic() const {
  double lon, lat;

  double r = ra().hours();
  double d = dec().deg();

  switch(_sys.reference_system()) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      equ2ecl(NOVAS_JD_J2000, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, r, d, &lon, &lat);
      return Ecliptic::icrs(lon * Unit::deg, lat * Unit::deg, distance().m());
    case NOVAS_J2000:
      equ2ecl(NOVAS_JD_J2000, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, r, d, &lon, &lat);
      return Ecliptic::j2000(lon * Unit::deg, lat * Unit::deg, distance().m());
    case NOVAS_MOD:
      equ2ecl(_sys.jd(), NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, r, d, &lon, &lat);
      return Ecliptic::mod(_sys.jd(), lon * Unit::deg, lat * Unit::deg, distance().m());
    case NOVAS_CIRS:
      r -= ira_equinox(_sys.jd(), NOVAS_MEAN_EQUINOX, NOVAS_FULL_ACCURACY); // @suppress("No break at end of case")
      /* fallthrough */
    default: // TOD
      equ2ecl(_sys.jd(), NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, r, d, &lon, &lat);
      return Ecliptic::tod(_sys.jd(), lon * Unit::deg, lat * Unit::deg, distance().m());
  }
}

Galactic Equatorial::as_galactic() const {
  Equatorial icrs = to_icrs();
  double longitude, latitude;
  equ2gal(icrs.ra().hours(), icrs.dec().deg(), &longitude, &latitude);
  return Galactic(longitude * Unit::deg, latitude * Unit::deg, distance().m());
}

std::string Equatorial::to_string(enum novas_separator_type separator, int decimals) const {
  return "EQU  " + ra().to_string(separator, (decimals > 1 ? decimals - 1 : decimals)) + "  "
          + dec().to_string(separator, decimals) + "  " + _sys.to_string();
}

const Equatorial& Equatorial::invalid() {
  static const Equatorial _invalid = Equatorial(NAN, NAN, EquatorialSystem::icrs(), NAN);
  return _invalid;
}

} // namespace supernovas


