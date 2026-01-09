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

static Interval from_tt(double x, enum novas_timescale timescale) {
  switch(timescale) {
    case NOVAS_TCB: return Interval(x * (1.0 + Constant::L_B), timescale);
    case NOVAS_TCG: return Interval(x * (1.0 + Constant::L_G), timescale);
    default: return Interval(x, timescale);
  }
}

static double tt_seconds(const Interval& interval) {
  switch(interval.timescale()) {
    case NOVAS_TCB: return  interval.seconds() / (1.0 + Constant::L_B);
    case NOVAS_TCG: return  interval.seconds() / (1.0 + Constant::L_G);
    default: return interval.seconds();
  }
}

/**
 * Instantiates a new time interval with the specified time in seconds and
 * a timescale (TT by default).
 *
 * @param seconds     [s] time span in seconds.
 * @param timescale   (optional) timescale in which the interval was defined (default: `novas:NOVAS_TT`)
 *
 * @sa zero()
 */
Interval::Interval(double seconds, enum novas_timescale timescale)
: _seconds(seconds), _scale(timescale) {
  static const char *fn = "Interval(seconds, timescale)";

  if(isnan(seconds))
    novas_set_errno(EINVAL, fn , "input seconds is NAN");
  else if((unsigned) timescale >= NOVAS_TIMESCALES)
    novas_set_errno(EINVAL, fn , "invalid timescale: %d", timescale);
  else
    _valid = true;
}

/**
 * Returns the distance travelled in this time interval at the specified speed.
 *
 * @param v   speed value
 * @return    the distance travelled under this time interval at the specified speed.
 */
Distance Interval::operator*(const Speed& v) const {
  return v.travel(*this);
}

/**
 * Returns the spatial vector travelled in this time interval at the specified velocity.
 *
 * @param v   speed value
 * @return    the statial vector travelled under this interval at the specified velocity.
 */
Position Interval::operator*(const Velocity& v) const {
  return v.travel(*this);
}

/**
 * Returns a new time interval that is the sum of this time interval and the specified other
 * time interval.
 *
 * @param r   the other time interval
 * @return    the sum of this time interval and the argument time interval.
 *
 * @sa operator-()
 */
Interval Interval::operator+(const Interval& r) const {
  return from_tt(tt_seconds(*this) + tt_seconds(r), timescale());
}

/**
 * Returns a new time interval that is the signed difference of this time interval and the
 * specified other time interval.
 *
 * @param r   the other time interval
 * @return    the signed difference of this time interval and the argument time interval.
 *
 * @sa operator+()
 */
Interval Interval::operator-(const Interval& r) const {
  return from_tt(tt_seconds(*this) - tt_seconds(r), timescale());
}

/**
 * Checks if this time interval is equal to the specified other time interval within the
 * specified precision. The comparison is performed in Terrestrial Time (TT).
 *
 * @param interval    the reference time interval.
 * @param precision   [s] the precision used for checking equality.
 * @return            `true` if this time interval is equal to the reference time interval within
 *                    the specified precision, or else `false`.
 *
 * @sa operator==(), operator!=()
 */
bool Interval::equals(const Interval& interval, double precision) const {
  return fabs(tt_seconds(*this) - tt_seconds(interval)) < fabs(precision);
}

/**
 * Checks if this time interval is equal to the specified other time interval at the full
 * double-precision. The comparison is performed in Terrestrial Time (TT).
 *
 * @param interval    the reference time interval.
 * @return            `true` if this time interval is equal to the reference time interval, or
 *                    else `false`.
 *
 * @sa equals(), operator!=()
 */
bool Interval::operator==(const Interval& interval) const {
  return equals(interval);
}

/**
 * Checks if this time interval differs from the specified other time interval at the full
 * double-precision. The comparison is performed in Terrestrial Time (TT).
 *
 * @param interval    the reference time interval.
 * @return            `true` if this time interval differs from the reference time interval, or
 *                    else `false`.
 *
 * @sa equals(), operator==()
 */
bool Interval::operator!=(const Interval& interval) const {
  return !equals(interval);
}

/**
 * Returns the time scale in which this time interval was defined.
 *
 * @return      the time scale for this interval.
 */
enum novas_timescale Interval::timescale() const {
  return _scale;
}

/**
 * Returns the inverse of this time interval ie, its negated value.
 *
 * @return    a new time interval with the same absolute value, but negated.
 */
Interval Interval::inv() const {
  return Interval(-_seconds);
}

/**
 * Returns this time interval in milliseconds
 *
 * @return  [ms] the time interval
 *
 * @sa seconds(), minutes(), hours(), days(), weeks(), years(), julian_years(), julian_centuries()
 */
double Interval::milliseconds() const {
  return _seconds / Unit::ms;
}

/**
 * Returns this time interval in seconds
 *
 * @return  [s] the time interval
 *
 *  @sa Interval::milliseconds(), Interval::minutes(), Interval::hours(), Interval::days(),
 *      Interval::weeks(), Interval::years(), Interval::julian_years(),
 *      Interval::julian_centuries()
 */
double Interval::seconds() const {
  return _seconds;
}

/**
 * Returns this time interval in minutes
 *
 * @return  [min] the time interval
 *
 *  @sa milliseconds(), seconds(), hours(), days(), weeks(), years(), julian_years(),
 *      julian_centuries()
 */
double Interval::minutes() const {
  return _seconds / Unit::min;
}

/**
 * Returns this time interval in hours
 *
 * @return  [h] the time interval
 *
 *  @sa milliseconds(), seconds(), minutes(), days(), weeks(), years(), julian_years(),
 *      julian_centuries()
 */
double Interval::hours() const {
  return _seconds / Unit::hour;
}

/**
 * Returns this time interval in days
 *
 * @return  [day] the time interval
 *
 *  @sa milliseconds(), seconds(), minutes(), hours(), weeks(), years(), julian_years(),
 *      julian_centuries()
 */
double Interval::days() const {
  return _seconds / Unit::day;
}

/**
 * Returns this time interval in weeks
 *
 * @return  [wk] the time interval
 *
 *  @sa milliseconds(), seconds(), minutes(), hours(), days(), years(), julian_years(),
 *      julian_centuries()
 */
double Interval::weeks() const {
  return _seconds / Unit::week;
}

/**
 * Returns this time interval in calendar years
 *
 * @return  [yr] the time interval
 *
 *  @sa milliseconds(), seconds(), minutes(), hours(), days(), weeks(), julian_years(),
 *      julian_centuries()
 */
double Interval::years() const {
  return _seconds / Unit::yr;
}

/**
 * Returns this time interval in Julian years
 *
 * @return  [yr] the time interval
 *
 *  @sa milliseconds(), seconds(), minutes(), hours(), days(), weeks(), years(),
 *      julian_centuries()
 */
double Interval::julian_years() const {
  return _seconds / Unit::julian_year;
}

/**
 * Returns this time interval in Julian centuries.
 *
 * @return  [cy] the time interval
 *
 *  @sa milliseconds(), seconds(), minutes(), hours(), days(), weeks(), years(), julian_years()
 */
double Interval::julian_centuries() const {
  return _seconds / Unit::julian_century;
}

/**
 * Returns a new time interval, which matches this time interval in the specified other timescale.
 *
 * @param scale   the timescale of the returned equivalent interval.
 * @return        the equivalent time interval in the specified timescale
 */
Interval Interval::to_timescale(enum novas_timescale scale) const {
  return from_tt(tt_seconds(*this), scale);
}

/**
 * Returns a reference to the zero time interval.
 *
 * @return  a reference to a statically defined zero time interval.
 */
const Interval& Interval::zero() {
  static const Interval _zero = Interval(0.0);
  return _zero;
}

/**
 * Returns a string representation of this interval using the specified number of significant
 * figures and a best matched time unit.
 *
 * @return    A human readable string representation of the distance and a unit specifier.
 */
std::string Interval::to_string(int decimals) const {
  char fmt[20] = {'\0'};
  char s[40] = {'\0'};

  double value;
  const char *unit;

  if(decimals < 0)
    decimals = 0;
  else if(decimals > 16)
    decimals = 16;

  double d = fabs(_seconds);

  if(d < Unit::s) {
    value = milliseconds();
    unit = "ms";
  }
  else if(d < Unit::hour) {
    value = _seconds;
    unit = "s";
  }
  else if(d < Unit::day) {
    value = hours();
    unit = "h";
  }
  else if(d < Unit::yr) {
    value = days();
    unit = "d";
  }
  else {
    value = years();
    unit = "yr";
  }

  snprintf(fmt, sizeof(fmt), "%%.%df", decimals);
  snprintf(s, sizeof(s), fmt, value);

  return std::string(s) + " " + std::string(unit);
}


} // namespace supernovas
