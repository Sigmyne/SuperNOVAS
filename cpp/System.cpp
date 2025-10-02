/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include <string>

#include "supernovas.h"


using namespace supernovas;


System::System(const std::string& name) : _name(name), _jd(novas_epoch(name.c_str())) {}

double System::jd() const {
  return _jd;
}

double System::epoch() const {
  return 2000.0 + (_jd - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

const std::string& System::name() const {
  return _name;
}

std::string System::str() const {
  return _name;
}

