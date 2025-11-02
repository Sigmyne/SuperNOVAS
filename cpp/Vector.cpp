/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

Vector::Vector(double x, double y, double z) {
  _component[0] = x;
  _component[1] = y;
  _component[2] = z;

  if(isnan(abs()))
    novas_error(0, EINVAL, "Vector()", "input has NAN component(s)");
  else
    _valid = true;
}

Vector Vector::operator*(double r) const {
  return scaled(r);
}

double Vector::x() const {
  return _component[0];
}

double Vector::y() const {
  return _component[1];
}

double Vector::z() const {
  return _component[2];
}


bool Vector::equals(const Vector& v, double precision) const {
  double d2 = 0.0;
  for(int i = 0; i < 3; i++) {
    double d = _component[i] - v._component[i];
    d2 += d * d;
  }
  return d2 < precision * precision;
}

Vector Vector::scaled(double factor) const {
  return Vector(_component[0] * factor, _component[1] * factor, _component[2] * factor);
}

const double *Vector::_array() const {
  return _component;
}

double Vector::abs() const {
  return novas_vlen(_component);
}

double Vector::dot(const Vector &v) const {
  return novas_vdot(_component, v._component);
}

double Vector::projection_on(const Vector& v) const {
  return dot(v) / v.abs();
}

std::string Vector::to_string(int decimals) const {
  char sx[40] = {'\0'}, sy[40] = {'\0'}, sz[40] = {'\0'};

  novas_print_decimal(_component[0], decimals, sx, sizeof(sx));
  novas_print_decimal(_component[1], decimals, sy, sizeof(sy));
  novas_print_decimal(_component[2], decimals, sz, sizeof(sz));

  return "VEC (" + std::string(sx) + ", " + std::string(sy) + ", " + std::string(sz) + ")";
}


Vector Vector::unit_vector() const {
  return scaled(1.0 / abs());
}

Vector operator*(double factor, const Vector& v) {
  return v * factor;
}


} // namespace supernovas
