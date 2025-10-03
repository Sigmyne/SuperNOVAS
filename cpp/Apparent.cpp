/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace supernovas;
using namespace novas;


Apparent::Apparent(const Frame& f, enum novas_reference_system system)
: _frame(f), _sys(system) {}

Apparent::Apparent(const Frame& f, const Equatorial& eq, double rv, enum novas_reference_system system)
: Apparent(f, system) {
  _pos.ra = eq.ra().hours();
  _pos.dec = eq.dec().deg();
  _pos.rv = rv / (Unit::au / Unit::day);
  _pos.dis = NOVAS_DEFAULT_DISTANCE;
  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}

Apparent::Apparent(const Frame& f, const Equatorial& eq, const Speed& rv, enum novas_reference_system system)
: Apparent(f, eq, rv.ms(), system) {}

Apparent::Apparent(const Frame& f, const sky_pos *p, enum novas_reference_system system)
: Apparent(f, system) {
  _pos = *p;
}

const Frame& Apparent::frame() const {
  return _frame;
}

enum novas_reference_system Apparent::system() const {
  return _sys;
}

const sky_pos *Apparent::_sky_pos() const {
  return &_pos;
}

Angle Apparent::ra() const {
  return Angle(_pos.ra * Unit::hourAngle);
}

Angle Apparent::dec() const {
  return Angle(_pos.dec * Unit::deg);
}

Speed Apparent::radial_velocity() const {
  return Speed(_pos.rv * Unit::au / Unit::day);
}

Distance Apparent::distance() const {
  return Distance(_pos.dis * Unit::au);
}

Equatorial Apparent::equatorial() const {
  return Equatorial(_pos.ra * Unit::hourAngle, _pos.dec * Unit::deg, _frame.time().epoch_str(), _pos.dis * Unit::au);
}

Ecliptic Apparent::ecliptic() const {
  return equatorial().as_ecliptic();
}

Galactic Apparent::galactic() const {
  return equatorial().as_galactic();
}

Horizontal Apparent::horizontal() const {
  double az = 0.0, el = 0.0;
  novas_app_to_hor(_frame._novas_frame(), _sys, _pos.ra, _pos.dec, NULL, &az, &el);
  return Horizontal(az * Unit::deg, el * Unit::deg, _pos.dis * Unit::au);
}

Apparent Apparent::to_system(enum novas_reference_system system) const {
  if(system == _sys)
    return *this;

  Apparent app = Apparent(_frame, system);
  novas_transform T;

  novas_make_transform(_frame._novas_frame(), _sys, system, &T);
  novas_transform_sky_pos(&_pos, &T, &app._pos);
  return app;
}



