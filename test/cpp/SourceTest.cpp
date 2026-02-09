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
  c = CatalogSource(ce);
  if(!test.equals("name(catalog) sensitive", c.name(), "Test")) n++;

  const object *o = c._novas_object();
  if(!test.equals("_novas_object(catalog) type", o->type, NOVAS_CATALOG_OBJECT)) n++;
  if(!test.equals("_novas_object(catalog) name", std::string(o->name), "Test")) n++;

  Site site(10.0 * Unit::deg, 20.0 * Unit::deg, 30.0);
  EOP eop(37, 0.0, 0.0, 0.0);

  Frame frame(Observer::on_earth(site, eop), Time::j2000(), NOVAS_REDUCED_ACCURACY);

  if(!test.check("observer.is_geodetic()", Observer::on_earth(site, eop).is_geodetic())) n++;
  if(!test.check("frame.is_valid()", frame.is_valid())) n++;
  if(!test.check("frame.observer().is_geodetic()", frame.observer().is_geodetic())) n++;

  if(!test.equals("rises_above(catalog)",
          c.rises_above(Angle(20.0 * Unit::deg), frame).value().jd(),
          novas_rises_above(20.0, c._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;

  if(!test.equals("sets_below(catalog)",
          c.sets_below(Angle(20.0 * Unit::deg), frame).value().jd(),
          novas_sets_below(20.0, c._novas_object(), frame._novas_frame(), NULL), 1e-7)) n++;

  if(!test.equals("transits(catalog)",
          c.transits(frame).value().jd(),
          novas_transit_time(c._novas_object(), frame._novas_frame()), 1e-7)) n++;

  std::cout << "Source.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
