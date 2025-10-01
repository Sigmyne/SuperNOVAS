/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {
class Speed {
protected:
  double _ms;

public:
  Speed(double ms) : _ms(ms) {}

  Speed(const Distance d, const Interval& time) : _ms(d.m() / time.seconds()) {}

  Speed operator+(const Speed& r) const {
    return Speed((beta() + r.beta()) / (1 + beta() * r.beta()) * Constant::c);
  }

  Speed operator-(const Speed& r) const {
    return Speed((beta() - r.beta()) / (1 + beta() * r.beta()) * Constant::c);
  }

  double ms() const {
    return _ms;
  }

  double kms() const {
    return 1e-3 * _ms;
  }

  double auday() const {
    return _ms * Unit::day / Unit::au;
  }

  double beta() const {
    return _ms / Constant::c;
  }

  double redshift() const {
    return sqrt((1.0 + beta()) / (1.0 - beta()));
  }

  Distance travel(double seconds) const {
    return Distance(_ms / seconds);
  }

  Distance travel(Interval& time) const {
    return travel(time.seconds());
  }

  std::string str() const {
    char s[40] = {'\0'};
    snprintf(s, sizeof(s), "%.3g km/s", kms());
    return std::string(s);
  }

  template <typename T>
  Velocity to_velocity(const Vector<T>& direction) const {
    return Velocity(direction._array(), _ms / direction.abs());
  }

  static Speed from_redshift(double z) {
    return Speed(novas_z2v(z) * Unit::km / Unit::sec);
  }




};

} // namespace supernovas
