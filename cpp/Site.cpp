/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"


using namespace supernovas;


Site::Site() {}

Site::Site(double longitude, double latitude, double altitude, enum novas_reference_ellipsoid ellipsoid) {
  make_itrf_site(latitude / Unit::deg, longitude / Unit::deg, altitude, &_site);

  if(ellipsoid != NOVAS_GRS80_ELLIPSOID)
    novas_geodetic_transform_site(ellipsoid, &_site, NOVAS_GRS80_ELLIPSOID, &_site);
}

Site::Site(const Position& xyz) {
  novas_cartesian_to_geodetic(xyz._array(), NOVAS_GRS80_ELLIPSOID, &_site.longitude, &_site.latitude, &_site.height);
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

std::string Site::str(enum novas_separator_type separator, int decimals) const {
  return "Site: " + Angle(fabs(_site.longitude * Unit::deg)).str(separator, decimals) + (_site.longitude < 0 ? "W  " : "E  ") +
          Angle(fabs(_site.latitude * Unit::deg)).str(separator, decimals) + (_site.latitude < 0 ? "S  " : "N  ") +
          std::to_string((long) round(_site.height)) + "m";
}

Site Site::from_xyz(double x, double y, double z) {
  Position xyz(x, y, z);
  return from_xyz(xyz);
}

Site Site::from_xyz(const Position& v) {
  Site site = Site();
  make_xyz_site(v._array(), &site._site);
  return site;
}

Site Site::from_GPS(double longitude, double latitude, double altitude) {
  return Site(longitude, latitude, altitude, NOVAS_WGS84_ELLIPSOID);
}



