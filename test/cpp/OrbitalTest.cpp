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
  if(!test.equals("System::reference_system()", s.reference_system(), NOVAS_ICRS)) n++;

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

  if(!test.check("System::from_orbital_system(NULL)", !OrbitalSystem::from_novas_orbital_system(NULL).has_value())) n++;

  novas_orbital_system *ns = (novas_orbital_system *) s._novas_orbital_system();
  if(!test.check("System(novas)::is_valid()", OrbitalSystem::from_novas_orbital_system(ns).value().is_valid())) n++;

  ns->center = (novas_planet) -1;
  if(!test.check("System(novas) invalid(center)", !OrbitalSystem::from_novas_orbital_system(ns).value().is_valid())) n++;

  ns->center = NOVAS_JUPITER;
  ns->plane = (novas_reference_plane) -1;
  if(!test.check("System(novas) invalid(plane)", !OrbitalSystem::from_novas_orbital_system(ns).value().is_valid())) n++;

  ns->plane = NOVAS_EQUATORIAL_PLANE;
  ns->type = (novas_reference_system) -1;
  if(!test.check("System(novas) invalid(type)", !OrbitalSystem::from_novas_orbital_system(ns).value().is_valid())) n++;

  ns->type = NOVAS_ICRS;
  ns->obl = NAN;
  if(!test.check("System(novas) invalid(obl = NAN)", !OrbitalSystem::from_novas_orbital_system(ns).value().is_valid())) n++;

  ns->obl = 0.0;
  ns->Omega = NAN;
  if(!test.check("System(novas) invalid(Omega = NAN)", !OrbitalSystem::from_novas_orbital_system(ns).value().is_valid())) n++;

  ns->Omega = 0.0;
  if(!test.check("System(novas) OK", OrbitalSystem::from_novas_orbital_system(ns).value().is_valid())) n++;


  // ------------------------------------------------------------------------
  OrbitalSystem xs = (OrbitalSystem::equatorial(Planet((novas_planet) -1)));
  s = (OrbitalSystem::equatorial());

  if(!test.check("invalid(system)", !Orbital(xs, Time::j2000(), Distance(Unit::AU), Angle(0.0), Interval(Unit::yr)).is_valid())) n++;
  if(!test.check("invalid(time)", !Orbital(s, Time::invalid(), Distance(Unit::AU), Angle(0.0), Interval(Unit::yr)).is_valid())) n++;
  if(!test.check("invalid(a = NAN)", !Orbital(s, Time::j2000(), Distance(NAN), Angle(0.0), Interval(Unit::yr)).is_valid())) n++;
  if(!test.check("invalid(a = 0)", !Orbital(s, Time::j2000(), Distance(0.0), Angle(0.0), Interval(Unit::yr)).is_valid())) n++;
  if(!test.check("invalid(a < 0)", !Orbital(s, Time::j2000(), Distance(-Unit::AU), Angle(0.0), Interval(Unit::yr)).is_valid())) n++;
  if(!test.check("invalid(M0 = NAN)", !Orbital(s, Time::j2000(), Distance(Unit::AU), Angle(NAN), Interval(Unit::yr)).is_valid())) n++;
  if(!test.check("invalid(T = NAN)", !Orbital(s, Time::j2000(), Distance(Unit::AU), Angle(0.0), Interval(NAN)).is_valid())) n++;
  if(!test.check("invalid(T = 0)", !Orbital(s, Time::j2000(), Distance(Unit::AU), Angle(0.0), Interval(0.0)).is_valid())) n++;
  if(!test.check("invalid(T < 0)", !Orbital(s, Time::j2000(), Distance(Unit::AU), Angle(0.0), Interval(-1.0)).is_valid())) n++;
  if(!test.check("invalid(n = 0)", !Orbital::with_mean_motion(s, Time::j2000(), Distance(Unit::AU), Angle(0.0), 0.0).is_valid())) n++;



  Orbital o(s, Time::j2000(), Distance(Unit::AU), Angle(-1.0), Interval(Unit::yr));
  if(!test.check("is_valid()", o.is_valid())) n++;
  if(!test.equals("reference_jd_tdb()", o.reference_jd_tdb(), NOVAS_JD_J2000, 1e-6)) n++;
  if(!test.equals("semi_major_axis()", o.semi_major_axis().au(), 1.0, 1e-15)) n++;
  if(!test.equals("mean_anomaly()", o.reference_mean_anomaly().rad(), -1.0, 1e-15)) n++;
  if(!test.equals("mean_motion()", o.mean_motion(), Constant::two_pi / Unit::yr, 1e-14 * o.mean_motion())) n++;
  if(!test.equals("period()", o.period().years(), 1.0, 1e-15)) n++;

  o.eccentricity(NAN, Angle(0.0));
  if(!test.check("invalid(e = NAN)", !o.is_valid())) n++;

  o.eccentricity(-1e-6, Angle(0.0));
  if(!test.check("invalid(e < 0)", !o.is_valid())) n++;

  o.eccentricity(1.0, Angle(0.0));
  if(!test.check("eccentricity(1.0)", o.is_valid())) n++;
  if(!test.equals("eccentricity()", o.eccentricity(), 1.0, 1e-15)) n++;

  o.eccentricity(1.0, Angle(NAN));
  if(!test.check("invalid(omega = NAN)", !o.is_valid())) n++;

  o.eccentricity(1.0, Angle(2.2));
  if(!test.check("omega(2.2)", o.is_valid())) n++;
  if(!test.equals("periapsis()", o.periapsis().rad(), 2.2, 1e-15)) n++;

  o.inclination(Angle(NAN), Angle(0.0));
  if(!test.check("invalid(i = NAN)", !o.is_valid())) n++;

  o.inclination(Angle(1.0), Angle(0.0));
  if(!test.check("inclination(1.0)", o.is_valid())) n++;
  if(!test.equals("inclination()", o.inclination().rad(), 1.0, 1e-15)) n++;

  o.inclination(Angle(1.0), Angle(NAN));
  if(!test.check("invalid(Omega = NAN)", !o.is_valid())) n++;

  o.inclination(Angle(1.0), Angle(-3.1));
  if(!test.check("Omega(-3.1)", o.is_valid())) n++;
  if(!test.equals("ascending_node()", o.ascending_node().rad(), -3.1, 1e-15)) n++;

  o.inclination(Angle(1.0 * Unit::deg), Angle(30.0 * Unit::deg));
  if(!test.equals("pole() lon", o.pole().longitude().deg(), -60.0, 1e-13)) n++;
  if(!test.equals("pole() lat", o.pole().latitude().deg(), 89.0, 1e-13)) n++;

  o.pole(Spherical(NAN, 80.0 * Unit::deg));
  if(!test.check("invalid(pole.lon = NAN)", !o.is_valid())) n++;

  o.pole(Spherical(30.0 * Unit::deg, NAN));
  if(!test.check("invalid(pole.lat = NAN)", !o.is_valid())) n++;

  o.pole(Spherical(30.0 * Unit::deg, 80 * Unit::deg));
  if(!test.check("pole(30 deg, 80 deg)", o.is_valid())) n++;
  if(!test.equals("pole().longitude()", o.pole().longitude().deg(), 30.0, 1e-13)) n++;
  if(!test.equals("pole().lattitude()", o.pole().latitude().deg(), 80.0, 1e-13)) n++;

  o.apsis_period(Interval(NAN));
  if(!test.check("invalid(apsis period = NAN)", !o.is_valid())) n++;

  o.apsis_period(Interval(Unit::julian_century));
  if(!test.check("invalid(apsis period = 1 cy)", o.is_valid())) n++;
  if(!test.equals("apsis_period(T)", o.apsis_period().julian_centuries(), 1.0, 1e-15)) n++;
  if(!test.equals("apsis_rate(T)", o.apsis_rate(), Constant::two_pi / Unit::julian_century, 1e-14 * o.apsis_rate())) n++;

  o.apsis_rate(NAN);
  if(!test.check("invalid(apsis rate = NAN)", !o.is_valid())) n++;

  o.apsis_rate(Constant::two_pi / Unit::julian_century);
  if(!test.check("invalid(apsis rate = 2pi / 1 cy)", o.is_valid())) n++;
  if(!test.equals("apsis_period(beta)", o.apsis_period().julian_centuries(), 1.0, 1e-15)) n++;
  if(!test.equals("apsis_rate(beta)", o.apsis_rate(), Constant::two_pi / Unit::julian_century, 1e-14 * o.apsis_rate())) n++;

  o.node_period(Interval(NAN));
  if(!test.check("invalid(node period = NAN)", !o.is_valid())) n++;

  o.node_period(Interval(Unit::julian_century));
  if(!test.check("invalid(node period = 1 cy)", o.is_valid())) n++;
  if(!test.equals("node_period(T)", o.node_period().julian_centuries(), 1.0, 1e-15)) n++;
  if(!test.equals("node_rate(T)", o.node_rate(), Constant::two_pi / Unit::julian_century, 1e-14 * o.node_rate())) n++;

  o.node_rate(NAN);
  if(!test.check("invalid(node rate = NAN)", !o.is_valid())) n++;

  o.node_rate(Constant::two_pi / Unit::julian_century);
  if(!test.check("invalid(node rate = 2pi / 1 cy)", o.is_valid())) n++;
  if(!test.equals("node_period(beta)", o.node_period().julian_centuries(), 1.0, 1e-15)) n++;
  if(!test.equals("node_rate(beta)", o.node_rate(), Constant::two_pi / Unit::julian_century, 1e-14 * o.node_rate())) n++;


  double p[3] = {0.0}, v[3] = {0.0};

  novas_orbit_posvel(NOVAS_JD_HIP, o._novas_orbital(), NOVAS_FULL_ACCURACY, p, v);

  if(!test.check("position()", o.position(Time::hip(), NOVAS_FULL_ACCURACY) == Position(p, Unit::au))) n++;
  if(!test.check("velocity()", o.velocity(Time::hip(), NOVAS_FULL_ACCURACY) == Velocity(v, Unit::au / Unit::day))) n++;

  if(!test.check("position(time invalid)", !o.position(Time::invalid()).is_valid())) n++;
  if(!test.check("velocity(time invalid)", !o.velocity(Time::invalid()).is_valid())) n++;

  if(!test.check("position(acc invalid)", !o.position(Time::hip(), (enum novas_accuracy) -1).is_valid())) n++;
  if(!test.check("velocity(acc invalid)", !o.velocity(Time::hip(), (enum novas_accuracy) -1).is_valid())) n++;

  const novas_orbital *no0 = o._novas_orbital();
  novas_orbital no = *no0;
  if(!test.check("from_novas_orbit()", Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  if(!test.check("from_novas_orbit(NULL)", !Orbital::from_novas_orbit(NULL).has_value())) n++;

  no.jd_tdb = NAN;
  if(!test.check("from_novas_orbit(jd_tdb = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.jd_tdb = NOVAS_JD_J2000;
  if(!test.check("from_novas_orbit(jd_tdb = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  no.a = NAN;
  if(!test.check("from_novas_orbit(a = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.a = 0.0;
  if(!test.check("from_novas_orbit(a = 0)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.a = -Unit::au;
  if(!test.check("from_novas_orbit(a < 0)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.a = Unit::au;
  if(!test.check("from_novas_orbit(a = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.M0 = NAN;
  if(!test.check("from_novas_orbit(M0 = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.M0 = -1.0;
  if(!test.check("from_novas_orbit(M0 = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  no.n = NAN;
  if(!test.check("from_novas_orbit(n = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.n = 0.0;
  if(!test.check("from_novas_orbit(n = 0)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.n = -Constant::pi / Unit::yr;
  if(!test.check("from_novas_orbit(n < 0)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.n = Constant::pi / Unit::yr;
  if(!test.check("from_novas_orbit(n = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  no.e = NAN;
  if(!test.check("from_novas_orbit(e = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.e = -1e-6;
  if(!test.check("from_novas_orbit(e < 0)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.e = 1.0;
  if(!test.check("from_novas_orbit(e = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  no.omega = NAN;
  if(!test.check("from_novas_orbit(omega = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.omega = -1.0;
  if(!test.check("from_novas_orbit(omega = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  no.i = NAN;
  if(!test.check("from_novas_orbit(i = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.i = 1.0 * Unit::deg;
  if(!test.check("from_novas_orbit(i = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  no.Omega = NAN;
  if(!test.check("from_novas_orbit(Omega = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.Omega = -0.3;
  if(!test.check("from_novas_orbit(Omega = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  no.apsis_period = NAN;
  if(!test.check("from_novas_orbit(apsis period = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.apsis_period = -1.0 / Unit::julian_century;
  if(!test.check("from_novas_orbit(apsis period = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  no.node_period = NAN;
  if(!test.check("from_novas_orbit(node period = NAN)", !Orbital::from_novas_orbit(&no).value().is_valid())) n++;
  no.node_period = -1.0 / Unit::julian_century;
  if(!test.check("from_novas_orbit(node period = OK)", Orbital::from_novas_orbit(&no).value().is_valid())) n++;

  std::cout << "Orbital.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
