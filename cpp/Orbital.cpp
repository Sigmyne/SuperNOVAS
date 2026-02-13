/**
 * @file
 *
 * @date Created  on Oct 9, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

OrbitalSystem::OrbitalSystem(enum novas::novas_reference_plane plane, enum novas_planet center) : _system({}) {
  if((unsigned) center >= NOVAS_PLANETS)
    novas_set_errno(EINVAL, "OrbitalSystem()", "center planet is invalid: %d", center);
  else _valid = true;

  _system.plane = plane;
  _system.center = center;
}

OrbitalSystem::OrbitalSystem(const novas::novas_orbital_system *system) {
  static const char *fn = "OrbitalSystem()";

  errno = 0;

  if((unsigned) system->center >= NOVAS_PLANETS)
    novas_set_errno(EINVAL, fn, "input system center is invalid: %d", (int) system->center);
  if((unsigned) system->plane >= NOVAS_REFERENCE_PLANES)
    novas_set_errno(EINVAL, fn, "input reference plane is invalid: %d", (int) system->plane);
  if((unsigned) system->type >= NOVAS_REFERENCE_SYSTEMS)
    novas_set_errno(EINVAL, fn, "input system coordinate type is invalid: %d", (int) system->type);
  if(!isfinite(system->obl))
    novas_set_errno(EINVAL, fn, "input system obliquity is NAN or infinite");
  if(!isfinite(system->Omega))
    novas_set_errno(EINVAL, fn, "input system Omega is NAN or infinite");

  _system = *system;

  _valid = (errno == 0);
}

/**
 * (<i>primarily for internal use</i>) Returns the underlying NOVAS C data structure, which
 * defines the orbital system.
 *
 * @return  a pointer to the NOVAS C structure used internally to define the orbital system.
 */
const novas::novas_orbital_system * OrbitalSystem::_novas_orbital_system() const {
  return &_system;
}

/**
 * Returns the major planet (including the Sun, Moon, SSB, EMB, and Pluto system barycenter),
 * which is the center of the orbits that are defined in this orbital system.
 *
 * @return  the Solar-system body or barycenter position around which orbits are defined in
 *          this system.
 */
Planet OrbitalSystem::center() const {
  return Planet(_system.center);
}

/**
 * Returns the obliquity of this orbital system, relative to the reference plane (equatorial
 * or ecliptic) in which this orbital system was specified.
 *
 * @return    the obliquity of the orbital system relative to the reference plane.
 *
 * @sa ascending_node(), pole(), reference_system()
 */
Angle OrbitalSystem::obliquity() const {
  return Angle(_system.obl);
}

/**
 * Returns the angle of the orbital system's ascending node relative to the vernal equinox
 * on the reference plane (equatorial or ecliptic) relative to which the orbital system
 * was specified.
 *
 * @return    the ascending node's distance from the vernal equinox in the orbital
 *            reference plane.
 *
 * @sa obliquity(), pole(), reference_system()
 */
Angle OrbitalSystem::ascending_node() const {
  return Angle(_system.Omega);
}

/**
 * Returns the coordinate reference system type, in which the orbital system is defined.
 *
 * @return    the coordinate reference system type used for definining this orbital system.
 *
 * @sa obliquity(), ascending_node(), pole(),
 */
enum novas_reference_system OrbitalSystem::reference_system() const {
  return _system.type;
}

/**
 * Returns the orbital system's pole in the reference plane (equatorial or ecliptic), relative to
 * which the system is specified.
 *
 * @return    The spherical location of the orbital system pole, in the reference plane.
 *
 * @sa reference_system(), obliquity(), ascending_node()
 */
Spherical OrbitalSystem::pole() const {
  return Spherical(_system.Omega - Constant::half_pi, Constant::half_pi - _system.obl);
}

/**
 * Defines the orientation of this orbital system relative to the reference plane (equatorial or
 * ecliptic), relative to which the system is defined.
 *
 * @param obliquity_rad       [rad] orbital system's obliquity relative to equator or ecliptic
 * @param ascending_node_rad  [rad] ascending node of orbital system from the vernal equinox
 * @param system              (optional) the definition of the equinox to assume (default: ICRS).
 * @return                    itself
 *
 * @sa pole(), obliquity(), ascending_node()
 */
OrbitalSystem& OrbitalSystem::orientation(double obliquity_rad, double ascending_node_rad, const Equinox& system) {
  static const char *fn = "OrbitalSystem::orinetation";

  _system.type = system.reference_system();
  _system.obl = obliquity_rad;
  _system.Omega = ascending_node_rad;

  errno = 0;

  if(!isfinite(obliquity_rad))
    novas_set_errno(EINVAL, fn, "input obliquity is NAN or infinite");
  if(!isfinite(ascending_node_rad))
    novas_set_errno(EINVAL, fn, "input ascending node is NAN or infinite");
  if(!system.is_valid())
    novas_set_errno(EINVAL, fn, "input equatorial system is invalid");

  if((unsigned) _system.center < NOVAS_PLANETS)
    _valid = (errno == 0);

  return *this;
}

/**
 * Defines the orientation this orbital system relative to the reference plane (equatorial or
 * ecliptic), relative to which the system is defined.
 *
 * @param obliquity       orbital system's obliquity relative to equator or ecliptic
 * @param ascending_node  ascending node of orbital system from the vernal equinox
 * @param system          (optional) the definition of the equinox to assume (default: ICRS).
 * @return                itself
 *
 * @sa pole(), obliquity(), ascending_node()
 */
OrbitalSystem& OrbitalSystem::orientation(const Angle& obliquity, const Angle& ascending_node, const Equinox& system) {
  return orientation(obliquity.rad(), ascending_node.rad(), system);
}

/**
 * Defines the pole this orbital system relative to the reference plane (equatorial or ecliptic),
 * relative to which the system is defined.
 *
 * @param longitude_rad   [rad] longitude of orbital system pole in reference plane.
 * @param latitude_rad    [rad] latitude of orbital system pole in reference plane.
 * @param system          (optional) the definition of the equinox to assume (default: ICRS).
 * @return                itself
 *
 * @sa orientation(), obliquity(), ascending_node()
 */
OrbitalSystem& OrbitalSystem::pole(double longitude_rad, double latitude_rad, const Equinox& system) {
  OrbitalSystem& s = orientation(Constant::half_pi - latitude_rad, Constant::half_pi + longitude_rad, system);

  if(!isfinite(longitude_rad) || !isfinite(latitude_rad) || !system.is_valid())
    novas_trace_invalid("OrbitalSystem::pole");

  return s;
}

/**
 * Defines the pole this orbital system relative to the reference plane (equatorial or ecliptic),
 * relative to which the system is defined.
 *
 * @param longitude   longitude of orbital system pole in reference plane.
 * @param latitude    latitude of orbital system pole in reference plane.
 * @param system      (optional) the definition of the equinox to assume (default: ICRS).
 * @return            itself
 *
 * @sa orientation(), obliquity(), ascending_node()
 */
OrbitalSystem& OrbitalSystem::pole(const Angle& longitude, const Angle& latitude, const Equinox& system) {
  return pole(longitude.rad(), latitude.rad(), system);
}

/**
 * Defines the pole this orbital system relative to the reference plane (equatorial or ecliptic),
 * relative to which the system is defined.
 *
 * @param coords          equatorial or ecliptic coordinates of the orbital system's pole.
 * @param system          (optional) the definition of the equinox to assume (default: ICRS).
 * @return                itself
 *
 * @sa pole(), obliquity(), ascending_node()
 */
OrbitalSystem& OrbitalSystem::pole(const Spherical& coords, const Equinox& system) {
  return pole(coords.longitude(), coords.latitude(), system);
}

/**
 * Returns a new equarial orbital system around the specified major planet, Sun, Moon, or
 * barycenter position. The new orbital system is assumed to be aligned with the equatorial
 * plane and coordinate system, until its orientation / pole is defined otherwise.
 *
 * @param center    the major planet, Sun, Moon, or barycenter position
 * @return          a new equatorial orbital system around the specified center position.
 *
 * @sa ecliptic(), orientation(), pole()
 */
OrbitalSystem OrbitalSystem::equatorial(const Planet& center) {
  return OrbitalSystem(NOVAS_EQUATORIAL_PLANE, center.novas_id());
}

/**
 * Returns a new ecliptic orbital system around the specified major planet, Sun, Moon, or
 * barycenter position. The new orbital system is assumed to be aligned with the ecliptic
 * plane and coordinate system, until its orientation / pole is defined otherwise.
 *
 * @param center    the major planet, Sun, Moon, or barycenter position
 * @return          a new ecliptic orbital system around the specified center position.
 *
 * @sa equatorial(), orientation(), pole()
 */
OrbitalSystem OrbitalSystem::ecliptic(const Planet& center) {
  return OrbitalSystem(NOVAS_ECLIPTIC_PLANE, center.novas_id());
}

/**
 * (<i>primarily for internal use</i>) Constructs a new orbital system by copying the specified
 * NOVAS C orbital system data structure, or `std::nullopt` if the argument is NULL. It's best
 * practice to call `is_valid()` after to check that the supplied parameters do in fact define a
 * valid orbital system.
 *
 * @param system    The NOVAS C orbital system data structure (copied)
 * @return          A new orbital system with that copies the parameters of the argument.
 *
 * @sa is_valid()
 */
std::optional<OrbitalSystem> OrbitalSystem::from_novas_orbital_system(const novas::novas_orbital_system *system) {
  if(!system) {
    novas_trace_invalid("OrbitalSystem::from_novas_orbital_system");
    return std::nullopt;
  }

  return OrbitalSystem(system);
}

void Orbital::validate(const char *loc) {
  static const char *fn = "Orbital::validate";

  errno = 0;

  if(!system().is_valid())
    novas_set_errno(EINVAL, fn, "input orbital system is invalid");
  if(!isfinite(_orbit.jd_tdb))
    novas_set_errno(EINVAL, fn, "input orbit->jd_tdb is NAN or infinite");
  if(!isfinite(_orbit.a))
    novas_set_errno(EINVAL, fn, "input orbit->a is NAN or infinite");
  if(_orbit.a == 0.0)
    novas_set_errno(EINVAL, fn, "input orbit->a is zero");
  if(_orbit.a < 0.0)
    novas_set_errno(EINVAL, fn, "input orbit->a is negative");
  if(!isfinite(_orbit.M0))
    novas_set_errno(EINVAL, fn, "input orbit->M0 is NAN or infinite");
  if(!isfinite(_orbit.n))
    novas_set_errno(EINVAL, fn, "input orbit->n is NAN or infinite");
  if(_orbit.n == 0.0)
    novas_set_errno(EINVAL, fn, "input orbit->n is zero");
  if(_orbit.n < 0.0)
    novas_set_errno(EINVAL, fn, "input orbit->n is negative");
  if(!isfinite(_orbit.e))
    novas_set_errno(EINVAL, fn, "input orbit->e is NAN or infinite");
  if(_orbit.e < 0.0)
    novas_set_errno(EINVAL, fn, "input orbit->e is negative");
  if(!isfinite(_orbit.omega))
    novas_set_errno(EINVAL, fn, "input orbit->omega is NAN or infinite");
  if(!isfinite(_orbit.i))
    novas_set_errno(EINVAL, fn, "input orbit->i is NAN or infinite");
  if(!isfinite(_orbit.Omega))
    novas_set_errno(EINVAL, fn, "input orbit->Omega is NAN or infinite");
  if(!isfinite(_orbit.apsis_period))
    novas_set_errno(EINVAL, fn, "input orbit->apsis_period is NAN or infinite");
  if(!isfinite(_orbit.node_period))
    novas_set_errno(EINVAL, fn, "input orbit->node_period is NAN or infinite");

  if(errno)
    novas_trace_invalid(loc);

  _valid = (errno == 0);
}

Orbital::Orbital(const novas_orbital *orbit) : _orbit(*orbit) {
  validate("Orbital()");
}

/**
 * Instantiates a new Keplerian orbital in the specified orbital system and the basic circular
 * orbital parameters. You can further specify the parameters for elliptical orbits using a
 * builder pattern after instantiation.
 *
 * @param system            the orbital system in which the orbit is defined.
 * @param jd_tdb            [day] reference date of the orbital parameters as a Barycentric
 *                          Dynamical Time (TDB) based Julian date
 * @param semi_major_m      [m] semi-major axis (circular radius) of the orbit
 * @param mean_anomaly_rad  [rad] Mean anomaly (circular longitude) of the object at the reference
 *                          time, in the orbital system.
 * @param period_s          [s] orbital period.
 *
 * @sa with_mean_motion(), eccentricity(), inclination(), pole(), node_period(), node_rate()
 *     apsis_period(), apsis_rate()
 */
Orbital::Orbital(const OrbitalSystem& system, double jd_tdb, double semi_major_m, double mean_anomaly_rad, double period_s) {
  _orbit.system = *(system._novas_orbital_system());
  _orbit.jd_tdb = jd_tdb;
  _orbit.a = semi_major_m / Unit::au;
  _orbit.M0 = mean_anomaly_rad / Unit::deg;
  _orbit.n = 360.0 / (period_s / Unit::day);

  validate("Orbital()");
}

/**
 * Instantiates a new Keplerian orbital in the specified orbital system and the basic circular
 * orbital parameters. You can further specify the parameters for elliptical orbits using a
 * builder pattern after instantiation.
 *
 * @param system            the orbital system in which the orbit is defined.
 * @param jd_tdb            reference time of the orbital parameters.
 * @param semi_major_m      semi-major axis (circular radius) of the orbit
 * @param mean_anomaly_rad  Mean anomaly (circular longitude) of the object at the reference time,
 *                          in the orbital system.
 * @param period_s          orbital period.
 *
 * @sa with_mean_motion(), eccentricity(), inclination(), pole(), node_period(), node_rate()
 *     apsis_period(), apsis_rate()
 */
Orbital::Orbital(const OrbitalSystem& system, const Time& ref_time, const Distance& semi_major,
        const Angle& mean_anomaly, const Interval& period)
: Orbital(system, ref_time.jd(NOVAS_TDB), semi_major.m(), mean_anomaly.rad(), period.seconds()) {}

/**
 * (<i>for internal use</i>) Returns the underlying NOVAS C data structure containing the orbital
 * parameters.
 *
 * @return  pointer to the NOVAS C data structure that stores the orbital parameters internally.
 */
const novas::novas_orbital * Orbital::_novas_orbital() const {
  return &_orbit;
}

/**
 * Returns a new instance of the orbital system in which this orbit is defined.
 *
 * @return    a new instance of the orbital system, in which the orbit is defined.
 */
OrbitalSystem Orbital::system() const {
  return OrbitalSystem::from_novas_orbital_system(&_orbit.system).value();
}

/**
 * Returns the reference time, as a Barycentric Dynamical Time (TDB) based Julian date.
 *
 * @return    [day] the TDB-based Julian date for which the orbital parameters are defined.
 */
double Orbital::reference_jd_tdb() const {
  return _orbit.jd_tdb;
}

/**
 * Returns the semi-major axis (that is the radius for circular orbits) of this orbital.
 *
 * @return    the semi-major axis (circular) radius of this orbit.
 */
Distance Orbital::semi_major_axis() const {
  return Distance(_orbit.a * Unit::au);
}

/**
 * Returns the mean anomaly (or longitude for circular orbits) of the object at the
 * reference time, in the orbital system in which the orbit was defined.
 *
 * @return    the mean anomaly (circular longitude) of the object in the orbital
 *            system at the reference time.
 *
 * @sa reference_jd_tdb()
 */
Angle Orbital::reference_mean_anomaly() const {
  return Angle(_orbit.M0 * Unit::deg);
}

/**
 * Returns the mean motion (circular angular velocity) of the object in this orbit.
 *
 * @return    [rad/s] the mean motion (circular angular velocity) of the object on
 *            this orbit.
 *
 * @sa period()
 */
double Orbital::mean_motion() const {
  return _orbit.n * Unit::deg / Unit::day;
}

/**
 * Returns the period of the object on this orbit.
 *
 * @return    the time it takes for the object to complete a full orbit.
 *
 * @sa mean_motion()
 */
Interval Orbital::period() const {
  return Interval(Constant::two_pi / mean_motion());
}

/**
 * Returns the eccentricity of this orbital.
 *
 * @return    the eccentricity of this orbital (dimensionless).
 *
 * @sa periapsis()
 */
double Orbital::eccentricity() const {
  return _orbit.e;
}

/**
 * Returns the periapsis angle of this orbit, in the orbital system, in which the orbit was
 * defined.
 *
 * @return    the longitude (in the orbital system) at which point the object is closest
 *            to the orbital center (on an elliptical orbit).
 *
 * @sa eccentricity()
 */
Angle Orbital::periapsis() const {
  return Angle(_orbit.omega * Unit::deg);
}

/**
 * Returns the inclination angle of this orbit, relative to the orbital system's plane.
 *
 * @return    the inclination of the orbit relative to the orbital system's plane.
 *
 * @sa ascending_node(), pole()
 */
Angle Orbital::inclination() const {
  return Angle(_orbit.i * Unit::deg);
}

/**
 * Returns the longitude of the ascending node of this orbit in the orbital system, in which
 * the orbital was defined.
 *
 * @return      the longitude of the ascending node in the orbital system.
 *
 * @sa inclination(), pole()
 */
Angle Orbital::ascending_node() const {
  return Angle(_orbit.Omega * Unit::deg);
}

/**
 * Returns the spherical coordinates of the orbit's pole in the orbital system, in which the
 * orbital was defined.
 *
 * @return    the location of the orbit's pole in the orbital system, in which the orbital is
 *            defined.
 *
 * @sa inclination(), ascending_node()
 */
Spherical Orbital::pole() const {
  return Spherical(_orbit.Omega * Unit::deg - Constant::half_pi, Constant::half_pi - _orbit.i * Unit::deg);
}

/**
 * Returns the rotation period of the apsis location in the orbital system, in which the orbital
 * was defined (positive for counter-clockwise rotation, or negative for clockwise rotation, when
 * viewed from the orbital system's pole.)
 *
 * @return      the time it takes for the apsis to make a full counter-clockwise rotation in the
 *              orbital system, viewed from the orbital system's pole. It may be negative for
 *              retrograde (clockwise) motion.
 *
 * @sa apsis_rate(), eccentricity(), node_period()
 */
Interval Orbital::apsis_period() const {
  return Interval(_orbit.apsis_period * Unit::day);
}

/**
 * Returns the rotation period (due to precession) of the orbit's ascending node in the orbital
 * system, in which the orbital was defined (positive for counter-clockwise rotation, or negative
 * for clockwise rotation, when viewed from the orbital system's pole.)
 *
 * @return      the time it takes for the ascending to make a full counter-clockwise rotation in the
 *              orbital system, viewed from the orbital system's pole. It may be negative for
 *              retrograde (clockwise) motion.
 *
 * @sa node_rate(), inclination(), apsis_period()
 */
Interval Orbital::node_period() const {
  return Interval(_orbit.node_period * Unit::day);
}

/**
 * Returns the angular rate at which the apsis rotates in the orbital system (positive for
 * counter-clockwise rotation, or negative for clockwise rotation, when viewed from the orbital
 * system's pole.)
 *
 * @return    [rad/s] the angular velocity of the apsis' rotation in the orbital system, viewed
 *            from the orbital system's pole. It may be negative for retrograde motion.
 *
 * @sa apsis_period(), node_rate()
 */
double Orbital::apsis_rate() const {
  return Constant::two_pi / apsis_period().seconds();
}

/**
 * Returns the angular rate at which the ascending node of the orbit rotates in the orbital system
 * (positive for counter-clockwise rotation, or negative for clockwise rotation, when viewed from
 * the orbital  system's pole.)
 *
 * @return    [rad/s] the angular velocity of the ascending node rotation (precession) in the
 *            orbital system, viewed from the orbital system's pole. It may be negative for
 *            retrograde motion.
 *
 * @sa node_period(), apsis_rate()
 */
double Orbital::node_rate() const {
  return Constant::two_pi / node_period().seconds();
}

/**
 * Calculates a rectangular equatorial position vector for this Keplerian orbital for the
 * specified time of observation.
 *
 * REFERENCES:
 * <ol>
 * <li>E.M. Standish and J.G. Williams 1992.</li>
 * <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 * <li>https://en.wikipedia.org/wiki/Orbital_elements</li>
 * <li>https://orbitalofficial.com/</li>
 * <li>https://downloads.rene-schwarz.com/download/M001-Keplerian_Orbit_Elements_to_Cartesian_State_Vectors.pdf</li>
 * </ol>
 *
 * @param time          Astrometric time of observation
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1).
 * @return              The rectangular equatorial position vector of the orbital object, relative
 *                      to the equinox type of the orbital system.
 *
 * @sa velocity()
 */
Position Orbital::position(const Time& time, enum novas_accuracy accuracy) const {
  double p[3] = {0.0};

  if(novas_orbit_posvel(time.jd(), &_orbit, accuracy, p, NULL) != 0) {
    novas_trace_invalid("Orbital::position");
    return Position::invalid();
  }

  return Position(p, Unit::au);
}

/**
 * Calculates a rectangular equatorial velocity vector for this Keplerian orbital for the
 * specified time of observation.
 *
 * REFERENCES:
 * <ol>
 * <li>E.M. Standish and J.G. Williams 1992.</li>
 * <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 * <li>https://en.wikipedia.org/wiki/Orbital_elements</li>
 * <li>https://orbitalofficial.com/</li>
 * <li>https://downloads.rene-schwarz.com/download/M001-Keplerian_Orbit_Elements_to_Cartesian_State_Vectors.pdf</li>
 * </ol>
 *
 * @param time          Astrometric time of observation
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1).
 * @return              The rectangular equatorial velocity vector of the orbital object, relative
 *                      to the equinox type of the orbital system.
 *
 * @sa position()
 */
Velocity Orbital::velocity(const Time& time, enum novas_accuracy accuracy) const {
  double v[3] = {0.0};

  if(novas_orbit_posvel(time.jd(), &_orbit, accuracy, NULL, v) != 0) {
    novas_trace_invalid("Orbital::velocity");
    return Velocity::invalid();
  }

  return Velocity(v, Unit::au / Unit::day);
}

/**
 * Sets parameters for an elliptical orbit.
 *
 * @param e               eccenticity value (dimensionless).
 * @param periapsis_rad   [rad] longitude of the apsis (the point at which the elliptial orbit is
 *                        closest to the center), in the orbital system, in which the orbit is
 *                        defined.
 * @return                itself.
 *
 * @sa apsis_period(), apsis_rate()
 */
Orbital& Orbital::eccentricity(double e, double periapsis_rad) {
  static const char *fn = "Orbital::eccentricity";

  _orbit.e = e;
  _orbit.omega = periapsis_rad / Unit::deg;

  if(!isfinite(e)) {
    novas_set_errno(EINVAL, fn, "input eccentricity is NAN or infinite");
    _valid = false;
  }
  else if(e < 0.0) {
    novas_set_errno(EINVAL, fn, "input eccentricity is negative");
    _valid = false;
  }
  else if(!isfinite(periapsis_rad)) {
    novas_set_errno(EINVAL, fn, "input periapsis is NAN or infinite");
    _valid = false;
  }
  else if(!_valid) {
    validate(fn);
  }

  return *this;
}

/**
 * Sets parameters for an elliptical orbit.
 *
 * @param e          eccenticity value (dimensionless).
 * @param periapsis  longitude of the apsis (the point at which the elliptial orbit is closest to
 *                   the center), in the orbital system, in which the orbit is defined.
 * @return           itself.
 *
 * @sa apsis_period(), apsis_rate()
 */
Orbital& Orbital::eccentricity(double e, const Angle& periapsis_angle) {
  return eccentricity(e, periapsis_angle.rad());
}

/**
 * Sets parameters for an orbit that is inclined relative to the orbital system's native plane.
 *
 * @param angle_rad           [rad] inclination angle
 * @param ascending_node_rad  [rad] longitude of the ascending node in the orbital system, in
 *                            which the orbit is defined.
 * @return                    itself.
 *
 * @sa pole(), node_period(), node_rate()
 */
Orbital& Orbital::inclination(double angle_rad, double ascending_node_rad) {
  static const char *fn = "Orbital::inclination";

  _orbit.i = angle_rad / Unit::deg;
  _orbit.Omega = ascending_node_rad / Unit::deg;

  if(!isfinite(angle_rad)) {
    novas_set_errno(EINVAL, fn, "input inclination angle is NAN or infinite");
    _valid = false;
  }
  else if(!isfinite(ascending_node_rad)) {
    novas_set_errno(EINVAL, fn, "input ascending node is NAN or infinite");
    _valid = false;
  }
  else if(!_valid)
    validate(fn);

  return *this;
}

/**
 * Sets parameters for an orbit that is inclined relative to the orbital system's native plane.
 *
 * @param angle_rad       inclination angle
 * @param ascending_node  longitude of the ascending node in the orbital system, in which the
 *                        orbit is defined.
 * @return                itself.
 *
 * @sa pole(), node_period(), node_rate()
 */
Orbital& Orbital::inclination(const Angle& angle, const Angle& ascending_node_angle) {
  return inclination(angle.rad(), ascending_node_angle.rad());
}

/**
 * Sets the orbit's pole, in the orbital system in which the orbit is defined.
 *
 * @param longitude_rad   [rad] longitude of orbit's pole in the orbital system.
 * @param latitude_rad    [rad] latitude of the orbit's pole in the orbital system.
 * @return                itself
 *
 * @sa inclination(), node_pediod(), node_rate()
 */
Orbital& Orbital::pole(double longitude_rad, double latitude_rad) {
  Orbital& o = inclination(Constant::half_pi - latitude_rad, Constant::half_pi + longitude_rad);

  if(!isfinite(latitude_rad) || !isfinite(longitude_rad))
    novas_trace_invalid("Orbital::pole");

  return o;
}

/**
 * Sets the orbit's pole, in the orbital system in which the orbit is defined.
 *
 * @param longitude   longitude of orbit's pole in the orbital system.
 * @param latitude    latitude of the orbit's pole in the orbital system.
 * @return            itself
 *
 * @sa inclination(), node_pediod(), node_rate()
 */
Orbital& Orbital::pole(const Angle& longitude, const Angle& latitude) {
  return pole(longitude.rad(), latitude.rad());
}

/**
 * Sets the orbit's pole, in the orbital system in which the orbit is defined.
 *
 * @param coords      location of the pole in the orbital system.
 * @return            itself
 *
 * @sa inclination(), node_pediod(), node_rate()
 */
Orbital& Orbital::pole(const Spherical& coords) {
  return pole(coords.longitude(), coords.latitude());
}

/**
 * Sets the apsis rotation period (positive for counter-clockwise rotation when viewed from the
 * orbital system's pole).
 *
 * @param seconds     [s] counter-clockwise rotation period of the apsis. It may be negative for
 *                    clockwise (retrograde) rotation seen from the orbital system's pole.
 * @return            itself
 *
 * @sa apsis_rate(), periapsis(), eccentricity()
 */
Orbital& Orbital::apsis_period(double seconds) {
  static const char *fn = "Orbital::apsis_period";

  _orbit.apsis_period = seconds / Unit::day;

  if(!isfinite(seconds)) {
    novas_set_errno(EINVAL, fn, "input apsis period is NAN or infinite");
    _valid = false;
  }
  else if(!_valid) {
    validate(fn);
  }

  return *this;
}

/**
 * Sets the apsis rotation period (positive for counter-clockwise rotation when viewed from the
 * orbital system's pole).
 *
 * @param seconds     [s] time it takes for a full rotation of the apsis in the orbital system. It
 *                    may be negative for clockwise (retrograde) rotation seen from the orbital
 *                    system's pole.
 * @return            itself
 *
 * @sa apsis_rate(), periapsis(), eccentricity()
 */
Orbital& Orbital::apsis_period(const Interval& period) {
  return apsis_period(period.seconds());
}

/**
 * Sets the apsis rotation rate (positive for counter-clockwise rotation when viewed from the
 * orbital system's pole).
 *
 * @param rad_per_sec [rad/s] counter-clockwise rotation rate of the apsis. It may be negative for
 *                    clockwise (retrograde) rotation seen from the orbital system's pole.
 * @return            itself
 *
 * @sa apsis_rate(), periapsis(), eccentricity()
 */
Orbital& Orbital::apsis_rate(double rad_per_sec) {
  Orbital& orbit = apsis_period(Constant::two_pi / rad_per_sec);

  if(!isfinite(rad_per_sec))
    novas_trace_invalid("Orbital::apsis_rate");

  return orbit;
}

/**
 * Sets the node precession period (positive for counter-clockwise rotation when viewed from the
 * orbital system's pole).
 *
 * @param seconds     [s] counter-clockwise precession period of the node. It may be negative for
 *                    clockwise (retrograde) rotation seen from the orbital system's pole.
 * @return            itself
 *
 * @sa node_rate(), ascending_node(), inclination(), pole()
 */
Orbital& Orbital::node_period(double seconds) {
  static const char *fn = "Orbital::node_period";

  _orbit.node_period = seconds / Unit::day;

  if(!isfinite(seconds)) {
    novas_set_errno(EINVAL, fn, "input mode period is NAN or infinite");
    _valid = false;
  }
  else if(!_valid) {
    validate(fn);
  }

  return *this;
}

/**
 * Sets the node precession period (positive for counter-clockwise rotation when viewed from the
 * orbital system's pole).
 *
 * @param seconds     counter-clockwise precession period of the node. It may be negative for
 *                    clockwise (retrograde) rotation seen from the orbital system's pole.
 * @return            itself
 *
 * @sa node_rate(), ascending_node(), inclination(), pole()
 */
Orbital& Orbital::node_period(const Interval& period) {
  return node_period(period.seconds());
}

/**
 * Sets the node precession rate (positive for counter-clockwise rotation when viewed from the
 * orbital system's pole).
 *
 * @param rad_per_sec [rad/sec] counter-clockwise precession rate of the node. It may be negative
 *                    for clockwise (retrograde) rotation seen from the orbital system's pole.
 * @return            itself
 *
 * @sa node_period(), ascending_node(), inclination(), pole()
 */
Orbital& Orbital::node_rate(double rad_per_sec) {
  Orbital& orbit = node_period(Constant::two_pi / rad_per_sec);

  if(!isfinite(rad_per_sec))
    novas_trace_invalid("Orbital::node_rate");

  return orbit;
}

/**
 * Return a new instance of a new Keplerian orbital in the specified orbital system and the basic
 * circular orbital parameters, with mean motion used instead of a orbital period. You can further
 * specify the parameters for elliptical orbits using a builder pattern after instantiation.
 *
 * @param system            the orbital system in which the orbit is defined.
 * @param jd_tdb            [day] reference date of the orbital parameters as a Barycentric
 *                          Dynamical Time (TDB) based Julian date
 * @param semi_major_m      [m] semi-major axis (circular radius) of the orbit
 * @param M0                [rad] Mean anomaly (circular longitude) of the object at the reference
 *                          time, in the orbital system.
 * @param ead_per_sec       [rad/s] mean motion (circular angular velocity) on orbit.
 *
 * @sa Orbital(), eccentricity(), inclination(), pole(), node_period(), node_rate()
 *     apsis_period(), apsis_rate()
 */
Orbital Orbital::with_mean_motion(const OrbitalSystem& system, double jd_tdb, double a, double M0, double rad_per_sec) {
  return Orbital(system, jd_tdb, a, M0, Constant::two_pi / rad_per_sec);
}

/**
 * Return a new instance of a new Keplerian orbital in the specified orbital system and the basic
 * circular orbital parameters, with mean motion used instead of a orbital period. You can further
 * specify the parameters for elliptical orbits using a builder pattern after instantiation.
 *
 * @param system            the orbital system in which the orbit is defined.
 * @param jd_tdb            reference time of the orbital parameters.
 * @param a                 semi-major axis (circular radius) of the orbit
 * @param M0                Mean anomaly (circular longitude) of the object at the reference time,
 *                          in the orbital system.
 * @param ead_per_sec       [rad/s] mean motion (circular angular velocity) on orbit.
 *
 * @sa Orbital(), eccentricity(), inclination(), pole(), node_period(), node_rate()
 *     apsis_period(), apsis_rate()
 */
Orbital Orbital::with_mean_motion(const OrbitalSystem& system, const Time& time, const Distance& a, const Angle& M0, double rad_per_sec) {
  return Orbital::with_mean_motion(system, time.jd(NOVAS_TDB), a.m(), M0.rad(), rad_per_sec);
}

/**
 * (<i>for internal use</i>) Returns a new instance of a Keplerian orbital, using a copy of a
 * NOVAS C orbital data structure, or `std::nullopt` if the argument is NULL. It's best practice
 * to call `is_valid()` after to check that the supplied parameters do in fact define a valid
 * orbital system.
 *
 * @param orbit   The NOVAS C orbital data structure (copied)
 * @return        A new Keplerian orbital instance, with the specified parameters.
 *
 * @sa Orbital(), is_valid()
 */
std::optional<Orbital> Orbital::from_novas_orbit(const novas_orbital *orbit) {
  if(!orbit) {
    novas_trace_invalid("Orbital::from_novas_orbit");
    return std::nullopt;
  }
  return Orbital(orbit);
}



} // namespace supernovas


