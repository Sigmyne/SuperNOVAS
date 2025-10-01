/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Horizontal: public Spherical {
private:

  int location_with_weather(const Frame& frame, const Weather& weather, on_surface *s) {
    s->temperature = weather.temperature().celsius();
    s->pressure = weather.pressure().mbar();
    s->humidity = weather.humidity();
    return 0;
  }

public:
  Horizontal(double azimuth, double elevation, double distance = NOVAS_DEFAULT_DISTANCE)
  : Spherical(azimuth, elevation, distance) {}

  Horizontal(const Angle& azimuth, const Angle& elevation, double distance = NOVAS_DEFAULT_DISTANCE)
  : Spherical(azimuth, elevation, distance) {}

  Horizontal(const Angle& azimuth, const Angle& elevation, const Distance& distance)
  : Spherical(azimuth, elevation, distance) {}

  const Angle& azimuth() const {
    return longitude();
  }

  const Angle& elevation() const {
    return latitude();
  }

  Horizontal to_refracted(const Frame &frame, RefractionModel ref, const Weather& weather) {
    on_surface loc = {};
    location_with_weather(frame, weather, &loc);
    double del = ref ? ref(frame.time().jd(), &loc, NOVAS_REFRACT_ASTROMETRIC, elevation().deg()) : 0.0;
    return Horizontal(longitude().rad(), latitude().rad() + del * Unit::arcsec);
  }

  Horizontal to_unrefracted(const Frame &frame, RefractionModel ref, const Weather& weather) {
    on_surface loc = {};
    location_with_weather(frame, weather, &loc);
    double del = ref ? ref(frame.time().jd(), &loc, NOVAS_REFRACT_OBSERVED, elevation().deg()) : 0.0;
    return Horizontal(longitude().rad(), latitude().rad() - del * Unit::arcsec);
  }

  Apparent to_apparent(const Frame& frame, double rv = 0.0, double distance = 0.0) const {
    sky_pos p = {};
    novas_hor_to_app(frame._novas_frame(), _lon.deg(), _lat.deg(), NULL, NOVAS_TOD, &p.ra, &p.dec);
    p.rv = rv * Unit::au / Unit::day;
    p.dis = distance * Unit::au;
    radec2vector(p.ra, p.dec, 1.0, p.r_hat);
    return Apparent(frame, &p, NOVAS_TOD);
  }

  Apparent to_apparent(const Frame& frame, Speed& rv, Distance& distance) const {
    return to_apparent(frame, rv.ms(), distance.m());
  }

  const std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const {
    return "HOR  " + Spherical::str(separator, decimals);
  }
};


} // namespace supernovas


