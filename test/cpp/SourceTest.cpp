/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>
#include <string.h>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("CatalogSource");

  int n = 0;

  CatalogEntry xe("Invalid", Equatorial::undefined());
  if(!test.check("invalid", !CatalogSource(xe).is_valid())) n++;

  char lname[SIZE_OF_OBJ_NAME + 1] = {'\0'};
  memset(lname, 'a', SIZE_OF_OBJ_NAME);

  if(!test.check("invalid long name", !CatalogEntry(lname, Equatorial("12:34:56.789", "12:34:56.789")).is_valid())) n++;

  CatalogEntry ce("Test", Equatorial("12:34:56.789", "12:34:56.789"));
  CatalogSource c(ce);

  const Source *c1 = c.copy();
  if(!test.check("copy()", memcmp(c1->_novas_object(), c._novas_object(), sizeof(object)) == 0)) n++;

  if(!test.check("is_valid()", c.is_valid())) n++;
  if(!test.equals("type()", c.type(), NOVAS_CATALOG_OBJECT)) n++;
  if(!test.equals("name() insensitive", c.name(), "TEST")) n++;
  if(!test.check("catalog_entry()", memcmp(c.catalog_entry()._cat_entry(), ce._cat_entry(), sizeof(cat_entry)) == 0)) n++;
  if(!test.equals("to_string()", c.to_string(), "CatalogSource Test @ 12h 34m 56.789s   12d 34m 56.789s ICRS")) n++;


  Source::set_case_sensitive(true);
  c = ce.to_source();
  if(!test.equals("name() sensitive", c.name(), "Test")) n++;

  Planet sun = Planet(NOVAS_SUN);

  const object *o = c._novas_object();
  if(!test.equals("_novas_object() type", o->type, NOVAS_CATALOG_OBJECT)) n++;
  if(!test.equals("_novas_object() name", std::string(o->name), "Test")) n++;

  Site site(10.0 * Unit::deg, 20.0 * Unit::deg, 30.0);
  EOP eop(37, 0.0, 0.0, 0.0);

  Frame frame = Observer::on_earth(site, eop).reduced_accuracy_frame_at(Time::j2000());
  Frame gf = Observer::on_earth(site, eop).reduced_accuracy_frame_at(Time::j2000());
  Frame gfx = Observer::on_earth(site, eop).reduced_accuracy_frame_at(Time::undefined());
  Frame gc = Observer::at_geocenter().reduced_accuracy_frame_at(Time::j2000());

  if(!test.check("observer.is_geodetic()", Observer::on_earth(site, eop).is_geodetic())) n++;
  if(!test.check("frame.is_valid()", frame.is_valid())) n++;
  if(!test.check("frame.observer().is_geodetic()", frame.observer().is_geodetic())) n++;

  if(!test.equals("sun_angle()", c.sun_angle(frame).deg(), novas_sun_angle(o, frame._novas_frame()), 1e-13)) n++;
  if(!test.equals("moon_angle()", c.moon_angle(frame).deg(), novas_moon_angle(o, frame._novas_frame()), 1e-13)) n++;
  if(!test.equals("angle_to()", c.angle_to(sun, frame).deg(), novas_object_sep(o, sun._novas_object(), frame._novas_frame()), 1e-13)) n++;


  if(!test.equals("rises_above(geo)",
          c.rises_above(Angle(20.0 * Unit::deg), gf).jd(),
          novas_rises_above(20.0, c._novas_object(), gf._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("rises_above()",
          c.rises_above(Angle(20.0 * Unit::deg), frame).jd(),
          novas_rises_above(20.0, c._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("rises_above(refract)",
          c.rises_above(Angle(20.0 * Unit::deg), frame, novas_standard_refraction, site.average_weather()).jd(),
          novas_rises_above(20.0, c._novas_object(), frame._novas_frame(), novas_standard_refraction), 1e-7)) n++;
  if(!test.check("rises_above(gc)", !c.rises_above(Angle(20.0 * Unit::deg), gc).is_valid())) n++;
  if(!test.check("rises_above(invalid geo)", !c.rises_above(Angle(20.0 * Unit::deg), gfx).is_valid())) n++;

  if(!test.equals("sets_below(geo)",
          c.sets_below(Angle(20.0 * Unit::deg), gf).jd(),
          novas_sets_below(20.0, c._novas_object(), gf._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("sets_below()",
          c.sets_below(Angle(20.0 * Unit::deg), frame).jd(),
          novas_sets_below(20.0, c._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("sets_below(refract)",
          c.sets_below(Angle(20.0 * Unit::deg), frame, novas_standard_refraction, site.average_weather()).jd(),
          novas_sets_below(20.0, c._novas_object(), frame._novas_frame(), novas_standard_refraction), 1e-7)) n++;
  if(!test.check("sets_below(gc)", !c.sets_below(Angle(20.0 * Unit::deg), gc).is_valid())) n++;
  if(!test.check("sets_below(invalid geo)", !c.sets_below(Angle(20.0 * Unit::deg), gfx).is_valid())) n++;

  if(!test.equals("transits_in(geo)",
          c.transits_in(gf).jd(),
          novas_transit_time(c._novas_object(), gf._novas_frame()), 1e-7)) n++;
  if(!test.equals("transits_in()",
          c.transits_in(frame).jd(),
          novas_transit_time(c._novas_object(), frame._novas_frame()), 1e-7)) n++;
  if(!test.check("transits_in(gc)", !c.transits_in(gc).is_valid())) n++;
  if(!test.check("trasits(invalid geo)", !c.transits_in(gfx).is_valid())) n++;

  sky_pos tod = {};
  novas_sky_pos(o, frame._novas_frame(), NOVAS_TOD, &tod);
  Apparent app = c.apparent_in(frame);
  if(!test.check("apparent()", app.is_valid())) n++;
  if(!test.equals("apparent().ra()", app.equatorial().ra().hours(), tod.ra, 1e-13)) n++;
  if(!test.equals("apparent().dec()", app.equatorial().dec().deg(), tod.dec, 1e-12)) n++;
  if(!test.equals("apparent().radial_velocity()", app.radial_velocity().km_per_s(), tod.rv, 1e-13)) n++;
  if(!test.check("apparent(invalid)", !Planet((enum novas_planet) -1).apparent_in(gc).is_valid())) n++;

  double p[3] = {0.0}, v[3] = {0.0};


  novas_geom_posvel(sun._novas_object(), frame._novas_frame(), NOVAS_TOD, p, v);
  Geometric geom = sun.geometric_in(frame, NOVAS_TOD);
  if(!test.check("geometric(TOD)", geom.is_valid())) n++;
  if(!test.check("geometric(TOD).position()", geom.position() == Position(p, Unit::AU))) n++;
  if(!test.check("geometric(TOD).velocity()", geom.velocity() == Velocity(v, Unit::AU / Unit::day))) n++;
  if(!test.check("geometric(invalid)", !Planet((enum novas_planet) -1).geometric_in(frame).is_valid())) n++;

  if(!test.check("equatorial_track(frame invalid)", !c.equatorial_track(Frame::undefined(), Interval(Unit::hour)).is_valid())) n++;

  EquatorialTrack et = c.equatorial_track(frame, Interval(Unit::hour));
  if(!test.check("equatorial_track()", et.is_valid())) n++;

  novas_track tr = {};
  novas_equ_track(c._novas_object(), frame._novas_frame(), 10.0, &tr);
  if(!test.equals("equatorial_track().range()", et.range().hours(), 1.0, 1e-12)) n++;
  if(!test.equals("equatorial_track().lon(0)", et.longitude_evolution().value(), tr.pos.lon * Unit::deg, 1e-12)) n++;
  if(!test.equals("equatorial_track().lon(1) ", et.longitude_evolution().rate(), tr.rate.lon * Unit::deg, 1e-10)) n++;
  if(!test.equals("equatorial_track().lon(2)", et.longitude_evolution().acceleration(), tr.accel.lon * Unit::deg, 1e-12)) n++;
  if(!test.equals("equatorial_track().lat(0)", et.latitude_evolution().value(), tr.pos.lat * Unit::deg, 1e-12)) n++;
  if(!test.equals("equatorial_track().lat(1) ", et.latitude_evolution().rate(), tr.rate.lat * Unit::deg, 1e-12)) n++;
  if(!test.equals("equatorial_track().lat(2)", et.latitude_evolution().acceleration(), tr.accel.lat * Unit::deg, 1e-12)) n++;
  if(!test.equals("equatorial_track().dis(0)", et.distance_evolution().value(), tr.pos.dist * Unit::AU, 1e-3)) n++;
  // FIXME
  //if(!test.equals("equatorial_track().dis(1) ", et.distance_evolution().rate(), tr.rate.dist * Unit::AU, 1e-3)) n++;
  //if(!test.equals("equatorial_track().dis(2)", et.distance_evolution().acceleration(), tr.accel.dist * Unit::AU, 1e-3)) n++;


  if(!test.check("horizontal_track(gc)", !c.horizontal_track(gc).is_valid())) n++;

  // FIXME
  //if(!test.check("horizontal_track(time invalid)", !c.horizontal_track(gfx).is_valid())) n++;

  HorizontalTrack ht = c.horizontal_track(frame, NULL);
  if(!test.check("horizontal_track()", ht.is_valid())) n++;

  novas_hor_track(c._novas_object(), frame._novas_frame(), NULL, &tr);
  if(!test.equals("horizontal_track().lon(0)", ht.longitude_evolution().value(), tr.pos.lon * Unit::deg, 1e-12)) n++;
  if(!test.equals("horizontal_track().lon(1) ", ht.longitude_evolution().rate(), tr.rate.lon * Unit::deg, 1e-10)) n++;
  if(!test.equals("horizontal_track().lon(2)", ht.longitude_evolution().acceleration(), tr.accel.lon * Unit::deg, 1e-12)) n++;
  if(!test.equals("horizontal_track().lat(0)", ht.latitude_evolution().value(), tr.pos.lat * Unit::deg, 1e-12)) n++;
  if(!test.equals("horizontal_track().lat(1) ", ht.latitude_evolution().rate(), tr.rate.lat * Unit::deg, 1e-12)) n++;
  if(!test.equals("horizontal_track().lat(2)", ht.latitude_evolution().acceleration(), tr.accel.lat * Unit::deg, 1e-12)) n++;
  if(!test.equals("horizontal_track().dis(0)", ht.distance_evolution().value(), tr.pos.dist * Unit::AU, 1e-3)) n++;
  if(!test.equals("horizontal_track().dis(1) ", ht.distance_evolution().rate(), tr.rate.dist * Unit::AU, 1e-3)) n++;
  if(!test.equals("horizontal_track().dis(2)", ht.distance_evolution().acceleration(), tr.accel.dist * Unit::AU, 1e-3)) n++;


  test = TestUtil("Planet");
  Geometric gs = sun.geometric_in(frame).to_icrs();
  Geometric pv = sun.ssb_posvel_at(frame.time() - gs.distance().m() / Constant::c, NOVAS_REDUCED_ACCURACY);

  if(!test.check("ssb_posvel_at()", pv.is_valid())) n++;
  if(!test.check("ssb_posvel_at() position", pv.position().equals(gs.position() + frame.observer_ssb_position(), Unit::cm))) {
    std::cout << "### " << (pv.position() - gs.position() - frame.observer_ssb_position()).to_string(9) << "\n";
    n++;
  }
  if(!test.check("ssb_posvel_at() velocity", pv.velocity().equals(gs.velocity() + frame.observer_ssb_velocity(), Unit::mm / Unit::s))) {
    std::cout << "### " << (pv.velocity() - gs.velocity() - frame.observer_ssb_velocity()).to_string(9) << "\n";
    std::cout << "### " << pv.velocity().to_string(9) << " vs " << (gs.velocity() + frame.observer_ssb_velocity()).to_string(9) << "\n";
    n++;
  }

  // We don't have an ephemeris provider for mars...
  if(!test.check("ssb_posvel_at(invalid)", !Planet::mars().ssb_posvel_at(frame.time()).is_valid())) n++;

  if(!test.equals("rises_above(Sun)",
          sun.rises_above(Angle(20.0 * Unit::deg), frame).jd(),
          novas_rises_above(20.0, sun._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("sets_below(Sun)",
          sun.sets_below(Angle(20.0 * Unit::deg), frame).jd(),
          novas_sets_below(20.0, sun._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("transits_in(Sun)",
          sun.transits_in(frame).jd(),
          novas_transit_time(sun._novas_object(), frame._novas_frame()), 1e-7)) n++;


  test = TestUtil("OrbitalSource");

  Orbital xorb = OrbitalSystem::equatorial().orbit(Time::undefined(), Coordinate(Unit::AU), Angle(-1.0), Interval(Unit::yr));
  if(!test.check("invalid time invalid", !xorb.to_source("xxx").is_valid())) n++;
  if(!test.check("orbital(invalid time)", !xorb.to_source("xxx").orbital().is_valid())) n++;

  xorb = OrbitalSystem::equatorial().orbit(Time::j2000(), Coordinate(NAN), Angle(-1.0), Interval(Unit::yr));
  if(!test.check("invalid major invalid", !xorb.to_source("xxx").is_valid())) n++;
  if(!test.check("orbital(invalid major)", !xorb.to_source("xxx").orbital().is_valid())) n++;

  xorb = OrbitalSystem::equatorial().orbit(Time::j2000(), Coordinate(Unit::AU), Angle(NAN), Interval(Unit::yr));
  if(!test.check("invalid apsis invalid", !xorb.to_source("xxx").is_valid())) n++;
  if(!test.check("orbital(invalid apsis)", !xorb.to_source("xxx").orbital().is_valid())) n++;

  xorb = OrbitalSystem::equatorial().orbit(Time::j2000(), Coordinate(Unit::AU), Angle(-1.0), Interval(NAN));
  if(!test.check("invalid period invalid", !xorb.to_source("xxx").is_valid())) n++;
  if(!test.check("orbital(invalid period)", !xorb.to_source("xxx").orbital().is_valid())) n++;

  Orbital orb = OrbitalSystem::equatorial().orbit(Time::j2000(), Coordinate(Unit::AU), Angle(-1.0), Interval(Unit::yr));

  if(!test.check("invalid long name", !orb.to_source(lname).is_valid())) n++;

  OrbitalSource os = orb.to_source("test");
  if(!test.check("is_valid()", os.is_valid())) n++;
  if(!test.check("copy()", memcmp(os.copy()->_novas_object(), os._novas_object(), sizeof(object)) == 0)) n++;
  if(!test.check("_novas_orbital()", memcmp(os._novas_orbital(), orb._novas_orbital(), sizeof(novas_orbital)) == 0)) n++;
  if(!test.check("orbital()", memcmp(os.orbital()._novas_orbital(), orb._novas_orbital(), sizeof(novas_orbital)) == 0)) n++;
  if(!test.equals("solar_illumination()", os.solar_illumination(frame), novas_solar_illum(os._novas_object(), frame._novas_frame()), 1e-9)) n++;
  if(!test.equals("solar_power()", os.solar_power(frame.time()), novas_solar_power(frame.jd(NOVAS_TDB), os._novas_object()), 1e-9)) n++;

  double rate = 0.0;
  if(!test.equals("helio_distance()", os.helio_distance(frame.time()).au(), novas_helio_dist(frame.jd(NOVAS_TDB), os._novas_object(), &rate), 1e-9)) n++;
  if(!test.equals("helio_distance()", os.helio_rate(frame.time()).au_per_day(), rate, 1e-9)) n++;
  if(!test.equals("to_string()", os.to_string(), "OrbitalSource test")) n++;

  test = TestUtil("EphemerisSource");

  if(!test.check("invalid long name", !EphemerisSource(lname, 123456L).is_valid())) n++;


  EphemerisSource es = EphemerisSource("test", 123456L);
  if(!test.check("is_valid()", es.is_valid())) n++;
  if(!test.check("copy()", memcmp(es.copy()->_novas_object(), es._novas_object(), sizeof(object)) == 0)) n++;
  if(!test.check("copy()", memcmp(es.copy()->_novas_object(), es._novas_object(), sizeof(object)) == 0)) n++;
  if(!test.equals("type()", es.type(), NOVAS_EPHEM_OBJECT)) n++;
  if(!test.equals("name()", es.name(), "test")) n++;
  if(!test.equals("number()", es.number(), 123456L)) n++;
  if(!test.equals("to_string()", es.to_string(), "EphemerisSource test (nr. 123456)")) n++;

  std::cout << "Source.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
