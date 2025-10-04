/**
 * @file
 *
 * @date Created  on Sep 29, 2025
 * @author Attila Kovacs
 * @version 0.1.0
 *
 *  !!! Under construction !!!
 */

#ifndef INCLUDE_SUPERMOVAS_H_
#define INCLUDE_SUPERMOVAS_H_

#define SUPERNOVAS_CPP_API_VERSION    0.1.0   ///< C++ API version (different from library version)

#if __cplusplus

#include <string>
#include <time.h>
#include <errno.h>

#define NOVAS_NAMESPACE                       /// Make C API available under the 'novas' namespace
#include <novas.h>

namespace supernovas {


// Forward class declarations.
class Unit;
class Constant;
class Vector;
class   Position;
class   Velocity;
class System;
class Distance;
class Interval;
class Angle;
class   TimeAngle;
class Speed;
class Spherical;
class   Horizontal;
class   Equatorial;
class   Ecliptic;
class   Galactic;
class EOP;
class Time;
class Temperature;
class Pressure;
class Weather;
class Site;
class Observer;
class   GeodeticObserver;
class   GeocentricObserver;
class   SolarSystemObserver;
class CatalogEntry;
class Source;
class   CatalogSource;
class   SolarSystemSource;
class     Planet;
class     EphemerisSource;
class     OrbitalSource;
class Frame;
class Apparent;
class Geometric;

/// \ingroup util
class Unit {
private:
  // Private Constructor
  Unit() {}

  //Unit(Unit const&);            // Don't Implement
  void operator=(Unit const&);    // Don't implement

public:
  // Deleting the copy constructor to prevent copies
  Unit(const Unit& obj) = delete;
  //Unit(Unit const&)     = delete;

  static constexpr double au = NOVAS_AU;
  static constexpr double m = 1.0;
  static constexpr double cm = 0.01;
  static constexpr double mm = 1e-3;
  static constexpr double um = 1e-6;
  static constexpr double micron = um;
  static constexpr double nm = 1e-9;
  static constexpr double angstrom = 1e-10;
  static constexpr double km = NOVAS_KM;
  static constexpr double pc = NOVAS_PARSEC;
  static constexpr double kpc = 1000.0 * pc;
  static constexpr double Mpc = 1e6 * pc;
  static constexpr double Gpc = 1e9 * pc;
  static constexpr double lyr = NOVAS_LIGHT_YEAR;

  static constexpr double ns = 1e-9;
  static constexpr double us = 1e-6;
  static constexpr double ms = 1e-3;
  static constexpr double sec = 1.0;
  static constexpr double min = 60.0;
  static constexpr double hour = 3600.0;
  static constexpr double day = NOVAS_DAY;
  static constexpr double week = 7 * day;
  static constexpr double yr = NOVAS_TROPICAL_YEAR_DAYS * NOVAS_DAY;
  static constexpr double cy = 100.0 * yr;
  static constexpr double julianYear = NOVAS_JULIAN_YEAR_DAYS * NOVAS_DAY;
  static constexpr double julianCentury = NOVAS_JULIAN_YEAR_DAYS * NOVAS_DAY;

  static constexpr double rad = 1.0;
  static constexpr double hourAngle = NOVAS_HOURANGLE;
  static constexpr double deg = NOVAS_DEGREE;
  static constexpr double arcmin = deg / 60.0;
  static constexpr double arcsec = NOVAS_ARCSEC;
  static constexpr double mas = 1e-3 * arcsec;
  static constexpr double uas = 1e-6 * arcsec;

  static constexpr double Pa = 1.0;
  static constexpr double hPa = 100.0;
  static constexpr double mbar = hPa;
  static constexpr double bar = 1000.0 * mbar;
  static constexpr double kPa = 1000.0;
  static constexpr double MPa = 1e6;
  static constexpr double torr = 133.3223684211;
  static constexpr double atm = 101325.0;

  static constexpr double R_earth = NOVAS_GRS80_RADIUS;
};

/// \ingroup util
class Constant {
private:
  // Private Constructor
  Constant() {}

  //Constant(Constant const&);        // Don't Implement
  void operator=(Constant const&);    // Don't implement

public:
  // Deleting the copy constructor to prevent copies
  Constant(const Constant& obj) = delete;
  //Constant(Constant const&)     = delete;

  static constexpr double pi = M_PI;
  static constexpr double twoPi = TWOPI;

  static constexpr double c = NOVAS_C;                  ///< [m/s] speed of light
  static constexpr double G = 6.67428e-1;               ///< [m<sup>3</sup> kg<sup>-1</sup> s<sup>-2</sup>]

  static constexpr double L_B = 1.550519768e-8;
  static constexpr double L_G = 6.969290134e-10;

  static constexpr double F_earth = NOVAS_GRS80_FLATTENING;
  static constexpr double GM_sun = NOVAS_G_SUN;	        ///< [m<sup>3</sup> s<sup>-2</sup>] Solar graviational constant
  static constexpr double GM_earth = NOVAS_G_EARTH;     ///< [m<sup>3</sup> s<sup>-2</sup>] Earth graviational constant
  static constexpr double M_sun = GM_sun / G;           ///< [kg] Mass of the Sun
  static constexpr double M_earth = GM_sun / G;         ///< [kg] Earth mass
};

/// \ingroup equatorial nonequatorial
class System {
protected:
  std::string _name;
  double _jd;

public:
  System(const std::string& name);

  System(double jd_tt);

  double jd() const;

  double epoch() const;

  const std::string& name() const;

  std::string str() const;

  static System true_of_date(double jd_tt);

  static const System& icrs();

  static const System& j2000();

  static const System& hip();

  static const System& b1950();

  static const System& b1900();
};

/// \ingroup source apparent
class Distance {
private:
  double _meters;

public:
  Distance(double x);

  double m() const;

  double km() const;

  double au() const;

  double lyr() const;

  double pc() const;

  double kpc() const;

  double Mpc() const;

  double Gpc() const;

  Angle parallax() const;

  std::string str() const;

  static Distance from_parallax(double parallax);

  static const Distance& at_Gpc();
};

/// \ingroup time
class Interval {
private:

  double _seconds;
  enum novas::novas_timescale _scale;

public:

  Interval(double seconds, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Interval operator+(const Interval& r) const;

  Interval operator-(const Interval& r) const;

  TimeAngle operator+(const TimeAngle& base) const;

  enum novas::novas_timescale timescale() const;

  double milliseconds() const;

  double seconds() const;

  double minutes() const;

  double hours() const;

  double days() const;

  double years() const;

  double julian_years() const;

  double julian_centuries() const;

};

/// \ingroup util
class Angle {
protected:
  double _rad;

  Angle();

public:

  Angle(double x);

  Angle(const std::string& str);

  Angle operator+(const Angle& r);

  Angle operator-(const Angle& r);

  double rad() const;

  double deg() const;

  double arcmin() const;

  double arcsec() const;

  double mas() const;

  double uas() const;

  double fraction() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

/// \ingroup time util
class TimeAngle : public Angle {
public:

  TimeAngle(double x);

  TimeAngle(const std::string& str);

  TimeAngle(const Angle& angle);

  TimeAngle operator+(const Interval& other);

  TimeAngle operator-(const Interval& other);

  double hours() const;

  double minutes() const;

  double seconds() const;

  std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

/// \ingroup util
class Vector {
protected:
  double _component[3];

  Vector(double x = 0.0, double y = 0.0, double z = 0.0);

  Vector(const double v[3]);

public:

  virtual ~Vector() {}; // something virtual to make class polymorphic for dynamic casting.

  Vector operator*(double factor) const;

  Vector scaled(double factor) const;

  const double *_array() const;

  double abs() const;

  double dot(const Vector& v) const;

  double projection_on(const Vector& v) const;
};

/// \ingroup geometric
class Position : public Vector {
public:

  Position(double x = 0.0, double y = 0.0, double z = 0.0);

  Position(const double pos[3], double unit = 1.0);

  Position operator+(const Position &r) const;

  Position operator-(const Position &r) const;

  double x_m() const;

  double y_m() const;

  double z_m() const;

  Distance distance() const;

  Position inv() const;

  Spherical as_spherical() const;

  std::string str() const;

  static const Position& origin();
};

/// \ingroup geometric
class Velocity : public Vector {
public:
  Velocity(double x = 0.0, double y = 0.0, double z = 0.0);

  Velocity(const double vel[3], double unit = 1.0);

  Velocity operator+(const Velocity& r) const;

  Velocity operator-(const Velocity& r) const;

  Speed speed() const;

  double x_ms() const;

  double y_ms() const;

  double z_ms() const;

  Velocity inv() const;

  Speed along(Vector v) const;

  Position travel(const Interval& t) const;

  Position travel(double seconds) const;

  std::string str() const;

  static const Velocity& stationary();
};

/// \ingroup source spectral apparent
class Speed {
protected:
  double _ms;

public:
  Speed(double ms);

  Speed(const Distance d, const Interval& time);

  Speed operator+(const Speed& r) const;

  Speed operator-(const Speed& r) const;

  double ms() const;

  double kms() const;

  double auday() const;

  double beta() const;

  double Gamma() const;

  double redshift() const;

  Distance travel(double seconds) const;

  Distance travel(Interval& time) const;

  Velocity to_velocity(const Vector& direction) const;

  std::string str() const;

  static Speed from_redshift(double z);

  static const Speed& stationary();
};

/// \ingroup util
class Spherical {
protected:
  Angle _lon, _lat;
  Distance _distance;

  Spherical();

public:
  virtual ~Spherical() {}; // something virtual to make class polymorphic for dynamic casting.

  Spherical(double longitude, double latitude, double distance = NOVAS_DEFAULT_DISTANCE);

  Spherical(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  Spherical(const Position& pos);

  Position xyz() const;

  const Angle& longitude() const;

  const Angle& latitude() const;

  const Distance& distance() const;

  virtual const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

/// \ingroup equatorial
class Equatorial : public Spherical {
private:
  System _sys;

public:
  Equatorial(double ra, double dec, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE);

  Equatorial(const Angle& ra, const Angle& dec, const System& system = System::icrs(), const Distance& distance = Distance::at_Gpc());

  Equatorial(const Position& pos, const System& system = System::icrs());

  TimeAngle ra() const;

  const Angle& dec() const;

  const System& system() const;

  Ecliptic as_ecliptic() const;

  Galactic as_galactic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;
};

/// \ingroup nonequatorial
class Ecliptic : public Spherical {
private:
  System _sys;

public:
  Ecliptic(double longitude, double latitude, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE);

  Ecliptic(const Angle& ra, const Angle& dec, const System &system = System::icrs(), const Distance& distance = Distance::at_Gpc());

  Ecliptic(const Position& pos, const System& system = System::icrs());

  const System& system() const;

  Equatorial as_equatorial() const;

  Galactic as_galactic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;
};

/// \ingroup nonequatorial
class Galactic : public Spherical {
public:
  Galactic(double longitude, double latitude, double distance = NOVAS_DEFAULT_DISTANCE);

  Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  Galactic(const Position& pos, const Distance& distance = Distance::at_Gpc());

  Equatorial as_equatorial() const;

  Ecliptic as_ecliptic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;
};

/// \ingroup util
class Temperature {
private:
  double _deg_C;

  Temperature(double deg_C) : _deg_C(deg_C) {}

public:
  double celsius() const;

  double kelvin() const;

  double farenheit() const;

  std::string str() const;

  static Temperature celsius(double value);

  static Temperature kelvin(double value);

  static Temperature farenheit(double value);
};

/// \ingroup util
class Pressure {
private:
  double _pascal;

  Pressure(double value);

public:
  double Pa() const;

  double hPa() const;

  double kPa() const;

  double mbar() const;

  double bar() const;

  double torr() const;

  double atm() const;

  std::string str() const;

  static Pressure Pa(double value);

  static Pressure hPa(double value);

  static Pressure kPa(double value);

  static Pressure mbar(double value);

  static Pressure bar(double value);

  static Pressure torr(double value);

  static Pressure atm(double value);
};

/// \ingroup refract
class Weather {
private:
  Temperature _temperature;
  Pressure _pressure;
  double _humidity;

public:
  Weather(const Temperature& T, const Pressure& p, double humidity_percent);

  Weather(double celsius, double pascal, double humidity_percent);

  const Temperature& temperature() const;

  const Pressure& pressure() const;

  double humidity() const;

  double humidity_fraction() const;

  std::string str() const;

  static Weather guess(const Site& site);
};

/// \ingroup earth nonequatorial
class EOP {
private:
  int _leap;
  Angle _xp, _yp;
  double _t;
  double _dxp = 0.0, _dyp = 0.0, _dt = 0.0;  // [arcsec, s] Applied corrections, in novas units.

public:
  EOP(int leap_seconds, double dut1 = 0.0, double xp = 0.0, double yp = 0.0);

  EOP(int leap_seconds, double dut1, const Angle& xp, const Angle& yp);

  int leap_seconds() const;

  const Angle& xp() const;

  const Angle& yp() const;

  double dUT1() const;

  EOP itrf_transformed(int from_year, int to_year) const;

  EOP diurnal_corrected(const Time& time) const;

  std::string str() const;
};

/// \ingroup observer
class Site {
private:
  novas::on_surface _site;

  Site();

public:

  Site(double longitude, double latitude, double altitude=0.0, enum novas::novas_reference_ellipsoid ellipsoid = novas::NOVAS_GRS80_ELLIPSOID);

  Site(const Position& xyz);

  const novas::on_surface *_on_surface() const;

  const Angle longitude() const;

  const Angle latitude() const;

  const Distance altitude() const;

  const Position xyz(enum novas::novas_reference_ellipsoid ellipsoid = novas::NOVAS_GRS80_ELLIPSOID) const;

  Site itrf_transformed(int from_year, int to_year) const;

  Position xyz() const;

  std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;

  static Site from_GPS(double longitude, double latitude, double altitude = 0.0);

  static Site from_xyz(const Position& xyz);

  static Site from_xyz(double x, double y, double z);

};

/// \ingroup observer
class Observer {
protected:
  novas::observer _observer;

  Observer() {};

public:

  virtual ~Observer() {}; // something virtual to make class polymorphic for dynamic casting.

  const novas::observer * _novas_observer() const;

  enum novas::novas_observer_place type() const;

  virtual bool is_geodetic() const;

  virtual bool is_geocentric() const;

  static GeodeticObserver on_earth(const Site& site, const EOP& eop);

  static GeodeticObserver on_earth(const Site& geodetic, const Velocity& vel, const EOP& eop);

  static GeocentricObserver in_earth_orbit(const Position& pos, const Velocity& vel);

  static GeocentricObserver at_geocenter();

  static SolarSystemObserver in_solar_system(const Position& pos, const Velocity& vel);

  static SolarSystemObserver at_ssb();
};

/// \ingroup observer
class GeocentricObserver : public Observer {
public:
  GeocentricObserver();

  GeocentricObserver(const Position& pos, const Velocity& vel);

  bool is_geocentric() const override;

  Position geocetric_position() const;

  Velocity geocentric_velocity() const;
};

/// \ingroup observer
class SolarSystemObserver : public Observer {
public:

  SolarSystemObserver();

  SolarSystemObserver(const Position& pos, const Velocity& vel);

  Position ssb_position() const;

  Velocity ssb_velocity() const;
};

/// \ingroup observer
class GeodeticObserver : public Observer {
private:
  EOP _eop;

public:
  GeodeticObserver(const Site& site, const EOP& eop);

  GeodeticObserver(const Site& site, const Velocity& vel, const EOP& eop);

  bool is_geodetic() const override;

  Site site() const;

  const EOP& eop() const;
};


/// \ingroup time
class Time {
protected:
  novas::novas_timespec _ts;

  Time() {};

public:

  Time(double jd, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(long ijd, double fjd, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(const std::string& timestamp, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_UTC);

  Time(const struct timespec *t, const EOP& eop);

  Time(const novas::novas_timespec *t);

  Interval operator-(const Time &other) const;

  const novas::novas_timespec * _novas_timespec() const;

  double jd(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  TimeAngle time_of_day(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  int day_of_week(enum novas::novas_timescale timescale) const;

  double epoch() const;

  TimeAngle gst(enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;

  TimeAngle lst(const Site& site, enum novas::novas_accuracy accuracy) const;

  TimeAngle era() const;

  std::string str(enum novas::novas_timescale timescale = novas::NOVAS_UTC) const;

  std::string iso_str() const;

  std::string epoch_str() const;

  Time shifted(double seconds) const;

  Time shifted(Interval offset) const;

  static Time now(const EOP& eop);
};

/// \ingroup frame
class Frame {
private:
  novas::novas_frame _frame;
  Observer _observer;
  Time _time;

public:
  Frame(const Observer& obs, const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY);

  const novas::novas_frame *_novas_frame() const;

  const Observer& observer() const;

  const Time& time() const;

  enum novas::novas_accuracy accuracy() const;

  Apparent approx_apparent(const Planet& planet, enum novas::novas_reference_system system = novas::NOVAS_TOD) const;

  double clock_skew(enum novas::novas_timescale = novas::NOVAS_TT) const;
};


/// \ingroup source
class Source {
protected:
  struct novas::novas_object _object;

  Source() {}

public:
  virtual ~Source() {}; // something virtual to make class polymorphic for dynamic casting.

  const struct novas::novas_object *_novas_object() const;

  std::string name() const;

  Apparent apparent(const Frame &frame, enum novas::novas_reference_system system = novas::NOVAS_TOD) const;

  Geometric geometric(const Frame &frame, enum novas::novas_reference_system system = novas::NOVAS_TOD) const;

  Angle sun_angle(const Frame &frame) const;

  Angle moon_angle(const Frame &frame) const;

  Angle angle_to(const Source& source, const Frame& frame) const;

  Time rises_above(double el, const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  Time transits(const Frame &frame) const;

  Time sets_below(double el, const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  static void set_case_sensitive(bool value);
};

/// \ingroup source spectral
class CatalogEntry {
private:
  double _epoch;
  novas::cat_entry _entry;
  System _sys;

  void set_epoch();

public:
  CatalogEntry(const std::string &name, double RA, double Dec, const std::string& system = "ICRS");

  CatalogEntry(const std::string &name, const Angle& RA, const Angle& Dec, const System& system = System::icrs());

  CatalogEntry(const novas::cat_entry *e, const std::string& system = "ICRS");

  const novas::cat_entry* _cat_entry() const;

  const System& system() const;

  std::string name() const;

  long number() const;

  Angle ra() const;

  Angle dec() const;

  Speed v_lsr() const;

  Speed radial_velocity() const;

  Distance distance() const;

  Angle parallax() const;

  Equatorial equatorial() const;

  CatalogEntry& proper_motion(double ra, double dec);

  CatalogEntry& parallax(double angle);

  CatalogEntry& parallax(const Angle& angle);

  CatalogEntry& distance(double dist);

  CatalogEntry& distance(const Distance& dist);

  CatalogEntry& v_lsr(double v);

  CatalogEntry& v_lsr(const Speed& v);

  CatalogEntry& radial_velocity(double v);

  CatalogEntry& radial_velocity(const Speed& v);

  CatalogEntry& redshift(double z);

  CatalogEntry& catalog(const std::string& name, long number);
};

/// \ingroup source
class CatalogSource : public Source {
private:
  System _system;

public:
  CatalogSource(const CatalogEntry& e);

  const novas::cat_entry * _cat_entry() const;

  CatalogEntry catalog_entry() const;
};

/// \ingroup source
class SolarSystemSource : public Source {
protected:
  SolarSystemSource() {}

public:
  double solar_illumination(const Frame& frame) const;

  double helio_distance(const Time& time, double *rate = NULL) const;

  double solar_power(const Time& time) const;
};

/// \ingroup source
class Planet : public SolarSystemSource {
public:
  Planet(enum novas::novas_planet number);

  Planet(const std::string& name);

  enum novas::novas_planet novas_id() const;

  int naif_id() const;

  int de_number() const;

  double mean_radius() const;

  double mass() const;

  static const Planet& ssb();

  static const Planet& mercury();

  static const Planet& venus();

  static const Planet& earth();

  static const Planet& mars();

  static const Planet& jupiter();

  static const Planet& saturn();

  static const Planet& uranus();

  static const Planet& neptune();

  static const Planet& pluto();

  static const Planet& sun();

  static const Planet& moon();

  static const Planet& emb();

  static const Planet& pluto_system();

};

/// \ingroup source
class EphemerisSource : public SolarSystemSource {
public:
  EphemerisSource(const std::string &name, long number);

  long number() const;
};

/// \ingroup source
class OrbitalSource : public SolarSystemSource {
public:
  OrbitalSource(const std::string& name, long number, const novas::novas_orbital *orbit);

  const novas::novas_orbital *_novas_orbital() const;

  Position orbital_position(const Time& time) const; // TODO

  Velocity orbital_velocity(const Time& time) const; // TODO
};

/// \ingroup apparent spectral
class Apparent {
private:
  Frame _frame;
  enum novas::novas_reference_system _sys;

  novas::sky_pos _pos;

  Apparent(const Frame& frame, enum novas::novas_reference_system system);

public:
  Apparent(const Frame& frame, const Equatorial& eq, double rv = 0.0, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  Apparent(const Frame& frame, const Equatorial& eq, const Speed& rv, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  Apparent(const Frame& frame, const novas::sky_pos *p, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  const novas::sky_pos *_sky_pos() const;

  const Frame& frame() const;

  enum novas::novas_reference_system system() const;

  Angle ra() const;

  Angle dec() const;

  Speed radial_velocity() const;

  Distance distance() const;

  Equatorial equatorial() const;

  Ecliptic ecliptic() const;

  Galactic galactic() const;

  Horizontal horizontal() const;

  Apparent to_system(enum novas::novas_reference_system system) const;
};

/// \ingroup geometric
class Geometric {
private:
  Frame _frame;
  enum novas::novas_reference_system _sys;

  Position _pos;
  Velocity _vel;

public:
  Geometric(const Frame& frame, enum novas::novas_reference_system system, const Position& p, const Velocity& v);

  const Frame& frame() const;

  enum novas::novas_reference_system system() const;

  const Position& position() const;

  const Velocity& velocity() const;

  Equatorial equatorial() const;

  Ecliptic ecliptic() const;

  Galactic galactic() const;

  Geometric to_system(enum novas::novas_reference_system system) const;

};

/// \ingroup nonequatorial refract
class Horizontal : public Spherical {
private:

  int location_with_weather(const Frame& frame, const Weather& weather, novas::on_surface *s);

public:
  Horizontal(double azimuth, double elevation, double distance = NOVAS_DEFAULT_DISTANCE);

  Horizontal(const Angle& azimuth, const Angle& elevation, const Distance& distance = Distance::at_Gpc());

  const Angle& azimuth() const;

  const Angle& elevation() const;

  Horizontal to_refracted(const Frame &frame, novas::RefractionModel ref, const Weather& weather);

  Horizontal to_unrefracted(const Frame &frame, novas::RefractionModel ref, const Weather& weather);

  Apparent to_apparent(const Frame& frame, double rv = 0.0, double distance = NOVAS_DEFAULT_DISTANCE) const;

  Apparent to_apparent(const Frame& frame, const Speed& rv = Speed::stationary(), const Distance& distance = Distance::at_Gpc()) const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;
};



} // namespace supernovas

#  endif /* cplusplus */
#endif /* INCLUDE_SUPERMOVAS_H_ */
