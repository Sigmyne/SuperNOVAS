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
  if(!Spherical::is_valid())
     novas_trace_invalid("Equatorial()");

  if(_equator == NOVAS_GCRS_EQUATOR)
    _jd = NOVAS_JD_J2000;
}

/**
 * Instantiates new ecliptic coordinates with the specified parameters.
 *
 * @param longitude_rad   [rad] ecliptic longitude coordinate
 * @param latitude_rad    [rad] ecliptic latitude coordinate
 * @param system          (optional) the equatorial coordinate reference system that defines the
 *                        the origin of ecliptic longitude, that is the equinox of date (default:
 *                        ICRS).
 * @param distance_m      [m] (optional) the distance, in needed / known (default: 1 Gpc).
 *
 * @sa Ecliptic(Angle&, Angle&, Equinox&, Distance&), Ecliptic(Position&, Equinox&)
 */
Ecliptic::Ecliptic(double longitude_rad, double latitude_rad, const Equinox& system, double distance_m)
: Spherical(longitude_rad, latitude_rad, distance_m), _equator(system.equator_type()), _jd(system.jd()) {
  validate();
}

/**
 * Instantiates new ecliptic coordinates with the specified ecliptic cartesian position vector.
 *
 * @param pos             Ecliptic _xyz_ position vector
 * @param system          (optional) the equatorial coordinate reference system that defines the
 *                        the origin of ecliptic longitude, that is the equinox of date (default:
 *                        ICRS).
 *
 * @sa Ecliptic(Angle&, Angle&, Equinox&, Distance&),
 *     Ecliptic(double, double, Equinox&, double),
 */
Ecliptic::Ecliptic(const Position& pos, const Equinox& system)
: Spherical(pos.as_spherical()), _equator(system.equator_type()), _jd(system.jd()) {
  validate();
}

/**
 * Returns the type of equator (ICRS, mean, or true) that is used for these ecliptic coordinates.
 *
 * @return    the type of equator that defines the origin (equinox), uch as ICRS, mean, or true.
 *
 * @sa Equinox::equator_type()
 */
enum novas_equator_type Ecliptic::equator_type() const {
  return _equator;
}

/**
 * Returns the Julian date of the epoch for which the coordinates are defined.
 *
 * @return    [day] the (TDB-based) Julian date of the epoch for which the coordinates are defined.
 *
 * @sa mjd()
 */
double Ecliptic::jd() const {
  return _jd;
}

/**
 * Returns the Modified Julian Date (MJD) of the epoch for which the coordinates are defined.
 *
 * @return    [day] the (TDB-based) MJD of the epoch for which the coordinates are defined.
 *
 * @sa jd()
 */
double Ecliptic::mjd() const {
  return _jd - NOVAS_JD_MJD0;
}

/**
 * Converts these ecliptic coordinates to ICRS ecliptic coordinates.
 *
 * @return    the equivalent ICRS ecliptic coordinates.
 *
 * @sa to_j2000(), to_mod(double), to_mod(Time&), to_tod(double), to_tod(Time&)
 */
Ecliptic Ecliptic::to_icrs() const {
  if(_equator == NOVAS_GCRS_EQUATOR)
    return *this;
  return as_equatorial().to_icrs().as_ecliptic();
}

/**
 * Converts these ecliptic coordinates to J2000 ecliptic coordinates.
 *
 * @return    the equivalent J2000 ecliptic coordinates.
 *
 * @sa to_icrs(), to_mod(double), to_mod(Time&), to_tod(double), to_tod(Time&)
 */
Ecliptic Ecliptic::to_j2000() const {
  if(_equator == NOVAS_MEAN_EQUATOR && _jd == NOVAS_JD_J2000)
    return (*this);

  return as_equatorial().to_j2000().as_ecliptic();
}

/**
 * Converts these ecliptic coordinates to Mean-of-Date (MOD) ecliptic coordinates at the
 * specified epch.
 *
 * @param jd_tdb  [day] the (TDB-based) Julian date specifying the coordinate epoch.
 * @return        the equivalent MOD ecliptic coordinates at the specified date.
 *
 * @sa to_mod(Time&), to_tod(double), to_tod(Time&), to_icrs(), to_j2000()
 */
Ecliptic Ecliptic::to_mod(double jd_tdb) const {
  if(jd_tdb == NOVAS_JD_J2000)
    return to_j2000();

  if(_equator == NOVAS_MEAN_EQUATOR && _jd == jd_tdb)
    return (*this);

  return as_equatorial().to_mod(jd_tdb).as_ecliptic();
}

/**
 * Converts these ecliptic coordinates to Mean-of-Date (MOD) ecliptic coordinates at the
 * specified epch.
 *
 * @param time    the astronomical time specifying the coordinate epoch.
 * @return        the equivalent MOD ecliptic coordinates at the specified date.
 *
 * @sa to_mod(double), to_tod(Time&), to_tod(double), to_icrs(), to_j2000()
 */
Ecliptic Ecliptic::to_mod(const Time& time) const {
  return to_mod(time.jd(novas::NOVAS_TDB));
}

/**
 * Converts these ecliptic coordinates to True-of-Date (TOD) ecliptic coordinates at the
 * specified epch.
 *
 * @param jd_tdb  [day] the (TDB-based) Julian date specifying the coordinate epoch.
 * @return        the equivalent TOD ecliptic coordinates at the specified date.
 *
 * @sa to_tod(Time&), to_mod(double), to_mod(Time&), to_icrs(), to_j2000()
 */
Ecliptic Ecliptic::to_tod(double jd_tdb) const {
  if(_equator == NOVAS_TRUE_EQUATOR && _jd == jd_tdb)
    return (*this);

  return as_equatorial().to_tod(jd_tdb).as_ecliptic();
}

/**
 * Converts these ecliptic coordinates to True-of-Date (TOD) ecliptic coordinates at the
 * specified epch.
 *
 * @param time    the astronomical time specifying the coordinate epoch.
 * @return        the equivalent TOD ecliptic coordinates at the specified date.
 *
 * @sa to_tod(double), to_mod(Time&), to_mod(double), to_icrs(), to_j2000()
 */
Ecliptic Ecliptic::to_tod(const Time& time) const {
  return to_tod(time.jd(novas::NOVAS_TDB));
}
/**
 * Returns a reference to a statically defined standard invalid Galactic coordinates. These invalid
 * coordinates may be used inside any object that is invalid itself.
 *
 * @return    a reference to the static standard invalid coordinates.
 */
/**
 * Converts these ecliptic coordinates to equivalent equatorial coordinates
 *
 * @return    the equivalent equatorial coordinates for the same place on sky.
 *
 * @sa Equatorial::as_ecliptic(), as_galactic()
 */
Equatorial Ecliptic::as_equatorial() const {
  if(is_valid()) {
    double ra = 0.0, dec = 0.0;

    ecl2equ(_jd, _equator, NOVAS_FULL_ACCURACY, longitude().deg(), latitude().deg(), &ra, &dec);

    switch(_equator) {
      case NOVAS_GCRS_EQUATOR:
        return Equatorial(ra * Unit::hour_angle, dec * Unit::deg, Equinox::icrs(), distance().m());
      case NOVAS_MEAN_EQUATOR:
        if(_jd == NOVAS_JD_J2000)
          return Equatorial(ra * Unit::hour_angle, dec * Unit::deg, Equinox::j2000(), distance().m());
        else
          return Equatorial(ra * Unit::hour_angle, dec * Unit::deg, Equinox::mod(_jd), distance().m());
      case NOVAS_TRUE_EQUATOR:
        return Equatorial(ra * Unit::hour_angle, dec * Unit::deg, Equinox::tod(_jd), distance().m());
    }
  }

  return Equatorial::invalid();
}

/**
 * Converts these ecliptic coordinates to equivalent Galactic coordinates
 *
 * @return    the equivalent Galactic coordinates for the same place on sky.
 *
 * @sa Galactic::as_ecliptic(), as_equatorial()
 */
Galactic Ecliptic::as_galactic() const {
  return as_equatorial().as_galactic();
}

static std::string _sys_type(enum novas_equator_type equator, double jd_tt) {
  char s[20] = {'\0'};
  int n = 0;

  switch(equator) {
    case NOVAS_GCRS_EQUATOR:
      return "ICRS";
    case NOVAS_MEAN_EQUATOR:
      break;
    case NOVAS_TRUE_EQUATOR:
      n = snprintf(s, sizeof(s), "TOD ");
  }

  if(novas_time_equals(jd_tt, NOVAS_JD_B1900))
    snprintf(&s[n], sizeof(s) - n, "B1900");
  else if(novas_time_equals(jd_tt, NOVAS_JD_B1950))
    snprintf(&s[n], sizeof(s) - n, "B1950");
  else {
    n += snprintf(&s[n], sizeof(s) - n, "J%.3f", 2000.0 + (jd_tt - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS);

    // Remove trailing zeroes and decimal point.
    for(int i = n; --i >= 0; ) {
      if(s[i] == '.') {
        s[i] = '\0';
        break;
      }
      if(s[i] != '0') break;
      s[i] = '\0';
    }
  }

  return std::string(s);
}

/**
 * Returns a string representation of these ecliptic coordinates, optionally specifying a type of
 * separator to use for the DMS angles, and the decimal places to show for the seconds.
 *
 * @param separator   (optional) the type of separator to use for the DMS representation of angles
 *                    (default: units and spaces)
 * @param decimals    (optional) the number of decimal places to print for the seconds
 *                    (default: 3)
 * @return  a new string with a human-readable representation of these equatorial coordinates.
 */
std::string Ecliptic::to_string(enum novas_separator_type separator, int decimals) const {
  return "ECL  " + Spherical::to_string(separator, decimals) + "  " + _sys_type(_equator, _jd);
}

/**
 * Returns a reference to a statically defined standard invalid ecliptic coordinates. These invalid
 * coordinates may be used inside any object that is invalid itself.
 *
 * @return    a reference to the static standard invalid coordinates.
 */
const Ecliptic& Ecliptic::invalid() {
  static const Ecliptic _invalid = Ecliptic(NAN, NAN, Equinox::invalid(), NAN);
  return _invalid;
}


} // namespace supernovas


