/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include <string>

#include "supernovas.h"


namespace supernovas {

class System {
protected:
  std::string _name;
  double _jd;

public:
  System(const std::string& name) : _name(name), _jd(novas_epoch(name.c_str())) {}

  double jd() const {
    return _jd;
  }

  double epoch() const {
    return 2000.0 + (_jd - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
  }

  const std::string& name() const {
    return _name;
  }

  std::string str() const {
    return _name;
  }
};

} // namespace supernovas
