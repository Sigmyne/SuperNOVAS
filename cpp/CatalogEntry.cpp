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


CatalogEntry::CatalogEntry(const std::string &name, double RA, double Dec, const std::string& system)
: _sys(system) {
  static const char *fn = "CatalogEntry()";

  if(novas_init_cat_entry(&_entry, name.c_str(), RA / Unit::hourAngle, Dec / Unit::deg) != 0)
    novas_trace_invalid(fn);
  else if(isnan(RA))
    novas_error(0, EINVAL, fn, "input RA is NAN");
  else if(isnan(Dec))
    novas_error(0, EINVAL, fn, "input Dec is NAN");
  else if(fabs(Dec) > Constant::halfPi)
    novas_error(0, EINVAL, fn, "input Dec is outside of [-pi:pi] range: %g", Dec);
  else if(_sys.is_valid())
    novas_error(0, EINVAL, fn, "input catalog system is invalid: %s", system);
  else
    _valid = true;
}

CatalogEntry::CatalogEntry(const std::string &name, const Angle& RA, const Angle& Dec, const CatalogSystem& system)
: CatalogEntry(name, RA.rad(), Dec.rad(), system.name()) {}

CatalogEntry::CatalogEntry(cat_entry e, const std::string& system)
: _entry(e), _sys(system) {
  static const char *fn = "CatalogEntry()";

  if(isnan(e.ra))
    novas_error(0, EINVAL, fn, "input cat_entry.ra is NAN");
  if(isnan(e.dec))
    novas_error(0, EINVAL, fn, "input cat_entry.dec is NAN");
  else if(fabs(e.dec) > 90.0)
    novas_error(0, EINVAL, fn, "input cat_entry.dec is outside of [-90:90] range: %g", e.dec);
  if(isnan(e.parallax))
    novas_error(0, EINVAL, fn, "input cat_entry.parallax is NAN");
  if(e.parallax < 0.0)
    novas_error(0, EINVAL, fn, "input cat_entry.parallax is negative: %g mas", e.parallax);
  if(isnan(e.promora))
    novas_error(0, EINVAL, fn, "input cat_entry.promora is NAN");
  if(isnan(e.promodec))
    novas_error(0, EINVAL, fn, "input cat_entry.promodec is NAN");
  if(isnan(e.radialvelocity))
    novas_error(0, EINVAL, fn, "input cat_entry.radialvelocity is NAN");
  if(!_sys.is_valid())
    novas_error(0, EINVAL, fn, "input catalog system is invalid: %s", system);
  else
    _valid = true;

  set_epoch();
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
  return Speed(novas_ssb_to_lsr_vel(_sys.epoch(), _entry.ra, _entry.dec, _entry.radialvelocity) * Unit::km / Unit::sec);
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
  static const char *fn = "CatalogEntry::proper_motion()";

  if(isnan(ra)) {
    novas_error(0, EINVAL, fn, "RA motion is NAN");
    _valid = false;
  }
  else if(isnan(dec)) {
    novas_error(0, EINVAL, fn, "Dec motion is NAN");
    _valid = false;
  }

  novas_set_proper_motion(&_entry, ra / (Unit::mas / Unit::yr), dec / (Unit::mas / Unit::yr));
  return *this;
}

CatalogEntry& CatalogEntry::parallax(double radians) {
  static const char *fn = "CatalogEntry::parallax()";

  if(isnan(radians)) {
    novas_error(0, EINVAL, fn, "input parallax is NAN");
    _valid = false;
  }
  else if(radians < 0.0) {
    novas_error(0, EINVAL, fn, "input parallax is negative: %g mas", (radians / Unit::mas));
    _valid = false;
  }

  novas_set_parallax(&_entry, radians / Unit::mas);
  return *this;
}

CatalogEntry& CatalogEntry::parallax(const Angle& angle) {
  return parallax(angle.rad());
}

CatalogEntry& CatalogEntry::distance(double meters) {
  static const char *fn = "CatalogEntry::distance()";

  if(isnan(meters)) {
    novas_error(0, EINVAL, fn, "input distance is NAN");
    _valid = false;
  }
  else if(meters < 0) {
    novas_error(0, EINVAL, fn, "input distance is negative: %g pc", (meters / Unit::pc));
    _valid = false;
  }

  novas_set_distance(&_entry, meters / Unit::pc);
  return *this;
}

CatalogEntry& CatalogEntry::distance(const Distance& dist) {
  return distance(dist.m());
}

CatalogEntry& CatalogEntry::v_lsr(double v_ms) {
  static const char *fn = "CatalogEntry::v_lsr()";

  if(isnan(v_ms)) {
    novas_error(0, EINVAL, fn, "input LSR velocity is NAN");
    _valid = false;
  }
  else if(fabs(v_ms) > Constant::c) {
    novas_error(0, EINVAL, fn, "input LSR velocity exceeds the speed of light: %g m/s", v_ms);
    _valid = false;
  }

  novas_set_lsr_vel(&_entry, _sys.epoch(), v_ms / (Unit::km / Unit::sec));
  return *this;
}

CatalogEntry& CatalogEntry::v_lsr(const Speed& v) {
  return v_lsr(v.m_per_s());
}

CatalogEntry& CatalogEntry::radial_velocity(double v_ms) {
  static const char *fn = "CatalogEntry::radial_velocity()";

  if(isnan(v_ms)) {
    novas_error(0, EINVAL, fn, "input value is NAN");
    _valid = false;
  }
  else if(fabs(v_ms) > Constant::c) {
    novas_error(0, EINVAL, fn, "radial velocity exceeds the speed of light: %g m/s", v_ms);
    _valid = false;
  }

  novas_set_ssb_vel(&_entry, v_ms / (Unit::km / Unit::sec));
  return *this;
}

CatalogEntry& CatalogEntry::radial_velocity(const Speed& v) {
  return radial_velocity(v.m_per_s());
}

CatalogEntry& CatalogEntry::redshift(double z) {
  static const char *fn = "CatalogEntry::redshift";

  if(novas_set_redshift(&_entry, z) != 0) {
    novas_trace_invalid(fn);
    _valid = false;
  }
  else if(isnan(z)) {
    novas_error(0, EINVAL, fn, "input redshift is NAN");
    _valid = false;
  }

  return *this;
}

CatalogEntry& CatalogEntry::catalog(const std::string& name, long number) {
  if(novas_set_catalog(&_entry, name.c_str(), number) != 0)
    novas_trace_invalid("CatalogEntry::catalog(name, number)");
  return *this;
}

} // namespace supernovas

