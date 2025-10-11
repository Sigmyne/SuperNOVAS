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

Speed::Speed(double m_per_s) : _ms(m_per_s) {
  if(isnan(m_per_s))
    novas_error(0, EINVAL, "Speed(double)", "input value is NAN");
  if(fabs(m_per_s) > Constant::c)
    novas_error(0, ERANGE, "Speed(double)", "input speed exceeds the speed of light: %g m/s", m_per_s);
  else
    _valid = true;
}

Speed::Speed(const Distance& d, const Interval& time) : _ms(d.m() / time.seconds()) {}

Speed Speed::abs() const {
  return Speed(fabs(_ms));
}

double Speed::m_per_s() const {
  return _ms;
}

double Speed::km_per_s() const {
  return 1e-3 * _ms;
}

double Speed::au_per_day() const {
  return _ms * Unit::day / Unit::au;
}

double Speed::beta() const {
  return _ms / Constant::c;
}

double Speed::Gamma() const {
  return 1.0 / sqrt(1.0 - beta() * beta());
}

double Speed::redshift() const {
  return sqrt((1.0 + beta()) / (1.0 - beta()));
}

Distance Speed::travel(double seconds) const {
  return Distance(_ms / seconds);
}

Distance Speed::travel(const Interval& time) const {
  return travel(time.seconds());
}

std::string Speed::to_string() const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "%.3f km/s", km_per_s());
  return std::string(s);
}

Velocity Speed::in_direction(const Vector& direction) const {
  return Velocity(direction._array(), _ms / direction.abs());
}

Speed Speed::from_redshift(double z) {
  return Speed(novas_z2v(z) * Unit::km / Unit::sec);
}

Speed operator+(const Speed& l, const Speed& r) {
  return Speed((l.beta() + r.beta()) / (1 + l.beta() * r.beta()) * Constant::c);
}

Speed operator-(const Speed& l, const Speed& r) {
  return Speed((l.beta() - r.beta()) / (1 + l.beta() * r.beta()) * Constant::c);
}

static const Speed _stationary = Speed(0.0);
const Speed& Speed::stationary() {
  return _stationary;
}

} // namespace supernovas
