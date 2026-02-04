/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

using namespace novas;

int main() {
  TestUtil test = TestUtil("Site");

  int n = 0;
 
  if(!test.check("invalid (lon = NAN)", !Site(Angle(NAN), Angle(1.0), Distance(60.0)).is_valid())) n++;
  if(!test.check("invalid (lat = NAN)", !Site(Angle(-2.0), Angle(NAN), Distance(60.0)).is_valid())) n++;
  if(!test.check("invalid (lat < -90)", !Site(Angle(-2.0), Angle(-90.1 * Unit::deg), Distance(60.0)).is_valid())) n++;
  if(!test.check("invalid (lat > 90)", !Site(Angle(-2.0), Angle(90.1 * Unit::deg), Distance(60.0)).is_valid())) n++;
  if(!test.check("invalid (alt = NAN)", !Site(Angle(-2.0), Angle(1.0), Distance(NAN)).is_valid())) n++;
  if(!test.check("invalid (alt < -10km)", !Site(Angle(-2.0), Angle(1.0), Distance(-10.1 * Unit::km)).is_valid())) n++;
  if(!test.check("invalid (alt > 100km)", !Site(Angle(-2.0), Angle(1.0), Distance(100.1 * Unit::km)).is_valid())) n++;
  if(!test.check("invalid (ellipsoid = -1)", !Site(Angle(-2.0), Angle(1.0), Distance(60.0), (enum novas_reference_ellipsoid) -1).is_valid())) n++;

  double p[3] = {NOVAS_EARTH_RADIUS, 0.0, NAN};
  if(!test.check("invalid (xyz has NAN)", !Site(Position(p)).is_valid())) n++;

  p[2] = 0.0;
  p[0] = NOVAS_EARTH_RADIUS - 10.1 * Unit::km;
  if(!test.check("invalid (xyz below)", !Site(Position(p)).is_valid())) n++;

  p[0] = NOVAS_EARTH_RADIUS + 100.1 * Unit::km;
  if(!test.check("invalid (xyz above)", !Site(Position(p)).is_valid())) n++;

  Site a(Angle(-120.5 * Unit::deg), Angle(-75.25 * Unit::deg), Distance(60.0));
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("longitude()", a.longitude().deg(), -120.5, 1e-13)) n++;
  if(!test.equals("latitude()", a.latitude().deg(), -75.25, 1e-14)) n++;
  if(!test.equals("altitude()", a.altitude().m(), 60.0, 1e-14)) n++;

  double xyz[3] = {0.0};
  novas_geodetic_to_cartesian(a.longitude().deg(), a.latitude().deg(), a.altitude().m(), NOVAS_GRS80_ELLIPSOID, xyz);
  if(!test.check("xyz()", a.xyz() == Position(xyz))) n++;
  if(!test.check("equals()", Site(Position(xyz)) == a)) n++;
  if(!test.check("equals(Distance&)", Site(Position(xyz)).equals(a, Distance(Unit::mm)))) n++;
  if(!test.check("operator==()", Site(Position(xyz)) == a)) n++;
  if(!test.check("operator!=()", !(Site(Position(xyz)) != a))) n++;

  Site a1("W 120d 30m 00", "-75:15:00", Distance(60.0));
  if(!test.check("operator==(ITRS string)", a1 == a)) n++;

  novas_itrf_transform(2015, xyz, NULL, 1988, xyz, NULL);
  if(!test.check("itrf_transformed", a.itrf_transformed(2015, 1988) == Site(Position(xyz)))) n++;
  if(!test.check("ITRF88 != ITRF2015", a.itrf_transformed(2015, 1988) != a)) n++;

  Site b(Angle(-120.5 * Unit::deg), Angle(-75.25 * Unit::deg), Distance(60.0), NOVAS_WGS84_ELLIPSOID);
  if(!test.check("from_GPS()", Site::from_GPS(Angle(-120.5 * Unit::deg), Angle(-75.25 * Unit::deg), Distance(60.0)) == b)) n++;
  if(!test.check("GPS != ITRF", !b.equals(a, 0.1 * Unit::mm))) n++;

  Site b1 = Site::from_GPS("120 30 00 W", "75 15 00 S", Distance(60.0));
  if(!test.check("operator==(GPS string)", b1 == b)) n++;




  std::cout << "Site.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
