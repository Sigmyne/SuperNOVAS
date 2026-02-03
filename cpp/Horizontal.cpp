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

using namespace novas;

namespace supernovas {

static void use_weather(const Weather& weather, on_surface *s) {
  s->temperature = weather.temperature().celsius();
  s->pressure = weather.pressure().mbar();
  s->humidity = weather.humidity();
}

/**
 * Instantiates new horizontal (Az/El) coordinates with the specified components
 *
 * @param azimuth     [rad] azimuth angle
 * @param elevation   [rad] elevation angle
 */
Horizontal::Horizontal(double azimuth, double elevation)
: Spherical(azimuth, elevation) {}

/**
 * Instantiates new horizontal (Az/El) coordinates with the specified components
 *
 * @param azimuth     azimuth angle
 * @param elevation   elevation angle
 */
Horizontal::Horizontal(const Angle& azimuth, const Angle& elevation)
: Spherical(azimuth, elevation) {}

/**
 * Instantiates horizontal coordinates with the specified string representations of the azimuth
 * and elevation coordinates, optionally specifying a system and a distance if needed. After
 * instantiation, you should check that the resulting coordinates are valid, e.g. as:
 *
 * ```c++
 *   Horizontal coords = Horizontal(..., ...);
 *   if(!coords.is_valid()) {
 *     // oops, looks like the angles could not be parsed...
 *     return;
 *   }
 * ```
 *
 * @param azimuth     string representation of the azimuth coordinate in DMS or a decimnal
 *                    degrees.
 * @param elevation   string representation of the elevation coordinate as DMS or decimal
 *                    degrees.
 *
 * @sa novas_str_degrees() for details on string representation that can be parsed.
 */
Horizontal::Horizontal(const std::string& azimuth, const std::string& elevation)
: Horizontal(Angle(azimuth), Angle(elevation)) {}

/**
 * Returns the azimuth angle. Same as Spherical::longitude().
 *
 * @return  the reference to the azimuth angle stored internally.
 *
 * @sa longitude(), elevation(), zenith_angle()
 */
const Angle& Horizontal::azimuth() const {
  return longitude();
}

/**
 * Returns the elevation angle. Same as Spherical::latitude().
 *
 * @return  the reference to the elevation angle stored internally.
 *
 * @sa latitude(), zenith_angle(), azimuth()
 */
const Angle& Horizontal::elevation() const {
  return latitude();
}

/**
 * Returns the zenith distance angle. It is 90&deg; - elevation.
 *
 * @return  the zenith angle.
 *
 * @sa elevation(), azimuth()
 */
const Angle Horizontal::zenith_angle() const {
  return Angle(Constant::half_pi - latitude().rad());
}

/**
 * Checks if these horizontal coordinates are the same as another, within the specified
 * precision.
 *
 * @param other           the reference horizontal coordinates
 * @param precision_rad   [rad] (optional) precision for equality test (default: 1 &mu;as).
 * @return                `true` if these coordinates are the same as the reference within the
 *                        precision, or else `false`.
 *
 * @sa operator==()
 */
bool Horizontal::equals(const Horizontal& other, double precision_rad) const {
  return Spherical::equals(other, precision_rad);
}

/**
 * Checks if these horizontal coordinates are the same as another, within the specified
 * precision.
 *
 * @param other           the reference horizontal coordinates
 * @param precision       (optional) precision for equality test (default: 1 &mu;as).
 * @return                `true` if these coordinates are the same as the reference within the
 *                        precision, or else `false`.
 *
 * @sa operator==()
 */
// cppcheck-suppress functionStatic
bool Horizontal::equals(const Horizontal& other, const Angle& precision) const {
  return equals(other, precision.rad());
}

/**
 * Checks if these horizontal coordinates are the same as another, within 1 &mu;as.
 *
 * @param other           the reference horizontal coordinates
 * @return                `true` if these coordinates are the same as the reference within 1 &mu;as,
 *                        or else `false`.
 *
 * @sa operator!=()
 */
bool Horizontal::operator==(const Horizontal& other) const {
  return equals(other);
}

/**
 * Checks if these horizontal coordinates differ from another, by more than 1 uas.
 *
 * @param other           the reference horizontal coordinates
 * @return                `true` if these coordinates differ from the reference, by more than
 *                        1 &mu;as, or else `false`.
 *
 * @sa operator==()
 */
bool Horizontal::operator!=(const Horizontal& other) const {
  return !equals(other);
}

/**
 * Returns the angular distance of these horizontal coordiantes to/from the specified other
 * horizontal coordinates.
 *
 * @param other   the reference horizontal coordinates
 * @return        the angular distance of these coordinates to/from the argument.
 */
Angle Horizontal::distance_to(const Horizontal& other) const {
  return Spherical::distance_to(other);
}

/**
 * Applies atmospheric refraction correction for these coordinates, returning the result.
 *
 * @param frame     an Earth-based observing frame, defining the time of observation and the
 *                  observer location, above (or slightly below) Earth's surface.
 * @param ref       refraction model to use, or NULL to skip refraction correction
 * @param weather   local weather parameters to use for the refraction correction.
 * @return          refracted horizontal coordinates.
 *
 * @sa to_unrefracted()
 */
Horizontal Horizontal::to_refracted(const Frame &frame, RefractionModel ref, const Weather& weather) {
  on_surface loc = {};
  use_weather(weather, &loc);
  double del = ref ? ref(frame.time().jd(), &loc, NOVAS_REFRACT_ASTROMETRIC, elevation().deg()) : 0.0;
  return Horizontal(longitude().rad(), latitude().rad() + del * Unit::arcsec);
}

/**
 * Undoes atmospheric refraction correction for these coordinates, returning the result.
 *
 * @param frame     an Earth-based observing frame, defining the time of observation and the
 *                  observer location, above (or slightly below) Earth's surface.
 * @param ref       refraction model to use, or NULL to skip refraction correction
 * @param weather   local weather parameters to use for the refraction correction.
 * @return          unrefracted (astrometric) horizontal coordinates.
 *
 * @sa to_refracted()
 */
Horizontal Horizontal::to_unrefracted(const Frame &frame, RefractionModel ref, const Weather& weather) {
  on_surface loc = {};
  use_weather(weather, &loc);
  double del = ref ? ref(frame.time().jd(), &loc, NOVAS_REFRACT_OBSERVED, elevation().deg()) : 0.0;
  return Horizontal(longitude().rad(), latitude().rad() - del * Unit::arcsec);
}

/**
 * Converts these horizontal coordinates to an apparent place on the sky. Typically you should call this
 * on unrefracted (astrometric) horizontal coordinates. If starting with observed (refracted)
 * coordinates you should call to_unrefracted() first, before calling this function.
 *
 * @param frame     an Earth-based observing frame, defining the time of observation and the
 *                  observer location, above (or slightly below) Earth's surface.
 * @param rv        [m/s] (optional) observed radial velocity, if any (default: 0.0).
 * @param distance  [m] (optional) apparent distance at which the observed light originated.
 * @return          the apparent equatorial place corresponding to these astrometric horizontal
 *                  coordinates on the sky.
 *
 * @sa to_unrefracted(), Apparent::horizontal()
 */
std::optional<Apparent> Horizontal::to_apparent(const Frame& frame, double rv, double distance) const {
  static const char *fn = "Horizontal::to_apparent";

  if(!frame.observer().is_geodetic()) {
    novas_set_errno(EINVAL, fn, "cannot convert for non-geodetic observer frame");
    return std::nullopt;
  }

  sky_pos p = {};
  if(novas_hor_to_app(frame._novas_frame(), longitude().deg(), latitude().deg(), NULL, NOVAS_TOD, &p.ra, &p.dec) != 0) {
    novas_trace_invalid(fn);
    return std::nullopt;
  }

  p.rv = rv / (Unit::au / Unit::day);
  p.dis = distance / Unit::au;
  radec2vector(p.ra, p.dec, 1.0, p.r_hat);
  return Apparent::from_tod_sky_pos(p, frame);
}

/**
 * Converts these horizontal coordinates to an apparent place on the sky. Typically you should
 * call this on unrefracted (astrometric) horizontal coordinates. If starting with observed
 * (refracted) coordinates you should call to_unrefracted() first, before calling this function.
 *
 * @param frame     an Earth-based observing frame, defining the time of observation and the
 *                  observer location, above (or slightly below) Earth's surface.
 * @param rv        (optional) observed radial velocity, if any (default: 0.0).
 * @param distance  (optional) apparent distance at which the observed light originated.
 * @return          the apparent equatorial place corresponding to these astrometric horizontal
 *                  coordinates on the sky.
 *
 * @sa to_unrefracted(), Apparent::to_horizontal()
 */
std::optional<Apparent> Horizontal::to_apparent(const Frame& frame, const Speed& rv, const Distance& distance) const {
  return to_apparent(frame, rv.m_per_s(), distance.m());
}

/**
 * Returns a string representation of these horizontal coordinates, optionally defining the separator type
 * to use for the DMS angle representations, and the number of decimal places to print also.
 *
 * @param separator   the DMS separator type (default: units and spaces).
 * @param decimals    the number of decimal places to print for the seconds (default: 3).
 * @return            a new string with the human-readable representation of these horizontal coordinates.
 */
std::string Horizontal::to_string(enum novas_separator_type separator, int decimals) const {
  return "HOR  " + Spherical::to_string(separator, decimals);
}

/**
 * Returns a reference to a statically defined standard invalid horizontal coordinates. These invalid
 * coordinates may be used inside any object that is invalid itself.
 *
 * @return    a reference to the static standard invalid coordinates.
 */
const Horizontal& Horizontal::invalid() {
  static const Horizontal _invalid = Horizontal(NAN, NAN);
  return _invalid;
}

} // namespace supernovas

