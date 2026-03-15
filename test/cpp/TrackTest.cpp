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
  TestUtil test = TestUtil("ScalarEvolution");

  int n = 0;


  if(!test.check("invalid(pos)", !ScalarEvolution(NAN, -2.0, 3.0).is_valid())) n++;
  if(!test.check("invalid(vel)", !ScalarEvolution(1.0, NAN, 3.0).is_valid())) n++;
  if(!test.check("invalid(acc)", !ScalarEvolution(1.0, -2.0, NAN).is_valid())) n++;

  ScalarEvolution e(1.0, -2.0, 3.0);
  if(!test.check("is_valid()", e.is_valid())) n++;
  if(!test.equals("value(0.0)", e.value(), 1.0)) n++;
  if(!test.equals("rate(0.0)", e.rate(), -2.0)) n++;
  if(!test.equals("acceleration(0.0)", e.acceleration(), 3.0)) n++;
  if(!test.equals("value(1.0)", e.value(Interval(1.0)), 2.0)) n++;
  if(!test.equals("rate(1.0)", e.rate(Interval(1.0)), 1.0)) n++;
  if(!test.equals("value(-1.0)", e.value(Interval(-1.0)), 6.0)) n++;
  if(!test.equals("rate(-1.0)", e.rate(Interval(-1.0)), -5.0)) n++;

  e = ScalarEvolution::stationary(1.23);
  if(!test.check("stationary()", e.is_valid())) n++;
  if(!test.equals("value(stationary)", e.value(Interval(1.0)), 1.23)) n++;
  if(!test.equals("rate(stationary)", e.rate(Interval(1.0)), 0.0)) n++;
  if(!test.equals("acceleration(stationary)", e.acceleration(), 0.0)) n++;


  // =========================================================================

  test = TestUtil("HorizontalTrack");

  ScalarEvolution xe(NAN);
  ScalarEvolution lon(1.0, 2.0), lat(-1.1, 2.1), r(1.2, -2.2);

  if(!test.check("invalid(time)", !HorizontalTrack(Time::undefined(), Interval(1.0), lon, lat, r).is_valid())) n++;
  if(!test.check("invalid(range)", !HorizontalTrack(Time::j2000(), Interval(NAN), lon, lat, r).is_valid())) n++;
  if(!test.check("invalid(lon)", !HorizontalTrack(Time::j2000(), Interval(1.0), xe, lat, r).is_valid())) n++;
  if(!test.check("invalid(lat)", !HorizontalTrack(Time::j2000(), Interval(1.0), lon, xe, r).is_valid())) n++;
  if(!test.check("invalid(r)", !HorizontalTrack(Time::j2000(), Interval(1.0), lon, lat, xe).is_valid())) n++;

  Interval ip(1.0 / Constant::pi);
  Time t1 = Time::j2000() + ip;

  HorizontalTrack h(Time::j2000(), Interval(1.0), lon, lat, r);
  if(!test.check("is_valid()", h.is_valid())) n++;
  if(!test.check("reference_time()", h.reference_time() == Time::j2000())) n++;
  if(!test.check("range()", h.range() == Interval(1.0))) n++;
  if(!test.check("is_valid_at(OK)", h.is_valid_at(t1))) n++;
  if(!test.check("is_valid_at(!)", !h.is_valid_at(t1 + 1.0))) n++;

  if(!test.equals("longitude_evolution()", h.longitude_evolution().value(ip), lon.value(ip), 1e-15)) n++;
  if(!test.equals("latitude_evolution()", h.latitude_evolution().value(ip), lat.value(ip), 1e-15)) n++;
  if(!test.equals("distance_evolution()", h.distance_evolution().value(ip), r.value(ip), 1e-15)) n++;

  std::optional<Angle> oa = h.longitude_at(t1);
  if(!test.check("longitude_at()", oa.has_value())) n++;
  if(!test.equals("longitude_at().value()", oa.value().rad(), lon.value(ip), 1e-15)) n++;

  oa = h.latitude_at(t1);
  if(!test.check("latitude_at()", oa.has_value())) n++;
  if(!test.equals("latitude_at().value()", oa.value().rad(), lat.value(ip), 1e-15)) n++;

  std::optional<Coordinate> oc = h.distance_at(t1);
  if(!test.check("distance_at()", oc.has_value())) n++;
  if(!test.equals("distance_at().value()", oc.value().m(), r.value(ip), 1e-15)) n++;

  std::optional<ScalarVelocity> ov = h.radial_velocity_at(t1);
  if(!test.check("radial_velocity_at()", ov.has_value())) n++;
  if(!test.equals("radial_velocity_at().value()", ov.value().m_per_s(), r.rate(ip), 1e-6)) n++;

  if(!test.check("redshift_at()", h.redshift_at(t1).has_value())) n++;
  if(!test.equals("redshift_at().value()", h.redshift_at(t1).value(), novas_v2z(r.rate(ip) / (Unit::km / Unit::s)), 1e-15)) n++;

  std::optional<Horizontal> oh = h.projected_at(t1);
  if(!test.check("projected_at()", oh.has_value())) n++;
  if(!test.equals("projected_at().azimuth()", oh.value().azimuth().rad(), lon.value(ip), 1e-15)) n++;
  if(!test.equals("projected_at().elevation()", oh.value().elevation().rad(), lat.value(ip), 1e-15)) n++;

  ScalarEvolution z(0.1, 0.001);
  h = HorizontalTrack(Time::j2000(), Interval(1.0), lon, lat, r, z);
  if(!test.check("is_valid(z)", h.is_valid())) n++;
  if(!test.equals("redshift_evolution()", h.redshift_evolution().value(ip), z.value(ip), 1e-15)) n++;

  novas_track tr0 = {};
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32.0, 0.0, &tr0.time);

  tr0.pos.lon = lon.value(Interval::zero()) / Unit::deg;
  tr0.rate.lon = lon.rate(Interval::zero()) / Unit::deg;
  tr0.accel.lon = lon.acceleration() / Unit::deg;

  tr0.pos.lat = lat.value(Interval::zero()) / Unit::deg;
  tr0.rate.lat = lat.rate(Interval::zero()) / Unit::deg;
  tr0.accel.lat = lat.acceleration() / Unit::deg;

  tr0.pos.dist = r.value(Interval::zero()) / Unit::AU;
  tr0.rate.dist = r.rate(Interval::zero()) / Unit::AU;
  tr0.accel.dist = r.acceleration() / Unit::AU;

  tr0.pos.z = NAN;

  novas_track trx = tr0; trx.pos.lon = NAN;
  if(!test.check("from_novas_track(pos0 = NAN)", !HorizontalTrack::from_novas_track(&trx, Interval(1.0)).has_value())) n++;

  trx = tr0; trx.rate.lon = NAN;
  if(!test.check("from_novas_track(pos1 = NAN)", !HorizontalTrack::from_novas_track(&trx, Interval(1.0)).has_value())) n++;

  trx = tr0; trx.accel.lon = NAN;
  if(!test.check("from_novas_track(pos2 = NAN)", !HorizontalTrack::from_novas_track(&trx, Interval(1.0)).has_value())) n++;

  std::optional<HorizontalTrack> oht = HorizontalTrack::from_novas_track(NULL, Interval(1.0));
  if(!test.check("from_novas_track(NULL)", !oht.has_value())) n++;


  oht = HorizontalTrack::from_novas_track(&tr0, Interval(1.0));
  if(!test.check("from_novas_track()", oht.has_value())) n++;

  h = oht.value();
  if(!test.check("is_valid(tr)", h.is_valid())) n++;


  oa = h.longitude_at(t1);
  if(!test.check("longitude_at(tr)", oa.has_value())) n++;
  if(!test.equals("longitude_at(tr).value()", oa.value().rad(), lon.value(ip), 1e-15)) n++;

  oa = h.latitude_at(t1);
  if(!test.check("latitude_at(tr)", oa.has_value())) n++;
  if(!test.equals("latitude_at(tr).value()", oa.value().rad(), lat.value(ip), 1e-15)) n++;

  oc = h.distance_at(t1);
  if(!test.check("distance_at(tr)", oc.has_value())) n++;
  if(!test.equals("distance_at(tr).value()", oc.value().m(), r.value(ip), 1e-15)) n++;

  ov = h.radial_velocity_at(t1);
  if(!test.check("radial_velocity_at(tr)", ov.has_value())) n++;
  if(!test.equals("radial_velocity_at(tr).value()", ov.value().m_per_s(), r.rate(ip), 1e-15)) n++;

  Time t2 = Time::j2000() - Interval(1.1);
  if(!test.check("longitude_at(range)", !h.longitude_at(t2).has_value())) n++;
  if(!test.check("latitude_at(range)", !h.latitude_at(t2).has_value())) n++;
  if(!test.check("distance_at(range)", !h.distance_at(t2).has_value())) n++;
  if(!test.check("radial_velocity_at(range)", !h.radial_velocity_at(t2).has_value())) n++;
  if(!test.check("redshift_at(range)", !h.redshift_at(t2).has_value())) n++;
  if(!test.check("projected_at(range)", !h.projected_at(t2).has_value())) n++;


  // =========================================================================

  test = TestUtil("EquatorialTrack");

  EquatorialTrack et(Equinox::icrs(), Time::j2000(), Interval(1.0), lon, lat, r);
  if(!test.check("is_valid()", et.is_valid())) n++;
  if(!test.check("reference_time()", et.reference_time() == Time::j2000())) n++;
  if(!test.check("range()", et.range() == Interval(1.0))) n++;
  if(!test.check("is_valid_at(OK)", et.is_valid_at(t1))) n++;
  if(!test.check("is_valid_at(!)", !et.is_valid_at(t1 + 1.0))) n++;

  if(!test.equals("longitude_evolution()", et.longitude_evolution().value(ip), lon.value(ip), 1e-15)) n++;
  if(!test.equals("latitude_evolution()", et.latitude_evolution().value(ip), lat.value(ip), 1e-15)) n++;
  if(!test.equals("distance_evolution()", et.distance_evolution().value(ip), r.value(ip), 1e-15)) n++;

  oa = et.longitude_at(t1);
  if(!test.check("longitude_at()", oa.has_value())) n++;
  if(!test.equals("longitude_at().value()", oa.value().rad(), lon.value(ip), 1e-15)) n++;

  oa = et.latitude_at(t1);
  if(!test.check("latitude_at()", oa.has_value())) n++;
  if(!test.equals("latitude_at().value()", oa.value().rad(), lat.value(ip), 1e-15)) n++;

  oc = et.distance_at(t1);
  if(!test.check("distance_at()", oc.has_value())) n++;
  if(!test.equals("distance_at().value()", oc.value().m(), r.value(ip), 1e-15)) n++;

  ov = et.radial_velocity_at(t1);
  if(!test.check("radial_velocity_at()", ov.has_value())) n++;
  if(!test.equals("radial_velocity_at().value()", ov.value().m_per_s(), r.rate(ip), 1e-6)) n++;

  if(!test.check("redshift_at()", et.redshift_at(t1).has_value())) n++;
  if(!test.equals("redshift_at().value()", et.redshift_at(t1).value(), novas_v2z(r.rate(ip) / (Unit::km / Unit::s)), 1e-15)) n++;

  std::optional<Equatorial> oe = et.projected_at(t1);
  if(!test.check("projected_at()", oe.has_value())) n++;
  if(!test.equals("projected_at().azimuth()", oe.value().ra().rad(), lon.value(ip), 1e-15)) n++;
  if(!test.equals("projected_at().elevation()", oe.value().dec().rad(), lat.value(ip), 1e-15)) n++;

  et = EquatorialTrack(Equinox::icrs(), Time::j2000(), Interval(1.0), lon, lat, r, z);
  if(!test.check("is_valid(z)", et.is_valid())) n++;
  if(!test.equals("redshift_evolution()", et.redshift_evolution().value(ip), z.value(ip), 1e-15)) n++;

  oe = et.projected_at(t1);
  if(!test.check("projected_at()", oe.has_value())) n++;
  if(!test.equals("projected_at().azimuth()", oe.value().ra().rad(), lon.value(ip), 1e-15)) n++;
  if(!test.equals("projected_at().elevation()", oe.value().dec().rad(), lat.value(ip), 1e-15)) n++;

  if(!test.check("longitude_at(range)", !et.longitude_at(t2).has_value())) n++;
  if(!test.check("latitude_at(range)", !et.latitude_at(t2).has_value())) n++;
  if(!test.check("distance_at(range)", !et.distance_at(t2).has_value())) n++;
  if(!test.check("radial_velocity_at(range)", !et.radial_velocity_at(t2).has_value())) n++;
  if(!test.check("redshift_at(range)", !et.redshift_at(t2).has_value())) n++;
  if(!test.check("projected_at(range)", !et.projected_at(t2).has_value())) n++;

  std::optional<EquatorialTrack> oet = EquatorialTrack::from_novas_track(Equinox::icrs(), NULL, Interval(1.0));
  if(!test.check("from_novas_track(NULL)", !oet.has_value())) n++;

  oet = EquatorialTrack::from_novas_track(Equinox::undefined(), &tr0, Interval(1.0));
  if(!test.check("from_novas_track(invalid system)", !oet.has_value())) n++;

  trx = tr0; trx.pos.lon = NAN;
  oet = EquatorialTrack::from_novas_track(Equinox::icrs(), &trx, Interval(1.0));
  if(!test.check("from_novas_track(pos0 = NAN)", !oet.has_value())) n++;

  oet = EquatorialTrack::from_novas_track(Equinox::icrs(), &tr0, Interval(1.0));
  if(!test.check("from_novas_track(pos0 = NAN)", oet.has_value())) n++;


  std::cout << "Track.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
