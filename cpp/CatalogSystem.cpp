/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include <string.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

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

CatalogSystem::CatalogSystem(const std::string& name, double jd_tt) : _name(name), _jd(jd_tt) {}

CatalogSystem::CatalogSystem(double jd_tt) : CatalogSystem(_name_for(jd_tt), jd_tt) {
  if(isnan(jd_tt))
    novas_trace_invalid("CatalogSystem(double)");
}

CatalogSystem::CatalogSystem(const std::string& name) : CatalogSystem(name, novas_epoch(name.c_str())) {
  if(isnan(_jd))
    novas_trace_invalid("CatalogSystem(string&)");
}

bool CatalogSystem::is_valid() const {
  return !isnan(_jd);
}

double CatalogSystem::jd() const {
  return _jd;
}

double CatalogSystem::epoch() const {
  return _epoch_for(_jd);
}

const std::string& CatalogSystem::name() const {
  return _name;
}

std::string CatalogSystem::str() const {
  return _name;
}

CatalogSystem CatalogSystem::at_julian_date(double jd_tt) {
  return CatalogSystem("J", jd_tt);
}

CatalogSystem CatalogSystem::at_besselian_epoch(double year) {
  return CatalogSystem("B", NOVAS_JD_B1950 + (year - 1950.0) * NOVAS_TROPICAL_YEAR_DAYS);
}

static const CatalogSystem _icrs = CatalogSystem("ICRS");
const CatalogSystem& CatalogSystem::icrs() {
  return _icrs;
}

static const CatalogSystem _j2000 = CatalogSystem("J2000");
const CatalogSystem& CatalogSystem::j2000() {
  return _j2000;
}

static const CatalogSystem _hip = CatalogSystem("HIP");
const CatalogSystem& CatalogSystem::hip() {
  return _hip;
}

static const CatalogSystem _b1950 = CatalogSystem("B1950");
const CatalogSystem& CatalogSystem::b1950() {
  return _b1950;
}

static const CatalogSystem _b1900 = CatalogSystem("B1900");
const CatalogSystem& CatalogSystem::b1900() {
  return _b1900;
}

} // namespace supernovas
