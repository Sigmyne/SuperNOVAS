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

Position Position::operator+(const Position& r) const {
  return Position(x() + r.x(), y() + r.y(), z() + r.z());
}

Position Position::operator-(const Position& r) const {
  return Position(x() - r.x(), y() - r.y(), z() - r.z());
}

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

std::string Position::to_string() const {
  return "POS ( " + Distance(x()).to_string() + ", " + Distance(y()).to_string() + ", " + Distance(z()).to_string() + ")";
}

static const Position _nan = Position(NAN, NAN, NAN);
const Position& Position::invalid() {
  return _nan;
}

} //namespace supernovas

