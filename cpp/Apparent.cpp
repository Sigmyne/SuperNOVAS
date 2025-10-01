/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {


class Apparent {
private:
  Frame _frame;
  enum novas_reference_system _sys;

  sky_pos _pos;

  Apparent(const Frame& f, enum novas_reference_system system) : _frame(f), _sys(system) {}

public:

  Apparent(const Frame& f, const Equatorial& eq, double rv = 0.0, enum novas_reference_system system = NOVAS_TOD)
  : Apparent(f, system) {
    _pos.ra = eq.ra().hours();
    _pos.dec = eq.dec().deg();
    _pos.rv = rv / (Unit::au / Unit::day);
    _pos.dis = NOVAS_DEFAULT_DISTANCE;
    radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
  }

  Apparent(const Frame& f, const Equatorial& eq, const Speed& rv, enum novas_reference_system system = NOVAS_TOD)
  : Apparent(f, eq, rv.ms(), system) {}

  Apparent(const Frame& f, const sky_pos *p, enum novas_reference_system system = NOVAS_TOD)
  : Apparent(f, system) {
    _pos = *p;
  }

  const Frame& frame() const {
    return _frame;
  }

  enum novas_reference_system system() const {
    return _sys;
  }

  const sky_pos *_sky_pos() const {
    return &_pos;
  }

  Angle ra() const {
    return Angle(_pos.ra * Unit::hourAngle);
  }

  Angle dec() const {
    return Angle(_pos.dec * Unit::deg);
  }

  Speed radial_velocity() const {
    return Speed(_pos.rv * Unit::au / Unit::day);
  }

  Distance distance() const {
    return Distance(_pos.dis * Unit::au);
  }

  Equatorial equatorial() const {
    return Equatorial(_pos.ra * Unit::hourAngle, _pos.dec * Unit::deg, _frame.time().epoch_str(), _pos.dis * Unit::au);
  }

  Ecliptic ecliptic() const {
    return equatorial().as_ecliptic();
  }

  Galactic galactic() const {
    return equatorial().as_galactic();
  }

  Horizontal horizontal() const {
    double az = 0.0, el = 0.0;
    novas_app_to_hor(_frame._novas_frame(), _sys, _pos.ra, _pos.dec, NULL, &az, &el);
    return Horizontal(az * Unit::deg, el * Unit::deg, _pos.dis * Unit::au);
  }

  Apparent to_system(enum novas_reference_system system) const {
    if(system == _sys)
      return *this;

    Apparent app = Apparent(_frame, system);
    novas_transform T;

    novas_make_transform(_frame._novas_frame(), _sys, system, &T);
    novas_transform_sky_pos(&_pos, &T, &app._pos);
    return app;
  }
};


} // namespace supernovas


