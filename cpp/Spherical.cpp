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

Spherical::Spherical(): Spherical(0.0, 0.0) {}


Spherical::Spherical(double longitude, double latitude, double distance)
: _lon(longitude), _lat(latitude), _distance(distance) {
  static const char *fn = "Spherical";

  if(isnan(longitude))
    novas_error(0, EINVAL, fn, "input longitude is NAN");
  else if(isnan(latitude))
    novas_error(0, EINVAL, fn, "input latitude is NAN");
  else if(fabs(latitude) > Constant::halfPi)
    novas_error(0, EINVAL, fn, "input latitude is outside [-pi:pi] range: %g", latitude);
  else if(isnan(distance))
    novas_error(0, EINVAL, fn, "input distance is NAN");
  else if(distance < 0.0)
    novas_error(0, EINVAL, fn, "input distance is negative: %g", distance);
  else
    _valid = true;
}

Spherical::Spherical(const Angle& longitude, const Angle& latitude, const Distance& distance)
: Spherical(longitude.rad(), latitude.rad(), distance.m()) {}

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

} // namespace supernovas
