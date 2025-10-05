/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace novas;

namespace supernovas {

Geometric::Geometric(const Frame& frame, enum novas_reference_system system, const Position& p, const Velocity& v)
          : _frame(frame), _sys(system) , _pos(p), _vel(v) {}

bool Geometric::is_valid() const {
  return _frame.is_valid() && _sys >= 0 && _sys < NOVAS_REFERENCE_SYSTEMS && _pos.is_valid() && _vel.is_valid();
}

const Frame& Geometric::frame() const {
  return _frame;
}

enum novas_reference_system Geometric::system() const {
  return _sys;
}

const Position& Geometric::position() const {
  return _pos;
}

const Velocity& Geometric::velocity() const {
  return _vel;
}

Equatorial Geometric::equatorial() const {
  return Equatorial(_pos);
}

Ecliptic Geometric::ecliptic() const {
  return equatorial().as_ecliptic();
}

Galactic Geometric::galactic() const {
  return equatorial().as_galactic();
}

Geometric Geometric::in_system(enum novas_reference_system system) const {
  if(system == _sys)
    return *this;

  novas_transform T = {};
  double p[3] = {0.0}, v[3] = {0.0};

  novas_make_transform(_frame._novas_frame(), _sys, system, &T);
  novas_transform_vector(_pos._array(), &T, p);
  novas_transform_vector(_vel._array(), &T, v);

  return Geometric(_frame, _sys, p, v);
}

static const Geometric _invalid = Geometric(Frame::invalid(), (enum novas_reference_system) -1, Position::invalid(), Velocity::invalid());
const Geometric& Geometric::invalid() {
  return _invalid;
}

} // namespace supernovas



