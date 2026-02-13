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
  TestUtil test = TestUtil("EquinoxTest");

  int n = 0;

  Equinox x = Equinox::invalid();
  if(!test.check("invalid()", !x.is_valid())) n++;
  if(!test.check("invalid().is_icrs()", !x.is_icrs())) n++;
  if(!test.check("invalid().is_mod()", !x.is_mod())) n++;
  if(!test.check("invalid().is_true()", !x.is_true())) n++;
  if(!test.check("invalid().jd() ? NAN", isnan(x.jd()))) n++;
  if(!test.check("invalid().epoch() ? NAN", isnan(x.epoch()))) n++;
  if(!test.equals("invalid().system()", x.reference_system(), -1)) n++;

  std::optional<Equinox> o = Equinox::for_reference_system(NOVAS_MOD, NAN);
  if(!test.check("has_value(jd = NAN)", !o.has_value())) n++;

  o = Equinox::for_reference_system((novas_reference_system) -1, NOVAS_JD_J2000);
  if(!test.check("has_value(system = -1)", !o.has_value())) n++;

  o = Equinox::for_reference_system((novas_reference_system) NOVAS_REFERENCE_SYSTEMS, NOVAS_JD_J2000);
  if(!test.check("has_value(system = hi)", !o.has_value())) n++;

  o = Equinox::for_reference_system(NOVAS_TIRS, NOVAS_JD_J2000);
  if(!test.check("has_value(system = TIRS)", !o.has_value())) n++;

  o = Equinox::for_reference_system(NOVAS_ITRS, NOVAS_JD_J2000);
  if(!test.check("has_value(system = ITRS)", !o.has_value())) n++;

  o = Equinox::from_string("blah");
  if(!test.check("invalid('blah')", !o.has_value())) n++;

  o = Equinox::from_string("Z");
  if(!test.check("invalid('Z')", !o.has_value())) n++;

  Equinox icrs = Equinox::icrs();
  if(!test.check("icrs().is_valid()", icrs.is_valid())) n++;
  if(!test.equals("icrs().name()", icrs.name(), "ICRS")) n++;
  if(!test.equals("icrs().reference_system()", icrs.reference_system(), NOVAS_ICRS)) n++;
  if(!test.equals("icrs().jd()", icrs.jd(), NOVAS_JD_J2000)) n++;
  if(!test.equals("icrs().mjd()", icrs.mjd(), NOVAS_JD_J2000 - NOVAS_JD_MJD0)) n++;
  if(!test.equals("icrs().to_string()", icrs.to_string(), NOVAS_SYSTEM_ICRS)) n++;
  if(!test.check("icrs().equals('ICRS')", icrs.equals(Equinox::from_string("ICRS").value(), Interval(Unit::s)))) n++;
  if(!test.check("icrs() == 'ICRS'", (icrs == Equinox::from_string("ICRS").value()))) n++;
  if(!test.check("icrs() == 'BCRS'", (icrs == Equinox::from_string("BCRS").value()))) n++;
  if(!test.check("icrs() == 'GCRS'", (icrs == Equinox::from_string("GCRS").value()))) n++;
  if(!test.check("icrs() == 'HCRS'", (icrs == Equinox::from_string("HCRS").value()))) n++;
  if(!test.check("icrs() == 'FK6'", (icrs == Equinox::from_string("FK6").value()))) n++;
  if(!test.check("icrs() == ICRS", (icrs == Equinox::for_reference_system(NOVAS_ICRS).value(), NAN))) n++;
  if(!test.check("icrs() == GCRS", (icrs == Equinox::for_reference_system(NOVAS_GCRS).value(), NAN))) n++;

  Equinox j2000 = Equinox::j2000();
  if(!test.check("j2000().is_valid()", j2000.is_valid())) n++;
  if(!test.equals("j2000().name()", j2000.name(), "J2000")) n++;
  if(!test.equals("j2000().reference_system()", j2000.reference_system(), NOVAS_J2000)) n++;
  if(!test.equals("j2000().jd()", j2000.jd(), NOVAS_JD_J2000)) n++;
  if(!test.equals("j2000().to_string()", j2000.to_string(), NOVAS_SYSTEM_J2000)) n++;
  if(!test.check("j2000().equals(mod::j2000)", j2000.equals(Equinox::mod(NOVAS_JD_J2000)))) n++;
  if(!test.check("j2000().equals(mod::Time(j2000))", j2000.equals(Equinox::mod(Time::j2000())))) n++;
  if(!test.check("j2000().equals('J2000')", j2000.equals(Equinox::from_string("J2000").value()))) n++;
  if(!test.check("j2000().equals('j2000.00')", j2000.equals(Equinox::from_string("j2000.00").value()))) n++;
  if(!test.check("j2000() == 'J2000'", (j2000 == Equinox::from_string("J2000").value()))) n++;
  if(!test.check("j2000() == 'FK5'", (j2000 == Equinox::from_string("FK5").value()))) n++;
  if(!test.check("j2000() == J2000", (j2000 == Equinox::for_reference_system(NOVAS_J2000).value(), NAN))) n++;
  if(!test.check("j2000() != icrs", (j2000 != icrs))) n++;

  Equinox b1950 = Equinox::b1950();
  if(!test.check("b1950().is_valid()", b1950.is_valid())) n++;
  if(!test.equals("b1950().name()", b1950.name(), "B1950")) n++;
  if(!test.equals("b1950().reference_system()", b1950.reference_system(), NOVAS_MOD)) n++;
  if(!test.equals("b1950().jd()", b1950.jd(), NOVAS_JD_B1950)) n++;
  if(!test.equals("1950().to_string()", b1950.to_string(), NOVAS_SYSTEM_B1950)) n++;
  if(!test.check("b1950().equals(mod::b1950)", b1950.equals(Equinox::mod(NOVAS_JD_B1950)))) n++;
  if(!test.check("b1950().equals(mod::Time(b1950))", b1950.equals(Equinox::mod(Time::b1950())))) n++;
  if(!test.check("b1950().equals('B1950')", b1950.equals(Equinox::from_string("B1950").value()))) n++;
  if(!test.check("b1950().equals('b1950.00')", b1950.equals(Equinox::from_string("b1950.00").value()))) n++;
  if(!test.check("b1950() == 'B1950'", (b1950 == Equinox::from_string("B1950").value()))) n++;
  if(!test.check("b1950() == 'MOD B1950'", (b1950 == Equinox::from_string("MOD B1950").value()))) n++;
  if(!test.check("b1950() == 'FK4'", (b1950 == Equinox::from_string("FK4").value()))) n++;
  if(!test.check("b1950() != icrs", (b1950 != icrs))) n++;


  Equinox b1900 = Equinox::b1900();
  if(!test.check("b1900().is_valid()", b1900.is_valid())) n++;
  if(!test.equals("b1900().name()", b1900.name(), "B1900")) n++;
  if(!test.equals("b1900().reference_system()", b1900.reference_system(), NOVAS_MOD)) n++;
  if(!test.equals("b1900().jd()", b1900.jd(), NOVAS_JD_B1900)) n++;
  if(!test.equals("1900().to_string()", b1900.to_string(), "B1900")) n++;

  Equinox tod_j2000 = Equinox::tod(NOVAS_JD_J2000);
  if(!test.check("tod(j2000).is_valid()", tod_j2000.is_valid())) n++;
  if(!test.equals("tod(j2000).reference_system()", tod_j2000.reference_system(), NOVAS_TOD)) n++;
  if(!test.equals("tod(j2000).name()", tod_j2000.name(), "TOD J2000")) n++;
  if(!test.equals("tod(j2000).jd()", tod_j2000.jd(), NOVAS_JD_J2000)) n++;
  if(!test.equals("tod(j2000).to_string()", tod_j2000.to_string(), "TOD " NOVAS_SYSTEM_J2000)) n++;
  if(!test.check("tod(j2000) == 'tod(Time::j2000)", (tod_j2000 == Equinox::tod(Time::j2000())))) n++;
  if(!test.check("tod(j2000) == 'TOD J2000'", (tod_j2000 == Equinox::from_string("TOD J2000").value()))) n++;
  if(!test.check("tod(j2000) != 'J2000'", (tod_j2000 != Equinox::from_string("J2000").value()))) n++;
  if(!test.check("tod(j2000) != J2000", (tod_j2000 != j2000))) n++;

  Equinox cirs_j2000 = Equinox::cirs(NOVAS_JD_J2000);
  if(!test.check("cirs(j2000).is_valid()", cirs_j2000.is_valid())) n++;
  if(!test.equals("cirs(j2000).reference_system()", cirs_j2000.reference_system(), NOVAS_CIRS)) n++;
  if(!test.equals("cirs(j2000).name()", cirs_j2000.name(), "CIRS J2000")) n++;
  if(!test.equals("cirs(j2000).jd()", cirs_j2000.jd(), NOVAS_JD_J2000)) n++;
  if(!test.equals("cirs(j2000).to_string()", cirs_j2000.to_string(), "CIRS " NOVAS_SYSTEM_J2000)) n++;
  if(!test.check("cirs(j2000) == 'cirs(Time::j2000)", (cirs_j2000 == Equinox::cirs(Time::j2000())))) n++;
  if(!test.check("cirs(j2000) == 'CIRS J2000'", (cirs_j2000 == Equinox::from_string("CIRS J2000").value()))) n++;
  if(!test.check("cirs(j2000) != 'J2000'", (cirs_j2000 != Equinox::from_string("J2000").value()))) n++;
  if(!test.check("cirs(j2000) != J2000", (cirs_j2000 != j2000))) n++;

  Equinox hip = Equinox::hip();
  if(!test.check("hip().is_valid()", hip.is_valid())) n++;
  if(!test.equals("hip().reference_system()", hip.reference_system(), NOVAS_MOD)) n++;
  if(!test.equals("hip().jd()", hip.jd(), NOVAS_JD_HIP)) n++;

  std::cout << "EquinoxTest.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}

