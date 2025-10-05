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

namespace supernovas {


Distance::Distance(double meters) : _meters(meters) {
  if(isnan(meters))
    novas::novas_error(0, EINVAL, "Distance(double)", "input value is NAN");
}

bool Distance::is_valid() const {
  return !isnan(_meters);
}

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

double Distance::kpc() const {
  return _meters / Unit::kpc;
}

double Distance::Mpc() const {
  return _meters / Unit::Mpc;
}

double Distance::Gpc() const {
  return _meters / Unit::Gpc;
}

Angle Distance::parallax() const {
  return Angle(Unit::arcsec / pc());
}

std::string Distance::str() const {
  char s[40] = {'\0'};

  if(_meters < 1e4) snprintf(s, sizeof(s), "%.1f m", _meters);
  else if(_meters < 1e9) snprintf(s, sizeof(s), "%.3f km", km());
  else if(_meters < 1000.0 * Unit::au) snprintf(s, sizeof(s), "%.3f AU", au());
  else if(_meters < 1000.0 * Unit::pc) snprintf(s, sizeof(s), "%.3f pc", pc());
  else if(_meters < 1e6 * Unit::pc) snprintf(s, sizeof(s), "%.3f kpc", kpc());
  else snprintf(s, sizeof(s), "%.3f Mpc", Mpc());

  return std::string(s);
}


Distance Distance::from_parallax(double parallax) {
  return Distance(Unit::pc / (parallax / Unit::arcsec));
}

static const Distance _at_Gpc = Distance(Unit::Gpc);
const Distance& Distance::at_Gpc() {
  return _at_Gpc;
}

} // namespace supernovas
