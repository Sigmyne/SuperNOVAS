/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include <string.h>

#include "supernovas.h"


using namespace novas;


namespace supernovas {

static double _epoch_for(double jd) {
  return 2000.0 + (jd - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

static std::string _name_for(double jd) {
  char s[20] = {'\0'};
  snprintf(s, sizeof(s), "J%.3f", _epoch_for(jd));

  // Remove trailing zeroes and decimal point.
  for(int i = strlen(s); --i >= 0; ) {
    if(s[i] == '.') {
      s[i] = '\0';
      break;
    }
    if(s[i] != '0') break;
    s[i] = '\0';
  }

  return std::string(s);
}

System::System(double jd_tt) : _name(_name_for(jd_tt)), _jd(jd_tt) {}

System::System(const std::string& name) : _name(name), _jd(novas_epoch(name.c_str())) {}


double System::jd() const {
  return _jd;
}

double System::epoch() const {
  return _epoch_for(_jd);
}

const std::string& System::name() const {
  return _name;
}

std::string System::str() const {
  return _name;
}

System true_of_date(double jd_tt) {
  return System(jd_tt);
}

static const System _icrs = System("ICRS");
const System& icrs() {
  return _icrs;
}

static const System _j2000 = System("J2000");
const System& j2000() {
  return _j2000;
}

static const System _hip = System("HIP");
const System& hip() {
  return _hip;
}

static const System _b1950 = System("B1950");
const System& b1950() {
  return _b1950;
}

static const System _b1900 = System("B1900");
const System& b1900() {
  return _b1900;
}

} // namespace supernovas
