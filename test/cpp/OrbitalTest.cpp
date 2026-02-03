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
  TestUtil test = TestUtil("Orbital");

  int n = 0;

  if(!test.check("System::ecliptic(invalid planet)", !OrbitalSystem::ecliptic(Planet((enum novas_planet) -1)).is_valid())) n++;
  if(!test.check("System::equatorial(invalid planet)", !OrbitalSystem::equatorial(Planet((enum novas_planet) -1)).is_valid())) n++;

  OrbitalSystem s = OrbitalSystem::ecliptic(Planet::sun());
  if(!test.check("System::is_valid()", s.is_valid())) n++;
  if(!test.equals("System::center()", s.center().novas_id(), NOVAS_SUN)) n++;
  
  s.orientation(Angle(1.0 * Unit::deg), Angle(-2.0 * Unit::deg), Equinox::icrs());
  if(!test.check("System::is_valid(orientation)", s.is_valid())) n++;
  if(!test.equals("System::obliquity()", s.obliquity().deg(), 1.0, 1e-15)) n++;
  if(!test.equals("System::ascending_node()", s.ascending_node().deg(), -2.0, 1e-15)) n++;
  if(!test.equals("System::reference_system()", s.reference_system(), NOVAS_ICRS));

  s.pole(Spherical(-92.0 * Unit::deg, 89.0 * Unit::deg), Equinox::j2000());
  if(!test.check("System::is_valid(pole)", s.is_valid())) n++;
  if(!test.equals("System::obliquity(pole)", s.obliquity().deg(), 1.0, 1e-12)) n++;
  if(!test.equals("System::ascending_node(pole)", s.ascending_node().deg(), -2.0, 1e-12)) n++;
  if(!test.equals("System::reference_system(pole)", s.reference_system(), NOVAS_J2000)) n++;
  if(!test.equals("System::pole().longitude()", s.pole().longitude().deg(), -92.0, 1e-12)) n++;
  if(!test.equals("System::pole().latitude()", s.pole().latitude().deg(), 89.0, 1e-12)) n++;

  s.orientation(NAN, -2.0 * Unit::deg, Equinox::icrs());
  if(!test.check("System::invalid(obliquity = NAN)", !s.is_valid())) n++;
  s.orientation(1.0 * Unit::deg, -2.0 * Unit::deg, Equinox::icrs());
  if(!test.check("System::invalid(obliquity OK)", s.is_valid())) n++;

  s.orientation(1.0 * Unit::deg, NAN, Equinox::icrs());
  if(!test.check("System::invalid(ascending_node = NAN)", !s.is_valid())) n++;
  s.orientation(1.0 * Unit::deg, -2.0 * Unit::deg, Equinox::icrs());
  if(!test.check("System::invalid(ascending_node OK)", s.is_valid())) n++;

  s.orientation(1.0 * Unit::deg, -2.0 * Unit::deg, Equinox::invalid());
  if(!test.check("System::invalid(equinox)", !s.is_valid())) n++;
  s.orientation(1.0 * Unit::deg, -2.0 * Unit::deg, Equinox::icrs());
  if(!test.check("System::invalid(equinox OK)", s.is_valid())) n++;

  s.pole(Equatorial::invalid(), Equinox::invalid());
  if(!test.check("System::invalid(pole)", !s.is_valid())) n++;
  s.pole(-92 * Unit::deg, 89.0 * Unit::deg, Equinox::icrs());
  if(!test.check("System::invalid(pole OK)", s.is_valid())) n++;

  std::cout << "Orbital.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
