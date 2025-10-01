/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class Ecliptic : public Spherical {
private:
  System _sys;

public:
  Ecliptic(double longitude, double latitude, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE)
  : Spherical(longitude, latitude, distance), _sys(system) {}

  Ecliptic(const Angle& longitude, const Angle& latitude, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE)
  : Spherical(longitude, latitude, distance), _sys(system) {}

  Ecliptic(const Angle& ra, const Angle& dec, const System &system, const Distance& distance)
  : Spherical(ra, dec, distance), _sys(system) {}

  Ecliptic(const Position& pos, const std::string& system = "ICRS")
  : Spherical(pos.as_spherical()), _sys(system) {}

  Ecliptic(const Position& pos, const System& system)
  : Spherical(pos.as_spherical()), _sys(system) {}

  const System& system() const {
    return _sys;
  }

  Equatorial as_equatorial() const {
    double ra = 0.0, dec = 0.0;
    ecl2equ(_sys.jd(), NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, _lon.deg(), _lat.deg(), &ra, &dec);
    return Equatorial(ra * Unit::hourAngle, dec * Unit::deg, _sys.name(), _distance.m());
  }

  Galactic as_galactic() const {
    return as_equatorial().as_galactic();
  }

  const std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const {
    return "ECL  " + Spherical::str(separator, decimals) + "  " + _sys.str();
  }
};

} // namespace supernovas


