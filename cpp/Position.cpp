/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cstring>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"


namespace supernovas {

/**
 * Instantiates a new 3D cartesian position vector from its components in S.I. units.
 *
 * @param x_m   [m] _x_ component
 * @param y_m   [m] _y_ component
 * @param z_m   [m] _z_ component
 */
Position::Position(double x_m, double y_m, double z_m)
: Vector(x_m, y_m, z_m) {
  if(!_valid)
    novas::novas_trace_invalid("Position()");
}

/**
 * Instantiates a new 3D cartesian position vector from its components in S.I. units.
 *
 * @param pos     position 3-vector expressed in some physical unit
 * @param unit    the physical unit, in which the components are given, such as Unit::m or Unit::au.
 */
Position::Position(const double pos[3], double unit)
: Vector(pos[0] * unit, pos[1] * unit, pos[2] * unit) {}


/**
 * Checks if this position is the same as another, within the specified precision.
 *
 * @param p           the reference position
 * @param precision   the precision for testing equality.
 * @return            `true` if this velocity equals the argument within the specified
 *                    precision, or else `false`.
 *
 * @sa operator==(), operator!=()
 */
bool Position::equals(const Position& p, double precision) const {
  return Vector::equals(p, precision);
}

/**
 * Returns the sum of this position and another (relative) position.
 *
 * @param r   the other position on the right-hand side
 * @return    a new position vector with the sum of this position and the argument.
 *
 * @sa operator-()
 */
Position Position::operator+(const Position& r) const {
  return Position(x() + r.x(), y() + r.y(), z() + r.z());
}

/**
 * Returns the difference of this position and another position.
 *
 * @param r   the other position on the right-hand side
 * @return    a new position vector with the difference of this position and the argument.
 *
 * @sa operator+()
 */
Position Position::operator-(const Position& r) const {
  return Position(x() - r.x(), y() - r.y(), z() - r.z());
}

/**
 * Returns the distance to the location indicated by this position (that is the absolute value of
 * this position vector).
 *
 * @return    the distance to the indicated position.
 */
Distance Position::distance() const {
  return Distance(abs());
}

/**
 * Returns a position vector with the same magnitude as this, but in the opposite direction.
 *
 * @return    the spatial inverse position vector of this one.
 */
Position Position::inv() const {
  return Position(-_component[0], -_component[1], -_component[2]);
}

/**
 * Converts this position vector to Spherical coordinates.
 *
 * @return    a new instance of spherical coordinates corresponding to this position.
 */
Spherical Position::to_spherical() const {
  double longitude = atan2(_component[1], _component[0]);
  double xy = hypot(_component[0], _component[1]);
  double latitude = atan2(_component[2], xy);
  return Spherical(isnan(longitude) ? 0.0 : longitude, isnan(latitude) ? 0.0 : latitude, abs());
}

/**
 * Returns a reference to the statically define position at the origin (that is a null posiution
 * vector).
 *
 * @return    a reference to a static null position vector.
 */
const Position& Position::origin() {
  static const Position _origin = Position(0.0, 0.0, 0.0);
  return _origin;
}

/**
 * Returns a string represetation of this position vector, optionally setting the number of
 * decimal places to show for each component.
 *
 * @param decimals    (optional) decimal places to print for the components (default: 3).
 * @return            a string representation of this position vector.
 */
std::string Position::to_string(int decimals) const {
  return "POS (" + Distance(x()).to_string(decimals) + ", " + Distance(y()).to_string(decimals) + ", " + Distance(z()).to_string(decimals) + ")";
}

/**
 * Returns a reference to a statically defined standard invalid position vector. This invalid
 * vector may be used inside any object that is invalid itself.
 *
 * @return    a reference to the static standard invalid vector.
 */
const Position& Position::invalid() {
  static const Position _nan = Position(NAN, NAN, NAN);
  return _nan;
}

} //namespace supernovas

