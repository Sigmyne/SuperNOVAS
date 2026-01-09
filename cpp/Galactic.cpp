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
 */
Galactic::Galactic(double longitude_rad, double latitude_rad, double distance_m)
: Spherical(longitude_rad, latitude_rad, distance_m) {}

/**
 * Instantiates new galactic coordinates with the specified components.
 *
 * @param longitude   galactic longitude coordinate
 * @param latitude    galactic latitude coordinate
 * @param distance    (optional) distance, if needed / known (default: 1 Gpc).
 */
Galactic::Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance)
: Spherical(longitude, latitude, distance) {}

/**
 * Instantiates galactic coordinates with the specified string representations of the longitude
 * and latitude coordinates, optionally specifying a system and a distance if needed. After
 * instantiation, you should check that the resulting coordinates are valid, e.g. as:
 *
 * ```c++
 *   Galactic coords = Galactic(..., ...);
 *   if(!coords.is_valid()) {
 *     // oops, looks like the angles could not be parsed...
 *     return;
 *   }
 * ```
 *
 * @param longitude   string representation of the longitude coordinate in DMS or a decimnal
 *                    degrees.
 * @param latitude    string representation of the declination coordinate as DMS or decimal
 *                    degrees.
 * @param distance    (optional) the distance, if needed / known (default: 1 Gpc)
 *
 * @sa novas_str_degrees() for details on string representation that can be parsed.
 */
Galactic::Galactic(const std::string& longitude, const std::string& latitude, const Distance& distance)
: Galactic(Angle(longitude), Angle(latitude), distance) {}

/**
 * Instantiates new galactic coordinates with the specified cartesian position vector.
 *
 * @param pos             _xyz_ position vector in the galactic coordinate system.
 */
Galactic::Galactic(const Position& pos)
: Spherical(pos.as_spherical()) {}

/**
 * Checks if these galactic coordinates are the same as another, within the specified
 * precision.
 *
 * @param other           the reference galactic coordinates
 * @param precision_rad   [rad] (optional) precision for equality test (default: 1 uas).
 * @return                `true` if these coordinates are the same as the reference within the
 *                        precision, or else `false`.
 *
 * operator==()
 */
bool Galactic::equals(const Galactic& other, double precision_rad) const {
  return Spherical::equals(other, precision_rad);
}

/**
 * Checks if these galactic coordinates are the same as another, within the specified
 * precision.
 *
 * @param other           the reference galactic coordinates
 * @param precision       (optional) precision for equality test (default: 1 uas).
 * @return                `true` if these coordinates are the same as the reference within the
 *                        precision, or else `false`.
 *
 * @sa operator==()
 */
// cppcheck-suppress functionStatic
bool Galactic::equals(const Galactic& other, const Angle& precision) const {
  return equals(other, precision.rad());
}

/**
 * Checks if these galactic coordinates are the same as another, within 1 uas.
 *
 * @param other           the reference galactic coordinates
 * @return                `true` if these coordinates are the same as the reference within 1 uas,
 *                        or else `false`.
 *
 * @sa operator!=()
 */
bool Galactic::operator==(const Galactic& other) const {
  return equals(other);
}

/**
 * Checks if these galactic coordinates differ from another, by more than 1 uas.
 *
 * @param other           the reference galactic coordinates
 * @return                `true` if these coordinates differ from the reference, by more than
 *                        1 uas, or else `false`.
 *
 * @sa operator==()
 */
bool Galactic::operator!=(const Galactic& other) const {
  return !equals(other);
}

/**
 * Returns the angular distance of these galactic coordiantes to/from the specified other
 * galactic coordinates.
 *
 * @param other   the reference galactic coordinates
 * @return        the angular distance of these coordinates to/from the argument.
 */
Angle Galactic::distance_to(const Galactic& other) const {
  return Spherical::distance_to(other);
}

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

