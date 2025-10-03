/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


using namespace supernovas;


void CatalogEntry::set_epoch() {
  _epoch = (_sys.jd() - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
}

CatalogEntry::CatalogEntry(const std::string &name, double RA, double Dec, const std::string& system)
: _sys(system) {
  novas_init_cat_entry(&_entry, name.c_str(), RA / Unit::hourAngle, Dec / Unit::deg);
  set_epoch();
}

CatalogEntry::CatalogEntry(const std::string &name, const Angle& RA, const Angle& Dec, const System& system)
: CatalogEntry(name, RA.rad(), Dec.rad(), system.name()) {}

CatalogEntry::CatalogEntry(const cat_entry *e, const std::string& system)
: _entry(*e), _sys(system) {
  set_epoch();
}

const System& CatalogEntry::system() const {
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
  novas_set_proper_motion(&_entry, ra / (Unit::mas / Unit::yr), dec / (Unit::mas / Unit::yr));
  return *this;
}

CatalogEntry& CatalogEntry::parallax(double angle) {
  novas_set_parallax(&_entry, angle / Unit::mas);
  return *this;
}

CatalogEntry& CatalogEntry::parallax(const Angle& angle) {
  novas_set_parallax(&_entry, angle.mas());
  return *this;
}

CatalogEntry& CatalogEntry::distance(double dist) {
  novas_set_distance(&_entry, dist / Unit::pc);
  return *this;
}

CatalogEntry& CatalogEntry::distance(const Distance& dist) {
  novas_set_distance(&_entry, dist.pc());
  return *this;
}

CatalogEntry& CatalogEntry::v_lsr(double v) {
  novas_set_lsr_vel(&_entry, _epoch, v / (Unit::km / Unit::sec));
  return *this;
}

CatalogEntry& CatalogEntry::v_lsr(const Speed& v) {
  novas_set_lsr_vel(&_entry, _epoch, v.kms());
  return *this;
}

CatalogEntry& CatalogEntry::radial_velocity(double v) {
  novas_set_ssb_vel(&_entry, v / (Unit::km / Unit::sec));
  return *this;
}

CatalogEntry& CatalogEntry::radial_velocity(const Speed& v) {
  novas_set_ssb_vel(&_entry, v.kms());
  return *this;
}

CatalogEntry& CatalogEntry::redshift(double z) {
  novas_set_redshift(&_entry, z);
  return *this;
}

CatalogEntry& CatalogEntry::catalog(const std::string& name, long number) {
  novas_set_catalog(&_entry, name.c_str(), number);
  return *this;
}



