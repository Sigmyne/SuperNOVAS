/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>
#include <string.h>

#include "TestUtil.hpp"

using namespace novas;

int main() {
  TestUtil test = TestUtil("Source");

  int n = 0;

  CatalogEntry xe("Invalid", Equatorial::invalid());
  if(!test.check("invalid(catalog)", !CatalogSource(xe).is_valid())) n++;

  CatalogEntry ce("Test", Equatorial("12:34:56.789", "12:34:56.789"));
  CatalogSource c(ce);

  if(!test.check("is_valid(catalog)", c.is_valid())) n++;
  if(!test.equals("name(catalog) insensitive", c.name(), "TEST")) n++;
  if(!test.check("catalog_entry()", memcmp(c.catalog_entry()._cat_entry(), ce._cat_entry(), sizeof(cat_entry)) == 0)) n++;
  if(!test.equals("to_string(catalog)", c.to_string(), "CatalogSource Test @ 12h 34m 56.789s   12d 34m 56.789s ICRS")) n++;

  Source::set_case_sensitive(true);
  c = ce.to_source();
  if(!test.equals("name(catalog) sensitive", c.name(), "Test")) n++;

  Planet sun = Planet(NOVAS_SUN);

  const object *o = c._novas_object();
  if(!test.equals("_novas_object(catalog) type", o->type, NOVAS_CATALOG_OBJECT)) n++;
  if(!test.equals("_novas_object(catalog) name", std::string(o->name), "Test")) n++;

  Site site(10.0 * Unit::deg, 20.0 * Unit::deg, 30.0);
  EOP eop(37, 0.0, 0.0, 0.0);

  Frame frame = Observer::on_earth(site, eop).reduced_accuracy_frame_at(Time::j2000());
  Frame gc = Observer::at_geocenter().reduced_accuracy_frame_at(Time::j2000());

  if(!test.check("observer.is_geodetic()", Observer::on_earth(site, eop).is_geodetic())) n++;
  if(!test.check("frame.is_valid()", frame.is_valid())) n++;
  if(!test.check("frame.observer().is_geodetic()", frame.observer().is_geodetic())) n++;

  if(!test.equals("sun_angle()", c.sun_angle(frame).deg(), novas_sun_angle(o, frame._novas_frame()), 1e-13)) n++;
  if(!test.equals("moon_angle()", c.moon_angle(frame).deg(), novas_moon_angle(o, frame._novas_frame()), 1e-13)) n++;
  if(!test.equals("angle_to()", c.angle_to(sun, frame).deg(), novas_object_sep(o, sun._novas_object(), frame._novas_frame()), 1e-13)) n++;

  if(!test.equals("rises_above(catalog)",
          c.rises_above(Angle(20.0 * Unit::deg), frame).value().jd(),
          novas_rises_above(20.0, c._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("rises_above(Sun)",
          sun.rises_above(Angle(20.0 * Unit::deg), frame).value().jd(),
          novas_rises_above(20.0, sun._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("rises_above(refract)",
          c.rises_above(Angle(20.0 * Unit::deg), frame, novas_standard_refraction, Weather::guess(site)).value().jd(),
          novas_rises_above(20.0, c._novas_object(), frame._novas_frame(), novas_standard_refraction), 1e-7)) n++;
  if(!test.check("rises_above(gc)", !c.rises_above(Angle(20.0 * Unit::deg), gc).has_value())) n++;

  if(!test.equals("sets_below(catalog)",
          c.sets_below(Angle(20.0 * Unit::deg), frame).value().jd(),
          novas_sets_below(20.0, c._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("sets_below(Sun)",
          sun.sets_below(Angle(20.0 * Unit::deg), frame).value().jd(),
          novas_sets_below(20.0, sun._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;
  if(!test.equals("sets_below(refract)",
          c.sets_below(Angle(20.0 * Unit::deg), frame, novas_standard_refraction, Weather::guess(site)).value().jd(),
          novas_sets_below(20.0, c._novas_object(), frame._novas_frame(), novas_standard_refraction), 1e-7)) n++;
  if(!test.check("sets_below(gc)", !c.sets_below(Angle(20.0 * Unit::deg), gc).has_value())) n++;

  if(!test.equals("transits(catalog)",
          c.transits(frame).value().jd(),
          novas_transit_time(c._novas_object(), frame._novas_frame()), 1e-7)) n++;
  if(!test.equals("transits(Sun)",
          sun.transits(frame).value().jd(),
          novas_transit_time(sun._novas_object(), frame._novas_frame()), 1e-7)) n++;
  if(!test.check("transits(gc)", !c.transits(gc).has_value())) n++;

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


  std::cout << "Source.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
