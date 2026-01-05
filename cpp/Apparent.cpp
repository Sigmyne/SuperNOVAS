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

static bool is_valid_rv(const char *fn, double rv_ms) {
  if(isnan(rv_ms))
    return novas_error(0, EINVAL, fn, "input radial velocity is NAN");
  else if(fabs(rv_ms) > Constant::c)
    return novas_error(0, EINVAL, fn, "input radial velocity exceeds the speed of light: %g m/s", rv_ms);
  return true;
}

static bool is_valid_sky_pos(const char *fn, const sky_pos *p) {
  if(isnan(p->ra))
    return novas_error(0, EINVAL, fn, "input pos->ra is NAN");

  else if(isnan(p->dec))
    return novas_error(0, EINVAL, fn, "input pos->dec is NAN");

  else if(!(p->dis > 0))
    return novas_error(0, EINVAL, fn, "input pos->dis is invalid: %g AU", p->dis / Unit::au);

  else if(isnan(p->rv))
    return novas_error(0, EINVAL, fn, "input pos->rv is NAN");

  else if(p->rv * Unit::au / Unit::day > Constant::c)
    return novas_error(0, EINVAL, fn, "input radial velocity exceeds the speed of light: %g m/s", p->rv * Unit::au / Unit::day);

  return true;
}

Apparent::Apparent(const Equinox& system, const Frame& f)
: _sys(system), _frame(f), _pos({}) {
  static const char *fn = "Apparent(frame, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "frame is invalid");
  else if(!system.is_valid())
    novas_error(0, EINVAL, fn, "equatorial system is invalid");
  else
    _valid = true;
}

Apparent::Apparent(const Equinox& system, const Frame& f, sky_pos p)
: Apparent(system, f) {
  static const char *fn = "Apparent(frame, sky_pos, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "input frame is invalid");
  else if(!is_valid_sky_pos(fn, &p)) {
    novas_error(0, EINVAL, fn, "input sky_pos is invalid");
  }
  else
    _valid = true;

  _pos = p;

  // Always recalculate r_hat to ensure it's consistent with ra/dec
  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}

Apparent::Apparent(const Equinox& system, const Frame& frame, double ra_rad, double dec_rad, double rv_ms)
: Apparent(system, frame) {
  static const char *fn = "Apparent(frame, eq, rv, system)";

  if(isnan(ra_rad))
    novas_error(0, EINVAL, fn, "input RA is NAN");
  else if(isnan(dec_rad))
    novas_error(0, EINVAL, fn, "input RA is NAN");

  _valid = is_valid_rv(fn, rv_ms);

  _pos.ra = ra_rad / Unit::hour_angle;
  _pos.dec = dec_rad / Unit::deg;
  _pos.rv = rv_ms / (Unit::km / Unit::sec);
  _pos.dis = NOVAS_DEFAULT_DISTANCE;

  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
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
 * @sa tod()
 */
Apparent Apparent::cirs(double ra_rad, double dec_rad, const Frame& frame, double rv_ms) {
  return Apparent(Equinox::cirs(frame.time().jd()), frame, ra_rad, dec_rad, rv_ms);
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
 * @sa tod()
 */
Apparent Apparent::cirs(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv) {
  return cirs(ra.rad(), dec.rad(), frame, rv.m_per_s());
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
 * @sa cirs()
 */
Apparent Apparent::tod(double ra_rad, double dec_rad, const Frame& frame, double rv_ms) {
  return Apparent(Equinox::tod(frame.time().jd()), frame, ra_rad, dec_rad, rv_ms);
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
 * @sa cirs()
 */
Apparent Apparent::tod(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv) {
  return tod(ra.rad(), dec.rad(), frame, rv.m_per_s());
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
 * Returns the equatorial coordinate system (equator type and equinox of date) for this
 * apparent position.
 *
 * @return equatorial()
 */
const Equinox& Apparent::system() const {
  return _sys;
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
  return Position(_pos.r_hat, _pos.dis * Unit::au);
}

/**
 * Returns the radial velocity.
 *
 * @return    the radiual velocity with respect to the observer
 *
 * @sa redshift()
 */
Speed Apparent::radial_velocity() const {
  return Speed(_pos.rv * Unit::km / Unit::sec);
}

/**
 * Returns the redshift measure, calculated from the stored radial velocity.
 *
 * @return    the redshift measure with respect to the observer.
 *
 * @sa radial_velocity()
 */
double Apparent::redshift() const {
  return novas_v2z(_pos.rv);
}

/**
 * Returns the apparent distance of this source. Note that this is the distance at which the source
 * appears to the observer at the time of observation, which is different from the geometric
 * distance from the source at the same time instant, due to:
 *
 *  - the motion of a Solar-system source since light originated from it,
 *  - aberration due to the movement of the observer, and
 *  - gravitational bending around the massive Solar-system bodies.
 *
 * @return the apparent distance of the source from the observer
 */
Distance Apparent::distance() const {
  return Distance(_pos.dis * Unit::au);
}

/**
 * Returns the apparent equatorial coordinates on the sky, in the coordinate system in which this
 * apparent position was defined (CIRS or TOD).
 *
 * @return    the apparent equatorial coordinates in the system in which they were defined.
 *
 * @sa system(), ecliptic(), galactic(), horizontal()
 */
Equatorial Apparent::equatorial() const {
  return Equatorial(_pos.ra * Unit::hour_angle, _pos.dec * Unit::deg, _sys, _pos.dis * Unit::au);
}

/**
 * Returns the apparent ecliptic coordinates on the sky, with respect to the true equinox of date.
 *
 * @return    the apparent ecliptic coordinates with respect to the true equinox of date.
 *
 * @sa equatorial(), galactic(), horizontal()
 * @sa Equatorial::as_ecliptic()
 */
Ecliptic Apparent::ecliptic() const {
  return equatorial().as_ecliptic();
}

/**
 * Returns the apparent galactic coordinates on the sky.
 *
 * @return    the apparent galactic coordinates for this position.
 *
 * @sa equatorial(), ecliptic(), horizontal()
 * @sa Equatorial::as_galactic()
 */
Galactic Apparent::galactic() const {
  return equatorial().as_galactic();
}

/**
 * Returns the apparent unrefracted horizontal coordinates for this position for a geodetic
 * observer located on or near Earth's surface, or `std::nullopt` if the observer location is not
 * Earth bound.
 *
 * @return    the unrefracted (astrometric) horizontal position on the Earth-bound observer's
 *            sky, or else `std::nullopt` if the observer is not on or near Earth's surface.
 *
 * @sa equatorial(), ecliptic(), galactic()
 * @sa Horizontal::to_apparent(), GeodeticObserver
 */
std::optional<Horizontal> Apparent::horizontal() const {
  static const char *fn = "Apparent::horizontal";

  if(!_frame.observer().is_geodetic()) {
    novas_error(0, EINVAL, fn, "cannot convert for non-geodetic observer frame");
    return std::nullopt;
  }

  double ra = 0.0, dec = 0.0, az = 0.0, el = 0.0;

  // pos.ra / pos.dec may be NAN for ITRS / TIRS...
  vector2radec(_pos.r_hat, &ra, &dec);

  if(novas_app_to_hor(_frame._novas_frame(), _sys.reference_system(), ra, dec, NULL, &az, &el) != 0) {
    novas_trace_invalid(fn);
    return std::nullopt;
  }

  return Horizontal(az * Unit::deg, el * Unit::deg, _pos.dis * Unit::au);
}

/**
 * Returns an apparent position for a NOVAS C `sky_pos` data structure defined with respect to the
 * true equator and equinox of date (that is in TOD), for the given observing frame.
 *
 * @param pos     a NOVAS C `sky_pos` data structure with respect to the true equinox of date
 *                (not referenced!).
 * @param frame   observing frame (time of observation and observer location)
 * @return        new apparent positions constructed with the parameters.
 *
 * @sa from_cirs_sky_pos()
 */
Apparent Apparent::from_tod_sky_pos(sky_pos pos, const Frame& frame) {
  return Apparent(Equinox::tod(frame.time().jd()), frame, pos);
}

/**
 * Returns an apparent position for a NOVAS C `sky_pos` data structure defined with respect to the
 * true equator and the CIO (that is in CIRS), for the given observing frame.
 *
 * @param pos     a NOVAS C `sky_pos` data structure with respect to the Celestial Intermediate
 *                Origin (CIO). The data passed is not referenced!
 * @param frame   observing frame (time of observation and observer location)
 * @return        new apparent positions constructed with the parameters.
 *
 * @sa from_tod_sky_pos()
 */
Apparent Apparent::from_cirs_sky_pos(sky_pos pos, const Frame& frame) {
  return Apparent(Equinox::cirs(frame.time().jd()), frame, pos);
}

/**
 * Returns a reference to a statically defined standard invalid apparent position. This invalid
 * position may be used inside any object that is invalid itself.
 *
 * @return    a reference to the static standard invalid coordinates.
 */
const Apparent& Apparent::invalid() {
  static const Apparent _invalid = Apparent::tod(NAN, NAN, Frame::invalid(), NAN);
  return _invalid;
}

} //namespace supernovas

