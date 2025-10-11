/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

namespace supernovas {

/**
 * Instantiates a distance (scalar separation along some direction) with the specified value in
 * meters. You may use Unit to convert other distance measures to meters. For example to set a
 * distance of 12.4 parsecs, you might simply write:
 *
 * ```c
 *   Distance d(12.4 * Unit::pc);
 * ```
 *
 * @param meters    [m] The initializing value.
 */
Distance::Distance(double meters) : _meters(meters) {
  if(isnan(meters))
    novas::novas_error(0, EINVAL, "Distance(double)", "input value is NAN");
  else
    _valid = true;
}

/**
 * Returns the absolute value of this distance.
 *
 * @return    the unsigned absolute value of this distance instance, as a distance itself.
 */
Distance Distance::abs() const {
  return Distance(fabs(_meters));
}

/**
 * Returns the distance in meters.
 *
 * @return    [m] the distance in meters.
 */
double Distance::m() const {
  return _meters;
}

/**
 * Returns the distance in kilometers.
 *
 * @return    [km] the distance in kilometers.
 */
double Distance::km() const {
  return 1e-3 * _meters;
}

/**
 * Returns the distance in astronomical units.
 *
 * @return    [AU] the distance in astronomical units.
 */
double Distance::au() const {
  return _meters / Unit::au;
}

/**
 * Returns the distance in lightyears.
 *
 * @return    [lyr] the distance in lightyears.
 */
double Distance::lyr() const {
  return _meters / Unit::lyr;
}

/**
 * Returns the distance in parsecs.
 *
 * @return    [pc] the distance in parsecs.
 */
double Distance::pc() const {
  return _meters / Unit::pc;
}

/**
 * Returns the distance in kiloparsecs.
 *
 * @return    [kpc] the distance in kiloparsecs.
 */
double Distance::kpc() const {
  return _meters / Unit::kpc;
}

/**
 * Returns the distance in megaparsecs.
 *
 * @return    [Mpc] the distance in megaparsecs.
 */
double Distance::Mpc() const {
  return _meters / Unit::Mpc;
}

/**
 * Returns the distance in gigaparsecs.
 *
 * @return    [Gpc] the distance in gigaparsecs.
 */
double Distance::Gpc() const {
  return _meters / Unit::Gpc;
}

/**
 * Returns the parallax angle that corresponds to this distance instance.
 *
 * @return    the parallax angle corresponding to this distance.
 *
 * @sa from_parallax()
 */
Angle Distance::parallax() const {
  return Angle(Unit::arcsec / pc());
}

/**
 * Returns a string representation of this distance using a modest numberof significant figures
 * and a best matched distance unit, e.g. "10.96 km", or 305.6 pc" etc.
 *
 * @return    A human readable string representation of the distance and a unit specifier.
 */
std::string Distance::to_string() const {
  char s[40] = {'\0'};

  if(_meters < 1e4) snprintf(s, sizeof(s), "%.1f m", _meters);
  else if(_meters < 1e9) snprintf(s, sizeof(s), "%.3f km", km());
  else if(_meters < 1000.0 * Unit::au) snprintf(s, sizeof(s), "%.3f AU", au());
  else if(_meters < 1000.0 * Unit::pc) snprintf(s, sizeof(s), "%.3f pc", pc());
  else if(_meters < 1e6 * Unit::pc) snprintf(s, sizeof(s), "%.3f kpc", kpc());
  else snprintf(s, sizeof(s), "%.3f Mpc", Mpc());

  return std::string(s);
}

/**
 * Returns a new distance instance corresponding to a parallax angle.
 *
 * @param parallax      The parallax angle, which defines the distance
 * @return              A new distance instance corresponding to the specified parallax angle
 *
 * @sa parallax()
 */
Distance Distance::from_parallax(const Angle& parallax) {
  return Distance(Unit::pc / (parallax.arcsec()));
}

static const Distance _at_Gpc = Distance(Unit::Gpc);

/**
 * Returns a standard distance of 1 Gpc. Historically NOVAS placed sidereal source at 1 Gpc
 * distance if the distance was not specified otherwise. SuperNOVAS follows that, and so this
 * static method can be used to obtain a persistent reference to a 1 Gpc instance.
 *
 * @return    A reference to a persistent standard 1 Gpc distance instance.
 */
const Distance& Distance::at_Gpc() {
  return _at_Gpc;
}

} // namespace supernovas
