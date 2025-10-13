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

Motion::Motion(double pos, double vel, double accel)
: _value(pos), _rate(vel), _accel(accel) {
  // TODO validation

  _valid = true;
}

double Motion::value(const Interval& offset) const {
  return _value + offset.seconds() * (_rate + offset.seconds() * _accel);
}

double Motion::rate(const Interval& offset) const {
  return _rate + _accel * offset.seconds();
}

double Motion::acceleration() const {
  return _accel;
}

static const Motion _zero = Motion(0.0, 0.0);
const Motion& Motion::zero() {
  return _zero;
}


static const novas_track _default_track = {};


Track::Track(const Time& ref_time, const Interval& range, const Motion& lon, const Motion& lat, const Motion& r)
: _ref_time(ref_time), _range(range), _lon(lon), _lat(lat), _r(r) {

  // TODO validation
  _valid = true;
}

Track::Track(const novas_track *track, const Interval& range)
: _ref_time(&track->time), _range(range),
  _lon(Motion(track->pos.lon * Unit::deg, track->rate.lon * Unit::deg / Unit::sec, track->accel.lon * Unit::deg / (Unit::sec * Unit::sec))),
  _lat(Motion(track->pos.lat * Unit::deg, track->rate.lat * Unit::deg / Unit::sec, track->accel.lat * Unit::deg / (Unit::sec * Unit::sec))),
  _r(Motion(track->pos.dist * Unit::au, track->rate.dist * Unit::au / Unit::sec, track->accel.dist * Unit::au / (Unit::sec * Unit::sec))) {

  // TODO validation
  _valid = (track != &_default_track);
}

bool Track::is_valid(const Time& time) const {
  return (time - _ref_time).seconds() <= _range.seconds();
}

const Interval& Track::range() const {
  return _range;
}

Angle Track::longitude(const Time& time) const {
  return Angle(_lon.value(time - _ref_time));
}

Angle Track::latitude(const Time& time) const {
  return Angle(_lat.value(time - _ref_time));
}

double Track::redshift(const Time& time) const {
  return novas_v2z(radial_velocity(time).km_per_s());
}

Distance Track::distance(const Time& time) const {
  return Distance(_r.value(time - _ref_time));
}

Speed Track::radial_velocity(const Time& time) const {
  return Speed(_r.rate(time - _ref_time));
}



Horizontal HorizontalTrack::projected(const Time& time) const {
  return Horizontal(longitude(time), latitude(time));
}

HorizontalTrack HorizontalTrack::from_novas_track(const novas_track *track, const Interval& range) {
  if(!track) {
    novas_error(0, EINVAL, "HorizontalTrack::from_novas_track", "input track is NULL");
    return HorizontalTrack(&_default_track, range);
  }
  return HorizontalTrack(track, range);

}


Equatorial EquatorialTrack::projected(const Time& time) const {
  return Equatorial(longitude(time), latitude(time), _system);
}

EquatorialTrack EquatorialTrack::from_novas_track(const EquatorialSystem& system, const novas_track *track, const Interval& range) {
  if(!track) {
    novas_error(0, EINVAL, "EquatorialTrack::from_novas_track", "input track is NULL");
    return EquatorialTrack(system, &_default_track, range);
  }
  return EquatorialTrack(system, track, range);

}


} // namespace supernovas
