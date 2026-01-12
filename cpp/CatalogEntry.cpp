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

/**
 * Instantiates a new catalog entry with a given name and catalog coordinates. After
 * instantiation, one may populate additional data using the builder functions of this class, such
 * as:
 *
 *  - radial or LSR velocity, or redshift
 *  - proper motion (if any)
 *  - parallax or distance, if known (by default 1 Gpc is assumed)
 *  - catalog name, and catalog ID number
 *
 *  For example:
 *
 *  ```c++
 *    CatalogEntry star =
 *       CatalogEntry("Antares", Equatorial("16h26m20.1918s", "-26d19m23.138s", Equinox::b1950()))
 *       .catalog("HIP", 80763)
 *       .proper_motion( -12.11 * Unit::mas / Unit::year, -23.30 * Unit::mas / Unit::year)
 *       .parallax(5.89 * Unit::arcsec)
 *       .radial_velocity(-3.4 * Unit::km / Unit::s);
 *  ```
 *
 * @param name      source name. It is treated case insensitively, unless the user calls
 *                  `novas_case_sensitive(true)` prior to instantiation. The user may chose
 *                  any name that is appropriate for their application.
 * @param coords    Catalog coordinates (equatorial)
 *
 * @sa proper_motion(), parallax(), distance(), radial_velocity(), v_lsr(), redshift(), catalog()
 */
CatalogEntry::CatalogEntry(const std::string &name, const Equatorial& coords) : _sys(coords.system()) {
  static const char *fn = "CatalogEntry()";

  if(novas_init_cat_entry(&_entry, name.c_str(), coords.ra().hours(), coords.dec().deg()) != 0)
    novas_trace_invalid(fn);
  else if(!coords.is_valid())
    novas_set_errno(EINVAL, fn, "input equatorial coordinates are invalid");
  else
    _valid = true;
}

/**
 * Instantiates a new catalog entry with a given name and ecliptic catalog coordinates. After
 * instantiation, one may populate additional data using the builder functions of this class, such as:
 *
 *  - radial or LSR velocity, or redshift
 *  - proper motion (if any)
 *  - parallax or distance, if known (by default 1 Gpc is assumed)
 *  - catalog name, and catalog ID number
 *
 * See CatalogEntry(strd::string&, Equatorial&) for more information on using builder patterns.
 *
 * @param name      Source name. It is treated case insensitively, unless the user calls
 *                  `novas_case_sensitive(true)` prior to instantiation. The user may chose
 *                  any name that is appropriate for their application.
 * @param coords    Catalog coordinates (ecliptic)
 *
 * @sa proper_motion(), parallax(), distance(), radial_velocity(), v_lsr(), redshift(), catalog()
 */
CatalogEntry::CatalogEntry(const std::string &name, const Ecliptic& coords)
: CatalogEntry(name, coords.to_equatorial()) {}

/**
 * Instantiates a new catalog entry with a given name and galactic catalog coordinates. After
 * instantiation, one may populate additional data using the builder functions of this class, such as:
 *
 *  - radial or LSR velocity, or redshift
 *  - proper motion (if any)
 *  - parallax or distance, if known (by default 1 Gpc is assumed)
 *  - catalog name, and catalog ID number
 *
 * See CatalogEntry(strd::string&, Equatorial&) for more information on using builder patterns.
 *
 * @param name      Source name. It is treated case insensitively, unless the user calls
 *                  `novas_case_sensitive(true)` prior to instantiation. The user may chose
 *                  any name that is appropriate for their application.
 * @param coords    Catalog coordinates (galactic)
 *
 * @sa proper_motion(), parallax(), distance(), radial_velocity(), v_lsr(), redshift(), catalog()
 */
CatalogEntry::CatalogEntry(const std::string &name, const Galactic& coords)
: CatalogEntry(name, coords.to_equatorial()) {}

/**
 * Instantiates a new catalog entry with a NOVAS C `cat_entry` data structure and the equatorial
 * coordinate system in which it is defined.
 *
 *  - radial or LSR velocity, or redshift
 *  - proper motion (if any)
 *  - parallax or distance, if known (by default 1 Gpc is assumed)
 *  - catalog name, and catalog ID number
 *
 * See CatalogEntry(strd::string&, Equatorial&) for more information on using builder patterns.
 *
 * @param e         NOVAS C `cat_entry` data structure (it is not referenced)
 * @param system    the equatorial coordinate system in which the `cat_entry` data was defined.
 */
CatalogEntry::CatalogEntry(cat_entry e, const Equinox& system)
: _entry(e), _sys(system) {
  static const char *fn = "CatalogEntry()";

  if(isnan(e.ra))
    novas_set_errno(EINVAL, fn, "input cat_entry.ra is NAN");
  if(isnan(e.dec))
    novas_set_errno(EINVAL, fn, "input cat_entry.dec is NAN");
  else if(fabs(e.dec) > 90.0)
    novas_set_errno(EINVAL, fn, "input cat_entry.dec is outside of [-90:90] range: %g", e.dec);
  if(isnan(e.parallax))
    novas_set_errno(EINVAL, fn, "input cat_entry.parallax is NAN");
  if(e.parallax < 0.0)
    novas_set_errno(EINVAL, fn, "input cat_entry.parallax is negative: %g mas", e.parallax);
  if(isnan(e.promora))
    novas_set_errno(EINVAL, fn, "input cat_entry.promora is NAN");
  if(isnan(e.promodec))
    novas_set_errno(EINVAL, fn, "input cat_entry.promodec is NAN");
  if(isnan(e.radialvelocity))
    novas_set_errno(EINVAL, fn, "input cat_entry.radialvelocity is NAN");
  if(!_sys.is_valid())
    novas_set_errno(EINVAL, fn, "input equatorial system is invalid");
  else
    _valid = true;
}

/**
 * Returns the equatorial coordinate system in which this catalog entry is defined.
 *
 * @return    the equatorial coordinate system of this catalog entry
 *
 * @sa ra(), dec(), distance(), parallax(), radial_velocity(), v_lsr(), redshift()
 */
const Equinox& CatalogEntry::system() const {
  return _sys;
}

/**
 * Returns a pointer to the underlying NOVAS C `cat_entry` data structure, for use with
 * the standard C API.
 *
 * @return    a pointer to the underlying NOVAS C `cat_entry` data structure.
 */
const cat_entry* CatalogEntry::_cat_entry() const {
  return &_entry;
}

/**
 * Returns the catalog name that was defined.
 *
 * @return    the given catalog name.
 *
 * @sa number()
 */
std::string CatalogEntry::name() const {
  return std::string(_entry.starname);
}

/**
 * Returns the catalog number that was set.
 *
 * @return   the catalong number that was specified, or 0 if it was not explicitly defined.
 *
 * @sa catalog(), name()
 */
long CatalogEntry::number() const {
  return _entry.starnumber;
}

/**
 * Returns the catalog right ascention (R.A.) coordinate as a time-angle.
 *
 * @return    the catalog right-ascention angle as defined in the catalog system.
 *
 * @sa system(), equatorial()
 * @sa dec(), distance(), parallax(), radial_velocity(), v_lsr(), redshift()
 */
TimeAngle CatalogEntry::ra() const {
  return TimeAngle(_entry.ra * Unit::hour_angle);
}

/**
 * Returns the catalog declination coordinate as a angle.
 *
 * @return    the catalog declination angle as defined in the catalog system.
 *
 * @sa system(), equatorial()
 * @sa ra(), distance(), parallax(), radial_velocity(), v_lsr(), redshift()
 */
Angle CatalogEntry::dec() const {
  return Angle(_entry.dec * Unit::deg);
}

/**
 * Returns the scalar source velocity with respect to the Local Standard of Rest (LSR).
 *
 * @return    the LSR velocity of the source (or the LSR velocity of the SSB if the source motion
 *            was not explicitly defined).
 *
 * @sa system(), radial_velocity(), redshift()
 * @sa ra(), dec(), distance(), parallax()
 */
Speed CatalogEntry::v_lsr() const {
  return Speed(novas_ssb_to_lsr_vel(_sys.epoch(), _entry.ra, _entry.dec, _entry.radialvelocity) * Unit::km / Unit::sec);
}

/**
 * Returns the barycentric radial velocity of the source, that is the radial velocity with
 * respect to the Solar System Barycenter (SSB).
 *
 * @return    the barycentric radial velocity (relative the SSB), or 0.0 if the source motion was
 *            not explicitly defined.
 *
 * @sa system(), v_lsr(), redshift()
 * @sa ra(), dec(), distance(), parallax()
 *
 */
Speed CatalogEntry::radial_velocity() const {
  return Speed(_entry.radialvelocity * Unit::km / Unit::sec);
}

/**
 * Returns the barycentric redshift of the source, that is the redshift measure with respect to
 * the Solar System Barycenter (SSB).
 *
 * @return    the nominal redshift (rel. to the SSB) of the source, or 0.0 if the source motion
 *            was not explicitly defined.
 *
 * @sa system(), radial_velocity(), v_lsr()
 * @sa ra(), dec(), distance(), parallax()
 */
double CatalogEntry::redshift() const {
  return novas_v2z(_entry.radialvelocity * Unit::km / Unit::sec);
}

/**
 * Returns the catalog distance of the source.
 *
 * @return    the distance of the source, or 1 Gpc if it was not explicitly defined.
 *
 * @sa system(), parallax()
 * @sa ra(), dec(), radial_velocity(), v_lsr(), redshift()
 */
Distance CatalogEntry::distance() const {
  return Distance(Unit::kpc / _entry.parallax);
}

/**
 * Returns the parallax of the source.
 *
 * @return    the parallax of the source, or 1 &mu;as if not explicitly defined.
 *
 * @sa system(), distance()
 * @sa ra(), dec(), radial_velocity(), v_lsr(), redshift()
 */
Angle CatalogEntry::parallax() const {
  return Angle(_entry.parallax * Unit::mas);
}

/**
 * Returns the equatorial catalog coordinates of this source.
 *
 * @return    the equatorial coordinates of the source in the catalog system
 *
 * @sa system(), ra(), dec(), distance()
 */
Equatorial CatalogEntry::equatorial() const {
  return Equatorial(ra(), dec(), system(), distance());
}

/**
 * Sets the proper motion of this source, defined at the same epoch as the catalog
 * coordinates, returning itself to enable builder pattern.
 *
 * @param ra    [rad/s] Proper motion in the R.A. direction
 * @param dec   [rad/s] Proper motion in the declination direction
 * @return      itself
 *
 * @sa parallax(), distance(), radial_velocity(), v_lsr(), redshift(), catalog()
 * @sa system()
 */
CatalogEntry& CatalogEntry::proper_motion(double ra, double dec) {
  static const char *fn = "CatalogEntry::proper_motion()";

  if(isnan(ra)) {
    novas_set_errno(EINVAL, fn, "RA motion is NAN");
    _valid = false;
  }
  else if(isnan(dec)) {
    novas_set_errno(EINVAL, fn, "Dec motion is NAN");
    _valid = false;
  }

  novas_set_proper_motion(&_entry, ra / (Unit::mas / Unit::yr), dec / (Unit::mas / Unit::yr));
  return *this;
}

/**
 * Sets the parallax of this source, defined at the same epoch as the catalog coordinates,
 * returning itself to enable builder pattern.
 *
 * @param radians   [rad] Parallax angle
 * @return          itself
 *
 * @sa distance()
 * @sa system(), radial_velocity(), v_lsr(), redshift(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::parallax(double radians) {
  static const char *fn = "CatalogEntry::parallax()";

  if(isnan(radians)) {
    novas_set_errno(EINVAL, fn, "input parallax is NAN");
    _valid = false;
  }
  else if(radians < 0.0) {
    novas_set_errno(EINVAL, fn, "input parallax is negative: %g mas", (radians / Unit::mas));
    _valid = false;
  }

  novas_set_parallax(&_entry, radians / Unit::mas);
  return *this;
}

/**
 * Sets the parallax this source, defined at the same epoch as the catalog coordinates,
 * returning itself to enable builder pattern.
 *
 * @param angle     Parallax angle
 * @return          itself
 *
 * @sa distance()
 * @sa system(), radial_velocity(), v_lsr(), redshift(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::parallax(const Angle& angle) {
  return parallax(angle.rad());
}

/**
 * Sets the distance of this source, defined at the same epoch as the catalog coordinates,
 * returning itself to enable builder pattern.
 *
 * @param meters    [m] source distance
 * @return          itself
 *
 * @sa parallax()
 * @sa system(),  radial_velocity(), v_lsr(), redshift(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::distance(double meters) {
  static const char *fn = "CatalogEntry::distance()";

  if(isnan(meters)) {
    novas_set_errno(EINVAL, fn, "input distance is NAN");
    _valid = false;
  }
  else if(meters < 0.0) {
    novas_set_errno(EINVAL, fn, "input distance is negative: %g pc", (meters / Unit::pc));
    _valid = false;
  }

  novas_set_distance(&_entry, meters / Unit::pc);
  return *this;
}

/**
 * Sets the distance of this source, defined at the same epoch as the catalog coordinates,
 * returning itself to enable builder pattern.
 *
 * @param dist      source distance
 * @return          itself
 *
 * @sa parallax()
 * @sa system(), radial_velocity(), v_lsr(), redshift(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::distance(const Distance& dist) {
  return distance(dist.m());
}

/**
 * Sets the motion of this source relative to the Local Standard of Rest (LSR), defined at the
 * same epoch as the catalog coordinates, and returning itself to enable builder pattern.
 *
 * @param v_ms      [m/s] LSR velocity
 * @return          itself
 *
 * @sa radial_velocity(), redshift()
 * @sa system(), distance(), parallax(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::v_lsr(double v_ms) {
  static const char *fn = "CatalogEntry::v_lsr()";

  if(isnan(v_ms)) {
    novas_set_errno(EINVAL, fn, "input LSR velocity is NAN");
    _valid = false;
  }
  else if(fabs(v_ms) > Constant::c) {
    novas_set_errno(EINVAL, fn, "input LSR velocity exceeds the speed of light: %g m/s", v_ms);
    _valid = false;
  }

  novas_set_lsr_vel(&_entry, _sys.epoch(), v_ms / (Unit::km / Unit::sec));
  return *this;
}

/**
 * Sets the motion of this source relative to the Local Standard of Rest (LSR), defined at the
 * same epoch as the catalog coordinates, and returning itself to enable builder pattern.
 *
 * @param v         LSR velocity
 * @return          itself
 *
 * @sa radial_velocity(), redshift()
 * @sa system(), distance(), parallax(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::v_lsr(const Speed& v) {
  return v_lsr(v.m_per_s());
}

/**
 * Sets the radial velocity of this source relative to the Solar System Barycenter (SSB),
 * defined at the same epoch as the catalog coordinates, and returning itself to enable builder
 * pattern.
 *
 * @param v_ms      [m/s] radial velocity with respect to the Solar System Barycenter (SSB)
 * @return          itself
 *
 * @sa v_lsr(), redshift()
 * @sa system(), distance(), parallax(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::radial_velocity(double v_ms) {
  static const char *fn = "CatalogEntry::radial_velocity()";

  if(isnan(v_ms)) {
    novas_set_errno(EINVAL, fn, "input value is NAN");
    _valid = false;
  }
  else if(fabs(v_ms) > Constant::c) {
    novas_set_errno(EINVAL, fn, "radial velocity exceeds the speed of light: %g m/s", v_ms);
    _valid = false;
  }

  novas_set_ssb_vel(&_entry, v_ms / (Unit::km / Unit::sec));
  return *this;
}

/**
 * Sets the radial velocity of this source relative to the Solar System Barycenter (SSB),
 * defined at the same epoch as the catalog coordinates, and returning itself to enable builder
 * pattern.
 *
 * @param v         radial velocity with respect to the Solar System Barycenter (SSB)
 * @return          itself
 *
 * @sa v_lsr(), redshift()
 * @sa system(), distance(), parallax(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::radial_velocity(const Speed& v) {
  return radial_velocity(v.m_per_s());
}

/**
 * Sets the redshift of this source relative to the Solar System Barycenter (SSB), defined at the
 * same epoch as the catalog coordinates, and returning itself to enable builder pattern.
 *
 * @param z         barycentric redshift measure.
 * @return          itself
 *
 * @sa radial_velocity(), v_lsr()
 * @sa system(), distance(), parallax(), proper_motion(), catalog()
 */
CatalogEntry& CatalogEntry::redshift(double z) {
  static const char *fn = "CatalogEntry::redshift";

  if(novas_set_redshift(&_entry, z) != 0) {
    novas_trace_invalid(fn);
    _valid = false;
  }
  else if(isnan(z)) {
    novas_set_errno(EINVAL, fn, "input redshift is NAN");
    _valid = false;
  }

  return *this;
}

/**
 * Sets the catalog name and the objects catalog ID number, returning itself to enable builder pattern.
 *
 * @param name      catalog name (only SIZE_OF_CAT_NAME character, including termination will be stored).
 *                  For example: "NGC", "HIP", "FK5", "3C", "IRAS".
 * @param number    catalog number of source in above catalog.
 * @return          itself
 *
 * @sa system(), radial_velocity(), v_lsr(), distance(), parallax(), proper_motion(), catalog()
 * @sa system()
 */
CatalogEntry& CatalogEntry::catalog(const std::string& name, long number) {
  if(novas_set_catalog(&_entry, name.c_str(), number) != 0)
    novas_trace_invalid("CatalogEntry::catalog(name, number)");
  return *this;
}

} // namespace supernovas

