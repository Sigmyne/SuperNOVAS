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
 * Constructs a new observer frame, given the observer location and time of observation, and
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
 * @sa Frame::create()
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

const novas_frame * Frame::_novas_frame() const {
  return &_frame;
}

enum novas_accuracy Frame::accuracy() const {
  return _frame.accuracy;
}

const Time& Frame::time() const {
  return _time;
}

const Observer& Frame::observer() const {
  return _observer;
}

bool Frame::has_planet_data(enum novas::novas_planet planet) const {
  return (_frame.planets.mask & (1 << planet)) != 0;
}

bool Frame::has_planet_data(const Planet& planet) const { return has_planet_data(planet.novas_id()); }

std::optional<Position> Frame::ephemeris_position(enum novas::novas_planet planet) const {
  if(!has_planet_data(planet)) {
    novas_error(0, ENOSYS, "Frame::ephemeris_position", "No data for planet %d", planet);
    return std::nullopt;
  }
  return Position(_frame.planets.pos[1 << planet], Unit::au);
}

std::optional<Position> Frame::ephemeris_position(const Planet& planet) const { return ephemeris_position(planet.novas_id()); }

std::optional<Velocity> Frame::ephemeris_velocity(enum novas::novas_planet planet) const {
  if(!has_planet_data(planet)) {
    novas_error(0, ENOSYS, "Frame::ephemeris_velocity", "No data for planet %d", planet);
    return std::nullopt;
  }
  return Velocity(_frame.planets.vel[1 << planet], Unit::au / Unit::day);
}

std::optional<Velocity> Frame::ephemeris_velocity(const Planet& planet) const { return ephemeris_velocity(planet.novas_id()); }

double Frame::clock_skew(enum novas_timescale timescale) const {
  return novas_clock_skew(&_frame, timescale);
}

Apparent Frame::approx_apparent(const Planet& planet) const {
  sky_pos pos = {};
  novas_approx_sky_pos(planet.novas_id(), _novas_frame(), NOVAS_TOD, &pos);
  return Apparent::from_tod_sky_pos(pos, *this);
}

std::optional<Frame> Frame::create(const Observer& obs, const Time& time, enum novas::novas_accuracy accuracy) {
  Frame f = Frame(obs, time, accuracy);
  if(f.is_valid())
    return f;

  return std::nullopt;
}

const Frame& Frame::invalid() {
  static const Frame _invalid = Frame(Observer::invalid(), Time::invalid(), (enum novas_accuracy) -1);
  return _invalid;
}

} // namespace supernovas
