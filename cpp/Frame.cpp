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
    novas_set_errno(EINVAL, fn, "input observer is invalid");
  else if(!time.is_valid())
    novas_set_errno(EINVAL, fn, "input time is invalid");
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
 *                    potential with p
  Frame to_observer(const Observer& observer) const; // TODOrecision (errno set to EAGAIN).
 *
 * @sa novas_clock_skew()
 */
double Frame::clock_skew(enum novas_timescale timescale) const {
  return novas_clock_skew(&_frame, timescale);
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
