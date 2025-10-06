/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include <string.h>
#include <ctype.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"


using namespace novas;


namespace supernovas {

static double _epoch_for(double jd) {
  return 2000.0 + (jd - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

static std::string _name_for(const char *base, double year) {
  char s[20] = {'\0'};
  snprintf(s, sizeof(s), "%s%.3f", base, year);

  // Remove trailing zeroes and decimal point.
  for(int i = strlen(s); --i >= 0; ) {
    if(s[i] == '.') {
      s[i] = '\0';
      break;
    }
    if(s[i] != '0') break;
    s[i] = '\0';
  }

  return std::string(s);
}

CatalogSystem::CatalogSystem(const std::string& name, double jd_tt) : _name(name), _jd(jd_tt) {
  if(isnan(_jd))
    novas_error(0, EINVAL, "CatalogSystem()", "input date is NAN");
  else
    _valid = true;
}

/**
 * Constructor from name, such as 'ICRS', 'J2000', 'FK5', B1950', or 'HIP'. Unless, you are certain
 * that SuperNOVAS will match the name to a system and corresponding dynamical epoch, you should
 * probably call is_valid() to verify that the name was in fact recongized by the library. E.g.:
 *
 * ```c
 *  CatalogSystem s = CatalogSystem("FK4");
 *  if(!s.is_valid()) {
 *    // Oops, that did not work, as expected...
 *    ...
 *  }
 * ```
 *
 * It is generally preferable to use one of the static initializers, such as icrs(), or j2000(),
 * which are guaranteed to return a valid instance.
 *
 * @param name      The name that defining the type of catalog system. If only a year is give,
 *                  then it prior to 1984.0 they map to Besselian epochs, e.g. '1950' &rarr;
 *                  'B1950', whereas for later dates Julian epochs are assumed, e.g. '2000'
 *                  &rarr; 'J2000'.
 *
 * @sa is_valid(), icrs(), j2000(), fk5(), fk4(), b1950(), b1900()
 */
CatalogSystem::CatalogSystem(const std::string& name) : CatalogSystem(name, novas_epoch(name.c_str())) {}

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
double CatalogSystem::jd() const {
  return _jd;
}

/**
 * Returns the years of the Julian epoch that matches this system. E.g. for both 'ICRS' and
 * 'J2000' this will return 2000.0.
 *
 * @return      [yr] The Julian epoch year of this catalog system.
 */
double CatalogSystem::epoch() const {
  return _epoch_for(_jd);
}

/**
 * Returns a reference to the name of this system.
 *
 * @return    A reference to the string that stores the system's name
 */
const std::string& CatalogSystem::name() const {
  return _name;
}

/**
 * Provides a string representation of this system. It is pretty much the same as calling name(),
 * except it returns a copy rather than the reference.
 *
 * @return    A string identification of this catalof system.
 */
std::string CatalogSystem::str() const {
  return _name;
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
CatalogSystem CatalogSystem::at_julian_date(double jd_tt) {
  return CatalogSystem(_name_for("J", 2000.0 + (jd_tt - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS), jd_tt);
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
CatalogSystem CatalogSystem::at_besselian_epoch(double year) {
  return CatalogSystem(_name_for("B", year), NOVAS_JD_B1950 + (year - 1950.0) * NOVAS_TROPICAL_YEAR_DAYS);
}

static const CatalogSystem _icrs = CatalogSystem("ICRS");

/**
 * International Celestial Reference System (ICRS) is the IAU standard catalog coordinate system.
 * It is defined by distant quasars, and is aligned with the J2000 dynamical equator within 22 mas.
 * In SuperNOVAS ICRS is the same as GCRS (the Geocentric Celestial Reference System) or BCRS (the
 * Barycentric International Reference System), which have the same alignment and differ only in the
 * location of their origin. In SuperNOVAS, the origin is determined by the @ref Observer location,
 * while the coordinate system defines only the orientation of the celestial pole. Thus, there is
 * no need to distinguish between these related systems explicitly in SuperNOVAS.
 *
 * @return A reference to a reusable statically allocated ICRS system instance.
 *
 * @sa NOVAS_ICRS, NOVAS_GCRS, NOVAS_SYSTEM_ICRS
 */
const CatalogSystem& CatalogSystem::icrs() {
  return _icrs;
}

static const CatalogSystem _j2000 = CatalogSystem("J2000");

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
const CatalogSystem& CatalogSystem::j2000() {
  return _j2000;
}

static const CatalogSystem _fk4 = CatalogSystem("FK4");

/**
 * The 4th fundamental catalog of stars (FK4). It is essentially the same as B1950, except for
 * the name.
 *
 * @return A reference to a reusable statically allocated FK4 coordinate system instance.
 *
 * @sa b1950()
 */
const CatalogSystem& CatalogSystem::fk4() {
  return _fk4;
}

static const CatalogSystem _fk5 = CatalogSystem("FK5");

/**
 * The 5th fundamental catalog of stars (FK5). It is essentially the same as J2000, except for
 * the name.
 *
 * @return A reference to a reusable statically allocated FK5 coordinate system instance.
 *
 * @sa j2000()
 */
const CatalogSystem& CatalogSystem::fk5() {
  return _fk5;
}

static const CatalogSystem _hip = CatalogSystem("HIP");

/**
 * The system of the mean dynamical equator at the J1991.25 epoch, which is adopted as the nominal
 * mean epoch of the Hipparcos catalog.
 *
 * @return A reference to a reusable statically allocated Hipparcos coordinate system instance.
 *
 * @sa icrs(), mod(), Time::hip() NOVAS_JD_HIP, NOVAS_SYSTEM_HIP
 */
const CatalogSystem& CatalogSystem::hip() {
  return _hip;
}

static const CatalogSystem _b1950 = CatalogSystem("B1950");

/**
 * The system of the dynamical equator at the B1950 epoch (0 UTC, 1 January 1950). This was a
 * commonly used catalog coordinate system of old. It is also known as FK4, since the 4th
 * realization of the fundamental catalog of stars used B1950 also.
 *
 * @return A reference to a reusable statically allocated B1950 coordinate system instance.
 *
 * @sa icrs(), mod(), Time::b1950(), NOVAS_JD_B1950, NOVAS_SYSTEM_B1950
 */
const CatalogSystem& CatalogSystem::b1950() {
  return _b1950;
}

static const CatalogSystem _b1900 = CatalogSystem("B1900");

/**
 * The system of the dynamical equator at the B1900 epoch (0 UTC, 1 January 1900). This was a
 * commonly used catalog coordinate system of old.
 *
 * @return A reference to a reusable statically allocated B1900 coordinate system instance.
 *
 * @sa icrs(), mod(), Time::b1900(), NOVAS_JD_B1900, NOVAS_SYSTEM_B1900
 */
const CatalogSystem& CatalogSystem::b1900() {
  return _b1900;
}

} // namespace supernovas
