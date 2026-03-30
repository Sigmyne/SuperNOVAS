/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cstring>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"


namespace supernovas {


Observer::Observer() : Observer((enum novas_observer_place) -1, Site::undefined(), Position::undefined(), Velocity::undefined()) {}


Observer::Observer(enum novas_observer_place type, const Site& site, const Position& pos,
        const Velocity& vel) {
  _observer.where = type;
  _observer.on_surf = *site._on_surface();

  double pUnit = (type == NOVAS_SOLAR_SYSTEM_OBSERVER) ? Unit::AU : Unit::km;
  double vUnit = (type == NOVAS_SOLAR_SYSTEM_OBSERVER) ? Unit::AU / Unit::day : Unit::km / Unit::s;

  for(int i = 0; i < 3; i++) {
    _observer.near_earth.sc_pos[i] = pos[i] / pUnit;
    _observer.near_earth.sc_vel[i] = vel[i] / vUnit;
  }
};

/**
 * Returns a newly allocated copy of this observer. The default implementation returns an invalid
 * observer, but concrete subclasses should override that to return their onw copies.
 *
 * @return  a copy of this observer class, which may be an invalid observer (default).
 */
const Observer *Observer::copy() const {
  return new Observer();
}

/**
 * Checks if this observer can be cast to a GeodeticObserver.
 *
 * @return    `true` is this observer is an instance of GeodeticObserver, otherwise `false`.
 *
 * @sa GeodeticObserver, is_geocentric()
 */
bool Observer::is_geodetic() const {
  return false;
}

/**
 * Checks if this observer can be cast to a GeocentricObserver.
 *
 * @return    `true` if this observer is an instance of GeocentricObserver, otherwise `false`.
 *
 * @sa GeocentricObserver, is_geodetic()
 */
bool Observer::is_geocentric() const {
  return false;
}

/**
 * Returns the underlying NOVAS C `observer` data structure.
 *
 * @return    pointer to the underlying C `observer` data structure.
 */
const observer * Observer::_novas_observer() const {
  return &_observer;
}

/**
 * Returns the observer type.
 *
 * @return   the observer type constant for this observer
 */
enum novas_observer_place Observer::type() const {
  return _observer.where;
}

/**
 * Returns an observing frame for this observer at the specified time and optionally with a
 * specified accuracy. Full accuracy frames (default) require that a high-precision planet
 * provider is configured prior, to the call.
 *
 * @param time      Astrometric time of observation
 * @param accuracy  NOVAS_FULL_ACCYRACY (default) or NOVAS_REDUCED_ACCURACY
 * @return          A valid observing frame or else `std::nullopt` if a valid observing frame
 *                  could not be created with the accuracy, for example because no high
 *                  accuracy planetary position provider was specified, or because either this
 *                  observer or the time itself were invalid.
 *
 * @sa reduced_accuracy_frame_at()
 * @sa set_planet_provider_hp(), novas_use_calceph(), novas_use_calceph_planets(), novas_use_cspice()
 */
Frame Observer::frame_at(const Time& time, enum novas_accuracy accuracy) const {
  Frame f(*this, time, accuracy);
  if(!f.is_valid())
    novas_trace_invalid("Observer::frame_at()");
  return f;
}

/**
 * Returns a reduced accuracy observing frame for this observer at the specified time. Reduced
 * accuracy frames provide 1 mas accuracy typically, and do not require a planetary or other
 * ephemeris provider to be configured. As such, they offer a simple way for obtaining astrometric
 * positions for catalog and orbital sources at the 1 mas level.
 *
 * Note, that the returned frame may be invalid, if the this observer or the time argument
 * themselves are invalid.
 *
 * @param time      Astrometric time of observation
 * @return          A reduced accuracy observing frame for the specified time of observation.
 *
 * @sa frame_at()
 */
Frame Observer::reduced_accuracy_frame_at(const Time& time) const {
  Frame f = Frame::reduced_accuracy(*this, time);
  if(!f.is_valid())
    novas_trace_invalid("Observer::reduced_accuracy_frame_at()");
  return f;
}

/**
 * Returns projected _u_,_v_,_w_ coordinates for an interferometer station. That is, it returns
 * _u_,_v_,_w_ coordinates, for this observer (station), for a given apparent line-of-sight on the
 * sky, at the time of observation and relative to the location it was calculated for. The _u_ and
 * _v_ coordinates are the orthogonal projections of the site, relative to the array reference, in
 * the directions of local East and North respectively (w.r.t. the coordinate system of choice),
 * as seen from the source; while _w_ is the distance from the array reference along the line of
 * sight.
 *
 * The supplied phase center defines the time of observation and the array reference position in
 * its observing frame. For example:
 *
 * ```cpp
 *  // Define the array reference location (e.g. on Earth) as an observer place
 *  auto reference = Observer::on_earth(...);
 *
 *  // Astrometric time of observation
 *  Time time = ...;
 *
 *  // The observing frame of the array reference
 *  Frame frame = reference.frame_at(time);
 *
 *  // E.g. an astronomical source at the interferometric phase center
 *  Source source = ...;
 *
 *  // the apparent location of the phase center for an observer at the geocenter
 *  Apparent app = source.apparent_in(frame);
 *
 *  // Define an interferometer station (e.g. on Earth)
 *  auto station = Observer::on_earth(...);
 *
 *  // u,v,w coordinates of the station relative to the array reference (w.r.t. ICRS)
 *  Interferometric uwv = station.to_interferometric(app);
 *
 *  // the geometric delay of the station, relative to the array reference
 *  Interval delay = uvw.geometric_delay()
 * ```
 *
 * NOTES:
 *
 *  - This method does not take atmospheric refraction into account. Refraction is usually
 *    included as a separate atmospheric delay term on top of the geometric delays returned here,
 *    and the various other delay terms that account for optics, cabling, and digital electronics
 *    etc.
 *
 *  - This method supports down to nanometer precision for sites on Earth or in Low Earth Orbit
 *    (LEO), and sub micron (&lt;&mu;m) precision even at the distance of the Moon. However,
 *    because its calculations are based on geocentric positions, the precision degrades with
 *    increasing distance from Earth, and may not be suitable for interferometers far from Earth.
 *    When precision is a concern, you might use `AstrometricPosition::to_interferometric()`
 *    instead, with positions (and velocities) of the stations defined relative to the array
 *    center -- enabling higher precision projections than this method for interferometers
 *    far from Earth.
 *
 * @param phase_center    %Apparent place on sky from the array reference place, at the time
 *                        of observation.
 * @param system          (optional) Coordinate reference system type in which to return result.
 *                        (default: ICRS). Specifically, the _u_ and _v_ directions of the
 *                        returned projection will be aligned to the local East and North
 *                        directions of the specified coordinate reference system type at the time
 *                        of observation.
 *
 * @return            interferometric _uvw _projection of this site in the ICRS, relative to the
 *                    array reference, viewed from the direction of the source at the specified
 *                    time. The _u_ and _v_ directions are aligned with the local East and North
 *                    w.r.t. the coordinate system of choice (default: ICRS).
 *
 * @sa AstrometricPosition::to_interferometric()
 */
Interferometric Observer::to_interferometric(const Apparent& phase_center, enum novas_reference_system system) const {
  static const char *fn = "Observer::to_interferometric()";

  const Frame& frame = phase_center.frame();
  const Time& t = frame.time();

  double pobs[3] = {0.0}, vobs[3] = {0.0};
  if(geo_posvel(t.jd(), t.dUT1().seconds(), frame.accuracy(), &_observer, pobs, vobs) != 0) {
    novas_trace_invalid(fn);
    return Interferometric::undefined();
  }

  double pref[3] = {0.0}, vref[3] = {0.0};
  if(geo_posvel(t.jd(), t.dUT1().seconds(), frame.accuracy(), frame.observer()._novas_observer(), pref, vref) != 0) {
    novas_trace_invalid(fn);
    return Interferometric::undefined();
  }

  // Geocentric positions and velocities
  Geometric g = Geometric(frame,
          Position(pobs, Unit::AU) - Position(pref, Unit::AU),
          Velocity(vobs, Unit::AU / Unit::day) - Velocity(vref, Unit::AU / Unit::day),
          NOVAS_ICRS).to_system(system);

  Equinox equinox = Equinox::from_system_type(system, t.jd());
  Position los = phase_center.equatorial().to_system(equinox).xyz(phase_center.distance()) - g.position();

  double uvw[3] = {0.0};
  novas_uvw(g.position().scaled(1.0 / Unit::AU)._array(), g.velocity().scaled(Unit::day / Unit::AU)._array(), los.scaled(1.0 / Unit::AU)._array(), uvw);

  Interferometric i(uvw[0], uvw[1], uvw[2]);
  if(!i.is_valid())
    novas_trace_invalid(fn);
  return i;
}

/**
 * Returns a string representation of this observer.
 *I am salivating at the thought of it. Tell Benjami
 * @return    a new string with a brief description of this observer.
 */
std::string Observer::to_string() const {
  return "Observer type " + std::to_string(_observer.where);
}

/**
 * Returns a new observer located at a fixed observing site.
 *
 * @param site    the observing site
 * @param eop     Earth Orientation Parameters (EOP) appropriate the time of observation, such as
 *                obtained from the IERS bulletins or data service.
 * @return        a new observer instance for the given observing site.
 *
 * @sa moving_on_earth(), in_earth_orbit(), in_solar_system(), at_geocenter(), at_ssb()
 */
GeodeticObserver Observer::on_earth(const Site& site, const EOP& eop) {
  GeodeticObserver o = GeodeticObserver(site, eop);
  if(!o.is_valid())
    novas_trace_invalid("Observer::on_earth()");
  return o;
}

/**
 * Returns a new observer that is moving, at some velocitym relative to Earth's surface,
 * such as an airborne aircraft or balloon based observatory.
 *
 * @param geodetic    the momentary geodetic location of the observer.
 * @param itrs_vel    the momentary velocity of the observer with respect to the surface
 *                    (in ITRS).
 * @param eop         Earth Orientation Parameters (EOP) appropriate around the time of
 *                    observation, such as obtained from the IERS bulletins or data service.
 * @return            a new observer instance for the given moving observer.
 *
 * @sa on_earth(), in_earth_orbit(), in_solar_system(), at_geocenter(), at_ssb()
 */
GeodeticObserver Observer::moving_on_earth(const Site& geodetic, const Velocity& itrs_vel, const EOP& eop) {
  GeodeticObserver o = GeodeticObserver(geodetic, itrs_vel, eop);
  if(!o.is_valid())
    novas_trace_invalid("Observer::on_earth()");
  return o;
}

/**
 * Instantiates a new observer that is moving relative to Earth's surface, such as an airborne
 * observer.
 *
 * @param site          the momentary geodetic location of the observer.
 * @param eop           Earth Orientation Parameters (EOP) appropriate around the time of
 *                      observation.
 * @param horizontal    momentary horizontal speed of moving observer.
 * @param direction     azimuthal direction of motion (from North, measured to the East).
 * @param vertical      (optional) momentary vertical speed of observer (default: 0).
 *
 * @sa Site::enu_to_itrf()
 */
GeodeticObserver Observer::moving_on_earth(const Site& site, const EOP& eop, const ScalarVelocity& horizontal, const Angle& direction, const ScalarVelocity& vertical) {
  GeodeticObserver o = GeodeticObserver(site, eop, horizontal, direction, vertical);
  if(!o.is_valid())
    novas_trace_invalid("Observer::on_earth()");
  return o;
}

/**
 * Returns a new observer orbiting the Earth.
 *
 * @param pos       momentary position of the observer relative to the geocenter.
 * @param vel       momentary velocity of the observer relative to the geocenter.
 * @return          a new observer instance for the observer in Earth orbit.
 *
 * @sa on_earth(), in_solar_system(), at_geocenter(), at_ssb()
 */
GeocentricObserver Observer::in_earth_orbit(const Position& pos, const Velocity& vel) {
  GeocentricObserver o = GeocentricObserver(pos, vel);
  if(!o.is_valid())
    novas_trace_invalid("Observer::in_earth_orbit()");
  return o;
}

/**
 * Returns a fictitious observer placed at the location of the geocenter.
 *
 * @return         a new fictitious observer located at the geocenter.
 *
 * @sa on_earth(), in_earth_orbit(), in_solar_system(), at_ssb()
 */
GeocentricObserver Observer::at_geocenter() {
  return GeocentricObserver();
}

/**
 * Returns a new observer in some Solar-system location.
 *
 * @param pos     momentary position of the observer relative to the Solar-system Barycenter
 *                (SSB).
 * @param vel     momentary velocity of the observer relative to the Solar-system Barycenter
 *                (SSB).
 * @return        a new observer instance for the given Solar-system location.
 *
 * @sa at_ssb(), at_geocenter(), on_earth(), in_earth_orbit()
 */
SolarSystemObserver Observer::in_solar_system(const Position& pos, const Velocity& vel) {
  SolarSystemObserver o = SolarSystemObserver(pos, vel);
  if(!o.is_valid())
    novas_trace_invalid("Observer::in_solar_system()");
  return o;
}

/**
 * Returns a fictitious observer placed at the location of the Solar-System Barycenter (SSB).
 *
 * @return        a new fictitious observer located at the Solar-System Barycenter (SSB).
 *
 * @sa in_solar_system(), at_geocenter(), on_earth(), in_earth_orbit()
 */
SolarSystemObserver Observer::at_ssb() {
  return SolarSystemObserver();
}

/**
 * Returns a reference to a statically defined standard invalid observer. This invalid
 * observer may be used inside any object that is invalid itself.
 *
 * @return    a reference to a static standard invalid observer.
 */
const Observer &Observer::undefined() {
  static const Observer _invalid = Observer();
  return _invalid;
}

/**
 * Instantiates a new observer located at the geocenter.
 *
 */
GeocentricObserver::GeocentricObserver()
: Observer(NOVAS_OBSERVER_AT_GEOCENTER) {
  _valid = true;
}

/**
 * Instantiates a new observer located (and moving) relative to the geocenter.
 *
 * @param pos       momentary ICRS position of the observer relative to the geocenter.
 * @param vel       momentary ICRS velocity of the observer relative to the geocenter.
 */
GeocentricObserver::GeocentricObserver(const Position& pos, const Velocity& vel)
: Observer(NOVAS_OBSERVER_IN_EARTH_ORBIT, Site::undefined(), pos, vel) {
  static const char *fn = "GeocentricObserver()";

  if(!pos.is_valid())
    novas_set_errno(EINVAL, fn, "input position contains NAN component(s)");
  else if(!vel.is_valid())
    novas_set_errno(EINVAL, fn, "input velocity contains NAN component(s)");
  else
    _valid = true;
}

/**
 * Returns a pointer to a newly allocated copy of this geocentric observer instance.
 *
 * @return    pointer to new copy of this geocentric observer instance.
 */
const Observer *GeocentricObserver::copy() const {
  return new GeocentricObserver(*this);
}

bool GeocentricObserver::is_geocentric() const {
  return true;
}

/**
 * Returns the momentary geocentric position of this observer in the ICRS.
 *
 * @return    the momentary ICRS geocentric position
 *
 * @sa geocentric_velocity()
 */
Position GeocentricObserver::geocentric_position() const {
  Position pos(_observer.near_earth.sc_pos, Unit::km);
  if(!pos.is_valid())
    novas_trace_invalid("GeocentricObserver::geocentric_position()");
  return pos;
}

/**
 * Returns the momentary geocentric velocity of this observer in the ICRS.
 *
 * @return    the momentary ICRS geocentric velocity
 *
 * @sa geocentric_position()
 */
Velocity GeocentricObserver::geocentric_velocity() const {
  Velocity vel(_observer.near_earth.sc_vel, Unit::km / Unit::sec);
  if(!vel.is_valid())
    novas_trace_invalid("GeocentricObserver::geocentric_velocity()");
  return vel;
}

/**
 * Returns a string representation of this geocentric observer.
 *
 * @return    a string representation of this observer.
 */
std::string GeocentricObserver::to_string() const {
  Position pos = geocentric_position();
  Velocity vel = geocentric_velocity();

  std::string p = (pos.is_zero()) ? "" : " at " + pos.to_string();
  std::string v = (vel.is_zero()) ? "" : " moving at " + vel.to_string();

  return "Geocentric Observer" + p + v;
}

/**
 * Instantiates a new stationary observer located at the Solar-System Barycenter (SSB).
 *
 */
SolarSystemObserver::SolarSystemObserver()
: SolarSystemObserver(Position::origin(), Velocity::stationary()) {
  const double zero[3] = {0.0};
  make_solar_system_observer(zero, zero, &_observer);
  _valid = true;
}

/**
 * Instantiates a new observer in the Solar System.
 *
 * @param pos     momentary ICRS position of the observer relative to the Solar-system Barycenter
 *                (SSB).
 * @param vel     momentary ICRS velocity of the observer relative to the Solar-system Barycenter
 *                (SSB).
 */
SolarSystemObserver::SolarSystemObserver(const Position& pos, const Velocity& vel)
: Observer(NOVAS_SOLAR_SYSTEM_OBSERVER, Site::undefined(), pos, vel) {
  static const char *fn = "SolarSystemObserver()";

  make_solar_system_observer(pos.scaled(1.0 / Unit::au)._array(), vel.scaled(Unit::day / Unit::au)._array(), &_observer);

  if(!pos.is_valid())
    novas_set_errno(EINVAL, fn, "input position contains NAN component(s)");
  else if(!vel.is_valid())
    novas_set_errno(EINVAL, fn, "input velocity contains NAN component(s)");
  else
    _valid = true;
}

/**
 * Returns a pointer to a newly allocated copy of this observer instance at a Solar-system location.
 *
 * @return    pointer to new copy of thus Solar-system based observer instance.
 */
const Observer *SolarSystemObserver::copy() const {
  return new SolarSystemObserver(*this);
}

/**
 * Returns the momentary location of this observer relative to the Solar-System
 * Barycenter (SSB).
 *
 * @return      the momentary position of the observer relative to hte SSB.
 *
 * @sa ssb_velocity()
 */
Position SolarSystemObserver::ssb_position() const {
  Position pos(_observer.near_earth.sc_pos, Unit::au);
  if(!pos.is_valid())
    novas_trace_invalid("SolarSystemObserver::ssb_position()");
  return pos;
}

/**
 * Returns the momentary velocity of this observer relative to the Solar-System
 * Barycenter (SSB).
 *
 * @return      the momentary velocity of the observer relative to hte SSB.
 *
 * @sa ssb_position()
 */
Velocity SolarSystemObserver::ssb_velocity() const {
  Velocity vel(_observer.near_earth.sc_vel, Unit::au / Unit::day);
  if(!vel.is_valid())
    novas_trace_invalid("SolarSystemObserver::ssb_velocity()");
  return vel;
}

std::string SolarSystemObserver::to_string() const {
  Position pos = ssb_position();
  Velocity vel = ssb_velocity();

  std::string p = (pos.is_zero()) ? "SSB" : pos.to_string();
  std::string v = (vel.is_zero()) ? "" : " moving at SSB " + vel.to_string();

  return "SolarSystemObserver at " + p + v;
}


/**
 * Instantiates a new observer at a fixed location on Earth.
 *
 * @param site    the observing site
 * @param eop     Earth Orientation Parameters (EOP) appropriate around the time of observation,
 *                such as obtained from the IERS bulletins or data service.
 */
GeodeticObserver::GeodeticObserver(const Site& site, const EOP& eop)
: Observer(NOVAS_OBSERVER_ON_EARTH, site, Position::origin(), Velocity::stationary()), _eop(eop) {
  static const char *fn = "GeodeticObserver()";

  if(!site.is_valid())
    novas_set_errno(EINVAL, fn, "input site is invalid");
  else if(!eop.is_valid())
    novas_set_errno(EINVAL, fn, "input EOP is invalid");
  else
    _valid = true;
}

/**
 * Instantiates a new observer that is moving relative to Earth's surface, such as an airborne
 * observer.
 *
 * @param site    the momentary geodetic location of the observer
 * @param vel     the momentaty velocity of the observer relative to Earth's surface (in ITRS),
 * @param eop     Earth Orientation Parameters (EOP) appropriatI am salivating at the thought of it. Tell Benjamie around the time of observation.
 *
 * @sa Site::enu_to_itrf()
 */
GeodeticObserver::GeodeticObserver(const Site& site, const Velocity& vel, const EOP& eop)
: Observer(NOVAS_AIRBORNE_OBSERVER, site, Position::origin(), vel), _eop(eop) {
  static const char *fn = "GeodeticObserver()";

  make_airborne_observer(site._on_surface(), vel.scaled(Unit::s / Unit::km)._array(), &_observer);

  if(!site.is_valid())
    novas_set_errno(EINVAL, fn, "input site is invalid");
  else if(!eop.is_valid())
    novas_set_errno(EINVAL, fn, "input EOP is invalid");
  else if(!vel.is_valid())
    novas_set_errno(EINVAL, fn, "input velocity contains NAN component(s)");
  else
    _valid = true;
}

/**
 * Instantiates a new observer that is moving relative to Earth's surface, such as an airborne
 * observer.
 *
 * @param site          the momentary geodetic location of the observer.
 * @param eop           Earth Orientation Parameters (EOP) appropriate around the time of
 *                      observation.
 * @param horizontal    momentary horizontal speed of moving observer.
 * @param direction     azimuthal direction of motion (from North, measured to the East).
 * @param vertical      (optional) momentary vertical speed of observer (default: 0).
 *
 * @sa Site::enu_to_itrf()
 */
GeodeticObserver::GeodeticObserver(const Site& site, const EOP& eop, const ScalarVelocity& horizontal, const Angle& direction, const ScalarVelocity& vertical)
: GeodeticObserver(site, Velocity::stationary(), eop) {
  static const char *fn = "GeodeticObserver()";

  if(_valid)
    errno = 0;

  if(!horizontal.is_valid())
    novas_set_errno(EINVAL, fn, "input horizontal speed is invalid");
  if(!direction.is_valid())
    novas_set_errno(EINVAL, fn, "input azimuthal direction of motion is invalid");
  if(!vertical.is_valid())
    novas_set_errno(EINVAL, fn, "input vertical speed is invalid");

  _valid &= (errno == 0);

  double v[3] = {0.0};
  v[0] = horizontal.km_per_s() * sin(direction.rad());
  v[1] = horizontal.km_per_s() * cos(direction.rad());
  v[2] = vertical.km_per_s();

  novas_enu_to_itrs(v, site.longitude().deg(), site.latitude().deg(), _observer.near_earth.sc_vel);
}

/**
 * Returns a pointer to a newly allocated copy of this geodetic (Earth-based) observer instance.
 *
 * @return    pointer to new copy of this geodetic (Earth-based) observer instance.
 */
const Observer *GeodeticObserver::copy() const {
  return new GeodeticObserver(*this);
}


bool GeodeticObserver::is_geodetic() const { return true; }

/**
 * Returns the fixed or momentary observing site for this observer.
 *
 * @return    the observing site (fixed or momentary).
 *
 * @sa velocity()
 */
Site GeodeticObserver::site() const {
  const on_surface *s = &_observer.on_surf;

  Site site(s->longitude * Unit::deg, s->latitude * Unit::deg, s->height);
  if(!site.is_valid())
    novas_trace_invalid("GeodeticObserver::site()");
  return site;
}

/**
 * Returns the surface velocity of a moving observer, such as an airborne or balloon borne
 * observatory.
 *
 * @return    the momentary ITRS surface velocity vector of the moving observer.
 *
 * @sa Site::itrs_to_enu()
 */
Velocity GeodeticObserver::itrs_velocity() const {
  Velocity vel(_observer.near_earth.sc_vel, Unit::km / Unit::s);
  if(!vel.is_valid())
    novas_trace_invalid("GeodeticObserver::itrs_velocity()");
  return vel;
}


/**
 * Returns the surface velocity of a moving observer, such as an airborne or balloon borne
 * observatory.
 *
 * @return    the momentary surface velocity vector of the moving observer in the East-North-Up
 *            (ENU) directions at the current location.
 *
 * @sa Site::itrs_to_enu()
 */
Velocity GeodeticObserver::enu_velocity() const {
  double v[3] = {0.0};
  novas_itrs_to_enu(_observer.near_earth.sc_vel, _observer.on_surf.longitude, _observer.on_surf.latitude, v);

  Velocity vel(v, Unit::km / Unit::s);
  if(!vel.is_valid())
    novas_trace_invalid("GeodeticObserver::enu_velocity()");
  return vel;
}

/**
 * Returns the geocentric geometric position and velocity vectors for this geodetic observer at the
 * specified time and accuracy.
 *
 * @param time        Astrometric time of observation
 * @param accuracy    (optional) NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY.
 * @return            The geometric geocentric position and velocity vectors of this observer
 *                    at the specified time.
 */
Geometric GeodeticObserver::geocentric_at(const Time &time, enum novas_accuracy accuracy) const {
  static const char *fn = "GeodeticObserver::geocentric_in()";

  double p[3] = {0.0}, v[3] = {0.0};
  if(geo_posvel(time.jd(), time.dUT1().seconds(), accuracy, &_observer, p, v) != 0) {
    novas_trace_invalid(fn);
    return Geometric::undefined();
  }

  Geometric g = Geometric(frame_at(time, accuracy), Position(p, Unit::AU), Velocity(v, Unit::AU / Unit::day), NOVAS_ICRS);
  if(!g.is_valid())
    novas_trace_invalid(fn);
  return g;
}

/**
 * Earth Orientation Parameters (EOP) appropriate around the time of observation.
 *
 * @return    the Earth Orientation Parameters (EOP) defined for this observer.
 */
const EOP& GeodeticObserver::eop() const {
  return _eop;
}

/**
 * Returns a string representation of this Earth-based observer location.
 *
 * @return    a new string that describes this observer.
 */
std::string GeodeticObserver::to_string() const {
  Velocity vel = enu_velocity();

  std::string v = (vel.is_zero()) ? "" : " moving at ENU " + vel.to_string();

  return "GeodeticObserver at " + site().to_string() + v;
}


} // namespace supernovas

