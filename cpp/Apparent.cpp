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

Apparent::Apparent(const Frame& f, enum novas_reference_system system)
: _frame(f), _sys(system) {
  static const char *fn = "Apparent(frame, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "frame is invalid");
  else if(system < 0 || system > NOVAS_REFERENCE_SYSTEMS)
    novas_error(0, EINVAL, fn, "system %d is invalid", _sys);
  else
    _valid = true;
}

Apparent::Apparent(const Frame& f, const Equatorial& eq, double rv, enum novas_reference_system system)
: Apparent(f, system) {
  static const char *fn = "Apparent(frame, eq, rv, system)";

  if(!f.is_valid())
      novas_error(0, EINVAL, fn, "input frame is invalid");
  else if(!eq.is_valid())
      novas_error(0, EINVAL, fn, "input equatorial coordinates are invalid");
  else if(isnan(rv))
      novas_error(0, EINVAL, fn, "input radial velocity is NAN");
  else if(system < 0 || system >= NOVAS_REFERENCE_SYSTEMS)
      novas_error(0, EINVAL, fn, "input reference system %d is invalid", system);
  else
    _valid = true;

  _pos.ra = eq.ra().hours();
  _pos.dec = eq.dec().deg();
  _pos.rv = rv / (Unit::au / Unit::day);
  _pos.dis = NOVAS_DEFAULT_DISTANCE;

  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}

Apparent::Apparent(const Frame& f, const Equatorial& eq, const Speed& rv, enum novas_reference_system system)
: Apparent(f, eq, rv.ms(), system) {}

Apparent::Apparent(const Frame& f, sky_pos p, enum novas_reference_system system)
: Apparent(f, system) {
  static const char *fn = "Apparent(frame, sky_pos, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "input frame is invalid");

  else if(isnan(p.ra))
    novas_error(0, EINVAL, fn, "input pos->ra is NAN");

  else if(isnan(p.dec))
    novas_error(0, EINVAL, fn, "input pos->dec is NAN");

  else if(!(p.dis > 0))
    novas_error(0, EINVAL, fn, "input pos->dis is invalid: %g AU", p.dis / Unit::au);

  else if(isnan(p.rv))
    novas_error(0, EINVAL, fn, "input pos->rv is NAN");

  else
    _valid = true;

  _pos = p;

  // Always recalculate r_hat to ensure it's consistent with ra/dec
  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}


const Frame& Apparent::frame() const {
  return _frame;
}

enum novas_reference_system Apparent::system() const {
  return _sys;
}

const sky_pos *Apparent::_sky_pos() const {
  return &_pos;
}

Angle Apparent::ra() const {
  return Angle(_pos.ra * Unit::hourAngle);
}

Angle Apparent::dec() const {
  return Angle(_pos.dec * Unit::deg);
}

Speed Apparent::radial_velocity() const {
  return Speed(_pos.rv * Unit::au / Unit::day);
}

Distance Apparent::distance() const {
  return Distance(_pos.dis * Unit::au);
}

Equatorial Apparent::equatorial() const {
  return Equatorial(_pos.ra * Unit::hourAngle, _pos.dec * Unit::deg, _frame.time().epoch_str(), _pos.dis * Unit::au);
}

Ecliptic Apparent::ecliptic() const {
  return equatorial().as_ecliptic();
}

Galactic Apparent::galactic() const {
  return equatorial().as_galactic();
}

Horizontal Apparent::horizontal() const {
  double az = 0.0, el = 0.0;
  if(novas_app_to_hor(_frame._novas_frame(), _sys, _pos.ra, _pos.dec, NULL, &az, &el) != 0) {
    novas_trace_invalid("Apparent::horizontal");
    return Horizontal::invalid();
  }
  return Horizontal(az * Unit::deg, el * Unit::deg, _pos.dis * Unit::au);
}

Apparent Apparent::in_system(enum novas_reference_system system) const {
  if(system == _sys)
    return *this;

  Apparent app = Apparent(_frame, system);
  novas_transform T = {};

  if(novas_make_transform(_frame._novas_frame(), _sys, system, &T)) {
    novas_trace_invalid("Apparent::in_system");
    return Apparent::invalid();
  }
  novas_transform_sky_pos(&_pos, &T, &app._pos);
  return app;
}

static const Apparent _invalid = Apparent(Frame::invalid(), Equatorial::invalid(), NAN, (enum novas_reference_system) -1);
const Apparent& Apparent::invalid() {
  return _invalid;
}

} //namespace supernovas

