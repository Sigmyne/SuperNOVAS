/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "supernovas.h"


namespace supernovas {

class Vector {
protected:
  double _component[3] = {0.0};

public:

  Vector(double x = 0.0, double y = 0.0, double z = 0.0) {
    _component[0] = x;
    _component[1] = y;
    _component[2] = z;
  }

  Vector(const double v[3]) : Vector(v[0], v[1], v[2]) {}

  Vector scaled(double factor) {
    return Vector(_component[0] * factor, _component[1] * factor, _component[2] * factor);
  }

  const double *_array() const {
    return _component;
  }

  double abs() const {
    return novas_vlen(_component);
  }

  double dot(const Vector &v) const {
    return novas_vdot(_component, v._component);
  }
};

} // namespace supernovas


