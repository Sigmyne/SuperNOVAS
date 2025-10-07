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


Position::Position(double x_m, double y_m, double z_m)
: Vector(x_m, y_m, z_m) {
  if(!_valid)
    novas::novas_trace_invalid("Position()");
}


Position::Position(const double pos[3], double unit)
: Vector(pos[0] * unit, pos[1] * unit, pos[2] * unit) {}

Distance Position::distance() const {
  return Distance(abs());
}

Position Position::inv() const {
  return Position(-_component[0], -_component[1], -_component[2]);
}

Spherical Position::as_spherical() const {
  double longitude = atan2(_component[1], _component[0]);
  double xy = hypot(_component[0], _component[1]);
  double latitude = atan2(_component[2], xy);
  return Spherical(isnan(longitude) ? 0.0 : longitude, isnan(latitude) ? 0.0 : latitude, abs());
}

static const Position _origin = Position(0.0, 0.0, 0.0);
const Position& Position::origin() {
  return _origin;
}

std::string Position::str() const {
  return "POS ( " + Distance(x()).str() + ", " + Distance(y()).str() + ", " + Distance(z()).str() + ")";
}

Position operator+(const Position& l, const Position& r) {
  return Position(l.x() + r.x(), l.y() + r.y(), l.z() + r.z());
}

Position operator-(const Position& l, const Position& r) {
  return Position(l.x() - r.x(), l.y() - r.y(), l.z() - r.z());
}

static const Position _nan = Position(NAN, NAN, NAN);
const Position& Position::invalid() {
  return _nan;
}

} //namespace supernovas

