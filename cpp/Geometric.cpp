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
 * Instantiates new geometric coordinates, relative to an observer frame, in the equatorial
 * coordinate reference system of choice.
 *
 * @param p       equatorial position vector, with respect to the observer
 * @param v       equatorial velocity vector, with respect to the observer
 * @param frame   observing frame (observer location and time of observation)
 * @param system  equatorial coordinate reference_system, in which position and velocity vectors
 *                are defined
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
 * @sa to_system()
 */
Geometric Geometric::operator>>(enum novas_reference_system system) const {
  return to_system(system);
}

/**
 * Returns the observing frame for which these geometric c Geometric c(pos, vel, frame, NOVAS_CIRS);

  opt = b.to_system(NOVAS_ITRS);
  if(!test.check("to_itrs(site).has_value()", opt.has_value())) n++;
  else {
    Geometric c1 = opt.value();

    novas_make_transform(frame._novas_frame(), NOVAS_CIRS, NOVAS_ITRS, &T);
    novas_transform_vector(pos._array(), &T, pos1);
    novas_transform_vector(vel._array(), &T, vel1);

    if(!test.equals("to_itrs().system()", c1.system().system_type(), NOVAS_ITRS)) n++;
    if(!test.check("to_itrs(site).position()", c1.position() == Position(pos1))) n++;
    if(!test.check("to_itrs(site).velocity()", c1.velocity() == Velocity(vel1))) n++;
  }
 * oordinates were defined.
 *
 * @return    a reference to the observing frame (observer location and time of observation)
 *
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
 * @sa equatorial(), velocity(), system()
 */
const Position& Geometric::position() const {
  return _pos;
}

/**
 * Returns the cartesian equatorial velocity vector, relative to the observer.
 *
 * @return    the equatorial velocity vector.
 *
 * @sa position(), system()
 */
const Velocity& Geometric::velocity() const {
  return _vel;
}

/** Geometric c(pos, vel, frame, NOVAS_CIRS);

  opt = b.to_system(NOVAS_ITRS);
  if(!test.check("to_itrs(site).has_value()", opt.has_value())) n++;
  else {
    Geometric c1 = opt.value();

    novas_make_transform(frame._novas_frame(), NOVAS_CIRS, NOVAS_ITRS, &T);
    novas_transform_vector(pos._array(), &T, pos1);
    novas_transform_vector(vel._array(), &T, vel1);

    if(!test.equals("to_itrs().system()", c1.system().system_type(), NOVAS_ITRS)) n++;
    if(!test.check("to_itrs(site).position()", c1.position() == Position(pos1))) n++;
    if(!test.check("to_itrs(site).velocity()", c1.velocity() == Velocity(vel1))) n++;
  }
 *
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
 * @sa Apparent::equatorial(), ecliptic(), galactic(), position(), velocity()
 */
Equatorial Geometric::equatorial() const {
  Equatorial e = Equatorial(_pos, Equinox::from_system_type(_system, _frame.time().jd()));
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
 * @sa Apparent::ecliptic(), equatorial(), galactic()
 */
Ecliptic Geometric::ecliptic() const {
  return equatorial().to_ecliptic();
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
 * @sa Apparent::galactic(), equatorial(), ecliptic()
 */
Galactic Geometric::galactic() const {
  return equatorial().to_galactic();
}

Geometric Geometric::to_system(const novas::novas_frame *f, enum novas::novas_reference_system system) const {
  novas_transform T = {};
  double p[3] = {0.0}, v[3] = {0.0};

  if(novas_make_transform(f, _system, system, &T) != 0) {
    novas_trace_invalid("Geometric::to_system");
    return Geometric::invalid();
  }

  novas_transform_vector(_pos._array(), &T, p);
  novas_transform_vector(_vel._array(), &T, v);

  return Geometric(_frame, Position(p), Velocity(v), system);
}


/**
 * Returns new geometric coordinates that are transformed from these into a different coordinate
 * reference system. For dynamical coordinate systems, the result is in the coordinate epoch
 * of observation.
 *
 * @param system    the new coordinate reference system type
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the other type of coordinate reference system.
 *
 * @sa operator>>(), to_icrs(), to_j2000(), to_mod(), to_tod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_system(enum novas_reference_system system) const {
  if(system == _system)
    return *this;

  if(system == NOVAS_ITRS)
    return to_itrs().value_or(Geometric::invalid()); // TODO error trace...

  return to_system(_frame._novas_frame(), system);
}

/**
 * Returns new geometric coordinates that are transformed from these into the International
 * Coordinate Reference System (ICRS).
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the ICRS.
 *
 * @sa to_system(), to_j2000(), to_mod(), to_tod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_icrs() const {
  return to_system(NOVAS_ICRS);
}

/**
 * Returns new geometric coordinates that are transformed from these into the J2000 mean
 * dynamical catalog coordinate system.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the J2000 catalog system.
 *
 * @sa to_system(), to_icrs(), to_mod(), to_tod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_j2000() const {
  return to_system(NOVAS_J2000);
}

/**
 * Returns new geometric coordinates that are transformed from these into the Mean-of-Date (MOD)
 * dynamical system, with respect to the mean dynamical equator and equinox of date.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the MOD system of date.
 *
 * @sa to_system(), to_icrs(), to_j2000(), to_tod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_mod() const {
  return to_system(NOVAS_MOD);
}

/**
 * Returns new geometric coordinates that are transformed from these into the True-of-Date (TOD)
 * dynamical system, with respect to the true dynamical equator and equinox of date.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the TOD system of date.
 *
 * @sa to_system(), to_icrs(), to_j2000(), to_mod(), to_cirs(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_tod() const {
  return to_system(NOVAS_TOD);
}

/**
 * Returns new geometric coordinates that are transformed from these into the Celestial
 * Intermediate Reference System (CIRS), with respect to the true dynamical equator and the
 * Celestial Intermediate Origin (CIO) of date.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the CIRS.
 *
 * @sa to_system(), to_icrs(), to_j2000(), to_mod(), to_tod(), to_tirs(), to_itrs()
 */
Geometric Geometric::to_cirs() const {
  return to_system(NOVAS_CIRS);
}

/**
 * Returns new geometric coordinates that are transformed from these into the rotating Terrestrial
 * Intermediate Reference System (TIRS), with respect to the true dynamical equator and the
 * Terrestrial Intermediate Origin (TIO) of date.
 *
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the TIRS.
 *
 * @sa to_system(), to_icrs(), to_j2000(), to_mod(), to_tod(), to_cirs(), to_itrs()
 */
Geometric Geometric::to_tirs() const {
  return to_system(NOVAS_TIRS);
}

/**
 * Returns new geometric coordinates that are transformed from these into the rotating
 * International Terrestrial Reference System (ITRS), with respect to the true dynamical equator
 * and the Greenwich meridian.
 *
 * @param eop       Earth Orientation Parameters (EOP) appropriate for the date, such as obtained
 *                  from the IERS bulletins or web service.
 * @return          geometric coordinates for the same position and velocity as this, but
 *                  expressed in the ITRS.
 *
 * @sa to_system(), to_icrs(), to_j2000(), to_mod(), to_tod(), to_cirs(), to_tirs()
 */
std::optional<Geometric> Geometric::to_itrs(const EOP& eop) const {
  if(_system == NOVAS_ITRS)
    return Geometric(*this);

  // Apply specified EOP to frame
  if(eop.is_valid()) {
    novas_frame f = * _frame._novas_frame();

    f.dx = eop.xp().mas();
    f.dy = eop.yp().mas();

    if(_frame.accuracy() == NOVAS_FULL_ACCURACY) {
      // Add diurnal corrections
      double xp = 0.0, yp = 0.0;
      novas_diurnal_eop_at_time(_frame.time()._novas_timespec(), &xp, &yp, NULL);

      f.dx += 1000.0 * xp;
      f.dy += 1000.0 * yp;
    }

    return to_system(&f, NOVAS_ITRS);
  }

  // Or, use observer's EOP
  if(_frame.observer().is_geodetic())
    return to_itrs(dynamic_cast<const GeodeticObserver &>(_frame.observer()).eop());

  // Or, we can't really convert to ITRS
  novas_set_errno(EINVAL, "Geometric::to_itrs()", "Needs valid EOP for non geodetic observer frame");
  return std::nullopt;
}

/**
 * Returns a reference to a statically defined standard invalid geometric coordinates. These invalid
 * coordinates may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid geometric coordinates.
 */
const Geometric& Geometric::invalid() {
  static const Geometric _invalid = Geometric(Frame::invalid(), Position::invalid(), Velocity::invalid(), (enum novas_reference_system) -1);
  return _invalid;
}

} // namespace supernovas



