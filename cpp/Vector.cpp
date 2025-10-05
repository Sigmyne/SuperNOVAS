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
}

Vector::Vector(const double v[3]) : Vector(v[0], v[1], v[2]) {}

bool Vector::is_valid() const {
  return !isnan(_component[0]) && !isnan(_component[1]) && !isnan(_component[2]);
}

bool Vector::is_equal(const Vector& v, double precision) const {
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

std::string Vector::str() const {
  char sx[20] = {'\0'}, sy[20] = {'\0'}, sz[20] = {'\0'};
  snprintf(sx, sizeof(sx), "%.6g", _component[0]);
  snprintf(sy, sizeof(sy), "%.6g", _component[1]);
  snprintf(sz, sizeof(sz), "%.6g", _component[2]);
  return "VEC ( " + std::string(sx) + ", " + std::string(sy) + ", " + std::string(sz) + " )";
}

Vector operator*(const Vector& l, double r) {
  return l.scaled(r);
}

Vector operator*(double l, const Vector& r) {
  return r.scaled(l);
}


} // namespace supernovas
