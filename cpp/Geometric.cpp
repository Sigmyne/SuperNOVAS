/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Geometric {
private:
  Frame _frame;
  enum novas_reference_system _sys;

  Position _pos;
  Velocity _vel;

public:

  Geometric(const Frame& frame, enum novas_reference_system system, const Position& p, const Velocity& v)
  : _frame(frame), _sys(system) , _pos(p), _vel(v) {}

  const Frame& frame() const {
    return _frame;
  }

  enum novas_reference_system system() const {
    return _sys;
  }

  const Position& position() const {
    return _pos;
  }

  const Velocity& velocity() const {
    return _vel;
  }

  Equatorial equatorial() const {
    return Equatorial(_pos);
  }

  Ecliptic ecliptic() const {
    return equatorial().as_ecliptic();
  }

  Galactic galactic() const {
    return equatorial().as_galactic();
  }

  Geometric to_system(enum novas_reference_system system) const {
    if(system == _sys)
      return *this;

    novas_transform T = {};
    double p[3] = {0.0}, v[3] = {0.0};

    novas_make_transform(_frame._novas_frame(), _sys, system, &T);
    novas_transform_vector(_pos._array(), &T, p);
    novas_transform_vector(_vel._array(), &T, v);

    return Geometric(_frame, _sys, p, v);
  }

};

} // namespace supernovas


