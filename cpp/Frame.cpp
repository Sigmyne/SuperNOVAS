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

Frame::Frame(const Observer& obs, const Time& time, enum novas_accuracy accuracy)
: _observer(obs), _time(time) {
  static const char *fn = "Frame()";

  double xp = 0.0, yp = 0.0;


  if(obs.is_geodetic()) {
    GeodeticObserver& eobs = (GeodeticObserver&) obs;
    xp = eobs.eop().xp().mas();
    yp = eobs.eop().yp().mas();
  }

  if(novas_make_frame(accuracy, obs._novas_observer(), time._novas_timespec(), xp, yp, &_frame) != 0)
    novas_trace_invalid(fn);
  else if(!obs.is_valid())
    novas_error(0, EINVAL, fn, "input observer is invalid");
  else if(!time.is_valid())
    novas_error(0, EINVAL, fn, "input time is invalid");
  else
    _valid = true;
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

Position Frame::ephemeris_position(enum novas::novas_planet planet) const {
  if(!has_planet_data(planet))
    return Position::invalid();
  return Position(_frame.planets.pos[1 << planet], Unit::au);
}

Position Frame::ephemeris_position(const Planet& planet) const { return ephemeris_position(planet.novas_id()); }

Velocity Frame::ephemeris_velocity(enum novas::novas_planet planet) const {
  if(!has_planet_data(planet))
    return Velocity::invalid();
  return Velocity(_frame.planets.vel[1 << planet], Unit::au / Unit::day);
}

Velocity Frame::ephemeris_velocity(const Planet& planet) const { return ephemeris_velocity(planet.novas_id()); }

double Frame::clock_skew(enum novas_timescale timescale) const {
  return novas_clock_skew(&_frame, timescale);
}

Apparent Frame::approx_apparent(const Planet& planet, enum novas_reference_system system) const {
  sky_pos pos = {};
  novas_approx_sky_pos(planet.novas_id(), _novas_frame(), system, &pos);
  return Apparent(*this, pos, system);
}

static const Frame &_invalid = Frame(Observer::invalid(), Time::invalid(), (enum novas_accuracy) -1);
const Frame& Frame::invalid() {
  return _invalid;
}

} // namespace supernovas
