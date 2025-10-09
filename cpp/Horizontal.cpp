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

int Horizontal::location_with_weather(const Frame& frame, const Weather& weather, on_surface *s) {
  s->temperature = weather.temperature().celsius();
  s->pressure = weather.pressure().mbar();
  s->humidity = weather.humidity();
  return 0;
}


Horizontal::Horizontal(double azimuth, double elevation, double distance)
: Spherical(azimuth, elevation, distance) {}

Horizontal::Horizontal(const Angle& azimuth, const Angle& elevation, const Distance& distance)
: Spherical(azimuth, elevation, distance) {}

const Angle& Horizontal::azimuth() const {
  return longitude();
}

const Angle& Horizontal::elevation() const {
  return latitude();
}

const Angle Horizontal::zenith_angle() const {
  return Angle(Constant::halfPi - latitude().rad());
}

Horizontal Horizontal::to_refracted(const Frame &frame, RefractionModel ref, const Weather& weather) {
  on_surface loc = {};
  location_with_weather(frame, weather, &loc);
  double del = ref ? ref(frame.time().jd(), &loc, NOVAS_REFRACT_ASTROMETRIC, elevation().deg()) : 0.0;
  return Horizontal(longitude().rad(), latitude().rad() + del * Unit::arcsec);
}

Horizontal Horizontal::to_unrefracted(const Frame &frame, RefractionModel ref, const Weather& weather) {
  on_surface loc = {};
  location_with_weather(frame, weather, &loc);
  double del = ref ? ref(frame.time().jd(), &loc, NOVAS_REFRACT_OBSERVED, elevation().deg()) : 0.0;
  return Horizontal(longitude().rad(), latitude().rad() - del * Unit::arcsec);
}

std::optional<Apparent> Horizontal::to_apparent(const Frame& frame, double rv, double distance) const {
  static const char *fn = "Horizontal::to_apparent";

  if(!frame.observer().is_geodetic()) {
    novas_error(0, EINVAL, fn, "cannot convert for non-geodetic observer frame");
    return std::nullopt;
  }

  sky_pos p = {};
  if(novas_hor_to_app(frame._novas_frame(), _lon.deg(), _lat.deg(), NULL, NOVAS_TOD, &p.ra, &p.dec) != 0) {
    novas_trace_invalid(fn);
    return std::nullopt;
  }

  p.rv = rv / (Unit::au / Unit::day);
  p.dis = distance / Unit::au;
  radec2vector(p.ra, p.dec, 1.0, p.r_hat);
  return Apparent::from_sky_pos(p, frame, NOVAS_TOD);
}

std::optional<Apparent> Horizontal::to_apparent(const Frame& frame, const Speed& rv, const Distance& distance) const {
  return to_apparent(frame, rv.m_per_s(), distance.m());
}

const std::string Horizontal::str(enum novas_separator_type separator, int decimals) const {
  return "HOR  " + Spherical::str(separator, decimals);
}



static const Horizontal _invalid = Horizontal(NAN, NAN, NAN);
const Horizontal& Horizontal::invalid() {
  return _invalid;
}

} // namespace supernovas

