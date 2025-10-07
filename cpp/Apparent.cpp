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

static bool is_valid_sky_pos(const char *fn, const sky_pos *p) {
  if(isnan(p->ra))
    return novas_error(0, EINVAL, fn, "input pos->ra is NAN");

  else if(isnan(p->dec))
    return novas_error(0, EINVAL, fn, "input pos->dec is NAN");

  else if(!(p->dis > 0))
    return novas_error(0, EINVAL, fn, "input pos->dis is invalid: %g AU", p->dis / Unit::au);

  else if(isnan(p->rv))
    return novas_error(0, EINVAL, fn, "input pos->rv is NAN");

  else if(p->rv * Unit::au / Unit::day > Constant::c)
    return novas_error(0, EINVAL, fn, "input radial velocity exceeds the speed of light: %g m/s", p->rv * Unit::au / Unit::day);

  return true;
}


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

Apparent::Apparent(sky_pos p, const Frame& f, enum novas_reference_system system)
: Apparent(f, system) {
  static const char *fn = "Apparent(frame, sky_pos, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "input frame is invalid");
  else
    _valid = true;

  _pos = p;

  // Always recalculate r_hat to ensure it's consistent with ra/dec
  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}



Apparent::Apparent(const Equatorial& eq, const Frame& f, double rv_ms, enum novas_reference_system system)
: Apparent(f, system) {
  static const char *fn = "Apparent(frame, eq, rv, system)";

  if(!f.is_valid())
      novas_error(0, EINVAL, fn, "input frame is invalid");
  else if(!eq.is_valid())
      novas_error(0, EINVAL, fn, "input equatorial coordinates are invalid");
  else if(isnan(rv_ms))
      novas_error(0, EINVAL, fn, "input radial velocity is NAN");
  else if(fabs(rv_ms) > Constant::c)
      novas_error(0, EINVAL, fn, "input radial velocity exceeds the speed of light: %g m/s", rv_ms);
  else if(system < 0 || system >= NOVAS_REFERENCE_SYSTEMS)
      novas_error(0, EINVAL, fn, "input reference system %d is invalid", system);
  else
    _valid = true;

  _pos.ra = eq.ra().hours();
  _pos.dec = eq.dec().deg();
  _pos.rv = rv_ms / (Unit::au / Unit::day);
  _pos.dis = NOVAS_DEFAULT_DISTANCE;

  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}

Apparent::Apparent(const Equatorial& eq, const Frame& f, const Speed& rv, enum novas_reference_system system)
: Apparent(eq, f, rv.m_per_s(), system) {}


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
  return Equatorial(_pos.ra * Unit::hourAngle, _pos.dec * Unit::deg, CatalogSystem::at_julian_date(_frame.time().jd()), _pos.dis * Unit::au);
}

Ecliptic Apparent::ecliptic() const {
  return equatorial().as_ecliptic();
}

Galactic Apparent::galactic() const {
  return equatorial().as_galactic();
}

std::optional<Horizontal> Apparent::horizontal() const {
  static const char *fn = "Apparent::horizontal";

  if(!_frame.observer().is_geodetic()) {
    novas_error(0, EINVAL, fn, "cannot convert for non-geodetic observer frame");
    return std::nullopt;
  }

  double az = 0.0, el = 0.0;
  if(novas_app_to_hor(_frame._novas_frame(), _sys, _pos.ra, _pos.dec, NULL, &az, &el) != 0) {
    novas_trace_invalid(fn);
    return std::nullopt;
  }

  return Horizontal(az * Unit::deg, el * Unit::deg, _pos.dis * Unit::au);
}

Apparent Apparent::in_system(enum novas_reference_system system) const {
  if(system == _sys)
    return *this;

  if(system == NOVAS_ITRS)
    in_itrs().value_or(Apparent::invalid());

  Apparent app = Apparent(_frame, system);
  novas_transform T = {};

  if(novas_make_transform(_frame._novas_frame(), _sys, system, &T) != 0) {
    novas_trace_invalid("Apparent::in_system");
    return Apparent::invalid();
  }
  novas_transform_sky_pos(&_pos, &T, &app._pos);
  return app;
}

std::optional<Apparent> Apparent::in_itrs(const EOP& eop) const {
  if(eop.is_valid()) {
    Time t = Time(_frame.time().jd(), eop);
    Apparent app = Apparent(*this);
    app._frame = Frame(_frame.observer(), t, _frame.accuracy());
    return app.in_system(NOVAS_ITRS);
  }

  if(_frame.observer().is_geodetic())
    return in_system(NOVAS_ITRS);

  novas_error(0, EINVAL, "Apparent::in_itrs()", "Needs valid EOP for non geodetic observer frame");
  return std::nullopt;
}

std::optional<Apparent> Apparent::from_sky_pos(sky_pos pos, const Frame& frame, enum novas_reference_system system) {
  if(!is_valid_sky_pos("Apparent::from_sky_pos", &pos))
    return std::nullopt;

  return Apparent(pos, frame, system);
}

static const Apparent _invalid = Apparent(Equatorial::invalid(), Frame::invalid(), NAN, (enum novas_reference_system) -1);
const Apparent& Apparent::invalid() {
  return _invalid;
}

} //namespace supernovas

