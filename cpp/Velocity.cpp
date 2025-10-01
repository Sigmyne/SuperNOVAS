/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Velocity : public Vector<Speed> {
protected:

  Velocity cast(double u) {
    return Velocity(_component[0] / u, _component[1] / u, _component[2] / u);
  }

public:
  Velocity(double x = 0.0, double y = 0.0, double z = 0.0)
  : Vector(x, y, z) {}

  Velocity(const double vel[3], double unit = 1.0)
  : Vector(vel[0] * unit, vel[1] * unit, vel[2] * unit) {}

  Velocity operator+(const Velocity& r) const {
    return Velocity(_component[0] + r._component[0], _component[1] + r._component[1], _component[2] + r._component[2]);
  }

  Velocity operator-(const Velocity &r) const {
    return Velocity(_component[0] - r._component[0], _component[1] - r._component[1], _component[2] - r._component[2]);
  }

  double x_ms() const {
    return _component[0];
  }

  double y_ms() const {
    return _component[1];
  }

  double z_ms() const {
    return _component[2];
  }

  Speed x() const {
    return Speed(_component[0]);
  }

  Speed y() const {
    return Speed(_component[1]);
  }

  Speed z() const {
    return Speed(_component[2]);
  }

  Speed length() const {
    return Speed(abs());
  }

  Speed along(Vector v) const {
    return Speed(dot(v) / v.abs());
  }

  Velocity inv() const {
    return Velocity(-_component[0], -_component[1], -_component[2]);
  }

};


} // namespace supernovas

