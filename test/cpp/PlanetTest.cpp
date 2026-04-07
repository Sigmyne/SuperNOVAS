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
  TestUtil test = TestUtil("Planet");

  int n = 0;

  Planet x = Planet((enum novas_planet) -1);
  if(!test.check("(invalid)", !x.is_valid())) n++;
  if(!test.equals("type(invalid)", x.type(), NOVAS_PLANET)) n++;
  if(!test.equals("novas_id(invalid)", x.novas_id(), -1)) n++;
  if(!test.equals("naif_id(invalid)", x.naif_id(), -1)) n++;
  if(!test.equals("de_number_id(invalid)", x.de_number(), -1)) n++;
  if(!test.check("mass(invalid)", isnan(x.mass()))) n++;
  if(!test.check("mean_radius(invalid)", !x.mean_radius().is_valid())) n++;

  if(!test.check("for_name(invalid)", !Planet::for_name("blah").is_valid())) n++;
  if(!test.check("for_naif_id(invalid)", !Planet::for_naif_id(-1).is_valid())) n++;

  for(int i = 0; i < NOVAS_PLANETS; i++) {
    Planet pl = Planet((enum novas_planet) i);
    if(!test.check(std::to_string(i), pl.is_valid())) n++;
    if(!test.equals("novas_id(" + std::to_string(i) + ")", pl.novas_id(), i)) n++;
    if(!test.equals("naif_id(" + std::to_string(i) + ")", pl.naif_id(), (int) novas_to_naif_planet(pl.novas_id()))) n++;
    if(!test.equals("de_number(" + std::to_string(i) + ")", pl.de_number(), (int) novas_to_dexxx_planet(pl.novas_id()))) n++;

    Orbital orb = pl.orbit(Time::b1950());
    if((i >= NOVAS_MERCURY && i != NOVAS_EARTH && i <= NOVAS_PLUTO) || i == NOVAS_PLUTO_BARYCENTER) {
      novas_orbital orb0 = {};
      novas_make_planet_orbit(pl.novas_id(), NOVAS_JD_B1950, &orb0);
      if(!test.check("orbit(" + std::to_string(i) + ")", orb.is_valid())) n++;
      if(!test.check("orbit(" + std::to_string(i) + ") == ", memcmp(orb._novas_orbital(), &orb0, sizeof(novas_orbital)))) n++;
    }
    else {
      if(!test.check("orbit(" + std::to_string(i) + ")", !orb.is_valid())) n++;
    }

    Planet opt = Planet::for_naif_id(novas_to_naif_planet((enum novas_planet) i));
    if(!test.check("for_naif_id(" + std::to_string(i) + ").is_valid()", opt.is_valid())) n++;
    if(!test.equals("for_naif_id(" + std::to_string(i) + ").novas_id()", opt.novas_id(), i)) n++;

    const Source *p1 = pl.copy();
    if(!test.check("to_string(catalog)", memcmp(p1->_novas_object(), pl._novas_object(), sizeof(object)) == 0)) n++;
  }



  std::string names[] = NOVAS_PLANET_NAMES_INIT;
  double radius[] = NOVAS_PLANET_RADII_INIT;
  double rmass[] = NOVAS_RMASS_INIT;

  for(int i = 0; i < NOVAS_PLANETS; i++) {
     Planet pl = Planet::for_name(names[i]);
     if(!test.check("for_name(" + names[i] + ")", pl.is_valid())) n++;
     if(!test.equals("for_name(" + names[i] + ").novas_id()", pl.novas_id(), i)) n++;
     if(!test.equals("type(" + names[i] + ")", pl.type(), NOVAS_PLANET)) n++;
     if(!test.equals("mean_radius(" + std::to_string(i) + ")", pl.mean_radius().m(), radius[i], 1e-3)) n++;
     if(!test.equals("mean_radius(" + std::to_string(i) + ")", pl.mass(), Constant::M_sun / rmass[i], 1e13)) n++;
     if(!test.equals("to_string(" + names[i] + ")", pl.to_string(), "Planet " + pl.name())) n++;
  }

  if(!test.equals("ssb()", Planet::ssb().novas_id(), NOVAS_SSB)) n++;
  if(!test.equals("sun()", Planet::sun().novas_id(), NOVAS_SUN)) n++;
  if(!test.equals("mercury()", Planet::mercury().novas_id(), NOVAS_MERCURY)) n++;
  if(!test.equals("venus()", Planet::venus().novas_id(), NOVAS_VENUS)) n++;
  if(!test.equals("earth()", Planet::earth().novas_id(), NOVAS_EARTH)) n++;
  if(!test.equals("moon()", Planet::moon().novas_id(), NOVAS_MOON)) n++;
  if(!test.equals("emb()", Planet::emb().novas_id(), NOVAS_EMB)) n++;
  if(!test.equals("mars()", Planet::mars().novas_id(), NOVAS_MARS)) n++;
  if(!test.equals("jupiter()", Planet::jupiter().novas_id(), NOVAS_JUPITER)) n++;
  if(!test.equals("saturn()", Planet::saturn().novas_id(), NOVAS_SATURN)) n++;
  if(!test.equals("uranus()", Planet::uranus().novas_id(), NOVAS_URANUS)) n++;
  if(!test.equals("neptune()", Planet::neptune().novas_id(), NOVAS_NEPTUNE)) n++;
  if(!test.equals("pluto()", Planet::pluto().novas_id(), NOVAS_PLUTO)) n++;
  if(!test.equals("pluto_barycenter()", Planet::pluto_system().novas_id(), NOVAS_PLUTO_BARYCENTER)) n++;

  if(!test.check("approx_apparent_in(invalid)", !Planet::mars().approx_apparent_in(Frame::undefined()).is_valid())) n++;

  Frame frame = Observer::at_geocenter().reduced_accuracy_frame_at(Time::j2000());
  Apparent app = Planet::mars().approx_apparent_in(frame);
  sky_pos pos = {};
  novas_approx_sky_pos(NOVAS_MARS, frame._novas_frame(), NOVAS_TOD, &pos);

  if(!test.check("approx_apparent_in(mars)", app.is_valid())) n++;
  if(!test.equals("approx_apparent_in(mars).ra", app.equatorial().ra().hours(), pos.ra, 1e-13)) n++;
  if(!test.equals("approx_apparent_in(mars).dec", app.equatorial().dec().deg(), pos.dec, 1e-12)) n++;
  if(!test.equals("approx_apparent_in(mars).radial_velocity", app.radial_velocity().km_per_s(), pos.rv, 1e-10)) n++;

  Geometric geom = Planet::jupiter().approx_geometric_in(frame);
  double p[3] = {0.0}, v[3] = {0.0};
  novas_approx_heliocentric(NOVAS_JUPITER, frame.jd(NOVAS_TDB), p, v);
  const novas_frame *f = frame._novas_frame();

  if(!test.check("approx_geometric_in(invalid)", !Planet::mars().approx_geometric_in(Frame::undefined()).is_valid())) n++;

  if(!test.check("approx_geometric_in(jupiter)", geom.is_valid())) n++;
  if(!test.check("approx_geometric_in(jupiter).position()", geom.position() == (Position(p, Unit::AU) + Position(f->sun_pos, Unit::AU)))) n++;
  if(!test.check("approx_geometric_in(jupiter).velocity()", geom.velocity() == (Velocity(v, Unit::AU_per_day) + Velocity(f->sun_vel, Unit::AU_per_day)))) n++;

  std::cout << "Planet.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
