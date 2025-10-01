/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"

namespace supernovas {

class Distance {
private:

  double _meters;

public:
  Distance(double x) : _meters(fabs(x)) {}

  double m() const {
    return _meters;
  }

  double km() const {
    return 1e-3 * _meters;
  }

  double au() const {
    return _meters / Unit::au;
  }

  double lyr() const {
    return _meters / Unit::lyr;
  }

  double pc() const {
    return _meters / Unit::pc;
  }

  Angle parallax() const {
    return Angle(Unit::arcsec / pc());
  }

  std::string str() const {
    char s[40] = {'\0'};

    if(_meters < 1e9) snprintf(s, sizeof(s), "%.1f m", _meters);
    else if(_meters < 1000.0 * Unit::au) snprintf(s, sizeof(s), "%.3f AU", au());
    else if(_meters < 1000.0 * Unit::pc) snprintf(s, sizeof(s), "%.3f pc", pc());
    else if(_meters < 1e6 * Unit::pc) snprintf(s, sizeof(s), "%.3f kpc", pc() / 1e3);
    else snprintf(s, sizeof(s), "%.3f Mpc", pc() / 1e6);

    return std::string(s);
  }

  static Distance from_parallax(double parallax) {
    return Distance(Unit::pc / (parallax / Unit::arcsec));
  }
};


} // namespace supernovas
