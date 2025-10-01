/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {
class Frame {
private:
  novas_frame _frame;
  Observer _observer;
  Time _time;
  EOP _eop;

public:

  Frame(const Observer& obs, const Time& time, enum novas_accuracy accuracy = NOVAS_FULL_ACCURACY)
  : _observer(obs), _time(time), _eop({}) {
    novas_make_frame(accuracy, obs._novas_observer(), time._novas_timespec(), 0.0, 0.0, &_frame);
  }

  Frame(const Observer& obs, const Time& time, const EOP& eop, enum novas_accuracy accuracy = NOVAS_FULL_ACCURACY)
  : _observer(obs), _time(time), _eop(eop) {
    novas_make_frame(accuracy, obs._novas_observer(), time._novas_timespec(), eop.xp().mas(), eop.yp().mas(), &_frame);
  }

  Weather weather() const {
    const on_surface *s = &_observer._novas_observer()->on_surf;
    return Weather(s->temperature, s->pressure * Unit::mbar, s->humidity);
  }

  void set_weather(const Weather& weather) {
    on_surface *s = (on_surface *) &_observer._novas_observer()->on_surf;
    s->temperature = weather.temperature().celsius();
    s->pressure = weather.pressure().mbar();
    s->humidity = weather.humidity();
  }

  const novas_frame *_novas_frame() const {
    return &_frame;
  }

  const Time& time() const {
    return _time;
  }

  const Observer& observer() const {
    return _observer;
  }

  const EOP& eop() const {
    return _eop;
  }
};

} // namespace supernovas
