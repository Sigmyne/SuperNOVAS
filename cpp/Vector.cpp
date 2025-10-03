/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "supernovas.h"


using namespace novas;

namespace supernovas {

Vector::Vector(double x, double y, double z) {
  _component[0] = x;
  _component[1] = y;
  _component[2] = z;
}

Vector::Vector(const double v[3]) : Vector(v[0], v[1], v[2]) {}

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

Vector operator*(const Vector& l, double r) {
  return l.scaled(r);
}

Vector operator*(double l, const Vector& r) {
  return r.scaled(l);
}

} // namespace supernovas
