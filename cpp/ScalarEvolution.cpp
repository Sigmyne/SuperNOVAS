/**
 * @file
 *
 * @date Created  on Oct 11, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include <cmath>
#include <type_traits>


#include "supernovas.h"

using namespace novas;

namespace supernovas {


/**
 * Instantiate an time evolution set for a scalar quantity.
 *
 * @param value   [?] momentary value
 * @param rate    [?/s] (optional) momentary rate of change in value (default: 0).
 * @param accel   [?/s<sup>2</sup>] (optional) momentary acceleration of value (default: 0).
 */
ScalarEvolution::ScalarEvolution(double value, double rate, double accel)
        : _value(value), _rate(rate), _accel(accel) {
  static const char *fn = "ScalarEvolution()";

  errno = 0;

  if(!isfinite(_value))
    novas_set_errno(EINVAL, fn, "value is NAN or infinite");
  if(!isfinite(_rate))
    novas_set_errno(EINVAL, fn, "rate is NAN or infinite");
  if(!isfinite(_accel))
    novas_set_errno(EINVAL, fn, "acceleration is NAN or infinite");

  _valid = (errno == 0);
}

/**
 * Returns an extrapolated momentary value of the evolving quantity at an offset time, using
 * the defined derivatives.
 *
 * @param offset    time offset from when reference value and derivatives were defined.
 * @return          the extrapolated momentary scalar value at the offset time.
 *
 * @sa rate(), acceleration()
 */
double ScalarEvolution::value(const Interval& offset) const {
  return _value + offset.seconds() * (_rate + offset.seconds() * _accel);
}

/**
 * Returns an extrapolated momentary rate of change for the evolving quantity at an offset
 * time, using the defined acceleration.
 *
 * @param offset    time offset from when reference value and derivatives were defined.
 * @return          the extrapolated momentary rate of change at the offset time.
 *
 * @sa value(), acceleration()
 */
double ScalarEvolution::rate(const Interval& offset) const {
  return _rate + _accel * offset.seconds();
}

/**
 * Returns the defined (constant) acceleration value.
 *
 * @return    the acceleration that was defined.
 *
 * @sa value(), rate()
 */
double ScalarEvolution::acceleration() const {
  return _accel;
}

/**
 * Returns a new scalar evolution instance for a stationary quantity.
 *
 * @param value   The stationary (unevolving) value
 * @return        a non-evolving scalar evolution instance with the specified stationary value.
 */
ScalarEvolution ScalarEvolution::stationary(double value) {
  return ScalarEvolution(value);
}

/**
 * Returns a reference to a statically allocated undefined scalar evolution instance.
 *
 * @return    a reference to a static undefined scalar evolution instance.
 */
const ScalarEvolution& ScalarEvolution::undefined() {
  static const ScalarEvolution _undefined = ScalarEvolution();
  return _undefined;
}



} // namespace supernovas

