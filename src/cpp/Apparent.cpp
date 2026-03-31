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


static bool is_valid_sky_pos(const char *fn, const sky_pos *p) {
  errno = 0;

  if(!isfinite(p->ra))
    novas_set_errno(EINVAL, fn, "input RA is NAN or infinite");

  if(!isfinite(p->dec))
    novas_set_errno(EINVAL, fn, "input Dec is NAN or infinite");

  if(!isfinite(p->rv))
    novas_set_errno(EINVAL, fn, "input radial velocity is NAN or infinite");

  if(p->rv * Unit::au / Unit::day > Constant::c)
    novas_set_errno(EINVAL, fn, "input radial velocity exceeds the speed of light: %g m/s", p->rv * Unit::au / Unit::day);

  return (errno == 0);
}

Apparent::Apparent(const Frame& f)
: cirs2tod_ra(0.0), _frame(f), _pos({}) {
  if(!f.is_valid())
    novas_set_errno(EINVAL, "Apparent()", "frame is invalid");
  else
    _valid = true;

  cirs2tod_ra = -ira_equinox(f.jd(), NOVAS_TRUE_EQUINOX, f.accuracy());
}

Apparent::Apparent(const Frame& frame, enum novas_reference_system sys, double ra_rad, double dec_rad, double rv_ms)
: Apparent(frame) {
  static const char *fn = "Apparent()";

  _pos.ra = ra_rad / Unit::hour_angle;

  // CIRS -> TOD as necessary...
  if(sys == NOVAS_CIRS)
    _pos.ra += cirs2tod_ra;

  _pos.dec = dec_rad / Unit::deg;
  _pos.rv = rv_ms / (Unit::km / Unit::sec);
  _pos.dis = NOVAS_DEFAULT_DISTANCE;

  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);

  _valid = frame.is_valid() && is_valid_sky_pos(fn, &_pos);
}

Apparent::Apparent(const Frame& frame, enum novas_reference_system sys, const sky_pos *p)
: Apparent(frame, sys, p->ra * Unit::hour_angle, p->dec * Unit::deg, p->rv * Unit::km / Unit::s) {
  if(!(p->dis > 0)) {
    novas_set_errno(EINVAL, "Apparent()", "input pos.dis is invalid: %g AU", p->dis / Unit::au);
    _valid = false;
  }

  _pos.dis = p->dis;
}

/**
 * Instantiates apparent sky coordinates in the Celestrial Intermediate Reference System (CIRS).
 *
 * @param ra_rad    [rad] right ascention (R.A.) in CIRS (from the CIO)
 * @param dec_rad   [rad] declination in CIRS
 * @param frame     observing frame (time of observation and observer location)
 * @param rv_ms     [m/s] radial velocity
 * @return          new apparent location on sky with the specified parameters.
 *
 * @sa from_tod()
 */
Apparent Apparent::from_cirs(double ra_rad, double dec_rad, const Frame& frame, double rv_ms) {
  Apparent a(frame, NOVAS_CIRS, ra_rad, dec_rad, rv_ms);
  if(!a.is_valid())
    novas_trace_invalid("Apparent::cirs()");
  return a;
}

/**
 * Instantiates apparent sky coordinates in the Celestrial Intermediate Reference System (CIRS).
 *
 * @param ra        right ascention (R.A.) angle in CIRS (from the CIO)
 * @param dec       declination angle in CIRS
 * @param frame     observing frame (time of observation and observer location)
 * @param rv        radial velocity
 * @return          new apparent location on sky with the specified parameters.
 *
 * @sa from_tod()
 */
Apparent Apparent::from_cirs(const Angle& ra, const Angle& dec, const Frame& frame, const ScalarVelocity& rv) {
  return from_cirs(ra.rad(), dec.rad(), frame, rv.m_per_s());
}

/**
 * Instantiates apparent sky coordinates in the True-of-Date (TOD) system, with respect to the
 * true dynamical equator and equinox of date.
 *
 * @param ra_rad    [rad] true right ascention (R.A.) of date (from the true equinox of date)
 * @param dec_rad   [rad] true declination of date
 * @param frame     observing frame (time of observation and observer location)
 * @param rv_ms     [m/s] radial velocity
 * @return          new apparent location on sky with the specified parameters.
 *
 * @sa from_cirs()
 */
Apparent Apparent::from_tod(double ra_rad, double dec_rad, const Frame& frame, double rv_ms) {
  Apparent a(frame, NOVAS_TOD, ra_rad, dec_rad, rv_ms);
  if(!a.is_valid())
    novas_trace_invalid("Apparent::tod()");
  return a;
}

/**
 * Instantiates apparent sky coordinates in the True-of-Date (TOD) system, with respect to the
 * true dynamical equator and equinox of date.
 *
 * @param ra        true right ascention (R.A.) angle of date (from the equinox of date)
 * @param dec       true declination angle of date
 * @param frame     observing frame (time of observation and observer location)
 * @param rv        radial velocity
 * @return          new apparent location on sky with the specified parameters.
 *
 * @sa from_cirs()
 */
Apparent Apparent::from_tod(const Angle& ra, const Angle& dec, const Frame& frame, const ScalarVelocity& rv) {
  return from_tod(ra.rad(), dec.rad(), frame, rv.m_per_s());
}

/**
 * Retuns the reference to the frame for which these apparent positions are defined.
 *
 * @return      the ovbserving frame (time of observation and observer location) for this
 *              apparent position.
 */
const Frame& Apparent::frame() const {
  return _frame;
}

/**
 * Returns a pointer to the underlying NOVAS C `sky_pos` data structure, which stores the data for
 * this apparent position.
 *
 * @return    pointer to the NOVAS C sky_pos data used internally.
 */
const sky_pos *Apparent::_sky_pos() const {
  return &_pos;
}

/**
 * Returns the projected 3D position vector corresponding to this apparent position. Note, that
 * the projected position is where the source appears to the observer at the time of observation,
 * which is different from the true geometric location of the source, due to:
 *
 *  - the motion of a Solar-system source since light originated from it,
 *  - aberration due to the movement of the observer, and
 *  - gravitational bending around the massive Solar-system bodies.
 *
 * @return    the projected position vector of where the source appears to be from the observer's
 *            point of view.
 */
Position Apparent::xyz() const {
  Position p(_pos.r_hat, _pos.dis * Unit::au);
  if(!p.is_valid())
    novas_trace_invalid("Apparent::xyz()");
  return p;
}

/**
 * Returns the radial velocity.
 *
 * @return    the radiual velocity with respect to the observer
 *
 * @sa redshift()
 */
ScalarVelocity Apparent::radial_velocity() const {
  ScalarVelocity v(_pos.rv * Unit::km / Unit::sec);
  if(!v.is_valid())
    novas_trace_invalid("Apparent::radial_velocity()");
  return v;
}

/**
 * Returns the redshift measure, calculated from the stored radial velocity.
 *
 * @return    the redshift measure with respect to the observer.
 *
 * @sa radial_velocity()
 */
double Apparent::redshift() const {
  return novas_check_nan("Apparent::redshift()", novas_v2z(_pos.rv));
}

/**
 * Returns the apparent light-time distance of this source. Note that this is the distance at
 * which the source appears to the observer at the time of observation, which is different from
 * the geometric distance from the source at the same time instant, due to:
 *
 *  - the motion of a Solar-system source since light originated from it,
 *  - aberration due to the movement of the observer, and
 *  - gravitational bending around the massive Solar-system bodies.
 *
 * @return the apparent (light-time) distance of the source from the observer
 */
Coordinate Apparent::distance() const {
  Coordinate d(_pos.dis * Unit::au);
  if(!d.is_valid())
    novas_trace_invalid("Apparent::distance()");
  return d;
}

/**
 * Returns the apparent equatorial coordinates on the sky, with respect to the true equator and
 * equinox of date (True-of-Date; TOD).
 *
 * @return    True-of-date (TOD) equatorial coordinates in the observing frame.
 *
 * @sa cirs(), ecliptic(), galactic(), to_horizontal()
 */
Equatorial Apparent::equatorial() const {
  Equatorial e(_pos.ra * Unit::hour_angle, _pos.dec * Unit::deg, Equinox::tod(_frame.time()));
  if(!e.is_valid())
    novas_trace_invalid("Apparent::equatorial()");
  return e;
}

/**
 * Returns the apparent equatorial coordinates on the sky, in the Celestial Intermediate Reference
 * System (CIRS). CIRS is defined on the true equator of date, but its origin is the Celestial
 * Intermediate Origin (CIO), not the true equinox of date.
 *
 * @return    the CIRS equatorial coordinates in the observing frame.
 *
 * @sa equatorial(), ecliptic(), galactic(), to_horizontal()
 */
Equatorial Apparent::cirs() const {
  Equatorial e((_pos.ra - cirs2tod_ra) * Unit::hour_angle, _pos.dec * Unit::deg, Equinox::cirs(_frame.time()));
  if(!e.is_valid())
    novas_trace_invalid("Apparent::cirs()");
  return e;
}

/**
 * Returns the apparent ecliptic coordinates on the sky, with respect to the true equinox of date.
 *
 * @return    the apparent ecliptic coordinates with respect to the true equinox of date.
 *
 * @sa equatorial(), galactic(), to_horizontal()
 * @sa Equatorial::to_ecliptic()
 */
Ecliptic Apparent::ecliptic() const {
  Ecliptic e = equatorial().to_ecliptic();
  if(!e.is_valid())
    novas_trace_invalid("Apparent::ecliptic()");
  return e;
}

/**
 * Returns the apparent galactic coordinates on the sky.
 *
 * @return    the apparent galactic coordinates for this position.
 *
 * @sa equatorial(), ecliptic(), to_horizontal()
 * @sa Equatorial::to_galactic()
 */
Galactic Apparent::galactic() const {
  Galactic g = equatorial().to_galactic();
  if(!g.is_valid())
    novas_trace_invalid("Apparent::to_galactic()");
  return g;
}

/**
 * Returns the apparent unrefracted horizontal coordinates for this position for a geodetic
 * observer located on or near Earth's surface, or as an invalid set of coordinates if the observer
 * location is not Earth bound. It's best practice to check if the returned coordinates are valid,
 * e.g. as:
 *
 * ```c++
 *  Apparent app = ...;
 *  Horizontal h = app.to_horizontal();
 *  if(!h) {
 *    // Oops, could not provide valid horizontal coordinates...
 *    return;
 *  }
 * ```
 *
 * @return    the unrefracted (astrometric) horizontal position on the Earth-bound observer's
 *            sky, or else Horizontal::undefined() if the observer is not on or near Earth's surface.
 *
 * @sa equatorial(), ecliptic(), galactic()
 * @sa Horizontal::to_apparent(), GeodeticObserver
 */
Horizontal Apparent::to_horizontal() const {
  double ra = 0.0, dec = 0.0, az = 0.0, el = 0.0;

  // pos.ra / pos.dec may be NAN for ITRS / TIRS...
  vector2radec(_pos.r_hat, &ra, &dec);

  if(novas_app_to_hor(_frame._novas_frame(), NOVAS_TOD, ra, dec, NULL, &az, &el) != 0) {
    novas_trace_invalid("Apparent::to_horizontal()");
    return Horizontal::undefined();
  }

  Horizontal h(az * Unit::deg, el * Unit::deg);
  if(!h.is_valid())
    novas_trace_invalid("Apparent::to_horizontal()");
  return h;
}

/**
 * Returns the projected 3D position, relative to the SSB or other Solar-System place, which gave
 * rise to these apparent place on sky. The position is antedated to the time the observed light
 * originated from the observed body,
 *
 * @return        The position referenced to the given Solar-system body or place, and antedated
 *                for light travel time for this apparent position.
 */
AstrometricPosition Apparent::astrometric_position() const {
  double p[3] = {0.0};
  if(novas_app_to_geom(_frame._novas_frame(), NOVAS_TOD, _pos.ra, _pos.dec, _pos.dis, p) != 0) {
    novas_trace_invalid("Apparent::astrometric_position()");
    return AstrometricPosition(Position::undefined(), _frame, NOVAS_TOD);
  }
  return AstrometricPosition(Position(p, Unit::au), _frame, NOVAS_TOD);
}

/**
 * Returns a human-readable basic string description of these apparent positions.
 *
 * @param decimals    (optional) Number of decimal places to print after the decimal point (default: 3).
 * @return            a string description of these apparent positions.
 */
std::string Apparent::to_string(int decimals) const {
  return "Apparent " + equatorial().to_string(NOVAS_SEP_UNITS_AND_SPACES, decimals) + " in " + _frame.to_string();
}

/**
 * Returns an apparent position for a NOVAS C `sky_pos` data structure defined with respect to the
 * true equator and equinox of date (that is in TOD), for the given observing frame.
 *
 * @param frame   observing frame (time of observation and observer location)
 * @param pos     a NOVAS C `sky_pos` data structure with respect to the true equinox of date
 *                (not referenced!).
 * @return        new apparent positions constructed with the parameters. It may be invalid if
 *                the input values themselves are invalid.
 *
 * @sa from_cirs_sky_pos(), from_tod()
 */
Apparent Apparent::from_tod_sky_pos(const Frame& frame, const sky_pos *pos) {
  static const char *fn = "Apparent::from_cirs_sky_pos";

  if(!pos) {
     novas_set_errno(EINVAL, fn, "input sky_pos is NULL");
     return Apparent::undefined();
   }

  Apparent a(frame, NOVAS_TOD, pos);
  if(!a.is_valid())
    novas_trace_invalid(fn);
  return a;
}

/**
 * Returns an apparent position for a NOVAS C `sky_pos` data structure defined with respect to the
 * true equator and the CIO (that is in CIRS), for the given observing frame.
 *
 * @param frame   observing frame (time of observation and observer location)
 * @param pos     a NOVAS C `sky_pos` data structure with respect to the Celestial Intermediate
 *                Origin (CIO). The data passed is not referenced!
 * @return        new apparent positions constructed with the parameters. It may be invalid if
 *                the input values themselves are invalid.
 *
 * @sa from_tod_sky_pos(), from_cirs()
 */
Apparent Apparent::from_cirs_sky_pos(const Frame& frame, const sky_pos *pos) {
  static const char *fn = "Apparent::from_cirs_sky_pos";

  if(!pos) {
    novas_set_errno(EINVAL, fn, "input sky_pos is NULL");
    return Apparent::undefined();
  }

  Apparent a(frame, NOVAS_CIRS, pos);
  if(!a.is_valid())
    novas_trace_invalid(fn);

  return a;
}

/**
 * Returns a reference to a statically defined standard invalid apparent position. This invalid
 * position may be used inside any object that is invalid itself.
 *
 * @return    a reference to the static standard invalid coordinates.
 */
const Apparent& Apparent::undefined() {
  static const Apparent _invalid = Apparent();
  return _invalid;
}

} //namespace supernovas

