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

static bool is_valid_rv(const char *fn, double rv_ms) {
  if(isnan(rv_ms))
    return novas_error(0, EINVAL, fn, "input radial velocity is NAN");
  else if(fabs(rv_ms) > Constant::c)
    return novas_error(0, EINVAL, fn, "input radial velocity exceeds the speed of light: %g m/s", rv_ms);
  return true;
}

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


Apparent::Apparent(const EquatorialSystem& system, const Frame& f)
: _sys(system), _frame(f), _pos({}) {
  static const char *fn = "Apparent(frame, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "frame is invalid");
  else if(!system.is_valid())
    novas_error(0, EINVAL, fn, "equatorial system is invalid");
  else
    _valid = true;
}

Apparent::Apparent(const EquatorialSystem& system, const Frame& f, sky_pos p)
: Apparent(system, f) {
  static const char *fn = "Apparent(frame, sky_pos, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "input frame is invalid");
  else if(!is_valid_sky_pos(fn, &p)) {
    novas_error(0, EINVAL, fn, "input sky_pos is invalid");
  }
  else
    _valid = true;

  _pos = p;

  // Always recalculate r_hat to ensure it's consistent with ra/dec
  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}

Apparent::Apparent(const EquatorialSystem& system, const Frame& frame, double ra_rad, double dec_rad, double rv_ms)
: Apparent(system, frame) {
  static const char *fn = "Apparent(frame, eq, rv, system)";

  if(isnan(ra_rad))
    novas_error(0, EINVAL, fn, "input RA is NAN");
  else if(isnan(dec_rad))
    novas_error(0, EINVAL, fn, "input RA is NAN");

  _valid = is_valid_rv(fn, rv_ms);

  _pos.ra = ra_rad / Unit::hourAngle;
  _pos.dec = dec_rad / Unit::deg;
  _pos.rv = rv_ms / (Unit::km / Unit::sec);
  _pos.dis = NOVAS_DEFAULT_DISTANCE;

  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}

Apparent Apparent::cirs(double ra_rad, double dec_rad, const Frame& frame, double rv_ms) {
  return Apparent(EquatorialSystem::cirs(frame.time().jd()), frame, ra_rad, dec_rad, rv_ms);
}

Apparent Apparent::cirs(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv) {
  return cirs(ra.rad(), dec.rad(), frame, rv.m_per_s());
}

Apparent Apparent::tod(double ra_rad, double dec_rad, const Frame& frame, double rv_ms) {
  return Apparent(EquatorialSystem::tod(frame.time().jd()), frame, ra_rad, dec_rad, rv_ms);
}

Apparent Apparent::tod(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv) {
  return tod(ra.rad(), dec.rad(), frame, rv.m_per_s());
}

const Frame& Apparent::frame() const {
  return _frame;
}

const EquatorialSystem& Apparent::system() const {
  return _sys;
}

const sky_pos *Apparent::_sky_pos() const {
  return &_pos;
}

Position Apparent::xyz() const {
  return Position(_pos.r_hat, _pos.dis * Unit::au);
}

Speed Apparent::radial_velocity() const {
  return Speed(_pos.rv * Unit::km / Unit::sec);
}

double Apparent::redshift() const {
  return novas_v2z(_pos.rv);
}

Distance Apparent::distance() const {
  return Distance(_pos.dis * Unit::au);
}

Equatorial Apparent::equatorial() const {
  return Equatorial(_pos.ra * Unit::hourAngle, _pos.dec * Unit::deg, _sys, _pos.dis * Unit::au);
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

  double ra = 0.0, dec = 0.0, az = 0.0, el = 0.0;

  // pos.ra / pos.dec may be NAN for ITRS / TIRS...
  vector2radec(_pos.r_hat, &ra, &dec);

  if(novas_app_to_hor(_frame._novas_frame(), _sys.reference_system(), ra, dec, NULL, &az, &el) != 0) {
    novas_trace_invalid(fn);
    return std::nullopt;
  }

  return Horizontal(az * Unit::deg, el * Unit::deg, _pos.dis * Unit::au);
}

Apparent Apparent::from_tod_sky_pos(sky_pos pos, const Frame& frame) {
  return Apparent(EquatorialSystem::tod(frame.time().jd()), frame, pos);
}

Apparent Apparent::from_cirs_sky_pos(sky_pos pos, const Frame& frame) {
  return Apparent(EquatorialSystem::cirs(frame.time().jd()), frame, pos);
}

static const Apparent _invalid = Apparent::tod(NAN, NAN, Frame::invalid(), NAN);
const Apparent& Apparent::invalid() {
  return _invalid;
}

} //namespace supernovas

