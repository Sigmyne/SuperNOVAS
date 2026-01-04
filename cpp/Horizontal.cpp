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
 * @param distance    [m] (optional) distance if needed / known (default: 1 Gpc)
 *
 * @sa Horizontal(Angle&, Angle&, Distance&)
 */
Horizontal::Horizontal(double azimuth, double elevation, double distance)
: Spherical(azimuth, elevation, distance) {}

/**
 * Instantiates new horizontal (Az/El) coordinates with the specified components
 *
 * @param azimuth     azimuth angle
 * @param elevation   elevation angle
 * @param distance    (optional) distance if needed / known (default: 1 Gpc)
 *
 * @sa Horizontal(double, double, double)
 */
Horizontal::Horizontal(const Angle& azimuth, const Angle& elevation, const Distance& distance)
: Spherical(azimuth, elevation, distance) {}

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
  return Angle(Constant::halfPi - latitude().rad());
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
 * on unrefracted (astrometric) horizontal coordinates. If starting with observed (refracted) coordinates
 * you should call to_unrefracted() first, before calling this function.
 *
 * @param frame   an Earth-based observing frame, defining the time of observation and the
 *                observer location, above (or slightly below) Earth's surface.
 * @param rv      [m/s] (optional) observed radial velocity, if any (default: 0.0).
 * @return        the apparent equatorial place corresponding to these astrometric horizontal coordinates
 *                on the sky.
 *
 * @sa to_apparent(Frame&, Speed&), to_unrefracted()
 */
std::optional<Apparent> Horizontal::to_apparent(const Frame& frame, double rv) const {
  static const char *fn = "Horizontal::to_apparent";

  if(!frame.observer().is_geodetic()) {
    novas_error(0, EINVAL, fn, "cannot convert for non-geodetic observer frame");
    return std::nullopt;
  }

  sky_pos p = {};
  if(novas_hor_to_app(frame._novas_frame(), longitude().deg(), latitude().deg(), NULL, NOVAS_TOD, &p.ra, &p.dec) != 0) {
    novas_trace_invalid(fn);
    return std::nullopt;
  }

  p.rv = rv / (Unit::au / Unit::day);
  p.dis = distance().au();
  radec2vector(p.ra, p.dec, 1.0, p.r_hat);
  return Apparent::from_tod_sky_pos(p, frame);
}

/**
 * Converts these horizontal coordinates to an apparent place on the sky. Typically you should call this
 * on unrefracted (astrometric) horizontal coordinates. If starting with observed (refracted) coordinates
 * you should call to_unrefracted() first, before calling this function.
 *
 * @param frame   an Earth-based observing frame, defining the time of observation and the
 *                observer location, above (or slightly below) Earth's surface.
 * @param rv      (optional) observed radial velocity, if any (default: 0.0).
 * @return        the apparent equatorial place corresponding to these astrometric horizontal coordinates
 *                on the sky.
 *
 * @sa to_apparent(Frame&, double), to_unrefracted()
 */
std::optional<Apparent> Horizontal::to_apparent(const Frame& frame, const Speed& rv) const {
  return to_apparent(frame, rv.m_per_s());
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
  static const Horizontal _invalid = Horizontal(NAN, NAN, NAN);
  return _invalid;
}

} // namespace supernovas

