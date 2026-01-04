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

/**
 * Instantiates a vector from its cartesian components.
 *
 * @param x   [arb.u.] _x_ component
 * @param y   [arb.u.] _y_ component
 * @param z   [arb.u.] _z_ component
 */
Vector::Vector(double x, double y, double z) {
  _component[0] = x;
  _component[1] = y;
  _component[2] = z;

  if(isnan(abs()))
    novas_error(0, EINVAL, "Vector()", "input has NAN component(s)");
  else
    _valid = true;
}

/**
 * Scales a vector by a factor, for example to cast it to/from a physical unit.
 *
 * @param r   scaling factor on the right-hand side
 * @return    a new vector that is the rescaled version of this vector.
 *
 * @sa operator*(double, Vector&)
 */
Vector Vector::operator*(double r) const {
  return scaled(r);
}

/**
 * Returns the _x_ component of this vector.
 *
 * @return    the _x_ component
 *
 * @sa y(), z()
 */
double Vector::x() const {
  return _component[0];
}

/**
 * Returns the _y_ component of this vector.
 *
 * @return    the _y_ component
 *
 * @sa x(), z()
 */
double Vector::y() const {
  return _component[1];
}

/**
 * Returns the _z_ component of this vector.
 *
 * @return    the _z_ component
 *
 * @sa x(), y()
 */
double Vector::z() const {
  return _component[2];
}

/**
 * Checks if this vector is the same as another vector, within the specified precision.
 *
 * @param v           the reference vector
 * @param precision   precision for the equality test
 * @return            `true` if this vector is equal to the argument within the specified
 *                    precision, or else `false`.
 */
bool Vector::equals(const Vector& v, double precision) const {
  double d2 = 0.0;
  for(int i = 0; i < 3; i++) {
    double d = _component[i] - v._component[i];
    d2 += d * d;
  }
  return d2 < precision * precision;
}

/**
 * Returns a rescaled version of this vector, for example to cast it to/from a physical unit.
 *
 * @param factor    scaling factor, such as a cast to/from a physical unit
 * @return          a new vector that is the rescaled version of this vector with the specified
 *                  scaling factor.
 */
Vector Vector::scaled(double factor) const {
  return Vector(_component[0] * factor, _component[1] * factor, _component[2] * factor);
}

/**
 * Returns the underlying `double[3]` C array that stores the components of this vector
 * internally. Such an array may be used with the SuperNOVAS C functions.
 *
 * @return  the uderlying `double[3]` array that stores the components of this vector.
 */
const double *Vector::_array() const {
  return _component;
}

/**
 * Returns the absolute value (length / magnitude) of this vector.
 *
 * @return    the absolute value (length / magnitude) of this vector.
 */
double Vector::abs() const {
  return novas_vlen(_component);
}

/**
 * Returns the dot product of this vector and the specified other vector
 *
 * @param v   the other vector
 * @return    the dot product of this vector and the argument.
 */
double Vector::dot(const Vector &v) const {
  return novas_vdot(_component, v._component);
}

/**
 * Returns the projection of this vector along the direction of another vector.
 *
 * @param v   the other vector
 * @return    the projection of this vector along the direction of the argument vector.
 */
double Vector::projection_on(const Vector& v) const {
  return dot(v) / v.abs();
}

/**
 * Returns a string representation of this vector
 *
 * @param decimals    (optional) the numberof decimal places to print for the components
 *                    (default: 3).
 * @return
 */
std::string Vector::to_string(int decimals) const {
  char sx[40] = {'\0'}, sy[40] = {'\0'}, sz[40] = {'\0'};

  novas_print_decimal(_component[0], decimals, sx, sizeof(sx));
  novas_print_decimal(_component[1], decimals, sy, sizeof(sy));
  novas_print_decimal(_component[2], decimals, sz, sizeof(sz));

  return "VEC (" + std::string(sx) + ", " + std::string(sy) + ", " + std::string(sz) + ")";
}

/**
 * Returns a unit vector in the direction of this vector.
 *
 * @return    a new unit vector in the same direction as this vector.
 */
Vector Vector::unit_vector() const {
  return scaled(1.0 / abs());
}

/**
 * Returns a scaled version of the vector in the right-hand side with the factor on the left-hand
 * side
 *
 * @param factor    the scaling factor on the left-hand side
 * @param v         the vector on the right-hand side
 * @return          a new vector that is the scaled version of the input vector by the specified
 *                  factor.
 *
 * @sa Vector::operator*(double)
 */
Vector operator*(double factor, const Vector& v) {
  return v * factor;
}


} // namespace supernovas
