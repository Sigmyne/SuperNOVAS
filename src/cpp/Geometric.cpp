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



namespace supernovas {

/**
 * Instantiates new geometric coordinates, relative to an observer frame, in the equatorial
 * coordinate reference system of choice.
 *
 * @param p       equatorial position vector, with respect to the observer
 * @param v       equatorial velocity vector, with respect to the observer
 * @param frame   observing frame (observer location and time of observation)
 * @param system  equatorial coordinate reference_system, in which position and velocity vectors
 *                are defined
 *
 * @since 1.6
 */
Geometric::Geometric(const Frame& frame, const Position& p, const Velocity& v, enum novas_reference_system system)
          : _frame(frame), _pos(p), _vel(v), _system(system) {
  static const char *fn = "Geometric()";

  if(!frame.is_valid())
    novas_set_errno(EINVAL, fn, "input frame is invalid");
  else if((unsigned) system >= NOVAS_REFERENCE_SYSTEMS)
    novas_set_errno(EINVAL, fn, "input reference_system is invalid: %d", system);
  else if(!p.is_valid())
    novas_set_errno(EINVAL, fn, "input position contains NAN coponent(s)");
  else if(!v.is_valid())
    novas_set_errno(EINVAL, fn, "input velocity contains NAN coponent(s)");
  else
    _valid = true;
}

/**
 * Returns new geometric coordinates that are transformed from these into a different coordinate
 * reference system. Same as `to_system()`. For dynamical coordinate systems, the result is in the
 * coordinate epoch of observation.
 *
 * @param system    the new coordinate reference system type
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the other type of coordinate reference system.
 *
 * @since 1.6
 * @sa to_system()
 */
Geometric Geometric::operator>>(enum novas_reference_system system) const {
  Geometric g = to_system(system);
  if(!g.is_valid())
    novas_trace_invalid("Geometric::operator>>()");
  return g;
}

/**
 * Returns the observing frame for which these geometric coordinates were defined.
 *
 * @return    a reference to the observing frame (observer location and time of observation)
 *
 * @since 1.6
 * @sa system()
 */
const Frame& Geometric::frame() const {
  return _frame;
}

/**
 * Returns the equatorial coordinate system in which these geometric coordinates are defined.
 *
 * @return    a reference to the equatorial coordinate system stored internally.
 *
 * @since 1.6
 * @sa position(), velocity(), equatorial()
 */
enum novas_reference_system Geometric::system_type() const {
  return _system;
}

/**
 * Returns the cartesian equatorial position vector, relative to the observer.
 *
 * @return    the equatorial position vector.
 *
 * @since 1.6
 * @sa astrometric_position(), equatorial(), velocity(), system()
 */
const Position& Geometric::position() const {
  return _pos;
}

/**
 * Returns the cartesian equatorial velocity vector, relative to the observer.
 *
 * @return    the equatorial velocity vector.
 *
 * @since 1.6
 * @sa position(), system()
 */
const Velocity& Geometric::velocity() const {
  return _vel;
}

/**
 * Returns the geometric distance.
 *
 * @return    the geoemtric distance.
 *
 * @since 1.6
 */
Coordinate Geometric::distance() const {
  return Coordinate(_pos.abs());
}

/**
 * Returns the geometric equatorial coordinates, in the system in which the geometric positions
 * and velocities were defined. Note, that these coordinates are phyisical, and not what an observer
 * would perceive at the time of observation, because:
 *
 *  - they are not corrected for aberration for a moving observer.
 *  - they do not account for gravitational bending around massive Solar-system bodies, as light
 *    travels to the observer.
 *
 * If you are interested in observable equatorial coordinates, see Apparent::equatorial() instead.
 *
 * @return    geometric equatorial coordinates.
 *
 * @since 1.6
 * @sa Apparent::equatorial(), ecliptic(), galactic(), position(), velocity()
 */
Equatorial Geometric::equatorial() const {
  Equatorial e = Equatorial(_pos, Equinox::from_system_type(_system, _frame.time()));
  if(!e.is_valid())
    novas_trace_invalid("Geometric::equatorial");
  return e;
}

/**
 * Returns the geometric ecliptic coordinates, in the system in which the geometric positions
 * and velocities were defined. Note, that these coordinates are physical, and not what an observer
 * would perceive at the time of observation, because:
 *
 *  - they are not corrected for aberration for a moving observer.
 *  - they do not account for gravitational bending around massive Solar-system bodies, as light
 *    travels to the observer.
 *
 * If you are interested in observable ecliptic coordinates, see Apparent::ecliptic() instead.
 *
 * @return    geometric ecliptic coordinates.
 *
 * @since 1.6
 * @sa Apparent::ecliptic(), equatorial(), galactic()
 */
Ecliptic Geometric::ecliptic() const {
  Ecliptic e = equatorial().to_ecliptic();
  if(!e.is_valid())
    novas_trace_invalid("Geometric::ecliptic()");
  return e;
}

/**
 * Returns the geometric galactic coordinates, in the system in which the geometric positions
 * and velocities were defined. Note, that these coordinates are physical, and not what an observer
 * would perceive at the time of observation, because:
 *
 *  - they are not corrected for aberration for a moving observer.
 *  - they do not account for gravitational bending around massive Solar-system bodies, as light
 *    travels to the observer.
 *
 * If you are interested in observable galactic coordinates, see Apparent::galactic() instead.
 *
 * @return    geometric galactic coordinates.
 *
 * @since 1.6
 * @sa Apparent::galactic(), equatorial(), ecliptic()
 */
Galactic Geometric::galactic() const {
  Galactic g = equatorial().to_galactic();
  if(!g.is_valid())
    novas_trace_invalid("Geometric::galactic()");
  return g;
}

/**
 * Returns new geometric coordinates that are transformed from these into a different coordinate
 * reference system. For dynamical coordinate systems, the result is in the coordinate epoch
 * of observation.
 *
 * Note, that using this pathway (as `to_system().equatorial()`) for converting
 * geometric positions to different equatorial systems is typically much faster than the
 * equivalent but slower `equatorial().to_system()`, since this method uses transformations
 * that are readily available for the observing frame.
 *
 * @param system    the new coordinate reference system type
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the other type of coordinate reference system.
 *
 * @since 1.6
 * @sa operator>>(), to_icrs(), to_j2000(), to_mod(), to_tod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_system(enum novas_reference_system system) const {
  static const char *fn = "Geometric::to_system()";

  if(system == _system)
    return *this;

  novas_transform T = {};
  double p[3] = {0.0}, v[3] = {0.0};

  if(novas_make_transform(_frame._novas_frame(), _system, system, &T) != 0) {
    novas_trace_invalid(fn);
    return Geometric::undefined();
  }

  novas_transform_vector(_pos._array(), &T, p);
  novas_transform_vector(_vel._array(), &T, v);

  Geometric g(_frame, Position(p), Velocity(v), system);
  if(!g.is_valid())
    novas_trace_invalid(fn);
  return g;
}

/**
 * Returns new geometric coordinates that are transformed from these into the International
 * Coordinate Reference System (ICRS).
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the ICRS.
 *
 * @since 1.6
 * @sa to_system(), to_j2000(), to_mod(), to_tod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_icrs() const {
  Geometric g = to_system(NOVAS_ICRS);
  if(!g.is_valid())
    novas_trace_invalid("Geometric::to_system()");
  return g;
}

/**
 * Returns new geometric coordinates that are transformed from these into the J2000 mean
 * dynamical catalog coordinate system.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the J2000 catalog system.
 *
 * @since 1.6
 * @sa to_system(), to_icrs(), to_mod(), to_tod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_j2000() const {
  Geometric g = to_system(NOVAS_J2000);
  if(!g.is_valid())
    novas_trace_invalid("Geometric::to_j2000()");
  return g;
}

/**
 * Returns new geometric coordinates that are transformed from these into the Mean-of-Date (MOD)
 * dynamical system, with respect to the mean dynamical equator and equinox of date.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the MOD system of date.
 *
 * @since 1.6
 * @sa to_system(), to_icrs(), to_j2000(), to_tod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_mod() const {
  Geometric g = to_system(NOVAS_MOD);
  if(!g.is_valid())
    novas_trace_invalid("Geometric::to_mod()");
  return g;
}

/**
 * Returns new geometric coordinates that are transformed from these into the True-of-Date (TOD)
 * dynamical system, with respect to the true dynamical equator and equinox of date.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the TOD system of date.
 *
 * @since 1.6
 * @sa to_system(), to_icrs(), to_j2000(), to_mod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_tod() const {
  Geometric g = to_system(NOVAS_TOD);
  if(!g.is_valid())
    novas_trace_invalid("Geometric::to_tod()");
  return g;
}

/**
 * Returns new geometric coordinates that are transformed from these into the Celestial
 * Intermediate Reference System (CIRS), with respect to the true dynamical equator and the
 * Celestial Intermediate Origin (CIO) of date.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the CIRS.
 *
 * @since 1.6
 * @sa to_system(), to_icrs(), to_j2000(), to_mod(), to_tod(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_cirs() const {
  Geometric g = to_system(NOVAS_CIRS);
  if(!g.is_valid())
    novas_trace_invalid("Geometric::to_cirs()");
  return g;
}

/**
 * Returns new geometric coordinates that are transformed from these into the rotating Terrestrial
 * Intermediate Reference System (TIRS), with respect to the true dynamical equator and the
 * Terrestrial Intermediate Origin (TIO) of date.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the TIRS.
 *
 * @since 1.6
 * @sa to_system(), to_icrs(), to_j2000(), to_mod(), to_tod(), to_cirs(), to_itrs()
 */
Geometric Geometric::to_tirs() const {
  Geometric g = to_system(NOVAS_TIRS);
  if(!g.is_valid())
    novas_trace_invalid("Geometric::to_tirs()");
  return g;
}

/**
 * Returns new geometric coordinates that are transformed from these into the rotating
 * International Terrestrial Reference System (ITRS), with respect to the true dynamical equator
 * and the Greenwich meridian.  The returned instance may be invalid if an invalid EOP was
 * supplied and the observer also does not define its own valid EOP. It's best practice to
 * check on the validity of the returned value, e.g. as:
 *
 * ```c++
 *  Geometric g = ...; // geometric coordinates defined in some reference system
 *
 *  Geometric itrs = g.to_itrs(eop);
 *  if(!itrs) {
 *    // Oops, there was no valid EOP to calculate true ITRS values.
 *    return;
 *  }
 * ```
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the ITRS. The returned instance may be invalid if an invalid EOP
 *                  was supplied and the observer also does not define its own valid EOP.
 *
 * @since 1.6
 * @sa to_system(), to_icrs(), to_j2000(), to_mod(), to_tod(), to_cirs(), to_tirs()
 */
Geometric Geometric::to_itrs() const {
  Geometric g = to_system(NOVAS_ITRS);
  if(!g.is_valid())
      novas_trace_invalid("Geometric::to_itrs()");
  return g;
}

/**
 * Returns a human-readable basic string description of these geometric positions and velocities.
 *
 * @param decimals    (optional) Number of decimal places to print after the decimal point (default: 3).
 * @return            a string description of these geometric coordinates.
 *
 * @since 1.6
 */
std::string Geometric::to_string(int decimals) const {
  return "Geometric " + Position(position()).to_string(decimals) + ", " + velocity().to_string(decimals) + " in " + _frame.to_string();
}

/**
 * Returns a reference to a statically defined standard invalid geometric coordinates. These invalid
 * coordinates may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid geometric coordinates.
 *
 * @since 1.6
 */
const Geometric& Geometric::undefined() {
  static const Geometric _invalid = Geometric();
  return _invalid;
}

} // namespace supernovas



