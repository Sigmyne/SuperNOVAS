/**
 * @file
 *
 * @date Created  on Oct 11, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

/**
 * Instantiate an time evolution set for a scalar quantity.
 *
 * @param pos     [?] momentary value
 * @param vel     [?/s] momentary rate of change in value
 * @param accel   [?/s<sup>2</sup>] momentary acceleration of value
 */
Evolution::Evolution(double pos, double vel, double accel)
: _value(pos), _rate(vel), _accel(accel) {
  // TODO validation

  _valid = true;
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
double Evolution::value(const Interval& offset) const {
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
double Evolution::rate(const Interval& offset) const {
  return _rate + _accel * offset.seconds();
}

/**
 * Returns the defined (constant) acceleration value.
 *
 * @return    the acceleration that was defined.
 *
 * @sa value(), rate()
 */
double Evolution::acceleration() const {
  return _accel;
}

/**
 * Returns a reference to a statically defined standard invalid time evolution. These invalid
 * evolutions may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid time evolution.
 */
const Evolution& Evolution::zero() {
  static const Evolution _zero = Evolution(0.0, 0.0);
  return _zero;
}


static const novas_track _default_track = {};

/**
 * Instantiates a local trajectory estimate for a source on sky, which can be used to extrapolate
 * positions around the specified reference time inside an interval of validity.
 *
 * @param ref_time  astrometric time at which the momentary coordinates and their time evolutions
 *                  are defined.
 * @param range     time range of validity around the reference time. Attempts to extrapolate
 *                  outside this interval will produce invalid data.
 * @param lon       the time evolution of the longitude (in whatever coordinate system)
 * @param lat       the time evolution of the latitude (in whatever coordinate system)
 * @param r         the time evolution of distance.
 */
Track::Track(const Time& ref_time, const Interval& range, const Evolution& lon, const Evolution& lat, const Evolution& r)
: _ref_time(ref_time), _range(range), _lon(lon), _lat(lat), _r(r) {

  // TODO validation
  _valid = true;
}

/**
 * Instantiates a local trajectory estimate for a source on sky, which can be used to extrapolate
 * positions around the specified reference time inside an interval of validity.
 *
 * @param track     Pointer to the C tajectory data structure
 * @param range     time range of validity around the reference time. Attempts to extrapolate
 *                  outside this interval will produce invalid data.
 *
 * @sa HorizontalTrack(), EquatorialTrack()
 */
Track::Track(const novas_track *track, const Interval& range)
: _ref_time(&track->time), _range(range),
  _lon(Evolution(track->pos.lon * Unit::deg, track->rate.lon * Unit::deg / Unit::sec, track->accel.lon * Unit::deg / (Unit::sec * Unit::sec))),
  _lat(Evolution(track->pos.lat * Unit::deg, track->rate.lat * Unit::deg / Unit::sec, track->accel.lat * Unit::deg / (Unit::sec * Unit::sec))),
  _r(Evolution(track->pos.dist * Unit::au, track->rate.dist * Unit::au / Unit::sec, track->accel.dist * Unit::au / (Unit::sec * Unit::sec))) {

  // TODO validation
  _valid = (track != &_default_track);
}

/**
 * Returns the longitudinal time evolution component of this trajectory.
 *
 * @return    the longitudinal time evolution component.
 *
 * @sa longitude_at(), latitude_evolution(), distance_evolution()
 */
const Evolution& Track::longitude_evolution() const {
  return _lon;
}

/**
 * Returns the latitudinal time evolution component of this trajectory.
 *
 * @return    the latitudinal time evolution component.
 *
 * @sa latitude_at(), longitude_evolution(), distance_evolution()
 */
const Evolution& Track::latitude_evolution() const {
  return _lat;
}

/**
 * Returns the time evolution of radial distance in this trajectory.
 *
 * @return    the time evolution of distance.
 *
 * @sa distance_at(), longitude_evolution(), latitude_evolution()
 */
const Evolution& Track::distance_evolution() const {
  return _r;
}

/**
 * Checks if the trajectory provides valid extrapolated data at a given astrometric time.
 *
 * @param time    astrometric time
 * @return        `true` if this trajectory can provide valid extrapolated position / velocity
 *                data for the given time instant, or else `false` if the time is outside the
 *                range of validity for this trajectory.
 *
 * @sa reference_time(), range()
 */
bool Track::is_valid_at(const Time& time) const {
  return (time - _ref_time).seconds() <= _range.seconds();
}

/**
 * Returns the astrometic time of reference, at which the (quadratic) trajectory has been defined.
 *
 * @return  the reference time for this trajectory.
 *
 * @sa range(), is_valid()
 */
const Time& Track::reference_time() const {
  return _ref_time;
}

/**
 * Returns the time range of validity for this trajectory. The trajectory can only provide valid
 * extrapolation within that time range around the reference time, for which the trajectory's
 * paramteres have been defined.
 *
 * @return   the time range of validity around the reference time.
 *
 * @sa reference_time(), is_valid()
 */
const Interval& Track::range() const {
  return _range;
}

/**
 * Returns the momentary extrapolated longitude angle, without checking if the requested time is
 * inside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated longitude angle.
 *
 * @sa unchecked_latitude(), unchecked_distance()
 */
Angle Track::unchecked_longitude(const Time& time) const {
  return Angle(_lon.value(time - _ref_time));
}

/**
 * Returns the momentary extrapolated latitude angle, without checking if the requested time is
 * inside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated latitude angle.
 *
 * @sa unchecked_longitude(), unchecked_distance()
 */
Angle Track::unchecked_latitude(const Time& time) const {
  return Angle(_lat.value(time - _ref_time));
}

/**
 * Returns the momentary extrapolated distance, without checking if the requested time is
 * inside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated distance.
 *
 * @sa unchecked_longitude(), unchecked_latitude()
 */
Distance Track::unchecked_distance(const Time& time) const {
  return Distance(_r.value(time - _ref_time));
}

/**
 * Returns the momentary extrapolated longitude angle (if valid), or else `std::nullopt` if
 * the time is outside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated longitude angle (if valid), or else `std::nullopt`.
 *
 * @sa latitude_at(), distance_at(), radial_velocity_at(), redshift_at()
 */
std::optional<Angle> Track::longitude_at(const Time& time) const {
  if(is_valid_at(time))
    return unchecked_longitude(time);
  return std::nullopt;
}

/**
 * Returns the momentary extrapolated latitude angle (if valid), or else `std::nullopt` if
 * the time is outside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated latitude angle (if valid), or else `std::nullopt`.
 *
 * @sa longitude_at(), distance_at(), radial_velocity_at(), redshift_at()
 */
std::optional<Angle> Track::latitude_at(const Time& time) const {
  if(is_valid_at(time))
    return unchecked_latitude(time);
  return std::nullopt;
}

/**
 * Returns the momentary extrapolated redshift measure (if valid), or else NAN if
 * the time is outside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated redshift measure (if valid), or else NAN.
 *
 * @sa longitude_at(), latitude_at(), distance_at(), radial_velocity_at()
 */
double Track::redshift_at(const Time& time) const {
  return is_valid_at(time) ? novas_v2z(radial_velocity_at(time).value().km_per_s()) : NAN;
}

/**
 * Returns the momentary extrapolated distance (if valid), or else `std::nullopt` if
 * the time is outside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated distance (if valid), or else `std::nullopt`.
 *
 * @sa longitude_at(), latitude_at(), radial_velocity_at(), redshift_at()
 */
std::optional<Distance> Track::distance_at(const Time& time) const {
  if(is_valid_at(time))
    return unchecked_distance(time);
  return std::nullopt;
}

/**
 * Returns the momentary extrapolated radial velocity (if valid), or else `std::nullopt` if
 * the time is outside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated radial velocity (if valid), or else `std::nullopt`.
 *
 * @sa longitude(), latitude(), distance(), redshift_at()
 */
std::optional<Speed> Track::radial_velocity_at(const Time& time) const {
  if(is_valid_at(time))
    return Speed(_r.rate(time - _ref_time));
  return std::nullopt;
}

/**
 * Instantiates a short-term horizontal source trajectory on sky for a given time range of
 * validity.
 *
 * @param track   pointer to a C `novas::novas_track` data structure.
 * @param range   time range of validity around the reference time of the trajectory.
 */
HorizontalTrack::HorizontalTrack(const novas::novas_track *track, const Interval& range)
: Track(track, range) {}

/**
 * Instantiates a short-term horizontal source trajectory on sky for a given reference time,
 * time evolution, and time range of validity.
 *
 * @param ref_time    astrometric reference time for which the data is defined.
 * @param range       time range of validity around the reference time for extrapolating.
 * @param azimuth     short-term time evolution of the azimuth coordinate.
 * @param elevation   short-term time evolution of the elevation coordinate.
 * @param distance    short-term time evolution of the distance coordinate.
 *
 * @sa from_novas_track(), EquatorialTrack::EquatorialTrack()
 */
HorizontalTrack::HorizontalTrack(const Time& ref_time, const Interval& range,
        const Evolution& azimuth, const Evolution& elevation, const Evolution& distance)
: Track(ref_time, range, azimuth, elevation, distance) {}


/**
 * Returns the momentary extrapolated horizontal coordinates (if valid), or else `std::nullopt` if
 * the time is outside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated horizontal coordinates (if valid), or else
 *                  `std::nullopt`.
 *
 * @sa EquatorialTrack::projected_at()
 */
std::optional<Horizontal> HorizontalTrack::projected_at(const Time& time) const {
  if(is_valid_at(time))
    return Horizontal(unchecked_longitude(time), unchecked_latitude(time));
  return std::nullopt;
}

/**
 * Instantiates a short-term trajectory estimate for a source in horizontal coordinates, which can
 * be used to extrapolate the horizontal (Az/El) positions of the source around the specified
 * reference time inside an interval of validity.
 *
 * @param track     Pointer to the C tajectory data structure
 * @param range     time range of validity around the reference time. Attempts to extrapolate
 *                  outside this interval will produce invalid data.
 *
 * @sa EquatorialTrack::from_novas_track()
 */
HorizontalTrack HorizontalTrack::from_novas_track(const novas_track *track, const Interval& range) {
  if(!track) {
    novas_set_errno(EINVAL, "HorizontalTrack::from_novas_track", "input track is NULL");
    return HorizontalTrack(&_default_track, range);
  }
  return HorizontalTrack(track, range);
}

/**
 * Returns the momentary extrapolated equatorial coordinates (if valid), or else `std::nullopt` if
 * the time is outside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated equatorial coordinates (if valid), or else
 *                  `std::nullopt`.
 *
 * @sa HorizontalTrack::projected_at()
 */
std::optional<Equatorial> EquatorialTrack::projected_at(const Time& time) const {
  if(is_valid_at(time))
    return Equatorial(unchecked_longitude(time), unchecked_latitude(time), _system);
  return std::nullopt;
}

/**
 * Instantiates a short-term equatorial source trajectory on sky for a given time range of
 * validity.
 *
 * @param track   pointer to a C `novas::novas_track` data structure.
 * @param range   time range of validity around the reference time of the trajectory.
 */
EquatorialTrack::EquatorialTrack(const Equinox& system, const novas::novas_track *track, const Interval& range)
: Track(track, range), _system(system) {}

/**
 * Instantiates a short-term equatorial source trajectory on sky for a given reference time,
 * time evolution, and time range of validity.
 *
 * @param system      the equatorial coordinate system in which the trajectory is defined.
 * @param ref_time    astrometric reference time for which the data is defined.
 * @param range       time range of validity around the reference time for extrapolating.
 * @param ra          short-term time evolution of the right-ascention (R.A.) coordinate.
 * @param dec         short-term time evolution of the declination coordinate.
 * @param distance    short-term time evolution of the distance coordinate.
 *
 * @sa from_novas_track(), HorizontalTrack::HorizontalTrackTrack()
 */
EquatorialTrack::EquatorialTrack(const Equinox& system, const Time& ref_time, const Interval& range,
        const Evolution& ra, const Evolution& dec, const Evolution& distance)
: Track(ref_time, range, ra, dec, distance), _system(system) {}


/**
 * Instantiates a short-term trajectory estimate for a source in equatorial coordinates, which can be
 * used to extrapolate the equatorial (RA/Dec) positions of the source around the specified
 * reference time inside an interval of validity.
 *
 * @param system    equatorial coordinate system
 * @param track     Pointer to the C tajectory data structure
 * @param range     time range of validity around the reference time. Attempts to extrapolate
 *                  outside this interval will produce invalid data.
 *
 * @sa from_novas_track()
 */
EquatorialTrack EquatorialTrack::from_novas_track(const Equinox& system, const novas_track *track, const Interval& range) {
  if(!track) {
    novas_set_errno(EINVAL, "EquatorialTrack::from_novas_track", "input track is NULL");
    return EquatorialTrack(system, &_default_track, range);
  }
  return EquatorialTrack(system, track, range);
}


} // namespace supernovas
