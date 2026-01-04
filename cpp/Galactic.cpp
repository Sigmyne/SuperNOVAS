/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"

using namespace novas;

namespace supernovas {

/**
 * Instantiates new galactic coordinates with the specified components.
 *
 * @param longitude_rad   [rad] galactic longitude coordinate
 * @param latitude_rad    [rad] galactic latitude coordinate
 * @param distance_m      [m] (optional) distance, if needed / known (default: 1 Gpc).
 *
 * @sa Galactic(Angle&, Angle&, Distance&), Galactic(Position&)
 */
Galactic::Galactic(double longitude_rad, double latitude_rad, double distance_m)
: Spherical(longitude_rad, latitude_rad, distance_m) {}

/**
 * Instantiates new galactic coordinates with the specified components.
 *
 * @param longitude   galactic longitude coordinate
 * @param latitude    galactic latitude coordinate
 * @param distance    (optional) distance, if needed / known (default: 1 Gpc).
 *
 * @sa Galactic(double, double, double), Galactic(Position&)
 */
Galactic::Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance)
: Spherical(longitude, latitude, distance) {}

/**
 * Instantiates new galactic coordinates with the specified cartesian position vector.
 *
 * @param pos             _xyz_ position vector in the galactic coordinate system.
 *
 * @sa Galactic(Angle&, Angle&, Distance&), Galactic(Position&), Galactic(double, double, double)
 */
Galactic::Galactic(const Position& pos)
: Spherical(pos.as_spherical()) {}

/**
 * Converts these galactic coordinates to equivalent ICRS equatorial coordinates.
 *
 * @return    the equivalent ICRS equatorial coordinates.
 *
 * @sa Equatorial::as_galactic(), as_ecliptic()
 */
Equatorial Galactic::as_equatorial() const {
  double ra = 0.0, dec = 0.0;
  gal2equ(longitude().deg(), latitude().deg(), &ra, &dec);
  return Equatorial(ra * Unit::hour_angle, dec * Unit::deg, Equinox::icrs(), distance().m());
}

/**
 * Converts these galactic coordinates to equivalent ICRS ecliptic coordinates.
 *
 * @return    the equivalent ICRS ecliptic coordinates.
 *
 * @sa Ecliptic::as_galactic(), as_equatorial()
 */
Ecliptic Galactic::as_ecliptic() const {
  return as_equatorial().as_ecliptic();
}

/**
 * Returns a string representation of these galactic coordinates, optionally with the specified
 * DMS sepatators for the angles, and the desired decimal places for the seconds.
 *
 * @param separator   (optional) the type of separator to use for the DMS representation of angles
 *                    (default: units and spaces).
 * @param decimals    (optional) the number of decimal places to print for the seconds
 *                    (default: 3)
 * @return  the human-readable string representation of these Galactic coordinates.
 */
std::string Galactic::to_string(enum novas_separator_type separator, int decimals) const {
  return "GAL  " + Spherical::to_string(separator, decimals);
}

/**
 * Returns a reference to a statically defined standard invalid galactic coordinates. These invalid
 * coordinates may be used inside any object that is invalid itself.
 *
 * @return    a reference to the static standard invalid coordinates.
 */
const Galactic& Galactic::invalid() {
  static const Galactic _invalid = Galactic(NAN, NAN, NAN);
  return _invalid;
}

} // namespace supernovas

