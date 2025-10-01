/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Source {
protected:
  struct novas_object _object = {};

  Source() {}

public:

  const struct novas_object *_novas_object() const {
    return &_object;
  }

  std::string name() const {
    return std::string(_object.name);
  }

  Apparent apparent(const Frame& frame, enum novas_reference_system system = NOVAS_TOD) const {
    sky_pos pos = {};
    novas_sky_pos(&_object, frame._novas_frame(), system, &pos);
    return Apparent(frame, &pos, system);
  }

  Geometric geometric(const Frame& frame, enum novas_reference_system system = NOVAS_TOD) {
    double p[3] = {0.0}, v[3] = {0.0};
    novas_geom_posvel(&_object, frame._novas_frame(), system, p, v);
    return Geometric(frame, system,
            Position(p[0] * Unit::au, p[1] * Unit::au, p[2] * Unit::au),
            Velocity(v[0] * Unit::au / Unit::day, v[1] * Unit::au / Unit::day, v[2] * Unit::au / Unit::day)
            );
  }

  Time rises_above(double el, const Frame &frame, RefractionModel ref, const Weather& weather) const {
    if(ref)
      el = Horizontal(0.0, el * Unit::deg).to_unrefracted(frame, ref, weather).elevation().deg();
    return Time(novas_rises_above(el / Unit::deg, &_object, frame._novas_frame(), NULL), frame.eop());
  }

  Time transits(const Frame &frame) const {
    return Time(novas_transit_time(&_object, frame._novas_frame()), frame.eop());
  }

  Time sets_below(double el, const Frame &frame, RefractionModel ref, const Weather& weather) const {
    if(ref)
      el = Horizontal(0.0, el * Unit::deg).to_unrefracted(frame, ref, weather).elevation().deg();
    return Time(novas_sets_below(el / Unit::deg, &_object, frame._novas_frame(), NULL), frame.eop());
  }

  Angle sun_angle(const Frame& frame) const {
    return Angle(novas_sun_angle(&_object, frame._novas_frame()) * Unit::deg);
  }

  Angle moon_angle(const Frame& frame) const {
    return Angle(novas_moon_angle(&_object, frame._novas_frame()) * Unit::deg);
  }

  Angle angle_from(const Source& source, Frame& frame) const {
    return Angle(novas_object_sep(&_object, &source._object, frame._novas_frame()) * Unit::deg);
  }

};


class CatalogSource : public Source {
private:
  System _system;
public:

  CatalogSource(const CatalogEntry& e)
  : Source(), _system(e.system()) {
    make_cat_object_sys(e._cat_entry(), e.system().c_str(), &_object);
  }

  const cat_entry * _cat_entry() const {
    return &_object.star;
  }

  const System& system() const {
    return _system;
  }

  CatalogEntry catalog_entry() const {
    return CatalogEntry(&_object.star, _object.star.starname);
  }

};


class SolarSystemSource : public Source {
protected:
  SolarSystemSource() : Source() {}

public:
  double solar_illumination(const Frame& frame) const {
    return novas_solar_illum(&_object, frame._novas_frame());
  }

  double helio_distance(const Time& time, double *rate = NULL) const {
    return novas_helio_dist(time.jd(NOVAS_TDB), &_object, rate);
  }

  double solar_power(const Time& time) const {
    return novas_solar_power(time.jd(NOVAS_TDB), &_object);
  }
};



class Planet : public SolarSystemSource {
public:
  Planet(enum novas_planet number) : SolarSystemSource() {
    make_planet(number, &_object);
  }

  Planet(const std::string& name) : SolarSystemSource() {
    make_planet(novas_planet_for_name(name.c_str()), &_object);
  }
};


class EphemerisSource : public SolarSystemSource {
public:
  EphemerisSource(const std::string &name, long number) : SolarSystemSource() {
    make_ephem_object(name.c_str(), number, &_object);
  }
};


class OrbitalSource : public SolarSystemSource {
public:
  OrbitalSource(const std::string& name, long number, const novas_orbital *orbit) : SolarSystemSource() {
    make_orbital_object(name.c_str(), number, orbit, &_object);
  }
};


} // namespace supernovas


