/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.hpp"


using namespace novas;


namespace supernovas {

Speed::Speed(double ms) : _ms(ms) {}

Speed::Speed(const Distance d, const Interval& time) : _ms(d.m() / time.seconds()) {}

double Speed::ms() const {
  return _ms;
}

double Speed::kms() const {
  return 1e-3 * _ms;
}

double Speed::auday() const {
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

Distance Speed::travel(Interval& time) const {
  return travel(time.seconds());
}

std::string Speed::str() const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "%.3f km/s", kms());
  return std::string(s);
}

Velocity Speed::to_velocity(const Vector& direction) const {
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
