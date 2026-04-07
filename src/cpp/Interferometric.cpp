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
 *
 * @since 1.6
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
 *
 * @since 1.6
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
 *
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
 * @sa w()
 */
Interval Interferometric::geometric_delay() const {
  return Interval(-w().m() / Constant::c);
}

/**
 * Checks if this interferometric projection is the same as another, within the specified
 * precision.
 *
 * @param p           the reference projection
 * @param precision   the precision for testing equality.
 * @return            `true` if this projection equals the argument within the specified
 *                    precision, or else `false`.
 *
 * @since 1.6
 * @sa operator==(), operator!=()
 */
bool Interferometric::equals(const Interferometric& p, double precision) const {
  return Vector::equals(p, precision);
}

/**
 * Checks if this interferometric projection is the same as the specified other propjection to 12
 * significant figures, or 1nm (whichever is larger).
 *
 * @param p   the reference projection
 * @return    `true` if this projection equals the argument to 12 significant figures or 1 nm, or
 *            else `false`.
 *
 * @since 1.6
 * @sa equals(), operator!=()
 */
bool Interferometric::operator==(const Interferometric& p) const {
  double tol = 1e-12 * p.abs();
  if(tol < Unit::nm)
    tol = Unit::nm;
  return equals(p, tol);
}

/**
 * Checks if this interferometric projection differs from the specified other projection by more
 * than the 12th significant figure, or 1nm (whichever is larger).
 *
 * @param p   the reference projection
 * @return    `true` if this projection equals the argument to 12 significant figures or 1 nm, or
 *            else `false`.
 *
 * @since 1.6
 * @sa equals(), operator==()
 */
bool Interferometric::operator!=(const Interferometric& p) const {
  return !(*this == p);
}


/**
 * Returns the vector sum of this interferometric projection and the specified other projection
 * on the right hand side.
 *
 * @param r   the interferometric projection on the right hand side.
 * @return    the difference between this projection and the argument.
 *
 * @since 1.6
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
 * @since 1.6
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
 *
 * @since 1.6
 */
std::string Interferometric::to_string(int decimals) const {
  return "u = " + u().to_string(decimals) + ", v = " + v().to_string(decimals) + ", delay = " + geometric_delay().to_string(decimals);
}

/**
 * Returns a reference to a statically defined standard invalid interferometric instance.
 *
 * @return    a reference to a standard instance of invalid interferometric projection.
 *
 * @since 1.6
 */
const Interferometric& Interferometric::undefined() {
  static Interferometric _undefined = Interferometric();
  return _undefined;
}


} // namespace supernovas
