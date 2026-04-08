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
 *
 * @since 1.6
 */
Position::Position(double x_m, double y_m, double z_m)
: Vector(x_m, y_m, z_m) {
  if(!_valid)
    novas_trace_invalid("Position()");
}

/**
 * Instantiates a new 3D cartesian position vector from its components in S.I. units.
 *
 * @param pos     position 3-vector expressed in some physical unit
 * @param unit    the physical unit, in which the components are given, such as Unit::m or Unit::au.
 *
 * @since 1.6
 */
Position::Position(const double pos[3], double unit)
: Vector(pos[0] * unit, pos[1] * unit, pos[2] * unit) {}


/**
 * Checks if this position is the same as another, within the specified precision.
 *
 * @param p           the reference position
 * @param precision   the precision for testing equality.
 * @return            `true` if this position equals the argument within the specified
 *                    precision, or else `false`.
 *
 * @since 1.6
 * @sa operator==(), operator!=()
 */
bool Position::equals(const Position& p, double precision) const {
  return Vector::equals(p, precision);
}

/**
 * Checks if this position is the same as the specified other position to 12 significant figures,
 * or 1mm (whichever is larger).
 *
 * @param p   the reference position
 * @return    `true` if this position equals the argument to 12 significant figures or 1 mm, or
 *            else `false`.
 *
 * @since 1.6
 * @sa equals(), operator!=()
 */
bool Position::operator==(const Position& p) const {
  double tol = 1e-12 * p.abs();
  if(tol < Unit::mm)
    tol = Unit::mm;
  return equals(p, tol);
}

/**
 * Checks if this position differs from the specified other position by more than the 12th
 * significant figure, or 1mm (whichever is larger).
 *
 * @param p   the reference position
 * @return    `true` if this position equals the argument to 12 significant figures or 1 mm, or
 *            else `false`.
 *
 * @since 1.6
 * @sa equals(), operator==()
 */
bool Position::operator!=(const Position& p) const {
  return !(*this == p);
}

/**
 * Returns the sum of this position and another (relative) position.
 *
 * @param r   the other position on the right-hand side
 * @return    a new position vector with the sum of this position and the argument.
 *
 * @since 1.6
 * @sa operator-()
 */
Position Position::operator+(const Position& r) const {
  Position p(_component[0] + r._component[0], _component[1] + r._component[1], _component[2] + r._component[2]);
  if(!p.is_valid())
    novas_trace_invalid("Position::operator+()");
  return p;
}

/**
 * Returns the difference of this position and another position.
 *
 * @param r   the other position on the right-hand side
 * @return    a new position vector with the difference of this position and the argument.
 *
 * @since 1.6
 * @sa operator+()
 */
Position Position::operator-(const Position& r) const {
  Position p(_component[0] - r._component[0], _component[1] - r._component[1], _component[2] - r._component[2]);
  if(!p.is_valid())
    novas_trace_invalid("Position::operator-()");
  return p;
}

/**
 * Returns the velocity vector that is equal to this position vector travelled under the specified
 * time interval.
 *
 * @param dt   the time interval on the right-hand-side of '/'.
 * @return     velocity __v__ = __x__ / dt, where __x__ is this position vector.
 *
 * @since 1.6
 * @sa Position::operator/()
 */
Velocity Position::operator/(const Interval& dt) const {
  Velocity v(_component[0] / dt.seconds(), _component[1] / dt.seconds(), _component[2] / dt.seconds());
  if(!v.is_valid())
    novas_trace_invalid("Position::operator/()");
  return v;
}


/**
 * Returns the _x_ component of this position vector.
 *
 * @return    the _x_ component
 *
 * @since 1.6
 * @sa y(), z()
 */
Coordinate Position::x() const {
  return Coordinate(_component[0]);
}

/**
 * Returns the _y_ component of this position vector.
 *
 * @return    the _y_ component
 *
 * @since 1.6
 * @sa x(), z()
 */
Coordinate Position::y() const {
  return Coordinate(_component[1]);
}

/**
 * Returns the _z_ component of this position vector.
 *
 * @return    the _z_ component
 *
 * @since 1.6
 * @sa x(), y()
 */
Coordinate Position::z() const {
  return Coordinate(_component[2]);
}

/**
 * Returns the distance to the location indicated by this position (that is the absolute value of
 * this position vector).
 *
 * @since 1.6
 * @return    the distance to the indicated position.
 */
Coordinate Position::distance() const {
  Coordinate l(abs());
  if(!l.is_valid())
    novas_trace_invalid("Position::distance()");
  return l;
}

/**
 * Returns a position vector with the same magnitude as this, but in the opposite direction.
 *
 * @since 1.6
 * @return    the spatial inverse position vector of this one.
 */
Position Position::inv() const {
  Position p(-_component[0], -_component[1], -_component[2]);
  if(!p.is_valid())
    novas_trace_invalid("Position::inv()");
  return p;
}

/**
 * Converts this position vector to Spherical coordinates.
 *
 * @return    a new instance of spherical coordinates corresponding to this position.
 *
 * @since 1.6
 */
Spherical Position::to_spherical() const {
  double longitude = atan2(_component[1], _component[0]);
  double xy = hypot(_component[0], _component[1]);
  double latitude = atan2(_component[2], xy);

  Spherical s((isfinite(longitude) || !isfinite(latitude)) ? longitude : 0.0, latitude);
  if(!s.is_valid())
    novas_trace_invalid("Position::to_spherical()");
  return s;
}

/**
 * Returns a reference to the statically define position at the origin (that is a null posiution
 * vector).
 *
 * @return    a reference to a static null position vector.
 *
 * @since 1.6
 */
const Position& Position::origin() {
  static const Position _origin = Position(0.0, 0.0, 0.0);
  return _origin;
}

/**
 * Returns this __equatorial__ position vector as an astrometric position, referenced to the
 * specified time and Solar-system barycentric observer location.
 *
 * IMPORTANT! Use this method only if you are 100% certain that the position represented by this
 * instance is an equatorial rectangular position vector in the specified observing frame.
 *
 * @param frame         Observing frame (observer location and time of observation)
 * @param system        (optional) %Equatorial coordinate reference system type used for the
 *                      position and observer location (default: TOD).
 *
 * @return              the astrometric position for this observed equatorial position vector,
 *                      referenced to the time when light was emitted, and the observer location
 *                      w.r.t. the Solar-system barycenter (SSB).
 *
 * @since 1.6
 */
AstrometricPosition Position::to_astrometric(const Frame& frame, enum novas_reference_system system) const {
  AstrometricPosition p = AstrometricPosition(*this, frame, system);
  if(!p.is_valid())
    novas_trace_invalid("Position::to_astrometric()");
  return p;
}

/**
 * Returns a string represetation of this position vector, optionally setting the number of
 * decimal places to show for each component.
 *
 * @param decimals    (optional) decimal places to print for the components (default: 3).
 * @return            a string representation of this position vector.
 *
 * @since 1.6
 */
std::string Position::to_string(int decimals) const {
  return "Position (" + Coordinate(_component[0]).to_string(decimals) + ", "
          + Coordinate(_component[1]).to_string(decimals) + ", " + Coordinate(_component[2]).to_string(decimals) + ")";
}

/**
 * Returns a reference to a statically defined standard invalid position vector. This invalid
 * vector may be used inside any object that is invalid itself.
 *
 * @return    a reference to the static standard invalid vector.
 *
 * @since 1.6
 */
const Position& Position::undefined() {
  static const Position _nan = Position();
  return _nan;
}

} //namespace supernovas

