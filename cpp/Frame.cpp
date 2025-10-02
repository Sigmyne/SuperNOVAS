/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"

using namespace supernovas;


Frame::Frame(const Observer& obs, const Time& time, enum novas_accuracy accuracy)
: _observer(obs), _time(time), _eop({}) {
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

const Time& Frame::time() const {
  return _time;
}

const Observer& Frame::observer() const {
  return _observer;
}
