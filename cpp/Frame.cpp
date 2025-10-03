/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.hpp"


using namespace novas;


namespace supernovas {

Frame::Frame(const Observer& obs, const Time& time, enum novas_accuracy accuracy)
: _observer(obs), _time(time) {
  double xp = 0.0, yp = 0.0;

  if(obs.is_geodetic()) {
    GeodeticObserver& eobs = (GeodeticObserver&) obs;
    xp = eobs.eop().xp().mas();
    yp = eobs.eop().yp().mas();
  }

  novas_make_frame(accuracy, obs._novas_observer(), time._novas_timespec(), xp, yp, &_frame);
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

double Frame::clock_skew(enum novas_timescale timescale) const {
  return novas_clock_skew(&_frame, timescale);
}

Apparent Frame::approx_apparent(const Planet& planet, enum novas_reference_system system) const {
  sky_pos pos = {};
  novas_approx_sky_pos(planet.novas_id(), _novas_frame(), system, &pos);
  return Apparent(*this, &pos, system);
}

} // namespace supernovas
