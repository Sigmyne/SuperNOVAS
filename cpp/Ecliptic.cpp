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

void Ecliptic::validate() {
  static const char *fn = "Equatorial()";

  if(!Spherical::is_valid())
     novas_trace_invalid(fn);

  if(_equator == NOVAS_GCRS_EQUATOR)
    _jd = NOVAS_JD_J2000;
}

Ecliptic::Ecliptic(double longitude_rad, double latitude_rad, enum novas_equator_type equator, double jd_tt, double distance_m)
: Spherical(longitude_rad, latitude_rad, distance_m), _equator(equator), _jd(jd_tt) {
  validate();
}

Ecliptic::Ecliptic(const Position& pos, enum novas_equator_type equator, double jd_tt)
: Spherical(pos.as_spherical()), _equator(equator), _jd(jd_tt) {
  validate();
}

enum novas_equator_type Ecliptic::equator() const {
  return _equator;
}

Ecliptic Ecliptic::to_icrs() const {
  if(_equator == NOVAS_GCRS_EQUATOR)
    return *this;
  return as_equatorial().to_icrs().as_ecliptic();
}

Ecliptic Ecliptic::to_j2000() const {
  if(_equator == NOVAS_MEAN_EQUATOR && _jd == NOVAS_JD_J2000)
    return (*this);

  return as_equatorial().to_j2000().as_ecliptic();
}

Ecliptic Ecliptic::to_hip() const {
  if(_equator == NOVAS_MEAN_EQUATOR && _jd == NOVAS_JD_HIP)
    return (*this);

  return as_equatorial().to_hip().as_ecliptic();
}

Ecliptic Ecliptic::to_mod(double jd_tt) const {
  if(jd_tt == NOVAS_JD_J2000)
    return to_j2000();

  if(_equator == NOVAS_MEAN_EQUATOR && _jd == jd_tt)
    return (*this);

  return as_equatorial().to_mod(jd_tt).as_ecliptic();
}

Ecliptic Ecliptic::to_tod(double jd_tt) const {
  if(_equator == NOVAS_TRUE_EQUATOR && _jd == jd_tt)
    return (*this);

  return as_equatorial().to_mod(jd_tt).as_ecliptic();
}

Equatorial Ecliptic::as_equatorial() const {
  double ra = 0.0, dec = 0.0;

  ecl2equ(_jd, _equator, NOVAS_FULL_ACCURACY, longitude().deg(), latitude().deg(), &ra, &dec);

  switch(_equator) {
    case NOVAS_GCRS_EQUATOR:
      return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, EquatorialSystem::icrs(), distance().m());
    case NOVAS_MEAN_EQUATOR:
      if(_jd == NOVAS_JD_J2000)
        return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, EquatorialSystem::j2000(), distance().m());
      else
        return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, EquatorialSystem::mod(_jd), distance().m());
    case NOVAS_TRUE_EQUATOR:
      return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, EquatorialSystem::tod(_jd), distance().m());
  }

  return Equatorial::invalid();
}

Galactic Ecliptic::as_galactic() const {
  return as_equatorial().as_galactic();
}

static std::string _sys_type(enum novas_equator_type equator, double jd_tt) {
  char s[20] = {'\0'};
  int n = 0;

  switch(equator) {
    case NOVAS_GCRS_EQUATOR:
      n = sprintf(s, "ICRS");
      break;
    case NOVAS_MEAN_EQUATOR:
      n = sprintf(s, "J");
      break;
    case NOVAS_TRUE_EQUATOR:
      n = sprintf(s, "TOD J");
  }

  n += snprintf(&s[n], sizeof(s) - n, "%.3f", (jd_tt - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS);

  // Remove trailing zeroes and decimal point.
  for(int i = n; --i >= 0; ) {
    if(s[i] == '.') {
      s[i] = '\0';
      break;
    }
    if(s[i] != '0') break;
    s[i] = '\0';
  }

  return std::string(s);
}

std::string Ecliptic::to_string(enum novas_separator_type separator, int decimals) const {
  return "ECL  " + Spherical::to_string(separator, decimals) + "  " + _sys_type(_equator, _jd);
}


Ecliptic Ecliptic::icrs(double longitude_rad, double latitude_rad, double distance) {
  return Ecliptic(longitude_rad, latitude_rad, NOVAS_GCRS_EQUATOR, NOVAS_JD_J2000, distance);
}

Ecliptic Ecliptic::icrs(const Angle& longitude, const Angle& latitude, const Distance& distance) {
  return Ecliptic::icrs(longitude.rad(), latitude.rad(), distance.m());
}

Ecliptic Ecliptic::j2000(double longitude_rad, double latitude_rad, double distance) {
  return Ecliptic(longitude_rad, latitude_rad, NOVAS_MEAN_EQUATOR, NOVAS_JD_J2000, distance);
}

Ecliptic Ecliptic::j2000(const Angle& longitude, const Angle& latitude, const Distance& distance) {
  return Ecliptic::j2000(longitude.rad(), latitude.rad(), distance.m());
}


Ecliptic Ecliptic::mod(double jd_tt, double longitude_rad, double latitude_rad, double distance) {
  return Ecliptic(longitude_rad, latitude_rad, NOVAS_MEAN_EQUATOR, jd_tt, distance);
}

Ecliptic Ecliptic::mod(const Time& time, const Angle& longitude, const Angle& latitude, const Distance& distance) {
  return Ecliptic::mod(time.jd(), longitude.rad(), latitude.rad(), distance.m());
}

Ecliptic Ecliptic::tod(double jd_tt, double longitude_rad, double latitude_rad, double distance) {
  return Ecliptic(longitude_rad, latitude_rad, NOVAS_TRUE_EQUATOR, jd_tt, distance);
}

Ecliptic Ecliptic::tod(const Time& time, const Angle& longitude, const Angle& latitude, const Distance& distance) {
  return Ecliptic::tod(time.jd(), longitude.rad(), latitude.rad(), distance.m());
}


static const Ecliptic _invalid = Ecliptic::icrs(NAN, NAN, NAN);
const Ecliptic& Ecliptic::invalid() {
  return _invalid;
}


} // namespace supernovas


