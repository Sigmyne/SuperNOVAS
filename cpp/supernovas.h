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

#define SUPERNOVAS_CPP_API_VERSION    0.1.0

#  if __cplusplus

#include <string>
#include <errno.h>

extern "C" {
#  include <novas.h>
}

#define NOVAS_DEFAULT_DISTANCE      (1e9 * NOVAS_PARSEC)

#include <time.h>

namespace supernovas {

}
class Unit;
class Constant;
template <typename Length> class Vector;
class System;
class Distance;
class Interval;
class Angle;
class TimeAngle;
class Position;
class Velocity;
class Speed;
class Spherical;
class Horizontal;
class Equatorial;
class Ecliptic;
class Galactic;
class EOP;
class Time;
class Temperature;
class Pressure;
class Weather;
class Site;
class Observer;
class EarthBasedObserver;
class CatalogEntry;
class Source;
class CatalogSource;
class SolarSystemSource;
class Planet;
class EphemerisSource;
class OrbitalSource;
class Frame;
class Apparent;
class Geometric;

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
  
  static constexpr double c = NOVAS_C;                  ///< [m/s] speed of light
  static constexpr double G = 6.67428e-1;               ///< [m<sup>3</sup> kg<sup>-1</sup> s<sup>-2</sup>]
  
  static constexpr double L_B = 1.550519768e-8;
  static constexpr double L_G = 6.969290134e-10;
  
  static constexpr double F_earth = NOVAS_GRS80_FLATTENING;
  static constexpr double GM_sun = NOVAS_G_SUN;	        ///< [[m<sup>3</sup> s<sup>-2</sup>] Solar graviational constant
  static constexpr double GM_earth = NOVAS_G_EARTH;     ///< [[m<sup>3</sup> s<sup>-2</sup>] Earth graviational constant
};


class System {
protected:
  std::string& _id;

public:
  System(const std::string& name);

  double jd() const;

  double epoch() const;

  std::string& name() const;

  std::string str() const;
};

class Distance {
protected:

  double _meters;

public:
  Distance(double x);

  double m() const;

  double km() const;

  double au() const;

  double lyr() const;

  double pc() const;

  Angle parallax() const;

  static Distance from_parallax(double parallax);

  std::string str();
};


class Interval {
protected:

  double _seconds;
  enum novas_timescale _scale;

public:

  Interval(double seconds, enum novas_timescale timescale = NOVAS_TT);

  Interval operator+(const Interval& r) const;

  Interval operator-(const Interval& r) const;

  TimeAngle operator+(const TimeAngle& base) const;

  enum novas_timescale timescale() const;

  double milliseconds() const;

  double seconds() const;

  double minutes() const;

  double hours() const;

  double days() const;

  double years() const;

  double julian_years() const;

  double julian_centuries() const;
};

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

  const std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

class TimeAngle {
public:

  TimeAngle(double x);

  TimeAngle(const std::string& str);

  TimeAngle(const Angle& angle);

  TimeAngle operator+(const Interval& other);

  TimeAngle operator-(const Interval& other);

  double hours() const;

  double minutes() const;

  double seconds() const;

  std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3);
};

template <typename Length>

class Vector {
protected:
  double _component[3];

public:

  Vector(double x = 0.0, double y = 0.0, double z = 0.0);

  Vector(const double v[3]);

  virtual ~Vector();

  Vector scaled(double factor) const;

  const double *_array() const;

  double abs() const;

  double dot(const Vector &v) const;

  virtual Length length() const;
};


class Position : public Vector<Distance> {
protected:

  Position cast(double u);

public:

  Position(double x = 0.0, double y = 0.0, double z = 0.0);

  Position(const double pos[3], double unit = 1.0);

  Position operator+(const Position &r) const;

  Position operator-(const Position &r) const;

  double x_m() const;

  double y_m() const;

  double z_m() const;

  Distance x() const;

  Distance y() const;

  Distance z() const;

  Distance length() const;

  Distance distance_to(const Position& pos) const;

  Position inv() const;

  Spherical as_spherical() const;

  static Position origin();
};


class Velocity : public Vector<Speed> {
protected:

  Velocity& cast(double u);

public:
  Velocity(double x = 0.0, double y = 0.0, double z = 0.0);

  Velocity(const double vel[3], double unit = 1.0);

  Velocity operator+(const Velocity& r) const;

  Velocity operator-(const Velocity& r) const;

  double x_ms() const;

  double y_ms() const;

  double z_ms() const;

  Speed x() const;

  Speed y() const;

  Speed z() const;

  Speed length() const;

  Speed along(const Vector v) const;

  Velocity& inv() const;

  static Velocity stationary();
};


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

  double redshift() const;

  Distance travel(double seconds) const;

  Distance travel(Interval& time) const;

  std::string str() const;

  template <typename T>
  Velocity to_velocity(const Vector<T>& direction) const;

  static Speed from_redshift(double z);
};

class Spherical {
protected:
  Angle _lon, _lat;
  Distance _distance;

  Spherical();

public:
  Spherical(double longitude, double latitude, double distance = NOVAS_DEFAULT_DISTANCE);

  Spherical(const Angle& longitude, const Angle& latitude, double distance = NOVAS_DEFAULT_DISTANCE);

  Spherical(const Angle& longitude, const Angle& latitude, const Distance& distance);

  Spherical(const Position& pos);

  Position xyz() const;

  const Angle& longitude() const;

  const Angle& latitude() const;

  const Distance& distance() const;

  const std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

class Horizontal : public Spherical {
public:
  Horizontal(double azimuth, double elevation, double distance = NOVAS_DEFAULT_DISTANCE);

  Horizontal(const Angle& azimuth, const Angle& elevation, double distance = NOVAS_DEFAULT_DISTANCE);

  Horizontal(const Angle& azimuth, const Angle& elevation, const Distance& distance);

  const Angle& azimuth() const;

  const Angle& elevation() const;

  Horizontal to_refracted(const Frame &frame, RefractionModel ref, const Weather& weather);

  Horizontal to_unrefracted(const Frame &frame, RefractionModel ref, const Weather& weather);

  Apparent to_apparent(const Frame& frame, double rv = 0.0, double distance = 0.0) const;

  Apparent to_apparent(const Frame& frame, Speed& rv, Distance& distance) const;
};


class Equatorial : public Spherical {
public:
  Equatorial(double ra, double dec, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE);

  Equatorial(const Angle& ra, const Angle& dec, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE);

  Equatorial(const Angle& ra, const Angle& dec, const System &system, const Distance& distance);

  Equatorial(const Position& pos, const std::string& system = "ICRS");

  Equatorial(const Position& pos, const System& system);

  TimeAngle ra() const;

  const Angle& dec() const;

  const System& system() const;

  Ecliptic as_ecliptic() const;

  Galactic as_galactic() const;
};

class Ecliptic : public Spherical {
public:
  Ecliptic(double longitude, double latitude, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE);

  Ecliptic(const Angle& longitude, const Angle& latitude, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE);

  Ecliptic(const Angle& ra, const Angle& dec, const System &system, const Distance& distance);

  Ecliptic(const Position& pos, const std::string& system = "ICRS");

  Ecliptic(const Position& pos, const System& system);

  const System& system() const;

  Equatorial as_equatorial() const;

  Galactic as_galactic() const;
};

class Galactic : public Spherical {
public:
  Galactic(double longitude, double latitude, double distance = NOVAS_DEFAULT_DISTANCE);

  Galactic(const Angle& longitude, const Angle& latitude, double distance = NOVAS_DEFAULT_DISTANCE);

  Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance);

  Galactic(const Position& pos);

  Equatorial as_equatorial() const;

  Ecliptic as_ecliptic() const;
};

class Temperature {
public:
  double celsius() const;

  double kelvin() const;

  double farenheit() const;

  std::string str() const;

  static Temperature celsius(double value);

  static Temperature kelvin(double value);

  static Temperature farenheit(double value);
};


class Pressure {
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

class Weather {
public:
  Weather(const Temperature& T, const Pressure& p, double humidity_percent);

  Weather(double celsius, double pascal, double humidity_percent);

  const Temperature& temperature() const;

  const Pressure& pressure() const;

  double humidity() const;

  double humidity_fraction() const;

  static Weather guess(const Site& site);
};

class Site {
public:

  Site(double longitude, double latitude, double altitude=0.0, enum novas_reference_ellipsoid ellipsoid = NOVAS_GRS80_ELLIPSOID);

  Site(const Position& xyz);

  const on_surface *_on_surface() const;

  const Angle longitude() const;

  const Angle latitude() const;

  const Distance altitude() const;

  const Position xyz(enum novas_reference_ellipsoid ellipsoid = NOVAS_GRS80_ELLIPSOID) const;

  Site itrf_transformed(int from_year, int to_year) const;

  Position xyz() const;

  std::string str(enum novas_separator_type separator = NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;

  static Site from_GPS(double longitude, double latitude, double altitude = 0.0);

  static Site from_xyz(const Position& xyz);

  static Site from_xyz(double x, double y, double z);

};

class Observer {
protected:
  observer _observer;

  Observer() {}

public:

  const observer * _novas_observer() const;

  enum novas_observer_place type();

  static Observer on_earth(const Site& site);

  static Observer on_earth(const Site& site, const Velocity& vel);

  static Observer in_earth_orbit(const Position& pos, const Velocity& vel);

  static Observer solar_system(const Position& pos, const Velocity& vel);

  static Observer at_geocenter();

  static Observer at_ssb();
};


class EarthBasedObserver : public Observer {
public:
  EarthBasedObserver(const Site& site);

  EarthBasedObserver(const Site& site, const Weather& weather);

  EarthBasedObserver(const Site& site, const Velocity& vel);

  EarthBasedObserver(const Site& site, const Velocity& vel, const Weather& weather);

  const Site site() const;
};

class EOP {

public:
  EOP(int leap_seconds, double dut1 = 0.0, double xp = 0.0, double yp = 0.0);

  EOP(int leap_seconds, double dut1, const Angle& xp, const Angle& yp);

  int leap_seconds() const;

  const Angle& xp() const;

  const Angle& yp() const;

  double dUT1() const;

  EOP itrf_transformed(int from_year, int to_year) const;

  EOP to_diurnal_corrected(const Time& time) const;

};


class Time {
protected:
  novas_timespec _ts;

  Time();

public:

  Time(double jd, const EOP& eop, enum novas_timescale timescale = NOVAS_TT);

  Time(const std::string& timestamp, const EOP& eop, enum novas_timescale timescale = NOVAS_UTC);

  Time(const struct timespec *t, const EOP& eop);

  Time(const novas_timespec *t);

  Interval operator-(const Time &other) const;

  const novas_timespec * _novas_timespec() const;

  double jd(enum novas_timescale timescale = NOVAS_TT) const;

  TimeAngle time_of_day(enum novas_timescale timescale = NOVAS_TT) const;

  double epoch() const;

  std::string str(enum novas_timescale timescale = NOVAS_UTC) const;

  std::string iso_str() const;

  std::string epoch_str() const;

  static Time now(const EOP& eop);
};


class CatalogEntry {
public:
  CatalogEntry(const std::string &name, double RA, double Dec, const std::string& system = "ICRS");

  CatalogEntry(const std::string &name, const Angle& RA, const Angle& Dec, const std::string& system = "ICRS");

  CatalogEntry(const cat_entry *e, const std::string& system = "ICRS");

  const cat_entry* _cat_entry() const;

  std::string system() const;

  std::string name() const;

  long number() const;

  Angle ra() const;

  Angle dec() const;

  Speed v_lsr() const;

  Speed radial_velocity() const;

  CatalogEntry& proper_motion(double ra, double dec);

  CatalogEntry& parallax(double angle);

  CatalogEntry& parallax(Angle& angle);

  CatalogEntry& distance(double dist);

  CatalogEntry& distance(Distance& dist);

  CatalogEntry& v_lsr(double v);

  CatalogEntry& v_lsr(Speed& v);

  CatalogEntry& radial_velocity(double v);

  CatalogEntry& radial_velocity(Speed& v);

  CatalogEntry& redshift(double z);

  CatalogEntry& catalog(std::string& name, long number);
};

class Source {
protected:
  struct novas_object _object;

  Source() {}

public:
  const struct novas_object *_novas_object() const;

  Apparent apparent(const Frame &frame, enum novas_reference_system system = NOVAS_TOD) const;

  Geometric geometric(const Frame &frame, enum novas_reference_system system = NOVAS_TOD) const;

  Time rises_above(double el, const Frame &frame, RefractionModel ref, const Weather& weather) const;

  Time transits(const Frame &frame) const;

  Time sets_below(double el, const Frame &frame, RefractionModel ref, const Weather& weather) const;

  Angle sun_angle(const Frame &frame) const;

  Angle moon_angle(const Frame &frame) const;

  Angle angle_from(const Source& source, const Frame& frame) const;
};

class CatalogSource : public Source {
public:
  CatalogSource(const CatalogEntry& e);

  const cat_entry * _cat_entry() const;

  const System& system() const;

  CatalogEntry catalog_entry() const;
};

class SolarSystemSource : public Source {
protected:
  SolarSystemSource();

public:
  double solar_illumination(const Frame& frame) const;

  double solar_distance(const Time& time, double *rate = NULL) const;

  double solar_power(const Time& time) const;
};


class Planet : public SolarSystemSource {
public:
  Planet(enum novas_planet number);

  Planet(const std::string& name);
};

class EphemerisSource : public SolarSystemSource {
public:
  EphemerisSource(const std::string &name, long number);
};

class OrbitalSource : public Source {
public:
  OrbitalSource(const std::string& name, long number, const novas_orbital *orbit);
};


class Apparent {
public:
  Apparent(const Frame& frame, Equatorial& eq, double rv = 0.0, enum novas_reference_system system = NOVAS_TOD);

  Apparent(const Frame& frame, Equatorial& eq, Speed& rv, enum novas_reference_system system = NOVAS_TOD);

  Apparent(const Frame& frame, const sky_pos *p, enum novas_reference_system system = NOVAS_TOD);

  Apparent(const Source& source, const Frame& frame, enum novas_reference_system system = NOVAS_TOD);

  const sky_pos *_sky_pos() const;

  const Frame& frame() const;

  Angle ra() const;

  Angle dec() const;

  Speed radial_velocity() const;

  Distance distance() const;

  Equatorial equatorial() const;

  Ecliptic ecliptic() const;

  Galactic galactic() const;

  Horizontal horizontal() const;

  Apparent to_system(enum novas_reference_system system) const;
};


class Geometric {


public:
  Geometric(const Frame& frame, enum novas_reference_system system, const Position& p, const Velocity& v);

  const Frame& frame();

  enum novas_reference_system system();

  const Position& position();

  const Velocity& velocity();

  Equatorial equatorial() const;

  Ecliptic ecliptic() const;

  Galactic galactic() const;

  Geometric to_system(enum novas_reference_system system) const;

};

class Frame {
public:
  Frame(const Observer& obs, const Time& time, const EOP& eop, enum novas_accuracy accuracy = NOVAS_FULL_ACCURACY);

  const novas_frame *_novas_frame() const;

  const Observer& observer() const;

  const Time& time() const;

  const EOP& eop() const;
};


#  endif /* cplusplus */

#endif /* INCLUDE_SUPERMOVAS_H_ */
