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
 * Instantiates a new observer frame, given the observer location and time of observation, and
 * optionally the required accuracy. After the new frame is returned you should check that it's
 * valid:
 *
 * ```c
 *   Frame f = Frame(obs, time);
 *   if(!f.is_valid()) {
 *     // This did not work as expected...
 *     ...
 *   }
 * ```
 *
 * The returned new frame may be invalid for multiple reasons, such as:
 *
 *  - the input observer or time is invalid.
 *  - the accuracy parameter is outside of the enum range.
 *  - SuperNOVAS had no suitable planet provider function for the given accuracy. (By default
 *    SuperNOVAS has only a reduced accuracy Earth-Sun calculator configured.)
 *  - The currently configured planet provider function, for the given accuracy, cannot provide
 *    positions and velocities for the Earth, Sun, observer location, or one of the major planets
 *    configured for gravitational deflection calculations.
 *
 * Alternatively, you might use the equivalent Frame::create() instead to return the Frame
 * as an optional.
 *
 * In either case, you can obtain more information on why things went awry, when they do, by
 * enabling debug mode is enabled via `novas_debug()` prior to constructing a Frame.
 *
 * @param obs         observer location
 * @param time        time of observation
 * @param accuracy    (optional) NOVAS_FULL_ACCURACY (default) or NOVAS_REDUCED_ACCURACY.
 *
 * @sa create()
 */
Frame::Frame(const Observer& obs, const Time& time, enum novas_accuracy accuracy)
: _observer(obs), _time(time) {
  static const char *fn = "Frame()";

  if(novas_make_frame(accuracy, obs._novas_observer(), time._novas_timespec(), 0.0, 0.0, &_frame) != 0)
    novas_trace_invalid(fn);
  else if(!obs.is_valid())
    novas_error(0, EINVAL, fn, "input observer is invalid");
  else if(!time.is_valid())
    novas_error(0, EINVAL, fn, "input time is invalid");
  else
    _valid = true;

  if(!obs.is_geodetic()) {
    // Force NANs if one tries to used EOP for a non-geodetic observer.
    _frame.dx = NAN;
    _frame.dy = NAN;
  }
}

/**
 * Returns the pointer to the underlying NOVAS C `novas_frame` data structure of this observing
 * frame.
 *
 * @return    pointer to the underlying NOVAS C data structure.
 */
const novas_frame * Frame::_novas_frame() const {
  return &_frame;
}

/**
 * Returns the accuracy type of this bserving frame.
 *
 * @return    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1).
 */
enum novas_accuracy Frame::accuracy() const {
  return _frame.accuracy;
}

/**
 * Returns the astrometric time of observation of this observing frame.
 *
 * @return    the astrometric time of observation
 *
 * @sa observer()
 */
const Time& Frame::time() const {
  return _time;
}

/**
 * Returns the observer location (and motion) of this observing frame
 *
 * @return    the observer location (and motion).
 *
 * @sa time()
 */
const Observer& Frame::observer() const {
  return _observer;
}

/**
 * Checks if this frame has calculated position / velocity data for a given planet. If yes, then
 * the frame can provide geometric ephemeris position for the planet directly, and the planet
 * can be used in calculations for gravitational bending also.
 *
 * @param planet    the NOVAS major planet ID, e.g. `NOVAS_JUPITER`.
 * @return          `true` if the observing frame contains position velocity data
 *                  for the planet, or else `false`.
 *
 * @sa planet_position(), planet_velocity()
 */
bool Frame::has_planet_data(enum novas::novas_planet planet) const {
  return (_frame.planets.mask & (1 << planet)) != 0;
}

/**
 * Checks if this frame has calculated position / velocity data for a given planet. If yes, then
 * the frame can provide geometric ephemeris position for the planet directly, and the planet
 * can be used in calculations for gravitational bending also.
 *
 * @param planet    the planet
 * @return          `true` if the observing frame contains position velocity data
 *                  for the planet, or else `false`.
 *
 * @sa planet_position(), planet_velocity()
 */
bool Frame::has_planet_data(const Planet& planet) const { return has_planet_data(planet.novas_id()); }

/**
 * Returns the geometric position for the specified planet that is readily calculated and stored
 * in this observing frame (if available), or else `std::nullopt` if no such data is available.
 * Typically, only the positions for the Sun, Earth, and the major gravitating bodies (depending
 * on the frame's accuracy) are precalculated and available in this way.
 *
 * @param planet    the NOVAS major planet ID, e.g. `NOVAS_JUPITER`.
 * @return          the planet's precalculated geometric ephemeris position, with respect to the
 *                  observer at the time that the observed light originated from the body -- or
 *                  else `std::nullopt` if no data is available.
 *
 * @sa planet_velocity(), has_planet_data()
 */
std::optional<Position> Frame::planet_position(enum novas::novas_planet planet) const {
  if(!has_planet_data(planet)) {
    novas_error(0, ENOSYS, "Frame::planet_position", "No data for planet %d", planet);
    return std::nullopt;
  }
  return Position(_frame.planets.pos[1 << planet], Unit::au);
}

/**
 * Returns the geometric position for the specified planet that is readily calculated and stored
 * in this observing frame (if available), or else `std::nullopt` if no such data is available.
 * Typically, only the positions for the Sun, Earth, and the major gravitating bodies (depending
 * on the frame's accuracy) are precalculated and available in this way.
 *
 * @param planet    the planet.
 * @return          the planet's precalculated geometric ephemeris position, with respect to the
 *                  observer at the time observed light originated from the body -- or else
 *                  `std::nullopt` if no data is readily available.
 *
 * @sa planet_velocity(), has_planet_data()
 */
std::optional<Position> Frame::planet_position(const Planet& planet) const { return planet_position(planet.novas_id()); }

/**
 * Returns the geometric velocity for the specified planet that is readily calculated and stored
 * in this observing frame (if available), or else `std::nullopt` if no such data is available.
 * Typically, only the positions for the Sun, Earth, and the major gravitating bodies (depending
 * on the frame's accuracy) are precalculated and available in this way.
 *
 * @param planet    the NOVAS major planet ID, e.g. `NOVAS_JUPITER`.
 * @return          the precalculated planet's geometric ephemeris velocity, with respect to the
 *                  observer at the time time observed light originated from the body -- or else
 *                  `std::nullopt` if no data is readily available.
 *
 * @sa planet_position(), has_planet_data()
 */
std::optional<Velocity> Frame::planet_velocity(enum novas::novas_planet planet) const {
  double v[3] = {0.0};

  if(!has_planet_data(planet)) {
    novas_error(0, ENOSYS, "Frame::planet_velocity", "No data for planet %d", planet);
    return std::nullopt;
  }

  const double *vp = _frame.planets.vel[1 << planet];
  const double *vo = _frame.obs_vel;

  for(int i = 3; --i >= 0; )
    v[i] = Constant::c * novas::novas_add_beta(vp[i] / Constant::c, -vo[i] / Constant::c);

  return Velocity(v, Unit::au / Unit::day);
}

/**
 * Returns the geometric velocity for the specified planet that is readily calculated and stored
 * in this observing frame (if available), or else `std::nullopt` if no such data is available.
 * Typically, only the positions for the Sun, Earth, and the major gravitating bodies (depending
 * on the frame's accuracy) are precalculated and available in this way.
 *
 * @param planet    the planet.
 * @return          the planet's precalculated geometric position, with respect to the observer at
 *                  the time time observed light originated from the body -- or else `std::nullopt`
 *                  if no data is available.
 *
 * @sa planet_position(), has_planet_data()
 */
std::optional<Velocity> Frame::planet_velocity(const Planet& planet) const { return planet_velocity(planet.novas_id()); }

/**
 * Returns the instantaneous incremental rate at which the observer's clock (i.e. proper time
 * &tau;) ticks faster than a clock in the specified timescale in this observing frame. I.e.,
 * it returns _D_, which is defined by:
 *
 * d&tau;<sub>obs</sub> / dt<sub>timescale</sub> = (1 + _D_)
 *
 * The instantaneous difference in clock rate includes tiny diurnal or orbital variationd for
 * Earth-bound observers as the they cycle through the tidal potential around the geocenter
 * (mainly due to the Sun and Moon). For a closer match to Earth-based timescales (TCG, TT, TAI,
 * GPS, or UTC) you may want to exclude the periodic tidal effects and calculate the averaged
 * observer clock rate over the geocentric cycle (see Eqs. 10.6 and 10.8 of the IERS Conventions
 * 2010), which is provided by `novas_mean_clock_skew()` instead.
 *
 * For reduced accuracy frames, the result will be approximate, because the gravitational effect
 * of the Sun and Earth alone may be accounted for.
 *
 * NOTES:
 * <ol>
 * <li>Based on the IERS Conventions 2010, Chapter 10, Eqa. 10.6 / 10.8 but also including the
 * near-Earth tidal effects, and modified for relativistic observer motion.</li>
 * <li>The potential for an observer inside 0.9 planet radii of a major Solar-system body's center
 * will not include the term for that body in the calculation.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>IERS Conventions 2010, Chapter 10, see at https://iers-conventions.obspm.fr/content/chapter10/tn36_c10.pdf</li>
 * </ol>
 *
 * @param timescale   Reference timescale for the comparison. All timescales except `NOVAS_UT1`
 *                    are supported. (UT1 advances at an irregular rate).
 * @return            The incremental rate at which the observer's proper time clock ticks faster
 *                    than the specified timescale, or else NAN if the input frame is NULL or
 *                    uninitialized, or if the timescale is not supported (errno set to EINVAL),
 *                    or if the frame is configured for full accuracy but it does not have
 *                    sufficient planet position information to evaluate the local gravitational
 *                    potential with precision (errno set to EAGAIN).
 *
 * @sa novas_clock_skew()
 */
double Frame::clock_skew(enum novas_timescale timescale) const {
  return novas_clock_skew(&_frame, timescale);
}

/**
 * Calculates an approximate apparent location on sky for a major planet, Sun, Moon, Earth-Moon
 * Barycenter (EMB) -- typically to arcmin level accuracy -- using Keplerian orbital elements. The
 * returned position is antedated for light-travel time (for Solar-System bodies). It also applies
 * an appropriate aberration correction (but not gravitational deflection).
 *
 * The orbitals can provide planet positions to arcmin-level precision for the rocky inner
 * planets, and to a fraction of a degree precision for the gas and ice giants and Pluto. The
 * accuracies for Uranus, Neptune, and Pluto are significantly improved (to the arcmin level) if
 * used in the time range of 1800 AD to 2050 AD. For a more detailed summary of the typical
 * accuracies, see either of the top two references below.
 *
 * For accurate positions, you should use planetary ephemerides (such as the JPL ephemerides via
 * the CALCEPH or CSPICE plugins) and `novas_sky_pos()` instead.
 *
 * While this function is generally similar to creating an orbital object with an orbit
 * initialized with `novas_make_planet_orbit()` or `novas_make_moon_orbit()`, and then calling
 * `novas_sky_pos()`, there are a few important differences to note:
 *
 * <ol>
 *  <li>This function calculates Earth and Moon positions about the Keplerian orbital position
 *  of the Earth-Moon Barycenter (EMB). In constrast, `novas_make_planet_orbit()` does not provide
 *  orbitals for the Earth directly, and `make_moot_orbit()` references the Moon's orbital to
 *  the Earth position returned by the currently configured planet calculator function (see
 *  `set_planet_provider()`).</li>
 *  <li>This function ignores gravitational deflection. It makes little sense to bother about
 *  corrections that are orders of magnitude below the accuracy of the orbital positions
 *  obtained.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>E.M. Standish and J.G. Williams 1992.</li>
 *  <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M., and Chapront, J. 1983, Astronomy and Astrophysics (ISSN 0004-6361),
 *      vol. 124, no. 1, July 1983, p. 50-62.</li>
 * </ol>
 *
 * @param planet    the planet.
 * @return          approximate apparent position for the given planet. The returned position may
 *                  be invalid if the planet argument is invalid, or is unsupported by the
 *                  Keplerian orbital model (Earth). You should check validity with
 *                  Apparent::is_valid() as appropriate.
 *
 * @sa novas_approx_sky_pos()
 */
Apparent Frame::approx_apparent(const Planet& planet) const {
  sky_pos pos = {};
  novas_approx_sky_pos(planet.novas_id(), _novas_frame(), NOVAS_TOD, &pos);
  return Apparent::from_tod_sky_pos(pos, *this);
}

/**
 * Attempts to create a new observing frame instance for a given observer location, time of
 * observation, and accuracy, if possible, or else returns `std::nullopt` if the frame could not
 * be initialized completely. Note, that full accuracy frames require that you have configured an
 * ephemeris provider for SuperNOVAS already. Otherwise, the returned optional will be invalid.
 *
 * @param obs       observer location
 * @param time      astrometric time of observation
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1).
 * @return          an observing frame instance with the provided parameters, if possible,
 *                  or else `std::nullopt`.
 */
std::optional<Frame> Frame::create(const Observer& obs, const Time& time, enum novas::novas_accuracy accuracy) {
  Frame f = Frame(obs, time, accuracy);
  if(f.is_valid())
    return f;

  return std::nullopt;
}

/**
 * Returns a reference to a statically defined standard invalid observing frame. This invalid
 * frame may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid equatorial coordinates.
 */
const Frame& Frame::invalid() {
  static const Frame _invalid = Frame(Observer::invalid(), Time::invalid(), (enum novas_accuracy) -1);
  return _invalid;
}

} // namespace supernovas
