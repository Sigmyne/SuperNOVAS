/**
 * @file
 *
 * @date Created  on Mar 19, 2026
 * @author Attila Kovacs
 */


/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"



namespace supernovas {

/**
 * Instantiates an astrometric position from a true-of-date (TOD) equatorial position vector
 * referenced to a specified time (when observed light originated from this position) and a
 * Solar-system barycentric observer location.
 *
 * @param equ_pos       Equatorial rectangual position vector defined relative to the reference
 *                      place.
 * @param time          Time of observation
 * @param ref_pos       Observer location relative to the Solar System Barycenter (SSB).
 * @param system        Equatorial coordinate reference system type used for the position and
 *                      observer location (default: TOD).
 */
AstrometricPosition::AstrometricPosition(const Position& equ_pos, const Time &time, const Position& ref_pos, enum novas_reference_system system) :
        Position(equ_pos), _emit_time(time - (equ_pos.distance().m() / Constant::c)), _obs_pos(ref_pos), _ref_sys(system) {
  static const char *fn = "AstrometricPosition()";

  if(!is_valid())
     novas_trace_invalid(fn);

   if(!_emit_time.is_valid()) {
     novas_set_errno(EINVAL, fn, "input observing time is invalid");
     _valid = false;
   }

   if(!_obs_pos.is_valid()) {
     novas_set_errno(EINVAL, fn, "input observer position is invalid");
     _valid = false;
   }

   if((unsigned) _ref_sys >= NOVAS_REFERENCE_SYSTEMS) {
     novas_set_errno(EINVAL, fn, "input reference system is invalid");
     _valid = false;
   }
}

/**
 * Instantiates an astrometric position from a true-of-date (TOD) equatorial position vector
 * references to an observing frame.
 *
 * @param equ_pos       Equatorial rectangual position vector defined relative to the reference
 *                      place.
 * @param frame         Observing frame (observer location and time of observation).
 * @param system        (optional) Equatorial coordinate reference system type used for the
 *                      position and observer location (default: TOD).
 */
AstrometricPosition::AstrometricPosition(const Position& equ_pos, const Frame &frame, enum novas_reference_system system) :
        AstrometricPosition(equ_pos, frame.time(), frame.observer_position(), system) {
}

/**
 * Returns the Solar system barycentric (SSB) position, relative to which this position is defined.
 *
 * @return    the Solar-system barycentric position that this position is referenced to.
 *
 */
const Position& AstrometricPosition::reference() const {
  return _obs_pos;
}

/**
 * Returns the equatorial coordinate reference system type in which this astrometric position
 * was defined.
 *
 * @return    the reference system type for this position.
 *
 * @sa equatorial()
 */
enum novas_reference_system AstrometricPosition::system_type() const {
  return _ref_sys;
}

/**
 * Returns the time at which the observed light was emitted from this position, that is antedated
 * from the time of observation by the time it takes for light to travel to the observer.
 *
 * @return    the time at which observed light was emitted from this position.
 *
 * @sa obs_time()
 */
const Time& AstrometricPosition::emit_time() const {
  return _emit_time;
}

/**
 * Returns the reference time at which this position was observed by an observer at the reference
 * location.
 *
 * @return    the time at which this position is defined.
 *
 * @sa emit_time(), reference()
 */
Time AstrometricPosition::obs_time() const {
  return _emit_time + (distance().m() / Constant::c);
}

/**
 * Returns the equatorial coordinates place of this position, as would be seen by a stationary
 * (w.r.t. the SSB) observer located at the reference place.
 *
 * @return            the nominal equatorial place of this position, in the reference system in
 *                    which this position was defined, at the time it is observed at the reference
 *                    position in the solar-system. It does not contain aberration corrections for
 *                    a moving observer, nor gravitational deflection around the major
 *                    Solar-system bodies. Both of those are included in `Apparent` places
 *                    instead.
 *
 * @sa Apparent::equatorial()
 */
Equatorial AstrometricPosition::as_equatorial() const {
  double ra = NAN, dec = NAN;
  vector2radec(_array(), &ra, &dec);
  return Equatorial(ra * Unit::hour_angle, dec * Unit::deg, Equinox::from_system_type(_ref_sys, obs_time().jd()));
}

/**
 * Returns the same astrometric location relative to a new location relative to the Solar-system
 * Barycenter.
 *
 * @param ref_pos   The new location, w.r.t. the SSB, relative to which to redefine this position. It
 *                  should be given in the same coordinate reference system in which this position
 *                  was defined.
 * @return          This position but referenced to the new location.
 *
 * @sa referenced_to_ssb()
 */
AstrometricPosition AstrometricPosition::referenced_to(const Position& ref_pos) const {
  return AstrometricPosition(*this + _obs_pos - ref_pos, obs_time(), ref_pos, _ref_sys);
}

/**
 * Returns the same astrometric location relative the Solar-system Barycenter (SSB).
 *
 * @return      This position but referenced to the SSB.
 *
 * @sa referenced_to()
 */
AstrometricPosition AstrometricPosition::referenced_to_ssb() const {
  return referenced_to(Position::origin());
}


/**
 * Returns a human-readable string representation of this referenced position.
 *
 * @param decimals    Number of decimal places to show for the position components.
 * @return            a string summary of this referenced position.
 */
std::string AstrometricPosition::to_string(int decimals) const {
  return Position::to_string(decimals) + " at " + _emit_time.to_string() + " from " + _obs_pos.to_string();
}


} // namespace supernovas
