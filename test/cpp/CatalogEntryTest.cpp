/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>
#include "string.h"

#include "TestUtil.hpp"

using namespace novas;

int main() {
  TestUtil test = TestUtil("CatalogEntry");

  int n = 0;
 
  if(!test.check("invalid(equatorial)", !CatalogEntry("test", Equatorial::invalid()).is_valid())) n++;
  if(!test.check("invalid(ecliptic)", !CatalogEntry("test", Ecliptic::invalid()).is_valid())) n++;
  if(!test.check("invalid(galactic)", !CatalogEntry("test", Galactic::invalid()).is_valid())) n++;

  char longName[SIZE_OF_OBJ_NAME + 1] = {'\0'};
  memset(longName, 'X', SIZE_OF_OBJ_NAME);

  if(!test.check("invalid(galactic)", !CatalogEntry(longName, Equatorial("12:00:00.00", "-30:00:00")).is_valid())) n++;

  CatalogEntry a("test", Equatorial("12:00:00.00", "-30:00:00"));
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("name()", a.name(), "test")) n++;
  if(!test.check("equatorial()", a.equatorial() == Equatorial("12:00:00.00", "-30:00:00"))) n++;
  if(!test.check("system()", a.system() == Equinox::icrs())) n++;

  a.distance(Distance(Unit::pc));
  if(!test.equals("distance(1pc)", a.distance().m(), Unit::pc, 1e-15 * Unit::pc)) n++;
  if(!test.equals("parallax(1pc)", a.parallax().arcsec(), 1.0, 1e-15)) n++;

  a.distance(NAN);
  if(!test.check("distance(NAN)", !a.is_valid())) n++;
  a.distance(-Unit::pc);
  if(!test.check("distance(neg)", !a.is_valid())) n++;
  a.distance(Unit::pc);
  if(!test.check("distance(OK)", a.is_valid())) n++;

  a.parallax(Angle(Unit::mas));
  if(!test.equals("parallax(1mas)", a.parallax().mas(), 1.0, 1e-15)) n++;
  if(!test.equals("distance(1mas)", a.distance().m(), Unit::kpc, 1e-15 * Unit::pc)) n++;

  a.parallax(NAN);
  if(!test.check("parallax(NAN)", !a.is_valid())) n++;
  a.parallax(0.0);
  if(!test.check("parallax(0)", a.is_valid())) n++;
  a.parallax(-Unit::mas);
  if(!test.check("parallax(neg)", !a.is_valid())) n++;
  a.parallax(Unit::mas);
  if(!test.check("parallax(OK)", a.is_valid())) n++;

  a.proper_motion(-1.0 * Unit::mas / Unit::yr, 2.0 * Unit::mas / Unit::yr);
  const cat_entry *e = a._cat_entry();
  if(!test.equals("->promora", e->promora, -1.0, 1e-15)) n++;
  if(!test.equals("->promodec", e->promodec, 2.0, 1e-15)) n++;

  a.proper_motion(NAN, 2.0 * Unit::mas / Unit::yr);
  if(!test.check("proper_motion(ra = NAN)", !a.is_valid())) n++;
  a.proper_motion(-1.0 * Unit::mas / Unit::yr, 2.0 * Unit::mas / Unit::yr);
  if(!test.check("parallax(ra = OK)", a.is_valid())) n++;
  a.proper_motion(-1.0 * Unit::mas / Unit::yr, NAN);
  if(!test.check("proper_motion(dec = NAN)", !a.is_valid())) n++;
  a.proper_motion(-1.0 * Unit::mas / Unit::yr, 2.0 * Unit::mas / Unit::yr);
  if(!test.check("parallax(dec = OK)", a.is_valid())) n++;

  a.radial_velocity(Speed(1.0 * Unit::km / Unit::s));
  if(!test.equals("radial_velocity(1 km/s)", a.radial_velocity().km_per_s(), 1.0, 1e-15)) n++;
  a.radial_velocity(NAN);
  if(!test.check("invalid(rv = NAN)", !a.is_valid())) n++;
  a.radial_velocity(0.0);
  if(!test.check("radial_velocity(0)", a.is_valid())) n++;
  a.radial_velocity(Constant::c + 1.0);
  if(!test.check("invalid(rv > c)", !a.is_valid())) n++;
  a.radial_velocity(1.0 * Unit::km / Unit::s);
  if(!test.check("radial_velocity(OK)", a.is_valid())) n++;


  a.v_lsr(Speed(1.0 * Unit::km / Unit::s));
  if(!test.equals("v_lsr(1 km/s)", a.v_lsr().km_per_s(), 1.0, 1e-9)) n++;
  if(!test.equals("v_lsr() -> rv",
          novas_lsr_to_ssb_vel(a.system().epoch(), a.equatorial().ra().hours(), a.equatorial().dec().deg(), a.v_lsr().km_per_s()),
          a.radial_velocity().km_per_s(), 1e-9)) n++;
  a.v_lsr(NAN);
  if(!test.check("invalid(v_lsr = NAN)", !a.is_valid())) n++;
  a.v_lsr(0.0);
  if(!test.check("v_lsr(0)", a.is_valid())) n++;
  a.v_lsr(Constant::c + 1.0);
  if(!test.check("invalid(v_lsr > c)", !a.is_valid())) n++;
  a.v_lsr(1.0 * Unit::km / Unit::s);
  if(!test.check("v_lsr(OK)", a.is_valid())) n++;


  a.redshift(0.1);
  if(!test.equals("redshift(0.1)", a.redshift(), 0.1, 1e-12)) n++;
  if(!test.equals("redshift() -> rv", novas_z2v(0.1), a.radial_velocity().km_per_s(), 1e-9)) n++;
  a.redshift(NAN);
  if(!test.check("invalid(redshift = NAN)", !a.is_valid())) n++;
  a.redshift(0.0);
  if(!test.check("redshift(0)", a.is_valid())) n++;
  a.redshift(-1.01);
  if(!test.check("invalid(z < -1)", !a.is_valid())) n++;
  a.redshift(0.1);
  if(!test.check("v_lsr(OK)", a.is_valid())) n++;

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

  CatalogEntry x = CatalogEntry(star, Equinox::invalid());
  if(!test.check("invalid(equinox)", !x.is_valid())) n++;

  cat_entry inv = star; inv.ra = NAN; x = CatalogEntry(inv, Equinox::b1950());
  if(!test.check("is_valid(ra = NAN)", !x.is_valid())) n++;

  inv = star; inv.dec = NAN; x = CatalogEntry(inv, Equinox::b1950());
  if(!test.check("is_valid(dec = NAN)", !x.is_valid())) n++;

  inv = star; inv.dec = 90.1; x = CatalogEntry(inv, Equinox::b1950());
  if(!test.check("is_valid(dec = 90.1)", !x.is_valid())) n++;

  inv = star; inv.parallax = NAN; x = CatalogEntry(inv, Equinox::b1950());
  if(!test.check("is_valid(parallax = NAN)", !x.is_valid())) n++;

  inv = star; inv.parallax = -1e-9; x = CatalogEntry(inv, Equinox::b1950());
  if(!test.check("is_valid(parallax < 0)", !x.is_valid())) n++;

  inv = star; inv.promora = NAN; x = CatalogEntry(inv, Equinox::b1950());
  if(!test.check("is_valid(promora = NAN)", !x.is_valid())) n++;

  inv = star; inv.promodec = NAN; x = CatalogEntry(inv, Equinox::b1950());
  if(!test.check("is_valid(promodec = NAN)", !x.is_valid())) n++;

  inv = star; inv.radialvelocity = NAN; x = CatalogEntry(inv, Equinox::b1950());
  if(!test.check("is_valid(radialvelocity = NAN)", !x.is_valid())) n++;

  std::cout << "CatalogEntry.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
