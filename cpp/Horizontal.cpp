/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"

using namespace supernovas;
using namespace novas;


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

Apparent Horizontal::to_apparent(const Frame& frame, double rv, double distance) const {
  sky_pos p = {};
  novas_hor_to_app(frame._novas_frame(), _lon.deg(), _lat.deg(), NULL, NOVAS_TOD, &p.ra, &p.dec);
  p.rv = rv / (Unit::au / Unit::day);
  p.dis = distance / Unit::au;
  radec2vector(p.ra, p.dec, 1.0, p.r_hat);
  return Apparent(frame, &p, NOVAS_TOD);
}

Apparent Horizontal::to_apparent(const Frame& frame, const Speed& rv, const Distance& distance) const {
  return to_apparent(frame, rv.ms(), distance.m());
}

const std::string Horizontal::str(enum novas_separator_type separator, int decimals) const {
  return "HOR  " + Spherical::str(separator, decimals);
}



