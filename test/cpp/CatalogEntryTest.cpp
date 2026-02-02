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
  TestUtil test = TestUtil("CatalogEntry");

  int n = 0;
 
  if(!test.check("invalid(equatorial)", !CatalogEntry("test", Equatorial::invalid()).is_valid())) n++;
  if(!test.check("invalid(ecliptic)", !CatalogEntry("test", Ecliptic::invalid()).is_valid())) n++;
  if(!test.check("invalid(galactic)", !CatalogEntry("test", Galactic::invalid()).is_valid())) n++;

  CatalogEntry a("test", Equatorial("12:00:00.00", "-30:00:00"));
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("name()", a.name(), "test")) n++;
  if(!test.check("equatorial()", a.equatorial() == Equatorial("12:00:00.00", "-30:00:00"))) n++;
  if(!test.check("system()", a.system() == Equinox::icrs())) n++;

  a.distance(Distance(Unit::pc));
  if(!test.equals("distance(1pc)", a.distance().m(), Unit::pc, 1e-15 * Unit::pc)) n++;
  if(!test.equals("parallax(1pc)", a.parallax().arcsec(), 1.0, 1e-15)) n++;

  a.parallax(Angle(Unit::mas));
  if(!test.equals("parallax(1mas)", a.parallax().mas(), 1.0, 1e-15)) n++;
  if(!test.equals("distance(1mas)", a.distance().m(), Unit::kpc, 1e-15 * Unit::pc)) n++;

  a.proper_motion(-1.0 * Unit::mas / Unit::yr, 2.0 * Unit::mas / Unit::yr);
  const cat_entry *e = a._cat_entry();
  if(!test.equals("->promora", e->promora, -1.0, 1e-15)) n++;
  if(!test.equals("->promodec", e->promodec, 2.0, 1e-15)) n++;

  // TODO...

  CatalogEntry b("TEST", Ecliptic("45:00:00.00", "-30:00:00"));
  if(!test.equals("name(ECL)", b.name(), "TEST")) n++;
  if(!test.check("ecliptic()", b.equatorial().to_ecliptic() == Ecliptic("45:00:00.00", "-30:00:00"))) n++;

  CatalogEntry c("test", Galactic("45:00:00.00", "-30:00:00"));
  if(!test.equals("name(GAL)", c.name(), "test")) n++;
  if(!test.check("galactic()", c.equatorial().to_galactic() == Galactic("45:00:00.00", "-30:00:00"))) n++;

  cat_entry star = {};
  novas_init_cat_entry(&star, "Antares", novas_str_hours("16h26m20.1918s"), novas_str_degrees("-26d19m23.138s"));
  novas_set_catalog(&star, "HIP", 80763);
  novas_set_proper_motion(&star, -12.11, -23.30);
  novas_set_parallax(&star, 5.89);
  novas_set_ssb_vel(&star, -3.4);

  CatalogEntry d(star, Equinox::b1950());
  if(!test.check("Antares.is_valid()", d.is_valid())) n++;
  if(!test.equals("Antares.name()", d.name(), "Antares")) n++;
  if(!test.check("Antares.equatorial()", d.equatorial() == Equatorial("16h26m20.1918s", "-26d19m23.138s", Equinox::b1950()))) n++;
  if(!test.equals("Antares.parallax()", d.parallax().mas(), 5.89, 1e-15)) n++;
  if(!test.equals("Antares.radial_velocity()", d.radial_velocity().km_per_s(), -3.4, 1e-15)) n++;
  if(!test.equals("Antares->promora", d._cat_entry()->promora, -12.11, 1e-14)) n++;
  if(!test.equals("Antares->promodec", d._cat_entry()->promodec, -23.30, 1e-14)) n++;


  std::cout << "CatalogEntry.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
