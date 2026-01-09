/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cmath>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

/**
 * Instantiates new spherical coordinates with the specified components.
 *
 * @param longitude_rad   [rad] longitude coordinate
 * @param latitude_rad    [rad] latitude coordinate
 * @param distance_m      [m] distance, if needed / known (default: 1 Gpc)
 */
Spherical::Spherical(double longitude_rad, double latitude_rad, double distance_m)
: _lon(longitude_rad), _lat(latitude_rad), _distance(distance_m) {
  static const char *fn = "Spherical";

  if(isnan(longitude_rad))
    novas_set_errno(EINVAL, fn, "input longitude is NAN");
  else if(isnan(latitude_rad))
    novas_set_errno(EINVAL, fn, "input latitude is NAN");
  else if(fabs(latitude_rad) > Constant::halfPi)
    novas_set_errno(EINVAL, fn, "input latitude is outside [-pi:pi] range: %g", latitude_rad);
  else if(isnan(distance_m))
    novas_set_errno(EINVAL, fn, "input distance is NAN");
  else if(distance_m < 0.0)
    novas_set_errno(EINVAL, fn, "input distance is negative: %g", distance_m);
  else
    _valid = true;
}

/**
 * Instantiates new spherical coordinates with the specified components.
 *
 * @param longitude   longitude coordinate
 * @param latitude    latitude coordinate
 * @param distance    distance, if needed / known (default: 1 Gpc)
 */
Spherical::Spherical(const Angle& longitude, const Angle& latitude, const Distance& distance)
: Spherical(longitude.rad(), latitude.rad(), distance.m()) {}


/**
 * Instantiates spherical coordinates with the specified string representations of the longitude
 * and latitude coordinates, optionally specifying a system and a distance if needed. After
 * instantiation, you should check that the resulting coordinates are valid, e.g. as:
 *
 * ```c++
 *   Spherical coords = Spherical(..., ...);
 *   if(!coords.is_valid()) {
 *     // oops, looks like the angles could not be parsed...
 *     return;
 *   }
 * ```
 *
 * @param lon         string representation of the longitude coordinate in DMS or a decimnal
 *                    degrees.
 * @param lat         string representation of the declination coordinate as DMS or decimal
 *                    degrees.
 * @param distance    (optional) the distance, if needed / known (default: 1 Gpc)
 *
 * @sa novas_str_degrees() for details on string representation that can be parsed.
 */
Spherical::Spherical(const std::string& lon, const std::string& lat, const Distance& distance)
: Spherical(Angle(lon), Angle(lat), distance) {}


/**
 * Returns the angular distance of these spherical coordiantes to/from the specified other
 * spherical coordinates.
 *
 * @param other   the reference spherical coordinates
 * @return        the angular distance of these coordinates to/from the argument.
 */
Angle Spherical::distance_to(const Spherical& other) const {
  return Angle(novas_sep(_lon.deg(), _lat.deg(), other._lon.deg(), other._lat.deg()) * Unit::deg);
}

/**
 * Returns the cartesian position vector corresponding to these spherical coordinates.
 *
 * @return    the equivalent rectanguar position vector.
 *
 * @sa Position::as_spherical()
 */
Position Spherical::xyz() const {
  double pos[3];
  double xy = _distance.m() * cos(_lat.rad());

  pos[0] = xy * cos(_lon.rad());
  pos[1] = xy * sin(_lon.rad());
  pos[2] = _distance.m() * sin(_lat.rad());

  return Position(pos);
}

/**
 * Returns the longitude coordinate as an angle.
 *
 * @return    the reference to the longitude coordinate stored internally.
 *
 * @sa latitude(), distance()
 */
const Angle& Spherical::longitude() const {
  return _lon;
}

/**
 * Returns the latitude coordinate as an angle.
 *
 * @return    the reference to the latitude coordinate stored internally.
 *
 * @sa longitude(), distance()
 */
const Angle& Spherical::latitude() const {
  return _lat;
}

/**
 * Returns the radial distance component.
 *
 * @return    the reference to the radial distance coordinate stored internally.
 *
 * @sa longitude(), latitude()
 */
const Distance& Spherical::distance() const {
  return _distance;
}

/**
 * Returns a string representation of these spherical coordinates in DMS format, optionally
 * specifying the type of separator to use and the precision to print.
 *
 * @param separator   (optional) the type of separators to use for the DMS representations
 *                    of the components (default: units and spaces).
 * @param decimals    (optional) the number of decimal places to print for the seconds
 *                    (default: 3)
 * @return    a new string with the human-readable representation of these spherical coordinates.
 */
std::string Spherical::to_string(enum novas_separator_type separator, int decimals) const {
  return _lon.to_string(separator, decimals) + "  " + _lat.to_string(separator, decimals);
}

} // namespace supernovas
