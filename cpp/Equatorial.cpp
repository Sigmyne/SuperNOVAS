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
    novas_set_errno(EINVAL, fn, "Invalid equatorial system: %s", _sys.to_string().c_str());
  }
}

/**
 * Instantiates equatorial coordinates with the specified right-ascention (R.A.) and declination
 * coordinates, optionally specifying a system and a distance if needed.
 *
 * @param ra_rad      [rad] right ascention (R.A.) coordinate
 * @param dec_rad     [rad] declination coordinate
 * @param system      (optional) the equatorial coordinate reference system in which the
 *                    coordinates are specified (default: ICRS)
 */
Equatorial::Equatorial(double ra_rad, double dec_rad, const Equinox &system)
: Spherical(ra_rad, dec_rad), _sys(system) {
  validate();
}

/**
 * Instantiates equatorial coordinates with the specified right-ascention (R.A.) and declination
 * coordinates, optionally specifying a system and a distance if needed.
 *
 * @param ra          right ascention (R.A.) coordinate
 * @param dec         declination coordinate
 * @param system      (optional) the equatorial coordinate reference system in which the
 *                    coordinates are specified (default: ICRS)
 */
Equatorial::Equatorial(const Angle& ra, const Angle& dec, const Equinox &system)
: Spherical(ra, dec), _sys(system) {
  validate();
}

/**
 * Instantiates equatorial coordinates with the specified string representations of
 * right-ascention (R.A.) and declination, optionally specifying a system and a distance if
 * needed. After instantiation, you should check that the resulting coordinates are valid, e.g.
 * as:
 *
 * ```c++
 *   Equatorial coords = Equatorial(..., ...);
 *   if(!coords.is_valid()) {
 *     // oops, looks like the angles could not be parsed...
 *     return;
 *   }
 * ```
 *
 * @param ra          string representation of the right ascention (R.A.) coordinate in HMS or
 *                    decimal hours.
 * @param dec         string representation of the declination coordinate as DMS or decimal
 *                    degrees.
 * @param system      (optional) the equatorial coordinate reference system in which the
 *                    coordinates are specified (default: ICRS)
 *
 * @sa novas_str_hours(), novas_str_degrees() for details on string representation that can be parsed.
 */
Equatorial::Equatorial(const std::string& ra, const std::string& dec, const Equinox &system)
: Equatorial(TimeAngle(ra), Angle(dec), system) {}

/**
 * Instantiates equatorial coordinates with the specified rectangular components
 *
 * @param pos         position vector
 * @param system      (optional) the equatorial coordinate reference system in which the
 *                    coordinates are specified (default: ICRS)
 */
Equatorial::Equatorial(const Position& pos, const Equinox& system)
: Spherical(pos.to_spherical()), _sys(system) {
  validate();
}

/**
 * Checks if these equatorial coordinates are the same as another, within the specified
 * precision.
 *
 * @param other           the reference equatorial coordinates
 * @param precision_rad   [rad] (optional) precision for equality test (default: 1 &mu;as).
 * @return                `true` if these coordinates are the same as the reference within the
 *                        precision, or else `false`.
 *
 * @sa equals(), operator==()
 */
bool Equatorial::equals(const Equatorial& other, double precision_rad) const {
  return (_sys == other._sys) && Spherical::equals(other, precision_rad);
}

/**
 * Checks if these equatorial coordinates are the same as another, within the specified
 * precision.
 *
 * @param other           the reference equatorial coordinates
 * @param precision       (optional) precision for equality test (default: 1 &mu;as).
 * @return                `true` if these coordinates are the same as the reference within the
 *                        precision, or else `false`.
 *
 * @sa operator==()
 */
// cppcheck-suppress functionStatic
bool Equatorial::equals(const Equatorial& other, const Angle& precision) const {
  return equals(other, precision.rad());
}

/**
 * Checks if these equatorial coordinates are the same as another, within 1 &mu;as.
 *
 * @param other           the reference equatorial coordinates
 * @return                `true` if these coordinates are the same as the reference within 1 &mu;as,
 *                        or else `false`.
 *
 * @sa operator!=()
 */
bool Equatorial::operator==(const Equatorial& other) const {
  return equals(other);
}

/**
 * Checks if these equatorial coordinates differ from another, by more than 1 &mu;as.
 * @param other           the reference equatorial coordinates
 * @return                `true` if these coordinates differ from the reference, by more than
 *                        1 &mu;as, or else `false`.
 *
 * @sa operator==()
 */
bool Equatorial::operator!=(const Equatorial& other) const {
  return !equals(other);
}

/**
 * Converts these equatorial coordinates to another equatorial coordinate system. Same as
 * `to_system()`.
 *
 * @param system    the equatorial coordinate system (type and epoch) to convert to.
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed in the specified other coordinate reference system.
 *
 * @sa to_system()
 */
Equatorial Equatorial::operator>>(const Equinox& system) const {
  return to_system(system);
}

/**
 * Returns the equatorial system (type and epoch) in which these equatorial coordinates are defined.
 *
 * @return    the coordinate reference system (type and epoch).
 *
 * @sa reference_system()
 */
const Equinox& Equatorial::system() const {
  return _sys;
}

/**
 * Retuens the equatorial reference system type in which thse equatorial coordinates are defined.
 *
 * @return    the type of coordinate reference system
 *
 * @sa system()
 */
enum novas::novas_reference_system Equatorial::reference_system() const {
  return _sys.reference_system();
}

/**
 * Returns the angular distance of these equatorial coordiantes to/from the specified other
 * equatorial coordinates.
 *
 * @param other   the reference equatorial coordinates
 * @return        the angular distance of thereturn Angle::operator+(r);se coordinates to/from the argument.
 */
Angle Equatorial::distance_to(const Equatorial& other) const {
  return Spherical::distance_to(other);
}

/**
 * Converts these equatorial coordinates to another equatorial coordinate system.
 *
 * @param system    the equatorial coordinate system (type and epoch) to convert to.
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed in the specified other coordinate reference system.
 *
 * @sa operator>>(), to_icrs(), to_j2000(), to_hip(), to_mod(), to_mod_at_besselian_epoch(),
 *     to_tod(), to_cirs()
 */
Equatorial Equatorial::to_system(const Equinox& system) const {

  if(_sys == system)
    return Equatorial(*this);

  double p[3] = {'\0'};
  radec2vector(ra().hours(), dec().deg(), 1.0, p);

  // Convert to ICRS...
  switch(_sys.reference_system()) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      break;
    case NOVAS_J2000:
      j2000_to_gcrs(p, p);
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
      return Equatorial::invalid();
  }

  // Convert from ICRS to output system...
  switch(system.reference_system()) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      break;
    case NOVAS_J2000:
      gcrs_to_j2000(p, p);
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
      return Equatorial::invalid();
  }

  double r = 0.0, d = 0.0;
  vector2radec(p, &r, &d);
  return Equatorial(r * Unit::hour_angle, d * Unit::deg, system);
}

/**
 * Converts these equatorial coordinates to the International Celestial Reference System (ICRS).
 *
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed in the ICRS (= FK6).
 *
 * @sa to_system()
 */
Equatorial Equatorial::to_icrs() const {
  return to_system(Equinox::icrs());
}

/**
 * Converts these equatorial coordinates to the J2000 (= FK5) catalog coordinate system.
 *
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed in the J2000 (= FK5) catalog system.
 *
 * @sa to_system(), to_icrs(), to_hip(), to_mod(), to_mod_at_besselian_epoch(), to_tod(), to_cirs()
 */
Equatorial Equatorial::to_j2000() const {
  return to_system(Equinox::j2000());
}

/**
 * Converts these equatorial coordinates to the Hipparcos catalog coordinate system (= J1991.25).
 *
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed in the Hipparcos (= J1991.25) catalog system.
 *
 * @sa to_system(), to_icrs(), to_j2000()
 */
Equatorial Equatorial::to_hip() const {
  return to_system(Equinox::mod(NOVAS_JD_HIP));
}

/**
 * Converts these equatorial coordinates to the Mean-of-Date (MOD) catalog coordinate system, at
 * the specified coordinate epoch.
 *
 * @param jd_tdb    [day] (TDB-based) Julian date of the coordinate epoch.
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed in the MOD catalog system of date.
 *
 * @sa to_mod_at_besselian_epoch(), to_system(), to_j2000(), to_tod()
 */
Equatorial Equatorial::to_mod(double jd_tdb) const {
  return to_system(Equinox::mod(jd_tdb));
}

/**
 * Converts these equatorial coordinates to the Mean-of-Date (MOD) catalog coordinate system, at
 * the specified coordinate epoch.
 *
 * @param time      [day] the astronomical time specification for the coordinate epoch.
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed in the MOD catalog system of date.
 *
 * @sa to_mod_at_besselian_epoch(), to_system(), to_j2000(), to_tod()
 */
Equatorial Equatorial::to_mod(const Time& time) const {
  return to_mod(time.jd(novas::NOVAS_TDB));
}

/**
 * Converts these equatorial coordinates to the Mean-of-Date (MOD) catalog coordinate system, at
 * the specified Besselian coordinate epoch.
 *
 * @param year      [yr] Besselian year for the coordinate epoch (e.g. 1950.0 for B1950).
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed in the catalog system of the specified Besselian epoch.
 *
 * @sa to_mod(), to_system(), to_j2000(), to_tod()
 */
Equatorial Equatorial::to_mod_at_besselian_epoch(double year) const {
  return to_system(Equinox::mod_at_besselian_epoch(year));
}

/**
 * Converts these equatorial coordinates to the True-of-Date (TOD) coordinate system, at the
 * specified coordinate epoch. TOD is defined on the true dynamical equator of date, with its
 * origin at the true equinox of date.
 *
 * @param jd_tdb    [day] (TDB-based) Julian date of the coordinate epoch.
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed with respect to the true equator and equinox of date.
 *
 * @sa to_system(), to_cirs(), to_j2000(), to_mod()
 */
Equatorial Equatorial::to_tod(double jd_tdb) const {
  return to_system(Equinox::tod(jd_tdb));
}

/**
 * Converts these equatorial coordinates to the True-of-Date (TOD) coordinate system, at the
 * specified coordinate epoch. TOD is defined on the true dynamical equator of date, with its
 * origin at the true equinox of date.
 *
 * @param time      [day] the astronomical time specification for the coordinate epoch.
 * @return          new equatorial coordinates, which represent the same equatorial position as
 *                  this, but expressed with respect to the true equator and equinox of date.
 *
 * @sa to_system(), to_cirs(), to_j2000(), to_mod()
 */
Equatorial Equatorial::to_tod(const Time& time) const {
  return to_tod(time.jd(novas::NOVAS_TDB));
}

/**
 * Converts these equatorial coordinates to the Celestial Intermediate Reference System (CIRS)
 * coordinate system, at the specified coordinate epoch. CIRS is defined on the true dynamical
 * equator of date, with its origin at the Celestial Intermediate Origin (CIO).
 *
 * @param jd_tdb    [day] (TDB-based) Julian date of the coordinate epoch.
 * @return          new equatorial coordinates, which represent the same equatorial position as this,
 *                  but with respect to the true equator and CIO of date.
 *
 * @sa to_system(), to_tod(), to_icrs()
 */
Equatorial Equatorial::to_cirs(double jd_tdb) const {
  return to_system(Equinox::cirs(jd_tdb));
}

/**
 * Converts these equatorial coordinates to the Celestial Intermediate Reference System (CIRS)
 * coordinate system, at the specified coordinate epoch. CIRS is defined on the true dynamical
 * equator of date, with its origin at the Celestial Intermediate Origin (CIO).
 *
 * @param time      [day] the astronomical time specification for the coordinate epoch.
 * @return          new equatorial coordinates, which represent the same equatorial position as this,
 *                  but with respect to the true equator and CIO of date.
 *
 * @sa to_system(), to_tod(), to_icrs()
 */
Equatorial Equatorial::to_cirs(const Time& time) const {
  return to_cirs(time.jd(novas::NOVAS_TDB));
}

/**
 * Returns the right ascention (R.A.) coordinate as a time-angle.
 *
 * @return    the right ascention (R.A.) coordinate.
 *
 * @sa dec()
 */
TimeAngle Equatorial::ra() const {
  return TimeAngle(longitude().rad());
}

/**
 * Returns the declination coordinate as an angle.
 *
 * @return    the declination coordinate.
 *
 * @sa ra()
 */
const Angle& Equatorial::dec() const {
  return latitude();
}

/**
 * Returns the equivalent ecliptic coordinates corresponding to these equatorial coordinates.
 *
 * @return    the ecliptic coordinates that represent the same position on sky as these equariorial
 *            coordinates.
 *
 * @sa Ecliptic::to_equatorial(), to_galactic()
 */
Ecliptic Equatorial::to_ecliptic() const {
  double lon, lat;

  if(!is_valid())
    return Ecliptic::invalid();

  double r = ra().hours();
  double d = dec().deg();

  if(_sys.reference_system() == NOVAS_CIRS)
    r -= ira_equinox(_sys.jd(), NOVAS_TRUE_EQUINOX, NOVAS_FULL_ACCURACY);

  equ2ecl(_sys.jd(), _sys.equator_type(), NOVAS_FULL_ACCURACY, r, d, &lon, &lat);
  return Ecliptic(lon * Unit::deg, lat * Unit::deg, _sys);
}

/**
 * Returns the equivalent galactic coordinates corresponding to these equatorial coordinates.
 *
 * @return    the galactic coordinates that represent the same position on sky as these equariorial
 *            coordinates.
 *
 * @sa Galactic::to_equatorial(), to_ecliptic()
 */
Galactic Equatorial::to_galactic() const {
  if(!is_valid())
    return Galactic::invalid();

  Equatorial icrs = to_icrs();
  double longitude = 0.0, latitude = 0.0;
  equ2gal(icrs.ra().hours(), icrs.dec().deg(), &longitude, &latitude);
  return Galactic(longitude * Unit::deg, latitude * Unit::deg);
}

/**
 * Returns a string representation of these equatorial coordinates in HMS / DMS format,
 * optionally specifying the type of separator to use and the precision to print.
 *
 * @param separator   (optional) the type of separators to use for the HMS / DMS representations
 *                    of the components (default: units and spaces).
 * @param decimals    (optional) the number of decimal places to print for the seconds
 *                    (default: 3)
 * @return    a new string with the human-readable representation of these equatorial coordinates.
 */
std::string Equatorial::to_string(enum novas_separator_type separator, int decimals) const {
  return "EQU " + ra().to_string(separator, decimals + 1) + "  "
          + dec().to_string(separator, decimals) + "  " + _sys.to_string();
}

/**
 * Returns a reference to a statically defined standard invalid equatorial coordinates. These invalid
 * coordinates may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid equatorial coordinates.
 */
const Equatorial& Equatorial::invalid() {
  static const Equatorial _invalid = Equatorial(NAN, NAN, Equinox::invalid());
  return _invalid;
}

} // namespace supernovas


