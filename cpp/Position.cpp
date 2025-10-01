/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cstring>

#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "supernovas.h"


namespace supernovas {

class Position : public Vector<Distance> {
public:

  Position(double x = 0.0, double y = 0.0, double z = 0.0)
  : Vector(x, y, z) {}

  Position(const double pos[3], double unit = 1.0)
  : Vector(pos[0] * unit, pos[1] * unit, pos[2] * unit) {}

  Position operator+(const Position& r) const {
    return Position(_component[0] + r._component[0], _component[1] + r._component[1], _component[2] + r._component[2]);
  }

  Position operator-(const Position &r) const {
    return Position(_component[0] - r._component[0], _component[1] - r._component[1], _component[2] - r._component[2]);
  }

  double x_m() const {
    return _component[0];
  }

  double y_m() const {
    return _component[1];
  }

  double z_m() const {
    return _component[2];
  }

  Distance x() const {
    return Distance(_component[0]);
  }

  Distance y() const {
    return Distance(_component[1]);
  }

  Distance z() const {
    return Distance(_component[2]);
  }

  Distance length() const {
    return Distance(abs());
  }

  Distance distance_to(const Position& pos) const {
    return Distance(novas_vdist(_component, pos._component));
  }

  Position inv() const {
    return Position(-_component[0], -_component[1], -_component[2]);
  }

  Spherical as_spherical() const {
    double longitude = atan2(_component[1], _component[0]);
    double xy = hypot(_component[0], _component[1]);
    double latitude = atan2(_component[2], xy);
    return Spherical(isnan(longitude) ? 0.0 : longitude, isnan(latitude) ? 0.0 : latitude, abs());
  }

  static Position origin() {
    return Position(0.0, 0.0, 0.0);
  }
};

} // namespace supernovas

