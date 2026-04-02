/**
 * @file
 *
 * @date Created  on Mar 24, 2026
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"


namespace supernovas {

/**
 * Instantiates an interferometric projection with the specified components.
 *
 * @param u           [m] Projection along the East direction
 * @param v           [m] Projection along the orthogonalized North Direction
 * @param w           [m] Projection along the line of sight
 */
Interferometric::Interferometric(double u, double v, double w)
: Vector(u, v, w) {
  if(!_valid)
      novas_trace_invalid("Interferometric()");
}

/**
 * Instantiates an interferometric projection with the specified components.
 *
 * @param u           Projection along the East direction
 * @param v           Projection along the orthogonalized North Direction
 * @param w           Projection along the line of sight
 */
Interferometric::Interferometric(const Coordinate& u, const Coordinate& v, const Coordinate& w)
: Interferometric(u.m(), v.m(), w.m()) {}

/**
 * Instantiates an interferometric projection with the specified components
 *
 * @param u           Projection along the East direction
 * @param v           Projection along the orthogonalized North Direction
 * @param geom_delay  geometric delay in the arrival time of light relative to the arrival time
 *                    at the interferometric reference place.
 */
Interferometric::Interferometric(const Coordinate& u, const Coordinate& v, const Interval& geom_delay)
: Interferometric(u.m(), v.m(), -geom_delay.seconds() * Constant::c) {}


/**
 * Returns the _u_ coordinate of this projection, which is the offset of this station projection from the
 * interferometric reference in the direction of the local East, as seen from the line of sight.
 *
 * @return    the station offset, relative to the interferometric reference station, in the direction of
 *            the local East from the line of sight.
 *
 * @sa v(), w()
 */
Coordinate Interferometric::u() const {
  return Coordinate(_component[0]);
}

/**
 * Returns the _v_ coordinate of this projection, which is the offset of this station projection from the
 * interferometric reference in the direction of the local North, as seen from the the line of sight.
 *
 * @return    the station offset, relative to the interferometric reference station, in the direction
 *            of the local North from the line of sight.
 *
 * @sa u(), w()
 */
Coordinate Interferometric::v() const {
  return Coordinate(_component[1]);
}

/**
 * Returns the _w_ coordinate of this projection, which is the offset of this station projection from the
 * interferometric reference along the line of sight.
 *
 * @return    the station offset, relative to the interferometric reference station, along the line-of-sight.
 *
 * @sa geometric_delay(), u(), v()
 */
Coordinate Interferometric::w() const {
  return Coordinate(_component[2]);
}

/**
 * Returns the geometric delay of this projection along the line of sight, relative to the interferometric
 * reference.
 *
 * @return    The geometric delay of this projection, relative to the interferometric reference station.
 *
 * @sa w()
 */
Interval Interferometric::geometric_delay() const {
  return Interval(-w().m() / Constant::c);
}

/**
 * Returns the vector sum of this interferometric projection and the specified other projection
 * on the right hand side.
 *
 * @param r   the interferometric projection on the right hand side.
 * @return    the difference between this projection and the argument.
 *
 * @sa operator-()
 */
Interferometric Interferometric::operator+(const Interferometric& r) const {
  Interferometric x(_component[0] + r._component[0], _component[1] + r._component[1], _component[2] + r._component[2]);
  if(!x.is_valid())
    novas_trace_invalid("Interferometric::operator+()");
  return x;
}

/**
 * Returns vector the difference of this interferometric projection and the specified other
 * projection on the right hand side.
 *
 * @param r   the interferometric projection on the right hand side.
 * @return    the difference between this projection and the argument.
 *
 * @sa operator+()
 */
Interferometric Interferometric::operator-(const Interferometric& r) const {
  Interferometric x(_component[0] - r._component[0], _component[1] - r._component[1], _component[2] - r._component[2]);
  if(!x.is_valid())
    novas_trace_invalid("Interferometric::operator-()");
  return x;
}

/**
 * Returns a human-readable representation of these interferometric projection.
 *
 * @param decimals    (optional) [0:16] Number of decimal places to print for _u_, _v_, and the
 *                    geometric delay (default: 6).
 * @return            a string representation of this interferometric projection.
 */
std::string Interferometric::to_string(int decimals) const {
  return "u = " + u().to_string(decimals) + ", v = " + v().to_string(decimals) + ", delay = " + geometric_delay().to_string(decimals);
}

/**
 * Returns a reference to a statically defined standard invalid interferometric instance.
 *
 * @return    a reference to a standard instance of invalid interferometric projection.
 */
const Interferometric& Interferometric::undefined() {
  static Interferometric _undefined = Interferometric();
  return _undefined;
}


} // namespace supernovas
