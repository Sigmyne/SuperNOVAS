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


template class Track<Horizontal>;
template class Track<Equatorial>;


template<class CoordType> void Track<CoordType>::validate() {
  static const char *fn = "Track()";

  errno = 0;

  if(!_ref_time.is_valid())
    novas_set_errno(EINVAL, fn, "reference time is invalid");
  if(!_range.is_valid())
    novas_set_errno(EINVAL, fn, "validity range is invalid");
  if(!_lon.is_valid())
    novas_set_errno(EINVAL, fn, "longitude evolution is invalid");
  if(!_lat.is_valid())
    novas_set_errno(EINVAL, fn, "latitude evolution is invalid");
  if(!_r.is_valid())
    novas_set_errno(EINVAL, fn, "radial evolution is invalid");

  _valid = (errno == 0);
}

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
 * @param r         the time evolution of distance (default: static 1 Gpc).
 * @param z         the time evolution of redshift, including gravitational effects.
 *                  If it's kinetic only, then you can leave it undefined to let the
 *                  distance evolution determine it automatically.
 */
template<class CoordType> Track<CoordType>::Track(const Time& ref_time, const Interval& range, const ScalarEvolution& lon, const ScalarEvolution& lat,
        const ScalarEvolution& r, const ScalarEvolution& z)
: _ref_time(ref_time), _range(range), _lon(lon), _lat(lat), _r(r), _z(z) {

  // make sure CoordType is a sublcass of Spherical
  static_assert(std::is_base_of<Spherical, CoordType>::value, "Track: CoordType is not a subclass of Spherical");

  if(!_z.is_valid()) {
    double dt = fabs(range.seconds());
    double z0 = novas_v2z(_r.rate(Interval::zero()) / (Unit::km / Unit::s));
    double zp = novas_v2z(_r.rate(Interval(dt)) / (Unit::km / Unit::s));
    double zm = novas_v2z(_r.rate(Interval(-dt)) / (Unit::km / Unit::s));
    double z1 = 0.5 * (zp - zm) / dt;
    _z = ScalarEvolution(z0, z1);
  }

  validate();
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
template<class CoordType> Track<CoordType>::Track(const novas_track *track, const Interval& range)
: _ref_time(&track->time), _range(range),
  _lon(ScalarEvolution(track->pos.lon * Unit::deg, track->rate.lon * Unit::deg / Unit::sec, track->accel.lon * Unit::deg / (Unit::sec * Unit::sec))),
  _lat(ScalarEvolution(track->pos.lat * Unit::deg, track->rate.lat * Unit::deg / Unit::sec, track->accel.lat * Unit::deg / (Unit::sec * Unit::sec))),
  _r(ScalarEvolution(track->pos.dist * Unit::au, track->rate.dist * Unit::au / Unit::sec, track->accel.dist * Unit::au / (Unit::sec * Unit::sec))),
  _z(ScalarEvolution(track->pos.z, track->rate.z, track->accel.z)) {

  // make sure CoordType is a sublcass of Spherical
  static_assert(std::is_base_of<Spherical, CoordType>::value, "CoordType is not a subclass of Spherical");

  validate();
}


/**
 * Returns the longitudinal time evolution component of this trajectory.
 *
 * @return    the longitudinal time evolution component.
 *
 * @sa longitude_at(), latitude_evolution(), distance_evolution(), redshift_evolution()
 */
template<class CoordType> const ScalarEvolution& Track<CoordType>::longitude_evolution() const {
  return _lon;
}

/**
 * Returns the latitudinal time evolution component of this trajectory.
 *
 * @return    the latitudinal time evolution component.
 *
 * @sa latitude_at(), longitude_evolution(), distance_evolution(), redshift_evolution()
 */
template<class CoordType> const ScalarEvolution& Track<CoordType>::latitude_evolution() const {
  return _lat;
}

/**
 * Returns the time evolution of radial distance in this trajectory.
 *
 * @return    the time evolution of distance.
 *
 * @sa distance_at(), redshift_evolution(), longitude_evolution(), latitude_evolution()
 */
template<class CoordType> const ScalarEvolution& Track<CoordType>::distance_evolution() const {
  return _r;
}

/**
 * Returns the redshift evolution in this trajectory.
 *
 * @return    the time evolution of redshift.
 *
 * @sa redshift_at(), radial_velocity_at(), longitude_evolution(), latitude_evolution(), distance_evolution()
 */
template<class CoordType> const ScalarEvolution& Track<CoordType>::redshift_evolution() const {
  return _z;
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
template<class CoordType> bool Track<CoordType>::is_valid_at(const Time& time) const {
  return fabs((time - _ref_time).seconds()) <= _range.seconds();
}

/**
 * Returns the astrometic time of reference, at which the (quadratic) trajectory has been defined.
 *
 * @return  the reference time for this trajectory.
 *
 * @sa range(), is_valid()
 */
template<class CoordType> const Time& Track<CoordType>::reference_time() const {
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
template<class CoordType> const Interval& Track<CoordType>::range() const {
  return _range;
}

/**
 * Returns the momentary extrapolated longitude angle, without checking if the requested time is
 * inside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated longitude angle.
 *
 * @sa unchecked_latitude(), unchecked_distance(), unchecked_redshift()
 */
template<class CoordType> Angle Track<CoordType>::unchecked_longitude(const Time& time) const {
  return Angle(_lon.value(time - _ref_time));
}

/**
 * Returns the momentary extrapolated latitude angle, without checking if the requested time is
 * inside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated latitude angle.
 *
 * @sa unchecked_longitude(), unchecked_distance(), unchecked_redshift()
 */
template<class CoordType> Angle Track<CoordType>::unchecked_latitude(const Time& time) const {
  return Angle(_lat.value(time - _ref_time));
}

/**
 * Returns the momentary extrapolated distance, without checking if the requested time is
 * inside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated distance.
 *
 * @sa unchecked_redshift(), unchecked_longitude(), unchecked_latitude()
 */
template<class CoordType> Coordinate Track<CoordType>::unchecked_distance(const Time& time) const {
  return Coordinate(_r.value(time - _ref_time));
}

/**
 * Returns the momentary extrapolated redshift, without checking if the requested time is
 * inside the range of validity.
 *
 * @param time      astrometric time for which we want the extrapolated value.
 * @return          the momentary extrapolated redshift.
 *
 * @sa unchecked_distance(), unchecked_longitude(), unchecked_latitude()
 */
template<class CoordType> double Track<CoordType>::unchecked_redshift(const Time& time) const {
  return _z.value(time - _ref_time);
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
template<class CoordType> std::optional<Angle> Track<CoordType>::longitude_at(const Time& time) const {
  if(is_valid_at(time))
    return unchecked_longitude(time);
  novas_set_errno(ERANGE, "Track::longitude_at()", "requested time is outside the trajectory valifity range");
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
template<class CoordType> std::optional<Angle> Track<CoordType>::latitude_at(const Time& time) const {
  if(is_valid_at(time))
    return unchecked_latitude(time);
  novas_set_errno(ERANGE, "Track::latitude_at()", "requested time is outside the trajectory valifity range");
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
template<class CoordType> std::optional<double> Track<CoordType>::redshift_at(const Time& time) const {
  if(is_valid_at(time))
    return unchecked_redshift(time);
  novas_set_errno(ERANGE, "Track::redshift_at()", "requested time is outside the trajectory valifity range");
  return std::nullopt;
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
template<class CoordType> std::optional<Coordinate> Track<CoordType>::distance_at(const Time& time) const {
  if(is_valid_at(time))
    return unchecked_distance(time);
  novas_set_errno(ERANGE, "Track::distance_at()", "requested time is outside the trajectory valifity range");
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
template<class CoordType> std::optional<ScalarVelocity> Track<CoordType>::radial_velocity_at(const Time& time) const {
  if(is_valid_at(time))
    return ScalarVelocity(novas_z2v(unchecked_redshift(time)) * (Unit::km / Unit::s));
  novas_set_errno(ERANGE, "Track::radial_velocity_at()", "requested time is outside the trajectory valifity range");
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
: Track(track, range) {
  if(!is_valid())
    novas_trace_invalid("HorizontalTrack()");
}

/**
 * Instantiates a short-term horizontal source trajectory on sky for a given reference time,
 * time evolution, and time range of validity.
 *
 * @param ref_time    astrometric reference time for which the data is defined.
 * @param range       time range of validity around the reference time for extrapolating.
 * @param azimuth     short-term time evolution of the azimuth coordinate.
 * @param elevation   short-term time evolution of the elevation coordinate.
 * @param distance    (optional) short-term time evolution of distance (default: static at 1 Gpc).
 * @param z           (optional) time evolution of redshift, including gravitational effects.
 *                    If it's kinetic only, then you can leave it undefined to let the
 *                    distance evolution determine it automatically.
 *
 * @sa from_novas_track(), EquatorialTrack::EquatorialTrack()
 */
HorizontalTrack::HorizontalTrack(const Time& ref_time, const Interval& range,
        const ScalarEvolution& azimuth, const ScalarEvolution& elevation, const ScalarEvolution& distance, const ScalarEvolution& z)
: Track(ref_time, range, azimuth, elevation, distance, z) {}


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
  novas_set_errno(ERANGE, "HorizontalTrack::projected_at()", "requested time is outside the trajectory valifity range");
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
std::optional<HorizontalTrack> HorizontalTrack::from_novas_track(const novas_track *track, const Interval& range) {
  static const char *fn = "HorizontalTrack::from_novas_track()";

  if(!track) {
    novas_set_errno(EINVAL, fn, "input track is NULL");
    return std::nullopt;
  }

  HorizontalTrack t(track, range);

  if(t.is_valid())
    return t;

  novas_trace_invalid(fn);
  return std::nullopt;
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
  novas_set_errno(ERANGE, "EquatorialTrack::projected_at()", "requested time is outside the trajectory valifity range");
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
: Track(track, range), _system(system) {
  static const char *fn = "EquatorialTrack()";

  if(!is_valid())
    novas_trace_invalid(fn);

  if(!_system.is_valid()) {
    novas_set_errno(EINVAL, fn, "input equatorial system is invalid");
    _valid = false;
  }
}

/**
 * Instantiates a short-term equatorial source trajectory on sky for a given reference time,
 * time evolution, and time range of validity.
 *
 * @param system      the equatorial coordinate system in which the trajectory is defined.
 * @param ref_time    astrometric reference time for which the data is defined.
 * @param range       time range of validity around the reference time for extrapolating.
 * @param ra          short-term time evolution of the right-ascention (R.A.) coordinate.
 * @param dec         short-term time evolution of the declination coordinate.
 * @param distance    (optional) short-term time evolution of distance (default: static at 1 Gpc).
 * @param z           (optional) time evolution of redshift, including gravitational effects.
 *                    If it's kinetic only, then you can leave it undefined to let the
 *                    distance evolution determine it automatically.
 *
 * @sa from_novas_track(), HorizontalTrack::HorizontalTrackTrack()
 */
EquatorialTrack::EquatorialTrack(const Equinox& system, const Time& ref_time, const Interval& range,
        const ScalarEvolution& ra, const ScalarEvolution& dec, const ScalarEvolution& distance, const ScalarEvolution& z)
: Track(ref_time, range, ra, dec, distance, z), _system(system) {}


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
 * @sa HorizontalTrack::from_novas_track()
 */
std::optional<EquatorialTrack> EquatorialTrack::from_novas_track(const Equinox& system, const novas_track *track, const Interval& range) {
  static const char *fn = "EquatorialTrack::from_novas_track()";

  if(!track) {
    novas_set_errno(EINVAL, fn, "input track is NULL");
    return std::nullopt;
  }

  EquatorialTrack t(system, track, range);

  if(t.is_valid())
    return t;

  novas_trace_invalid(fn);
  return std::nullopt;
}


} // namespace supernovas
