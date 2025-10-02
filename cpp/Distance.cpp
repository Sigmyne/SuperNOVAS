/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"

using namespace supernovas;


Distance::Distance(double x) : _meters(fabs(x)) {}

double Distance::m() const {
  return _meters;
}

double Distance::km() const {
  return 1e-3 * _meters;
}

double Distance::au() const {
  return _meters / Unit::au;
}

double Distance::lyr() const {
  return _meters / Unit::lyr;
}

double Distance::pc() const {
  return _meters / Unit::pc;
}

Angle Distance::parallax() const {
  return Angle(Unit::arcsec / pc());
}

std::string Distance::str() const {
  char s[40] = {'\0'};

  if(_meters < 1e9) snprintf(s, sizeof(s), "%.1f m", _meters);
  else if(_meters < 1000.0 * Unit::au) snprintf(s, sizeof(s), "%.3f AU", au());
  else if(_meters < 1000.0 * Unit::pc) snprintf(s, sizeof(s), "%.3f pc", pc());
  else if(_meters < 1e6 * Unit::pc) snprintf(s, sizeof(s), "%.3f kpc", pc() / 1e3);
  else snprintf(s, sizeof(s), "%.3f Mpc", pc() / 1e6);

  return std::string(s);
}


Distance Distance::from_parallax(double parallax) {
  return Distance(Unit::pc / (parallax / Unit::arcsec));
}
