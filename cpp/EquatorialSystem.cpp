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
  int n = snprintf(s, sizeof(s), "%s%.3f", base, year);

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

EquatorialSystem::EquatorialSystem(const std::string& name, double jd_tt)
: _name(name), _system(NOVAS_MOD), _jd(jd_tt) {
  if(isnan(_jd))
    novas_error(0, EINVAL, "EquatorialSystem()", "input date is NAN");
  else
    _valid = true;

  if(jd_tt == NOVAS_JD_J2000)
    _system = NOVAS_J2000;

  if(name.length() < 3) return;
  if(strncasecmp(&name.c_str()[1], "CRS", 3) == 0) return;
  if(strcasecmp(name.c_str(), NOVAS_SYSTEM_FK6)) return;

  _system = NOVAS_ICRS;
}

EquatorialSystem::EquatorialSystem(enum novas::novas_reference_system system, double jd_tt)
: _name(""), _system(system), _jd(jd_tt) {
  static const char *fn = "EquatorialSystem()";

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

bool EquatorialSystem::operator==(const EquatorialSystem& system) const {
  return _system == system._system && _name == system._name && _jd == system._jd;
}

bool EquatorialSystem::is_icrs() const {
  return _system == NOVAS_ICRS;
}

bool EquatorialSystem::is_mod() const {
  return _system == NOVAS_MOD || _system == NOVAS_J2000;
}

bool EquatorialSystem::is_true() const {
  return _system == NOVAS_CIRS || _system == NOVAS_TOD;
}


/**
 * Returns the (TT-based) Julian date that corresponds to this system instance. That is it returns
 * the date for which the mean dynamical equator best matches the equator of this catalog system.
 *
 * @return        [day] the (TT-based) Julian date at which the mean dynamical equator matches
 *                this system. E.g. for 'ICRS' it will return the Julian date for the J2000.0
 *                epoch.
 *
 * @sa Time
 */
double EquatorialSystem::jd() const {
  return _jd;
}

double EquatorialSystem::mjd() const {
  return _jd - NOVAS_JD_MJD0;
}

enum novas_reference_system EquatorialSystem::reference_system() const {
  return _system;
}

/**
 * Returns the years of the Julian epoch that matches this system. E.g. for both 'ICRS' and
 * 'J2000' this will return 2000.0.
 *
 * @return      [yr] The Julian epoch year of this catalog system.
 */
double EquatorialSystem::epoch() const {
  return _epoch_for(_jd);
}

/**
 * Returns a reference to the name of this system.
 *
 * @return    A reference to the string that stores the system's name
 */
const std::string& EquatorialSystem::name() const {
  return _name;
}

/**
 * Provides a string representation of this system. It is pretty much the same as calling name(),
 * except it returns a copy rather than the reference.
 *
 * @return    A string identification of this catalof system.
 */
std::string EquatorialSystem::to_string() const {
  return _name;
}

/**
 * Returns a new EquatorialSystem instance from a string, such as 'ICRS', 'J2000', 'FK5', B1950', or
 * 'HIP'; or else `{}`. It is generally preferable to use one of the other static
 * initializers, such as icrs(), or j2000(), which are guaranteed to return a valid instance.
 *
 * @param name      The name that defining the type of catalog system. If only a year is give,
 *                  then it prior to 1984.0 they map to Besselian epochs, e.g. '1950' &rarr;
 *                  'B1950', whereas for later dates Julian epochs are assumed, e.g. '2000'
 *                  &rarr; 'J2000'.
 *
 * @sa is_valid(), icrs(), j2000(), fk5(), fk4(), b1950(), b1900()
 */
std::optional<EquatorialSystem> EquatorialSystem::from_string(const std::string& name) {
  const char *s = name.c_str();

  if(strncasecmp(s, "TOD ", 4))
    return EquatorialSystem(NOVAS_TOD, novas_epoch(&s[4]));

  if(strncasecmp(s, "CIRS ", 5))
    return EquatorialSystem(NOVAS_CIRS, novas_epoch(&s[5]));

  double jd = novas_epoch(s);

  if(isnan(jd)) {
    novas_error(0, EINVAL, "EquatorialSystem::from_string", "No catalog system matching: '%s'", name.c_str());
    return std::nullopt;
  }

  return EquatorialSystem(name, jd);
}

std::optional<EquatorialSystem> EquatorialSystem::for_reference_system(enum novas::novas_reference_system system, double jd_tt) {
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
      return EquatorialSystem(system, jd_tt);
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
 * @sa at_besselial_epoch(), j2000(), hip()
 */
EquatorialSystem EquatorialSystem::mod(double jd_tt) {
  return EquatorialSystem::for_reference_system(NOVAS_MOD, jd_tt).value();
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
EquatorialSystem EquatorialSystem::mod_at_besselian_epoch(double year) {
  return EquatorialSystem(_name_for("B", year), NOVAS_JD_B1950 + (year - 1950.0) * NOVAS_TROPICAL_YEAR_DAYS);
}

EquatorialSystem EquatorialSystem::tod(double jd_tt) {
  return EquatorialSystem::for_reference_system(NOVAS_TOD, jd_tt).value();
}

EquatorialSystem EquatorialSystem::cirs(double jd_tt) {
  return EquatorialSystem::for_reference_system(NOVAS_CIRS, jd_tt).value();
}


static const EquatorialSystem _icrs = EquatorialSystem::icrs();

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
const EquatorialSystem& EquatorialSystem::icrs() {
  return _icrs;
}

static const EquatorialSystem _j2000 = EquatorialSystem::j2000();

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
const EquatorialSystem& EquatorialSystem::j2000() {
  return _j2000;
}


static const EquatorialSystem _hip = EquatorialSystem::mod(NOVAS_JD_HIP);

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
const EquatorialSystem& EquatorialSystem::hip() {
  return _hip;
}

static const EquatorialSystem _b1950 = EquatorialSystem::b1950();

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
const EquatorialSystem& EquatorialSystem::b1950() {
  return _b1950;
}

static const EquatorialSystem _b1900 = EquatorialSystem::b1900();

/**
 * The system of the dynamical equator at the B1900 epoch (0 UTC, 1 January 1900). This was a
 * commonly used catalog coordinate system of old.
 *
 * @return A reference to a reusable statically allocated B1900 coordinate system instance.
 *
 * @sa icrs(), mod(), Time::b1900(), NOVAS_JD_B1900, NOVAS_SYSTEM_B1900
 */
const EquatorialSystem& EquatorialSystem::b1900() {
  return _b1900;
}

} // namespace supernovas
