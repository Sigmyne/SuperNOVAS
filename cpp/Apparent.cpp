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

  cirs2tod_ra = -ira_equinox(f.time().jd(), NOVAS_TRUE_EQUINOX, f.accuracy());
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

  _valid = is_valid_sky_pos(fn, &_pos);
}

Apparent::Apparent(const Frame& frame, enum novas_reference_system sys, sky_pos p)
: Apparent(frame, sys, p.ra * Unit::hour_angle, p.dec * Unit::deg, p.rv * Unit::km / Unit::s) {
  if(!(p.dis > 0)) {
    novas_set_errno(EINVAL, "Apparent()", "input pos.dis is invalid: %g AU", p.dis / Unit::au);
    _valid = false;
  }

  _pos.dis = p.dis;
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
  return Apparent(frame, NOVAS_CIRS, ra_rad, dec_rad, rv_ms);
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
  return Apparent(frame, NOVAS_TOD, ra_rad, dec_rad, rv_ms);
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
 * Returns the apparent equatorial coordinates on the sky, with respect to the true equator and
 * equinox of date (True-of-Date; TOD).
 *
 * @return    the apparent equatorial coordinates in the system in which they were defined.
 *
 * @sa cirs(), ecliptic(), galactic(), to_horizontal()
 */
Equatorial Apparent::equatorial() const {
  return Equatorial(_pos.ra * Unit::hour_angle, _pos.dec * Unit::deg, Equinox::tod(_frame.time()));
}

/**
 * Returns the apparent equatorial coordinates on the sky, in the coordinate system in the Celestial
 * Intermediate Reference System (CIRS).
 *
 * @return    the apparent equatorial coordinates in the system in which they were defined.
 *
 * @sa equatorial(), ecliptic(), galactic(), to_horizontal()
 */
Equatorial Apparent::cirs() const {
  return Equatorial((_pos.ra - cirs2tod_ra) * Unit::hour_angle, _pos.dec * Unit::deg, Equinox::cirs(_frame.time()));
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
  return equatorial().to_ecliptic();
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
  return equatorial().to_galactic();
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
std::optional<Horizontal> Apparent::to_horizontal() const {

  if(!_frame.observer().is_geodetic()) {
    novas_set_errno(EINVAL, "Apparent::horizontal", "cannot convert for non-geodetic observer frame");
    return std::nullopt;
  }

  double ra = 0.0, dec = 0.0, az = 0.0, el = 0.0;

  // pos.ra / pos.dec may be NAN for ITRS / TIRS...
  vector2radec(_pos.r_hat, &ra, &dec);

  novas_app_to_hor(_frame._novas_frame(), NOVAS_TOD, ra, dec, NULL, &az, &el);

  return Horizontal(az * Unit::deg, el * Unit::deg);
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
  return Apparent(frame, NOVAS_TOD, pos);
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
  return Apparent(frame, NOVAS_CIRS, pos);
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

