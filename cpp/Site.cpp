/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

Site::Site() {}

Site::Site(double longitude_rad, double latitude_rad, double altitude_m, enum novas_reference_ellipsoid ellipsoid) {
  static const char *fn = "Site()";

  if(isnan(longitude_rad))
    novas_error(0, EINVAL, fn, "input longitude is NAN");
  else if(isnan(latitude_rad))
    novas_error(0, EINVAL, fn, "input latitude is NAN");
  else if(fabs(latitude_rad) < Constant::halfPi)
    novas_error(0, EINVAL, fn, "input latitude is outside of [-pi:pi] range: %g", latitude_rad);
  else if(isnan(altitude_m))
    novas_error(0, EINVAL, fn, "input altitude is NAN");
  else if(altitude_m < -10000.0)
    novas_error(0, EINVAL, fn, "altitude is more than 10 km below surface: %g m", altitude_m);
  else if(altitude_m > 100000.0)
    novas_error(0, EINVAL, fn, "altitude is more than 100 km above surface: %g m", altitude_m);
  else
    _valid = true;

  make_itrf_site(latitude_rad / Unit::deg, longitude_rad / Unit::deg, altitude_m, &_site);

  if(ellipsoid != NOVAS_GRS80_ELLIPSOID)
    novas_geodetic_transform_site(ellipsoid, &_site, NOVAS_GRS80_ELLIPSOID, &_site);
}

Site::Site(const Position& xyz) {
  static const char *fn = "Site()";

  novas_cartesian_to_geodetic(xyz._array(), NOVAS_GRS80_ELLIPSOID, &_site.longitude, &_site.latitude, &_site.height);

  if(!xyz.is_valid())
    novas_error(0, EINVAL, fn, "input xyz coordinates have NAN component(s)");
  else if(_site.height < -10000.0)
    novas_error(0, EINVAL, fn, "altitude is more than 10 km below surface: %g m", _site.height);
  else if(_site.height > 100000.0)
    novas_error(0, EINVAL, fn, "altitude is more than 100 km above surface: %g m", _site.height);
  else
    _valid = true;

}

const on_surface *Site::_on_surface() const {
  return &_site;
}

const Angle Site::longitude() const {
  return Angle(_site.longitude * Unit::deg);
}

const Angle Site::latitude() const {
  return Angle(_site.latitude * Unit::deg);
}

const Distance Site::altitude() const {
  return Distance(_site.height * Unit::m);
}

const Position Site::xyz(enum novas_reference_ellipsoid ellipsoid) const {
  double p[3] = {0.0};
  novas_geodetic_to_cartesian(_site.longitude, _site.latitude, _site.height, ellipsoid, p);
  return Position(p);
}

Site Site::itrf_transformed(int from_year, int to_year) const {
  Site site = Site();
  novas_itrf_transform_site(from_year, &_site, to_year, &site._site);
  return site;
}

Position Site::xyz() const {
  double p[3] = {0.0};
  novas_geodetic_to_cartesian(_site.longitude, _site.latitude, _site.height, NOVAS_GRS80_ELLIPSOID, p);
  return Position(p, Unit::au);
}

std::string Site::to_string(enum novas_separator_type separator, int decimals) const {
  return "Site: " + Angle(fabs(_site.longitude * Unit::deg)).to_string(separator, decimals) + (_site.longitude < 0 ? "W  " : "E  ") +
          Angle(fabs(_site.latitude * Unit::deg)).to_string(separator, decimals) + (_site.latitude < 0 ? "S  " : "N  ") +
          std::to_string((long) round(_site.height)) + "m";
}

Site Site::from_xyz(const Position& v) {
  Site site = Site();
  make_xyz_site(v._array(), &site._site);
  return site;
}

Site Site::from_GPS(double longitude, double latitude, double altitude) {
  return Site(longitude, latitude, altitude, NOVAS_WGS84_ELLIPSOID);
}

} // namespace supernovas

