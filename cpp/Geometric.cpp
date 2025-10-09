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

Geometric::Geometric(const Position& p, const Velocity& v, const Frame& frame, enum novas_reference_system system)
          : _frame(frame), _pos(p), _vel(v), _sys(system)  {
  static const char *fn = "Geometric()";

  if(! frame.is_valid())
    novas_error(0, EINVAL, fn, "input frame is invalid");
  else if(system < 0 || system >= NOVAS_REFERENCE_SYSTEMS)
    novas_error(0, EINVAL, fn, "input system is invalid: %d", system);
  else if(!p.is_valid())
    novas_error(0, EINVAL, fn, "input position contains NAN coponent(s)");
  else if(!v.is_valid())
    novas_error(0, EINVAL, fn, "input velocity contains NAN coponent(s)");
  else
    _valid = true;
}

const Frame& Geometric::frame() const {
  return _frame;
}

enum novas_reference_system Geometric::system() const {
  return _sys;
}

const Position& Geometric::position() const {
  return _pos;
}

const Velocity& Geometric::velocity() const {
  return _vel;
}

Equatorial Geometric::equatorial() const {
  return Equatorial(_pos);
}

Ecliptic Geometric::ecliptic() const {
  return equatorial().as_ecliptic();
}

Galactic Geometric::galactic() const {
  return equatorial().as_galactic();
}

Geometric Geometric::in_system(enum novas_reference_system system) const {
  if(system == _sys)
    return *this;

  if(system == NOVAS_ITRS)
    return in_itrs().value_or(Geometric::invalid());

  novas_transform T = {};
  double p[3] = {0.0}, v[3] = {0.0};

  if(novas_make_transform(_frame._novas_frame(), _sys, system, &T) != 0) {
    novas_trace_invalid("Geometric::in_system");
    return Geometric::invalid();
  }
  novas_transform_vector(_pos._array(), &T, p);
  novas_transform_vector(_vel._array(), &T, v);

  return Geometric(p, v, _frame, _sys);
}

std::optional<Geometric> Geometric::in_itrs(const EOP& eop) const {
  if(_sys == NOVAS_ITRS)
    return Geometric(*this);

  if(eop.is_valid()) {
    Time t = Time(_frame.time().jd(), eop);
    Geometric geom = Geometric(*this);
    geom._frame = Frame(_frame.observer(), t, _frame.accuracy());
    return geom.in_system(NOVAS_ITRS);
  }

  if(_frame.observer().is_geodetic())
    return in_system(NOVAS_ITRS);

  novas_error(0, EINVAL, "Geometric::in_itrs()", "Needs valid EOP for non geodetic observer frame");
  return std::nullopt;
}

static const Geometric _invalid = Geometric(Position::invalid(), Velocity::invalid(), Frame::invalid(), (enum novas_reference_system) -1);
const Geometric& Geometric::invalid() {
  return _invalid;
}

} // namespace supernovas



