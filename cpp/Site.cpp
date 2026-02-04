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

/**
 * Instantiates a new observing site with the specified geodetic location on the reference ellipsoid
 * of choice.
 *
 * @param longitude_rad   [rad] Observer's geodetic longitude
 * @param latitude_rad    [rad] Observer's geodetic latitude
 * @param altitude_m      [m] (optional) Observers's altitude above sea level (default: 0 m)
 * @param ellipsoid       (optional) reference ellipsoid to use (default: NOVAS_GRS80_ELLIPSOID)
 *
 * @sa from_xyz(), from_GPS()
 */
Site::Site(double longitude_rad, double latitude_rad, double altitude_m, enum novas_reference_ellipsoid ellipsoid) {
  static const char *fn = "Site()";

  if(!isfinite(longitude_rad))
    novas_set_errno(EINVAL, fn, "input longitude is NAN or infinite");
  else if(!isfinite(latitude_rad))
    novas_set_errno(EINVAL, fn, "input latitude is NAN or infinite");
  else if(fabs(latitude_rad) > Constant::half_pi)
    novas_set_errno(EINVAL, fn, "input latitude is outside of [-pi:pi] range: %g", latitude_rad);
  else if(!isfinite(altitude_m))
    novas_set_errno(EINVAL, fn, "input altitude is NAN or infinite");
  else if(altitude_m < -10000.0)
    novas_set_errno(EINVAL, fn, "altitude is more than 10 km below surface: %g m", altitude_m);
  else if(altitude_m > 100000.0)
    novas_set_errno(EINVAL, fn, "altitude is more than 100 km above surface: %g m", altitude_m);
  else if((unsigned) ellipsoid > NOVAS_REFERENCE_ELLIPSOIDS)
    novas_set_errno(EINVAL, fn, "invalid reference ellipsoid: %d", ellipsoid);
  else
    _valid = true;

  make_itrf_site(latitude_rad / Unit::deg, longitude_rad / Unit::deg, altitude_m, &_site);

  if(ellipsoid != NOVAS_GRS80_ELLIPSOID)
    novas_geodetic_transform_site(ellipsoid, &_site, NOVAS_GRS80_ELLIPSOID, &_site);
}

/**
 * Constructs a new observing site with the specified geodetic location on the reference ellipsoid
 * of choice.
 *
 * @param longitude       Observer's geodetic longitude
 * @param latitude        Observer's geodetic latitude
 * @param altitude        (optional) Observers's altitude above sea level (default: 0 m)
 * @param ellipsoid       (optional) reference ellipsoid to use (default: NOVAS_GRS80_ELLIPSOID)
 *
 * @sa from_xyz(), Site::from_GPS()
 */
Site::Site(const Angle& longitude, const Angle& latitude, const Distance& altitude, enum novas::novas_reference_ellipsoid ellipsoid)
: Site(longitude.rad(), latitude.rad(), altitude.m(), ellipsoid) {}

/**
 * Instantiates a new observing site with the specified geocentric position vector.
 *
 * @param xyz   Observers geocentric position vector in rectangular coordinates.
 */
Site::Site(const Position& xyz) {
  static const char *fn = "Site()";

  novas_cartesian_to_geodetic(xyz._array(), NOVAS_GRS80_ELLIPSOID, &_site.longitude, &_site.latitude, &_site.height);

  if(!xyz.is_valid())
    novas_set_errno(EINVAL, fn, "input xyz coordinates have NAN component(s)");
  else if(_site.height < -10000.0)
    novas_set_errno(EINVAL, fn, "altitude is more than 10 km below surface: %g m", _site.height);
  else if(_site.height > 100000.0)
    novas_set_errno(EINVAL, fn, "altitude is more than 100 km above surface: %g m", _site.height);
  else
    _valid = true;
}

/**
 * Constructs a new observing site with the specified geodetic location on the reference ellipsoid
 * of choice.
 *
 * @param longitude       String represenration of observer's geodetic longitude, as DMS or
 *                        decimal degrees.
 * @param latitude        String representation of observer's geodetic latitude, as DMS or decimal
 *                        degrees.
 * @param altitude        (optional) Observers's altitude above sea level (default: 0 m)
 * @param ellipsoid       (optional) reference ellipsoid to use (default: NOVAS_GRS80_ELLIPSOID)
 *
 * @sa from_xyz(), Site::from_GPS()
 */
Site::Site(const std::string& longitude, const std::string& latitude, const Distance& altitude, enum novas::novas_reference_ellipsoid ellipsoid)
: Site(Angle(longitude), Angle(latitude), altitude, ellipsoid) {
}

/**
 * Returns a pointer to the the NOVAS C `on_surface` data structure stored in this site data.
 *
 * @return    The pointer to the NOVAS C `on_surface` data structure.
 *
 * @sa xyz()
 */
const on_surface *Site::_on_surface() const {
  return &_site;
}

/**
 * Returns a new angle containing the geodetic longitude of this site on the GRS80 reference
 * ellipsoid.
 *
 * @return    The geodetic longitude on the GRS80 reference ellipsoid.
 *
 * @sa latitude()
 * @sa altitude()
 */
const Angle Site::longitude() const {
  return Angle(_site.longitude * Unit::deg);
}

/**
 * Returns a new angle containing the geodetic latitude of this site on the GRS80 reference
 * ellipsoid.
 *
 * @return    The geodetic longitude on the GRS80 reference ellipsoid.
 *
 * @sa longitude()
 * @sa altitude()
 */
const Angle Site::latitude() const {
  return Angle(_site.latitude * Unit::deg);
}

/**
 * Returns a new distance containing the altitude of this site above the GRS80 reference
 * ellipsoid (ie, above sea level).
 *
 * @return    The altitude above sea level (GRS80 reference ellipsoid).
 *
 * @sa longitude()
 * @sa latitude()
 */
const Distance Site::altitude() const {
  return Distance(_site.height * Unit::m);
}

/**
 * Checks if this site is the same as another site, within the specified precision.
 *
 * @param site    another site
 * @param tol_m   [m] distance tolerance for equality (default: 1 mm).
 * @return        `true` if the two sites are equal within the tolerance, or else `false`.
 *
 * @sa operator==()
 */
bool Site::equals(const Site& site, double tol_m) const {
  return xyz().equals(site.xyz(), tol_m);
}

/**
 * Checks if this site is the same as another site, within the specified precision.
 *
 * @param site    another site
 * @param tol     distance tolerance for equality (default: 1 mm).
 * @return        `true` if the two sites are equal within the tolerance, or else `false`.
 *
 * @sa operator==()
 */
bool Site::equals(const Site& site, const Distance& tol) const {
  return equals(site, tol.m());
}

/**
 * Checks if this site is the same as another site, within 1 mm.
 *
 * @param site    another site
 * @return        `true` if the two sites are equal within 1 mm, or else `false`.
 *
 * @sa equals(), operator!=()
 */
bool Site::operator==(const Site& site) const {
  return equals(site);
}

/**
 * Checks if this site differs from another site, by more than 1 mm.
 *
 * @param site    another site
 * @return        `true` if the two sites differ by more than 1 mm, or else `false`.
 *
 * @sa operator==()
 */
bool Site::operator!=(const Site& site) const {
  return !(*this == site);
}

/**
 * Returns a new site transformed into a different ITRF realization. The ITRF realizations are
 * defined by a year. While it is best practice to use years with actual ITRF realizations, any
 * year will be interpreted as to pick the last ITRF realization preceding it (or in case of
 * years before thefirst ITRF realization, the initial ITR realizatio will be used).
 *
 * @param from_year   [yr] The original ITRF realization year of this site, e.g. 2008.
 * @param to_year     [yr] The ITRF realization year of the returned new Site, e.g. 2014.
 * @return    a new observing site after transforming between ITRF realizations.
 *
 * @sa EOP::itrf_transformed()
 */
Site Site::itrf_transformed(int from_year, int to_year) const {
  on_surface s = {};
  novas_itrf_transform_site(from_year, &_site, to_year, &s);
  return Site(s.longitude * Unit::deg, s.latitude * Unit::deg, s.height * Unit::m);
}

/**
 * Returns the geocentric position of this site in rectangular coordinates.
 *
 * @return  a new position with the geocentric rectangular coordinates of the site.
 */
Position Site::xyz() const {
  double p[3] = {0.0};
  novas_geodetic_to_cartesian(_site.longitude, _site.latitude, _site.height, NOVAS_GRS80_ELLIPSOID, p);
  return Position(p);
}

/**
 * Returns a string representation of this observing site.
 *
 * @param separator  (optional) the separator to use for the representation of angles (default:
 *                   `novas::NOVAS_SEP_UNITS_AND_SPACES`).
 * @param decimals   (optional) the number of decimal places to print (default: 3).
 * @return   a new string representation of this observing site.
 */
std::string Site::to_string(enum novas_separator_type separator, int decimals) const {
  return "Site: " + Angle(fabs(_site.longitude * Unit::deg)).to_string(separator, decimals) + (_site.longitude < 0 ? "W  " : "E  ") +
          Angle(fabs(_site.latitude * Unit::deg)).to_string(separator, decimals) + (_site.latitude < 0 ? "S  " : "N  ") +
          std::to_string((long) round(_site.height)) + "m";
}

/**
 * Returns an observing site for its geodetic GPS location.
 *
 * @param longitude   [rad] GPS longitude (East positive)
 * @param latitude    [rad] GPS latitude
 * @param altitude    [m] (optional) GPS altitude (default: 0 m)
 * @return    a new observing site at the specified GSP location.
 *
 * @sa Site(), from_xyz()
 */
Site Site::from_GPS(double longitude, double latitude, double altitude) {
  return Site(longitude, latitude, altitude, NOVAS_WGS84_ELLIPSOID);
}

/**
 * Returns an observing site for its geodetic GPS location.
 *
 * @param longitude   GPS longitude angle (East positive)
 * @param latitude    GPS latitude angle
 * @param altitude    (optional) GPS altitude (default: 0 m)
 * @return    a new observing site at the specified GSP location.
 *
 * @sa Site(), from_xyz()
 */
Site Site::from_GPS(const Angle& longitude, const Angle& latitude, const Distance& altitude) {
  return from_GPS(longitude.rad(), latitude.rad(), altitude.m());
}

/**
 * Returns an observing site for its geodetic GPS location, with the longitude and latitude
 * provided in decimal or DMS string representations.
 *
 * @param longitude   string representation of GPS longitude as DSM or decimal degrees.
 * @param latitude    string representation of GPS latitude as DSM or decimal degrees.
 * @param altitude    (optional) GPS altitude (default: 0 m)
 * @return    a new observing site at the specified GSP location.
 *
 * @sa Site(), from_xyz(), Angle()
 */
Site Site::from_GPS(const std::string& longitude, const std::string& latitude, const Distance& altitude) {
  return from_GPS(Angle(longitude), Angle(latitude), altitude);
}

} // namespace supernovas

