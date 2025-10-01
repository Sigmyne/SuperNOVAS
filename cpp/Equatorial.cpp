/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"


namespace supernovas {

class Equatorial : public Spherical {
private:
  System _sys;

public:
  Equatorial(double ra, double dec, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE)
  : Spherical(ra, dec, distance), _sys(system) {}

  Equatorial(const Angle& ra, const Angle& dec, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE)
  : Spherical(ra, dec, distance), _sys(system) {}

  Equatorial(const Angle& ra, const Angle& dec, const System &system, const Distance& distance)
  : Spherical(ra, dec, distance), _sys(system) {}

  Equatorial(const Position& pos, const std::string& system = "ICRS")
  : Spherical(pos.as_spherical()), _sys(system) {}

  Equatorial(const Position& pos, const System& system)
  : Spherical(pos.as_spherical()), _sys(system) {}

  const System& system() const {
    return _sys;
  }

  TimeAngle ra() const {
    return TimeAngle(longitude().rad());
  }

  const Angle& dec() const {
    return latitude();
  }

  Ecliptic as_ecliptic() const {
    double longitude, latitude;
    equ2ecl(_sys.jd(), NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, ra().hours(), dec().deg(), &longitude, &latitude);
    return Ecliptic(longitude, latitude, _sys.name(), _distance.m());
  }

  Galactic as_galactic() const {
    double longitude, latitude;
    equ2gal(ra().hours(), dec().deg(), &longitude, &latitude);
    return Galactic(longitude, latitude, _distance.m());
  }

  const std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const {
    return "EQU  " + ra().str(separator, (decimals > 1 ? decimals - 1 : decimals)) + "  " + _lat.str(separator, decimals) + "  " + _sys.str();
  }

};

} /// namespace supernovas

