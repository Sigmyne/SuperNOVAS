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


using namespace novas;

namespace supernovas {

static bool is_valid_entry(const cat_entry *e) {
  if(isnan(e->ra))
    return false;
  if(isnan(e->dec))
    return false;
  if(isnan(e->parallax))
    return false;
  if(isnan(e->promora))
    return false;
  if(isnan(e->promodec))
    return false;
  if(isnan(e->radialvelocity))
    return false;
  return true;
}

void CatalogEntry::set_epoch() {
  _epoch = (_sys.jd() - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

CatalogEntry::CatalogEntry(const std::string &name, double RA, double Dec, const std::string& system)
: _sys(system) {
  static const char *fn = "CatalogEntry(name, RA, Dec, system)";

  if(novas_init_cat_entry(&_entry, name.c_str(), RA / Unit::hourAngle, Dec / Unit::deg) != 0)
    novas_trace_invalid(fn);
  if(!_sys.is_valid())
    novas_trace_invalid(fn);
  set_epoch();
}

CatalogEntry::CatalogEntry(const std::string &name, const Angle& RA, const Angle& Dec, const CatalogSystem& system)
: CatalogEntry(name, RA.rad(), Dec.rad(), system.name()) {}

CatalogEntry::CatalogEntry(cat_entry e, const std::string& system)
: _entry(e), _sys(system) {
  if(!is_valid_entry(&e))
    novas_error(0, EINVAL, "CatalogEntry(cat_entry, system)", "invalid cat_entry data");
  if(!_sys.is_valid())
    novas_trace_invalid("CatalogEntry(cat_entry, system)");
  set_epoch();
}

bool CatalogEntry::is_valid() const {
  if(!_sys.is_valid())
    return false;
  if(isnan(_epoch))
    return false;
  return is_valid_entry(&_entry);
}

const CatalogSystem& CatalogEntry::system() const {
  return _sys;
}

const cat_entry* CatalogEntry::_cat_entry() const {
  return &_entry;
}

std::string CatalogEntry::name() const {
  return std::string(_entry.starname);
}

long CatalogEntry::number() const {
  return _entry.starnumber;
}

Angle CatalogEntry::ra() const {
  return Angle(_entry.ra * Unit::hourAngle);
}

Angle CatalogEntry::dec() const {
  return Angle(_entry.dec * Unit::deg);
}

Speed CatalogEntry::v_lsr() const {
  return Speed(novas_ssb_to_lsr_vel(_epoch, _entry.ra, _entry.dec, _entry.radialvelocity) * Unit::km / Unit::sec);
}

Speed CatalogEntry::radial_velocity() const {
  return Speed(_entry.radialvelocity * Unit::km / Unit::sec);
}

Distance CatalogEntry::distance() const {
  return Distance(Unit::kpc / _entry.parallax);
}

Angle CatalogEntry::parallax() const {
  return Angle(_entry.parallax * Unit::mas);
}

Equatorial CatalogEntry::equatorial() const {
  return Equatorial(ra(), dec(), system(), distance());
}

CatalogEntry& CatalogEntry::proper_motion(double ra, double dec) {
  static const char *fn = "CatalogEntry::proper_motion(ra, dec)";
  if(isnan(ra))
    novas_error(0, EINVAL, fn, "RA motion is NAN");
  if(isnan(dec))
    novas_error(0, EINVAL, fn, "Dec motion is NAN");
  novas_set_proper_motion(&_entry, ra / (Unit::mas / Unit::yr), dec / (Unit::mas / Unit::yr));
  return *this;
}

CatalogEntry& CatalogEntry::parallax(double radians) {
  if(isnan(radians))
    novas_error(0, EINVAL, "CatalogEntry::parallax(double)", "input value is NAN");
  novas_set_parallax(&_entry, radians / Unit::mas);
  return *this;
}

CatalogEntry& CatalogEntry::parallax(const Angle& angle) {
  if(isnan(angle.rad()))
    novas_error(0, EINVAL, "CatalogEntry::parallax(Angle&)", "input value is NAN");
  novas_set_parallax(&_entry, angle.mas());
  return *this;
}

CatalogEntry& CatalogEntry::distance(double meters) {
  if(isnan(meters))
    novas_error(0, EINVAL, "CatalogEntry::distance(double)", "input value is NAN");
  novas_set_distance(&_entry, meters / Unit::pc);
  return *this;
}

CatalogEntry& CatalogEntry::distance(const Distance& dist) {
  if(isnan(dist.m()))
    novas_error(0, EINVAL, "CatalogEntry::distance(Distance&)", "input value is NAN");
  novas_set_distance(&_entry, dist.pc());
  return *this;
}

CatalogEntry& CatalogEntry::v_lsr(double v_ms) {
  if(isnan(v_ms))
    novas_error(0, EINVAL, "CatalogEntry::v_lsr(double)", "input value is NAN");
  novas_set_lsr_vel(&_entry, _epoch, v_ms / (Unit::km / Unit::sec));
  return *this;
}

CatalogEntry& CatalogEntry::v_lsr(const Speed& v) {
  if(isnan(v.ms()))
    novas_error(0, EINVAL, "CatalogEntry::v_lsr(Speed&)", "input value is NAN");
  novas_set_lsr_vel(&_entry, _epoch, v.kms());
  return *this;
}

CatalogEntry& CatalogEntry::radial_velocity(double v_ms) {
  if(isnan(v_ms))
    novas_error(0, EINVAL, "CatalogEntry::radial_velocity(double)", "input value is NAN");
  novas_set_ssb_vel(&_entry, v_ms / (Unit::km / Unit::sec));
  return *this;
}

CatalogEntry& CatalogEntry::radial_velocity(const Speed& v) {
  if(isnan(v.ms()))
    novas_error(0, EINVAL, "CatalogEntry::radial_velocity(Speed&)", "input value is NAN");
  novas_set_ssb_vel(&_entry, v.kms());
  return *this;
}

CatalogEntry& CatalogEntry::redshift(double z) {
  static const char *fn = "CatalogEntry::redshift(double)";
  if(isnan(z))
    novas_error(0, EINVAL, fn, "input value is NAN");
  if(novas_set_redshift(&_entry, z) != 0)
    novas_trace_invalid(fn);
  return *this;
}

CatalogEntry& CatalogEntry::catalog(const std::string& name, long number) {
  if(novas_set_catalog(&_entry, name.c_str(), number) != 0)
    novas_trace_invalid("CatalogEntry::catalog(name, number)");
  return *this;
}

} // namespace supernovas

