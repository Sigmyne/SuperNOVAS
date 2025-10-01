/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cmath>

#include "supernovas.h"


namespace supernovas {

class Spherical {
protected:
  Angle _lon, _lat;
  Distance _distance;

  Spherical(): Spherical(0.0, 0.0) {}

public:
  Spherical(double longitude, double latitude, double distance = NOVAS_DEFAULT_DISTANCE)
  : _lon(longitude), _lat(latitude), _distance(distance) {}

  Spherical(const Angle& longitude, const Angle& latitude, double distance = NOVAS_DEFAULT_DISTANCE)
  : _lon(longitude), _lat(latitude), _distance(distance) {}

  Spherical(const Angle& longitude, const Angle& latitude, const Distance distance)
  : _lon(longitude), _lat(latitude), _distance(distance) {}


  Position xyz() const {
    double pos[3];
    double xy = cos(_lat.rad());

    pos[0] = xy * cos(_lon.rad());
    pos[1] = xy * sin(_lon.rad());
    pos[2] = sin(_lat.rad());

    return Position(pos);
  }

  const Angle& longitude() const {
    return _lon;
  }

  const Angle& latitude() const {
    return _lat;
  }

  const Distance& distance() const {
    return _distance;
  }

  const std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const {
    return _lon.str(separator, decimals) + "  " + _lat.str(separator, decimals);
  }
};


} // namespace supernovas
