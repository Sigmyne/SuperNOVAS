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
 * @param equ_pos       %Equatorial rectangual position vector defined relative to the reference
 *                      place.
 * @param time          Time of observation
 * @param ref_pos       %Observer location relative to the Solar System Barycenter (SSB).
 * @param system        %Equatorial coordinate reference system type used for the position and
 *                      observer location (default: TOD).
 *
 * @since 1.6
 * @sa Apparent::astrometric_position(), Position::to_astrometric()
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
 * @param equ_pos       %Equatorial rectangual position vector defined relative to the reference
 *                      place.
 * @param frame         Observing frame (observer location and time of observation).
 * @param system        (optional) %Equatorial coordinate reference system type used for the
 *                      position and observer location (default: TOD).
 *
 * @since 1.6
 * @sa Apparent::astrometric_position(), Position::to_astrometric()
 */
AstrometricPosition::AstrometricPosition(const Position& equ_pos, const Frame &frame, enum novas_reference_system system) :
        AstrometricPosition(equ_pos, frame.time(), frame.observer_ssb_position(), system) {
}

/**
 * Returns the Solar system barycentric (SSB) position, relative to which this position is defined.
 *
 * @return    the Solar-system barycentric position that this position is referenced to.
 *
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
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
 * @since 1.6
 * @sa emit_time(), reference()
 */
Time AstrometricPosition::obs_time() const {
  Time t = _emit_time + (distance().m() / Constant::c);
  if(!t.is_valid())
    novas_trace_invalid("AstrometricPosition::obs_time()");
  return t;
}

/**
 * Returns the equatorial coordinates place of this position, as would be seen by a stationary
 * (w.r.t. the SSB) observer located at the reference place.
 *
 * @return            the nominal equatorial place of this position, in the reference system in
 *                    which this position was defined, at the time it is observed at the reference
 *                    position in the solar-system. It does not contain aberration corrections for
 *                    a moving observer, nor gravitational deflection around the major
 *                    Solar-system bodies. Both of those are included in Apparent places instead.
 *
 * @since 1.6
 * @sa Apparent::equatorial()
 */
Equatorial AstrometricPosition::to_equatorial() const {
  double ra = NAN, dec = NAN;
  vector2radec(_array(), &ra, &dec);

  Equatorial e(ra * Unit::hour_angle, dec * Unit::deg, Equinox::from_system_type(_ref_sys, obs_time()));
  if(!e.is_valid())
    novas_trace_invalid("AstrometricPosition::to_equatorial()");
  return e;
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
 * @since 1.6
 * @sa referenced_to_ssb()
 */
AstrometricPosition AstrometricPosition::referenced_to(const Position& ref_pos) const {
  AstrometricPosition p(*this + _obs_pos - ref_pos, obs_time(), ref_pos, _ref_sys);
  if(!p.is_valid())
    novas_trace_invalid("AstrometricPosition::referenced_to()");
  return p;
}

/**
 * Returns the same astrometric location relative the Solar-system Barycenter (SSB).
 *
 * @return      This position but referenced to the SSB.
 *
 * @since 1.6
 * @sa referenced_to()
 */
AstrometricPosition AstrometricPosition::referenced_to_ssb() const {
  AstrometricPosition p = referenced_to(Position::origin());
  if(!p.is_valid())
    novas_trace_invalid("AstrometricPosition::referenced_to_ssb()");
  return p;
}

/**
 * Returns _u_,_v_,_w_ coordinates for a space-based interferometer station represented by this
 * astrometric position. That is, it returns the _u_,_v_,_w_ coordinates of this astrometric place
 * (of a station), measured relative to the reference point (array reference location), for a
 * given apparent reference line-of-sight on the sky (source) at the time as when the station
 * location is defined. The _u_ and _v_ coordinates are the orthogonal projections of the site,
 * relative to the array reference, in the direction of the local East and North respectively, as
 * seen from the source; while _w_ is the distance from the array reference along the line of
 * sight.
 *
 * You could also use `Observer::to_interferometric()` instead. However, using relative astrometric
 * positions can overcome numerical precision issues for interferometers located far from the
 * geocenter or the SSB, such as at L2. Specifically, `Observer::to_interferometric()` uses absolute
 * geocentric and SSB station positions, and interferometric baselines are obtained from differencing
 * these with the reference position used in defining the phase center direction. In contrast,
 * astrometric positions are always defined as relative positions from the array reference, and
 * will be more accurate, in general.
 *
 * ```cpp
 *  // The momentary position of a station relative to the array reference
 *  AstrometricPosition station = ...;
 *
 *  // the location of the phase center
 *  Equatorial eq = ...;
 *
 *  // u,v,w coordinates for a source observed at the time the station position is defined
 *  Interferometric uwv = station.to_interferometric(eq);
 *
 *  // the geometric delay of the station, relative to the array reference
 *  Interval& dt = uvw.geometric_delay();
 * ```
 *
 * For ground-based interferometric application, see `GeodeticObserver::to_interferometric()`
 * instead.
 *
 * @param phase_center      %Apparent equatorial coordinates of the interferometric phase center,
 *                          as seen from the array reference position, at the time at which
 *                          this position is defined.
 * @param distance          (optional) %Apparent distance to phase center, from the array
 *                          reference, at the time of observation (default: 1 Gpc).
 * @param relative_motion   (optional station's velocity vector relative to the reference position
 *                          (default: stationary).
 *
 * @return            interferometric _uvw_ projection this astrometric place viewed from the
 *                    direction of the source at the time for which this position was defined.
 *                    The _u_ and _v_ directions are aligned with the local East and Noth in
 *                    the coordinate system in which the phase center was specified.
 *
 * @since 1.6
 * @sa Observer::to_interferometric()
 */
Interferometric AstrometricPosition::to_interferometric(const Equatorial& phase_center, const Coordinate& distance, const Velocity& relative_motion) const {
  Equinox system = Equinox::from_system_type(_ref_sys, obs_time());

  // Calculate the line-of-sight projection
  double uvw[3] = {0.0};
  novas_uvw(scaled(1.0 / Unit::AU)._array(), relative_motion.scaled(Unit::day / Unit::AU)._array(),
          phase_center.to_system(system).xyz(distance).scaled(1.0 / Unit::AU)._array(), uvw);

  Interferometric p(uvw[0], uvw[1], uvw[2]);
  if(!p.is_valid())
    novas_trace_invalid("AstrometricPosition::interferometric()");
  return p;
}

/**
 * Returns a human-readable string representation of this referenced position.
 *
 * @param decimals    Number of decimal places to show for the position components.
 * @return            a string summary of this referenced position.
 *
 * @since 1.6
 */
std::string AstrometricPosition::to_string(int decimals) const {
  return Position::to_string(decimals) + " at " + _emit_time.to_string() + " from SSB " + _obs_pos.to_string();
}


} // namespace supernovas
