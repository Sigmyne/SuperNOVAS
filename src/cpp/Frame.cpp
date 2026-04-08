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
 * In either case, you can obtain more information on why things went wrong, when they do, by
 * enabling debug mode is enabled via `novas_debug()` prior to constructing a Frame.
 *
 * @param obs         observer location
 * @param time        time of observation
 * @param accuracy    (optional) NOVAS_FULL_ACCURACY (default) or NOVAS_REDUCED_ACCURACY.
 *
 * @since 1.6
 * @sa Observer::frame_at(), reduced_accuracy()
 */
Frame::Frame(const Observer& obs, const Time& time, enum novas_accuracy accuracy)
: _observer(obs.copy()), _time(time) {
  static const char *fn = "Frame()";

  double xp = NAN, yp = NAN;

  errno = 0;

  if(obs.is_geodetic()) {
    const GeodeticObserver *go = dynamic_cast<const GeodeticObserver *>(_observer);
    xp = go->mean_eop().xp().mas();
    yp = go->mean_eop().yp().mas();
  }

  if(novas_make_frame(accuracy, obs._novas_observer(), time._novas_timespec(), xp, yp, &_frame) != 0)
    novas_trace_invalid(fn);
  if(!obs.is_valid())
    novas_set_errno(EINVAL, fn, "input observer is invalid");
  if(!time.is_valid())
    novas_set_errno(EINVAL, fn, "input time is invalid");

  _valid = (errno == 0);
}

/**
 * Custom copy contructor, that points to a copy of the observer.
 *
 * @param frame   the frame to be copied.
 *
 * @since 1.6
 */
Frame::Frame(const Frame& frame)
: Validating(frame), _observer(frame._observer->copy()), _time(frame.time()), _frame(frame._frame) {}

/**
 * Custom copy-assignment operator, which updates the observer pointer to
 * a copy of the original frame's observer.
 *
 * @param frame   the frame to be copied.
 * @return        itself
 *
 * @since 1.6
 */
Frame& Frame::operator=(const Frame& frame) {
  if(this == &frame)
    return *this;     // self assignment

  const Observer *old = _observer;
  _observer = frame._observer->copy();
  _time = frame._time;
  _frame = frame._frame;
  _valid = frame._valid;
  delete old;
  return *this;
}

/**
 * Returns the pointer to the underlying NOVAS C `novas_frame` data structure of this observing
 * frame.
 *
 * @return    pointer to the underlying NOVAS C data structure.
 *
 * @since 1.6
 */
const novas_frame * Frame::_novas_frame() const {
  return &_frame;
}

/**
 * Returns the accuracy type of this bserving frame.
 *
 * @return    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1).
 *
 * @since 1.6
 */
enum novas_accuracy Frame::accuracy() const {
  return _frame.accuracy;
}

/**
 * Returns the observer location (and motion) of this observing frame
 *
 * @return    the observer location (and motion).
 *
 * @since 1.6
 * @sa time()
 */
const Observer& Frame::observer() const {
  return *_observer;
}

/**
 * Returns the astrometric time of observation of this observing frame.
 *
 * @return    the astrometric time of observation
 *
 * @since 1.6
 * @sa jd(), observer()
 */
const Time& Frame::time() const {
  return _time;
}

/**
 * Returns the Earth Orientation Parameters contained in this frame. The returned parameters
 * include diurnal variations for libration and ocean tides, and are valid only if this frame was
 * defined for a geodetic observer location such as an observer at a fixed site on Earth or an
 * airborne observer location.
 *
 * @return    The Earth orientation parameters of this observing frame, including diurnal
 *            variations for libration and ocean tides; or else an undefined (invalid)
 *            EOP if the observer is not on or near Earth's surface.
 *
 * @since 1.6
 * @sa GeodeticObserver
 */
const EOP Frame::eop() const {
  EOP eop = EOP(novas_time_leap(&_frame.time), _frame.time.dut1, _frame.dx * Unit::mas, _frame.dy * Unit::mas);
  if(!eop.is_valid())
    novas_trace_invalid("Frame::eop()");
  return eop;
}

/**
 * Returns the precise Julian Date of this observing frame, in the specific timescale of choice.
 * It is but a shorthand for `time().jd(timescale)`.
 *
 * @param timescale   (optional) the timescale in which to return the result (default: TT).
 * @return            [day] the precise Julian date in the requested timescale.
 *
 * @since 1.6
 * @sa Time::jd(), time()
 */
double Frame::jd(enum novas_timescale timescale) const {
  return _time.jd(timescale);
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
 *
 *  1. Based on the IERS Conventions 2010, Chapter 10, Eqa. 10.6 / 10.8 but also including the
 *     near-Earth tidal effects, and modified for relativistic observer motion.
 *
 *  2. The potential for an observer inside 0.9 planet radii of a major Solar-system body's center
 *     will not include the term for that body in the calculation.
 *
 *
 * REFERENCES:
 *
 *  1. IERS Conventions 2010, Chapter 10, see at https://iers-conventions.obspm.fr/content/chapter10/tn36_c10.pdf
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
 * @since 1.6
 * @sa novas_clock_skew(), Time::timescale_offset()
 */
double Frame::clock_skew(enum novas_timescale timescale) const {
  double D = novas_clock_skew(&_frame, timescale);
  if((unsigned) timescale >= NOVAS_TIMESCALES)
    novas_trace_invalid("Frame::clock_skew");
  return D;
}

/**
 * Returns new geometric coordinates, relative to the observer in this frame, in the equatorial coordinate
 * reference system of choice.
 *
 * @param p       equatorial position vector, with respect to the observer
 * @param v       equatorial velocity vector, with respect to the observer
 * @param system  equatorial coordinate reference_system, in which position and velocity vectors
 *                are defined
 *
 * @since 1.6
 */
Geometric Frame::geometric(const Position& p, const Velocity& v, enum novas_reference_system system) const {
  Geometric g(*this, p, v, system);
  if(!g.is_valid())
    novas_trace_invalid("Frame::geometric()");
  return g;
}

/**
 * Returns the observer's ICRS position relative to the Solar System Barycenter (SSB).
 *
 * @return      The Solar system barycentric position of the observer at the time of observation.
 *
 * @since 1.6
 * @sa observer_ssb_velocity(), observer()
 */
Position Frame::observer_ssb_position() const {
  if(!is_valid()) {
    novas_trace_invalid("Frame::observer_ssb_position()");
    return Position::undefined();
  }
  return Position(_novas_frame()->obs_pos, Unit::AU);
}

/**
 * Returns the observer's ICRS velocity relative to the Solar System Barycenter (SSB).
 *
 * @return      The Solar system barycentric velocity of the observer at the time of observation.
 *
 * @since 1.6
 * @sa observer_ssb_position(), observer()
 */
Velocity Frame::observer_ssb_velocity() const {
  if(!is_valid()) {
    novas_trace_invalid("Frame::observer_ssb_velocity()");
    return Velocity::undefined();
  }
  return Velocity(_novas_frame()->obs_vel, Unit::AU_per_day);
}

/**
 * Returns the Moon's geometric position using the ELP/MPP02 model by Chapront &amp; Francou
 * (2003), down to the specified limiting term amplitude.
 *
 * NOTES:
 *
 *  - The initial implementation (in v1.6) truncates the full series, keeping only terms with
 *    amplitudes larger than 1 mas (around 3400 harmonic terms in total), resulting in a limiting
 *    accuracy below 1 km level (and less than 100 meter error typically for 1900 -- 2100).
 *
 * REFERENCES:
 *
 *  1. Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)
 *  2. Chapront, J., Francou G., 2003, A&amp;A, 404, 735
 *  3. Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 *     https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/
 *
 * @param limit_term    (optional) [arcsec|km] Sum only the harmonic terms with amplitudes larger
 *                      than this limit.
 * @return              The calculated geometric geocentric position and velocity of the Moon
 *                      in the ICRS.
 *
 * @since 1.6
 * @sa apparent_moon_elp2000()
 */
Geometric Frame::geometric_moon_elp2000(double limit_term) const {
  if(!is_valid()) {
    novas_trace_invalid("Frame::geometric_moon_elp_2000()");
    return Geometric::undefined();
  }

  double p[3] = {0.0}, v[3] = {0.0};
  novas_moon_elp_posvel_fp(_novas_frame(), limit_term, NOVAS_ICRS, p, v);
  return Geometric(*this, Position(p, Unit::AU), Velocity(v, Unit::AU_per_day), NOVAS_ICRS);
}

/**
 * Returns the Moon's apparent position using the ELP/MPP02 model by Chapront &amp; Francou
 * (2003) down to the specified limiting term amplitude.
 *
 * NOTES:
 *
 *  - The initial implementation (in v1.6) truncates the full series, keeping only terms with
 *    amplitudes larger than 1 mas (around 3400 harmonic terms in total), resulting in a limiting
 *    accuracy below 1 km level (and less than 100 meter error typically for 1900 -- 2100).
 *
 * REFERENCES:
 *
 *  1. Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)
 *  2. Chapront, J., Francou G., 2003, A&amp;A, 404, 735
 *  3. Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 *     https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/
 *
 * @param limit_term    (optional) [arcsec|km] Sum only the harmonic terms with amplitudes larger
 *                      than this limit.
 * @return              The apparent place of the moon on the observer's sky.
 *
 * @since 1.6
 * @sa geometric_moon_elp2000()
 */
Apparent Frame::apparent_moon_elp2000(double limit_term) const {
  if(!is_valid()) {
    novas_trace_invalid("Frame::apparent_moon_elp_2000()");
    return Apparent::undefined();
  }

  sky_pos p = {};
  novas_moon_elp_sky_pos_fp(_novas_frame(), limit_term, NOVAS_TOD, &p);

  return Apparent::from_tod_sky_pos(*this, &p);
}

/**
 * Returns a reduced accuracy observing frame for the specified observer at the specified time.
 * Reduced accuracy frames provide 1 mas accuracy typically, and do not require a planet provider
 * to be configured. As such, they offer a simplest way for obtaining astrometric positions for
 * catalog and orbital sources at the 1 mas level.
 *
 * Note, that the returned frame may be invalid, if the this observer or the time argument
 * themselves are invalid.
 *
 * @param obs       %Observer location
 * @param time      Astrometric time of observation
 * @return          A reduced accuracy observing frame for the specified time of observation.
 *
 * @since 1.6
 * @sa Observer::reduced_accuracy_frame_at()
 */
Frame Frame::reduced_accuracy(const Observer& obs, const Time& time) {
  Frame f(obs, time, NOVAS_REDUCED_ACCURACY);
  if(!f.is_valid())
    novas_trace_invalid("Frame::reduced_accuracy()");
  return f;
}

/**
 * Returns a human-readable string representation of this observing frame.
 *
 * @return    a string representation of this observing frame.
 *
 * @since 1.6
 */
std::string Frame::to_string() const {
  return "Frame for " + _observer->to_string() + " at " + _time.to_string();
}

/**
 * Returns a reference to a statically defined standard invalid observing frame. This invalid
 * frame may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid equatorial coordinates.
 *
 * @since 1.6
 */
const Frame& Frame::undefined() {
  static const Frame _invalid = Frame();
  return _invalid;
}



} // namespace supernovas
