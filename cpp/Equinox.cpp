/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include <string.h>
#include <cstring>
#include <ctype.h>
#ifndef _MSC_VER
#  include <strings.h>            // strcasecmp() / strncasecmp() -- POSIX.1-2001 / 4.4BSD
#endif

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

#if defined(_MSC_VER)
#  define strcasecmp _stricmp                       /// MSVC equivalent
#  define strncasecmp _strnicmp                     /// MSVC equivalent
#endif

using namespace novas;

namespace supernovas {

static double _epoch_for(double jd) {
  return 2000.0 + (jd - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

static std::string _name_for(const char *base, double year) {
  char s[20] = {'\0'};
  int n;

  n = snprintf(s, sizeof(s), "%s%.3f", base, year);

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

Equinox::Equinox(const std::string& name, double jd_tt)
: _name(name), _system(NOVAS_MOD), _jd(jd_tt) {
  if(isnan(_jd))
    novas_error(0, EINVAL, "Equinox()", "input date is NAN");
  else
    _valid = true;

  if(jd_tt == NOVAS_JD_J2000)
    _system = NOVAS_J2000;

  _name = name;

  if(name.length() < 3) return;
  if(strncasecmp(&name.c_str()[1], "CRS", 3) == 0) return;
  if(strcasecmp(name.c_str(), NOVAS_SYSTEM_FK6)) return;

  _system = NOVAS_ICRS;
}

Equinox::Equinox(enum novas::novas_reference_system system, double jd_tt)
: _name("invalid"), _system(system), _jd(jd_tt) {
  static const char *fn = "Equinox()";

  switch(system) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      _name = std::string(NOVAS_SYSTEM_ICRS);
      _system = NOVAS_ICRS;
      _jd = NOVAS_JD_J2000;
      break;
    case NOVAS_J2000:
      _name = std::string(NOVAS_SYSTEM_J2000);
      _jd = NOVAS_JD_J2000;
      break;
    case NOVAS_MOD:
      _name =_name_for("J", epoch());
      break;
    case NOVAS_TOD:
      _name =_name_for("TOD J", epoch());
      break;
    case NOVAS_CIRS:
      _name =_name_for("CIRS J", epoch());
      break;
    default:
      novas_error(0, EINVAL, fn, "invalid reference system: %d", system);
      return;
  }

  if(isnan(jd_tt))
    novas_error(0, EINVAL, fn, "input Julian date is NAN");
  else
    _valid = true;
}

/**
 * Checks if this equatorial system is the same exact equatorial system as the right-hand side.
 *
 * @param system  The equatorial system on the right-hand side.
 * @return        `true` if the two equatorial systems are exactly the same, or else `false`.
 *
 * @sa operator!=()
 */
bool Equinox::operator==(const Equinox& system) const {
  return _system == system._system && _name == system._name && _jd == system._jd;
}

/**
 * Checks if this equatorial system is the International Celestial Reference System (ICRS).
 * ICRS is the standard catalog system in the IAU 2000 convention.
 *
 * @return    `true` if this equatorial system is the ICRS, or else `false`.
 *
 * @sa is_mod(), is_true()
 */
bool Equinox::is_icrs() const {
  return _system == NOVAS_ICRS;
}

/**
 * Checks if this equatorial system is a Mean-of-Date (MOD) system. MODs include precession, but
 * not nutation, and have been commonly used for catalogs (e.g. J2000, B1950, FK4, or HIP) prior
 * to the IAU 2000 convention, when ICRS became the new standard catalog system.
 *
 * @return    `true` if this equatorial system is a Mean-of-Date (MOD) catalog system, or else
 *            `false`.
 *
 * @sa is_icrs(), is_true()
 */
bool Equinox::is_mod() const {
  return _system == NOVAS_MOD || _system == NOVAS_J2000;
}

/**
 * Checks if this equatorial system is based on the true dynamical equator of date. If `true`, it
 * may be a True-of-Date (TOD) system with the origin at the true equinox of date, or else
 * the Celestial Intermediate Reference System (CIRS) with its origin at the Celestial
 * Intermediate Origin (CIO).
 *
 * @return    `true` if this equatorial system is defined on the true dynamical equator of date, or
 *            else `false`.
 *
 * @sa is_icrs(), is_mod()
 */
bool Equinox::is_true() const {
  return _system == NOVAS_CIRS || _system == NOVAS_TOD;
}

/**
 * Returns the (TT-based) Julian date that corresponds to this system instance. That is, it returns
 * the date for which the mean dynamical equator best matches the equator of this catalog system.
 *
 * @return        [day] the (TT-based) Julian date at which the mean dynamical equator matches
 *                this system. E.g. for 'ICRS' it will return the Julian date for the J2000.0
 *                epoch.
 *
 * @sa mjd(), epoch(), Time
 */
double Equinox::jd() const {
  return _jd;
}

/**
 * Returns the (TT-based) Modified Julian Date (MJD) that corresponds to this system instance.
 * That is, it returns the MJD for which the mean dynamical equator best matches the equator of
 * this catalog system.
 *
 * @return        [day] the (TT-based) Modified Julian Date (MJD) at which the mean dynamical
 *                equator matches this system. E.g. for 'ICRS' it will return the MJD for the
 *                J2000.0 epoch.
 *
 * @sa jd(), epoch(), Time
 */
double Equinox::mjd() const {
  return _jd - NOVAS_JD_MJD0;
}

/**
 * Returns the coordinate reference system type for this equatorial system.
 *
 * @return      the coordinate reference system type
 */
enum novas_reference_system Equinox::reference_system() const {
  return _system;
}

/**
 * Returns the type of equator used in this equatorial coordinate system.
 *
 * @return    the type of equator used in this equatorial system.
 */
enum novas_equator_type Equinox::equator_type() const {
  switch(reference_system()) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      return NOVAS_GCRS_EQUATOR;
    case NOVAS_J2000:
    case NOVAS_MOD:
      return NOVAS_MEAN_EQUATOR;
    case NOVAS_TOD:
    case NOVAS_CIRS:
      return NOVAS_TRUE_EQUATOR;
    default:
      return (enum novas_equator_type) -1;
  }
}

/**
 * Returns the years of the Julian epoch that matches this system. E.g. for both 'ICRS' and
 * 'J2000' this will return 2000.0.
 *
 * @return      [yr] The Julian epoch year of this catalog system.
 *
 * @sa jd(), mjd(), Time::epoch()
 */
double Equinox::epoch() const {
  return _epoch_for(_jd);
}

/**
 * Returns a reference to the name of this system.
 *
 * @return    A reference to the string that stores the system's name
 */
const std::string& Equinox::name() const {
  return _name;
}

/**
 * Provides a string representation of this system. It is pretty much the same as calling name(),
 * except it returns a copy rather than the reference.
 *
 * @return    A string identification of this catalog system.
 */
std::string Equinox::to_string() const {
  return std::string(_name);
}

/**
 * Returns a new Equinox instance from a string, such as 'ICRS', 'J2000', 'FK5', B1950', or
 * 'HIP'; or else `{}`. It is generally preferable to use one of the other static
 * initializers, such as icrs(), or j2000(), which are guaranteed to return a valid instance.
 *
 * @param name      The name that defining the type of catalog system. If only a year is give,
 *                  then it prior to 1984.0 they map to Besselian epochs, e.g. '1950' &rarr;
 *                  'B1950', whereas for later dates Julian epochs are assumed, e.g. '2000'
 *                  &rarr; 'J2000'.
 * @return          an optional containing the corresponding valid equatorial system, or else
 *                  `std::nullopt`.
 *
 * @sa is_valid(), icrs(), j2000(), fk5(), fk4(), b1950(), b1900()
 */
std::optional<Equinox> Equinox::from_string(const std::string& name) {
  const char *s = name.c_str();

  if(strncasecmp(s, "TOD ", 4))
    return Equinox(NOVAS_TOD, novas_epoch(&s[4]));

  if(strncasecmp(s, "CIRS ", 5))
    return Equinox(NOVAS_CIRS, novas_epoch(&s[5]));

  double ejd = novas_epoch(s);

  if(isnan(ejd)) {
    novas_error(0, EINVAL, "Equinox::from_string", "No catalog system matching: '%s'", name.c_str());
    return std::nullopt;
  }

  return Equinox(name, ejd);
}

/**
 * Returns a new Equinox given a coordinate reference system type and a date.
 *
 * @param system    The coordinate reference system type
 * @param jd_tt     [day] The (TT-based) Julian date (for dynamical systems).
 * @return          an optional containing the corresponding valid equatorial system, or else
 *                  `std::nullopt`.
 */
std::optional<Equinox> Equinox::for_reference_system(enum novas::novas_reference_system system, double jd_tt) {
  static const char *fn = "Equatorial::for_reference_system";

  if(system == NOVAS_GCRS || system == NOVAS_ICRS || system == NOVAS_J2000) {
    jd_tt = NOVAS_JD_J2000;
  }
  else if(isnan(jd_tt)) {
    novas_error(0, EINVAL, fn, "input JD is NAN");
    return std::nullopt;
  }
  else if((unsigned) system >= NOVAS_REFERENCE_SYSTEMS) {
    novas_error(0, EINVAL, fn, "invalid reference system: %d", system);
    return std::nullopt;
  }

  switch(system) {
    case NOVAS_TIRS:
    case NOVAS_ITRS:
      return std::nullopt;
    default:
      return Equinox(system, jd_tt);
  }
}

/**
 * Mean-of-date (MOD) dynamical coordinate system, at the specified Julian epoch. MOD coordinates
 * take into account Earth's slow precession but not nutation. Julian-date based MODs were
 * commonly used for catalogs, such as J2000, or HIP.
 *
 * @param jd_tt     [day] TT-based Julian day.
 * @return          A reference system with the mean dynamical equator of date, with origin at
 *                  the mean equinox of date.
 *
 * @sa mod(Time&), mod_at_besselial_epoch(), j2000(), hip()
 */
Equinox Equinox::mod(double jd_tt) {
  return Equinox::for_reference_system(NOVAS_MOD, jd_tt).value();
}

/**
 * Mean-of-date (MOD) dynamical coordinate system, at the specified Julian epoch. MOD coordinates
 * take into account Earth's slow precession but not nutation. Julian-date based MODs were
 * commonly used for catalogs, such as J2000, or HIP.
 *
 * @param time      astrometric time specifying the coordinate epoch.
 * @return          A reference system with the mean dynamical equator of date, with origin at
 *                  the mean equinox of date.
 *
 * @sa mod(double), mod_at_besselial_epoch(), j2000(), hip()
 */
Equinox Equinox::mod(const Time& time) {
  return Equinox::mod(time.jd());
}

/**
 * Mean-of-date (MOD) dynamical coordinate system, at the specified Besselian epoch. MOD
 * coordinates take into account Earth's precession but not nutation. Besselian-date based MODs,
 * now a historical relic, were once commonly used for catalog systems, such as B1900, or B1950.
 *
 *
 * @param year      [yr] UTC-based decimal calendar year.
 * @return          A reference system with the mean dynamical equator of date, with origin at
 *                  the mean equinox of date.
 *
 * @sa at_julian_date(), b1900(), b1950()
 */
Equinox Equinox::mod_at_besselian_epoch(double year) {
  return Equinox(_name_for("B", year), NOVAS_JD_B1950 + (year - 1950.0) * Unit::besselian_year / Unit::day);
}

/**
 * Returns a True-of-Date (TOD) equatorial system for the given (TT-based) Julian date. It is
 * the system on the true dynamical equator of date, with its origin at the true equinox of
 * date.
 *
 * @param jd_tt     [day] The (TT-based) Julian date for when of the true dynamical equator and
 *                  true equinox of date define the returned equatorial coordinate system,
 * @return          the equatorial coordinate system based on the true dynamical equator and
 *                  equinox of date.
 *
 * @sa tod(Time&), cirs(double)
 */
Equinox Equinox::tod(double jd_tt) {
  return Equinox::for_reference_system(NOVAS_TOD, jd_tt).value();
}

/**
 * Returns a True-of-Date (TOD) equatorial system for the given (TT-based) Julian date. It is
 * the system on the true dynamical equator of date, with its origin at the true equinox of
 * date.
 *
 * @param time      astromemtric time specifying the coordinate epoch,
 * @return          the equatorial coordinate system based on the true dynamical equator and
 *                  equinox of date.
 *
 * @sa tod(double), cirs(Time&)
 */
Equinox Equinox::tod(const Time& time) {
  return Equinox::tod(time.jd());
}

/**
 * Returns a Celestial Intermediate Reference System (CIRS) equatorial system for the given
 * (TT-based) Julian date. It is the system on the true dynamical equator of date, with its origin
 * at the Celestial Intermediate Origin (CIO) of date.
 *
 * @param jd_tt     [day] The (TT-based) Julian date for when of the true dynamical equator and
 *                  true equinox of date define the returned equatorial coordinate system,
 * @return          the equatorial coordinate system based on the true dynamical equator, with
 *                  its origin at the CIO
 *
 * @sa cirs(Time&), tod(double)
 */
Equinox Equinox::cirs(double jd_tt) {
  return Equinox::for_reference_system(NOVAS_CIRS, jd_tt).value();
}

/**
 * Returns a Celestial Intermediate Reference System (CIRS) equatorial system for the given
 * (TT-based) Julian date. It is the system on the true dynamical equator of date, with its origin
 * at the Celestial Intermediate Origin (CIO) of date.
 *
 * @param time      astrometric time specifying the coordinate epoch.
 * @return          the equatorial coordinate system based on the true dynamical equator, with
 *                  its origin at the CIO
 *
 * @sa cirs(double), tod(Time&)
 */
Equinox Equinox::cirs(const Time& time) {
  return Equinox::cirs(time.jd());
}

/**
 * International Celestial Reference System (ICRS) is the IAU standard catalog coordinate system.
 * It is defined by distant quasars, and is aligned with the J2000 dynamical equator within 22 mas.
 * In SuperNOVAS ICRS is the same as GCRS (the Geocentric Celestial Reference System) or BCRS (the
 * Barycentric International Reference System), which have the same alignment and differ only in the
 * location of their origin. In SuperNOVAS, the origin is determined by the @ref Observer location,
 * while the coordinate system defines only the orientation of the celestial pole. Thus, there is
 * no need to distinguish between these related systems explicitly in SuperNOVAS.
 *
 * ICRS is also the system used for the 6th Catalog of Fundamental Stars (FK6).
 *
 * @return A reference to a reusable statically allocated ICRS system instance.
 *
 * @sa NOVAS_ICRS, NOVAS_GCRS, NOVAS_SYSTEM_ICRS
 */
const Equinox& Equinox::icrs() {
  static const Equinox _icrs = Equinox::for_reference_system(NOVAS_ICRS, NOVAS_JD_J2000).value();
  return _icrs;
}

/**
 * The system of the dynamical equator at the J2000 epoch (12 TT, 1 January 2000). This was a
 * commonly used catalog coordinate system before the advent of the IAU 2000 standard ICRS system.
 * It is also known as FK5, since the 5th realization of the fundamental catalog of stars used
 * J2000 also.
 *
 * @return A reference to a reusable statically allocated J2000 coordinate system instance.
 *
 * @sa icrs(), mod(), Time::j2000(), NOVAS_JD_J2000, NOVAS_SYSTEM_J2000
 */
const Equinox& Equinox::j2000() {
  static const Equinox _j2000 = Equinox::mod(NOVAS_JD_J2000);
  return _j2000;
}


/**
 * The system of the mean dynamical equator at the J1991.25 epoch, which is adopted as the nominal
 * mean epoch of the Hipparcos catalog.
 *
 * J2000 is also the system used for the 5th Catalog of Fundamental Stars (FK5).
 *
 * @return A reference to a reusable statically allocated Hipparcos coordinate system instance.
 *
 * @sa icrs(), mod(), Time::hip() NOVAS_JD_HIP, NOVAS_SYSTEM_HIP
 */
const Equinox& Equinox::hip() {
  static const Equinox _hip = Equinox::mod(NOVAS_JD_HIP);
  return _hip;
}

/**
 * The system of the dynamical equator at the B1950 epoch (0 UTC, 1 January 1950). This was a
 * commonly used catalog coordinate system of old. It is also known as FK4, since the 4th
 * realization of the fundamental catalog of stars used B1950 also.
 *
 * B1950 is also the system used for the 4th Catalog of Fundamental Stars (FK4).
 *
 * @return A reference to a reusable statically allocated B1950 coordinate system instance.
 *
 * @sa icrs(), mod(), Time::b1950(), NOVAS_JD_B1950, NOVAS_SYSTEM_B1950
 */
const Equinox& Equinox::b1950() {
  static const Equinox _b1950 = Equinox::mod_at_besselian_epoch(1950.0);
  return _b1950;
}

/**
 * The system of the dynamical equator at the B1900 epoch (0 UTC, 1 January 1900). This was a
 * commonly used catalog coordinate system of old.
 *
 * @return A reference to a reusable statically allocated B1900 coordinate system instance.
 *
 * @sa icrs(), mod(), Time::b1900(), NOVAS_JD_B1900, NOVAS_SYSTEM_B1900
 */
const Equinox& Equinox::b1900() {
  static const Equinox _b1900 = Equinox::mod_at_besselian_epoch(1900.0);
  return _b1900;
}

/**
 * Returns a reference to a statically defined standard invalid equatorial system. This invalid
 * equatorial system may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid equatorial system.
 */
const Equinox& Equinox::invalid() {
  static const Equinox _invalid = Equinox((enum novas_reference_system) -1, NAN);
  return _invalid;
}

} // namespace supernovas
