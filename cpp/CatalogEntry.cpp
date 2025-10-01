/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class CatalogEntry {
private:
  double _epoch;
  cat_entry _entry;
  std::string _sys;

  void set_epoch() {
    _epoch = (novas_epoch(_sys.c_str()) - NOVAS_JD_J2000) / NOVAS_JULIAN_YEAR_DAYS;
  }

public:
  CatalogEntry(const std::string &name, double RA, double Dec, const std::string& system="ICRS")
  : _sys(system) {
    novas_init_cat_entry(&_entry, name.c_str(), RA / Unit::hourAngle, Dec / Unit::deg);
    set_epoch();
  }

  CatalogEntry(const std::string &name, const Angle& RA, const Angle& Dec, const std::string& system = "ICRS")
  : CatalogEntry(name, RA.rad(), Dec.rad(), system) {}

  CatalogEntry(const cat_entry *e, const std::string& system = "ICRS")
  : _entry(*e), _sys(system) {
    set_epoch();
  }

  std::string system() const {
    return std::string(_sys);
  }

  const cat_entry* _cat_entry() const {
    return &_entry;
  }

  std::string name() const {
    return std::string(_entry.starname);
  }

  long number() const {
    return _entry.starnumber;
  }

  Angle ra() const {
    return Angle(_entry.ra * Unit::hourAngle);
  }

  Angle dec() const {
    return Angle(_entry.dec * Unit::deg);
  }

  Speed v_lsr() const {
    return Speed(novas_ssb_to_lsr_vel(_epoch, _entry.ra, _entry.dec, _entry.radialvelocity) * Unit::km / Unit::sec);
  }

  Speed radial_velocity() const {
    return Speed(_entry.radialvelocity * Unit::km / Unit::sec);
  }

  void proper_motion(double ra, double dec) {
    novas_set_proper_motion(&_entry, ra / (Unit::mas / Unit::yr), dec / (Unit::mas / Unit::yr));
  }

  void parallax(double angle) {
    novas_set_parallax(&_entry, angle / Unit::mas);
  }

  void parallax(Angle& angle) {
    novas_set_parallax(&_entry, angle.mas());
  }

  void distance(double dist) {
    novas_set_distance(&_entry, dist / Unit::pc);
  }

  void distance(Distance dist) {
    novas_set_distance(&_entry, dist.pc());
  }

  void v_lsr(double v) {
    novas_set_lsr_vel(&_entry, _epoch, v / (Unit::km / Unit::sec));
  }

  void v_lsr(Speed& v) {
    novas_set_lsr_vel(&_entry, _epoch, v.kms());
  }

  void radial_velocity(double v) {
    novas_set_ssb_vel(&_entry, v / (Unit::km / Unit::sec));
  }

  void radial_velocity(Speed& v) {
    novas_set_ssb_vel(&_entry, v.kms());
  }

  void redshift(double z) {
    novas_set_redshift(&_entry, z);
  }

  void catalog(std::string& name, long number) {
    novas_set_catalog(&_entry, name.c_str(), number);
  }
};


} // namespace supernovas


