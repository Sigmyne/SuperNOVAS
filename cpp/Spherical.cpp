/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cmath>

#include "supernovas.h"


using namespace supernovas;


Spherical::Spherical(): Spherical(0.0, 0.0) {}


Spherical::Spherical(double longitude, double latitude, double distance)
: _lon(longitude), _lat(latitude), _distance(distance) {}

Spherical::Spherical(const Angle& longitude, const Angle& latitude, const Distance& distance)
: _lon(longitude), _lat(latitude), _distance(distance) {}


Position Spherical::xyz() const {
  double pos[3];
  double xy = cos(_lat.rad());

  pos[0] = xy * cos(_lon.rad());
  pos[1] = xy * sin(_lon.rad());
  pos[2] = sin(_lat.rad());

  return Position(pos);
}

const Angle& Spherical::longitude() const {
  return _lon;
}

const Angle& Spherical::latitude() const {
  return _lat;
}

const Distance& Spherical::distance() const {
  return _distance;
}

const std::string Spherical::str(enum novas_separator_type separator, int decimals) const {
  return _lon.str(separator, decimals) + "  " + _lat.str(separator, decimals);
}

