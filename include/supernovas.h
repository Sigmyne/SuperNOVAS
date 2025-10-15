/**
 * @file
 *
 * @date Created  on Sep 29, 2025
 * @author Attila Kovacs
 * @since 1.6
 * @version 0.2.0
 *
 *  !!! Under construction !!!
 */

#ifndef INCLUDE_SUPERMOVAS_H_
#define INCLUDE_SUPERMOVAS_H_

#define SUPERNOVAS_CPP_API_VERSION    0.2.0   ///< C++ API version (different from library version)

#if __cplusplus

#include <string>
#include <optional>
#include <time.h>
#include <errno.h>

#ifndef _CONSTS_
#  define _CONSTS_                              ///< Don't use the old NOVAS constants with names like C, AU
#endif

#ifndef NOVAS_NAMESPACE
#  define NOVAS_NAMESPACE                       ///< Make C API available under the 'novas' namespace
#endif
#include <novas.h>


namespace supernovas {


// Forward class declarations.
class Unit;
class Constant;
class Validating;
class Vector;
class   Position;
class   Velocity;
class EquatorialSystem;
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
class Calendar;
class CalendarDate;
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
class OrbitalSystem;
class OrbitalSource;
class Source;
class   CatalogSource;
class   SolarSystemSource;
class     Planet;
class     EphemerisSource;
class     OrbitalSource;
class Frame;
class Apparent;
class Geometric;
class Evolution;
class Track;
class   HorizontalTrack;
class   EquatorialTrack;

/**
 * Various physical units for converting quantities expressed in conventional units to SI, and
 * vice versa. The SuperNOVAS C++ API uses SI quantities consistently for inputs, so these units
 * can be useful for converting quantities in other units to SI, or to convert return values from
 * SI to conventional units.
 *
 * For example, if you have distance in AU, then you can use Unit::au to convert it to an SI
 * distance (i.e. meters) as:
 *
 * ```c
 *   double d = 2.4103 * Unit::au;
 * ```
 *
 * @sa Constant
 * @ingroup util
 */
class Unit {
private:
  // Private Constructor
  Unit() {}

  //Unit(Unit const&);            // Don't Implement
  void operator=(Unit const&);    // Don't implement

public:
  /// \cond PRIVATE
  // Deleting the copy constructor to prevent copies
  Unit(const Unit& obj) = delete;
  //Unit(Unit const&)     = delete;
  /// \endcond

  static constexpr double AU = NOVAS_AU;                  /// [m] 1 Astronomical Unit in meters.
  static constexpr double au = AU;                        /// [m] 1 AU (lowe-case)
  static constexpr double m = 1.0;                        /// [m] 1 meter (standard unit of distance)
  static constexpr double cm = 0.01;                      /// [m] 1 centimeter in meters
  static constexpr double mm = 1e-3;                      /// [m] 1 millimeter in meters
  static constexpr double um = 1e-6;                      /// [m] 1 micrometer (=micron) in meters
  static constexpr double micron = um;                    /// [m] 1 micron (=micrometer) in meters, by another name.
  static constexpr double nm = 1e-9;                      /// [m] 1 nanometer in meters
  static constexpr double angstrom = 1e-10;               /// [m] 1 Angstrom in meters
  static constexpr double km = NOVAS_KM;                  /// [m] 1 kilometer in meters
  static constexpr double pc = NOVAS_PARSEC;              /// [m] 1 parsec in meters
  static constexpr double kpc = 1000.0 * pc;              /// [m] 1 kiloparsec in meters
  static constexpr double Mpc = 1e6 * pc;                 /// [m] 1 megaparsec in meters
  static constexpr double Gpc = 1e9 * pc;                 /// [m] 1 gigaparsec in meters
  static constexpr double lyr = NOVAS_LIGHT_YEAR;         /// [m] 1 light-year in meters

  static constexpr double ns = 1e-9;                      /// [s] 1 nanoseconds in seconds
  static constexpr double us = 1e-6;                      /// [s] 1 microsecond in seconds
  static constexpr double ms = 1e-3;                      /// [s] 1 millisecond in seconds
  static constexpr double s = 1.0;                        /// [s] 1 second (standard unit of time)
  static constexpr double sec = s;                        /// [s] 1 second by another name
  static constexpr double min = 60.0;                     /// [s] 1 minute in seconds
  static constexpr double hour = 3600.0;                  /// [s] 1 hour in seconds
  static constexpr double day = NOVAS_DAY;                /// [s] 1 day in seconds
  static constexpr double week = 7 * day;                 /// [s] 1 week in seconds

  /// [s] 1 tropical calendar year in seconds (at J2000)
  static constexpr double yr = NOVAS_TROPICAL_YEAR_DAYS * day;
  /// [s] 1 tropical calendar century in seconds (at J2000)
  static constexpr double cy = 100.0 * yr;
  /// [s] 1 Besselian year in seconds
  static constexpr double besselian_year = NOVAS_BESSELIAN_YEAR_DAYS * day;
  /// [s] 1 Julian year in seconds
  static constexpr double julian_year = NOVAS_JULIAN_YEAR_DAYS * day;
  /// [s] 1 Julian century in seconds
  static constexpr double julian_century = 100.0 * julian_year;

  static constexpr double rad = 1.0;                      /// [rad] 1 radian (standard unit of angle)
  static constexpr double hour_angle = NOVAS_HOURANGLE;   /// [rad] 1 hour of angle in radians on the 24h circle.
  static constexpr double deg = NOVAS_DEGREE;             /// [rad] 1 degree in radians
  static constexpr double arcmin = deg / 60.0;            /// [rad] 1 minute of arc in radians
  static constexpr double arcsec = NOVAS_ARCSEC;          /// [rad] 1 second or arc in radians
  static constexpr double mas = 1e-3 * arcsec;            /// [rad] 1 millisecond of arc in radians
  static constexpr double uas = 1e-6 * arcsec;            /// [rad] 1 microsecond of arc in radians

  static constexpr double Pa = 1.0;                       /// [Pa] 1 pascal (standard unit of perssure)
  static constexpr double hPa = 100.0;                    /// [Pa] 1 hectopascal in pascals
  static constexpr double mbar = hPa;                     /// [Pa] 1 millibar in pascals
  static constexpr double bar = 1000.0 * mbar;            /// [Pa] 1 bar in pascals
  static constexpr double kPa = 1000.0;                   /// [Pa] 1 kilopascal in pascals
  static constexpr double MPa = 1e6;                      /// [Pa] 1 megapascal in pascals
  static constexpr double torr = 133.3223684211;          /// [Pa] 1 torr (mm of Hg) in pascals
  static constexpr double atm = 101325.0;                 /// [Pa] 1 atmosphere in pascals
};

/**
 * Various physical constants that SuperNOVAS uses for astrometric calculations, all expressed in
 * terms of SI units. You can use them also. For example, you might use Constant::c to turn a
 * velocity (im m/s) to a unitless &beta;:
 *
 * ```c
 *   double beta = (29.5 * Unit::km / Unit::sec) / Constant::c;
 * ```
 *
 * @sa Unit
 * @ingroup util
 */
class Constant {
private:
  // Private Constructor
  Constant() {}

  //Constant(Constant const&);        // Don't Implement
  void operator=(Constant const&);    // Don't implement

public:
  /// \cond PRIVATE
  // Deleting the copy constructor to prevent copies
  Constant(const Constant& obj) = delete;
  //Constant(Constant const&)     = delete;
  /// \endcond

  static constexpr double pi = M_PI;                    /// [rad] &pi;
  static constexpr double twoPi = TWOPI;                /// [rad] 2&pi;
  static constexpr double halfPi = 0.5 * pi;            /// [rad] &pi;/2

  static constexpr double c = NOVAS_C;                  ///< [m/s] speed of light
  static constexpr double G = 6.67428e-1;               ///< [m<sup>3</sup> kg<sup>-1</sup> s<sup>-2</sup>]

  static constexpr double L_B = 1.550519768e-8;         ///< Barycentric clock rate increment over TT
  static constexpr double L_G = 6.969290134e-10;        ///< Geocentric clock rate increment over TT

  /// GRS80 Earth flattening
  static constexpr double F_earth = NOVAS_GRS80_FLATTENING;
  static constexpr double GM_sun = NOVAS_G_SUN;	        ///< [m<sup>3</sup> s<sup>-2</sup>] Solar graviational constant
  static constexpr double GM_earth = NOVAS_G_EARTH;     ///< [m<sup>3</sup> s<sup>-2</sup>] Earth graviational constant
  static constexpr double M_sun = GM_sun / G;           ///< [kg] Mass of the Sun
  static constexpr double M_earth = GM_sun / G;         ///< [kg] Earth mass

  static constexpr double R_earth = NOVAS_GRS80_RADIUS;   /// [m] 1 Earth quatorial radius (GRS80) in meters
};


/**
 * A simple interface class handling validation checking for classes that inherit it.
 *
 */
class Validating {
protected:
  /**
   * the state variable. Constructors should set it to `true` before returning if the instance has
   * been initialized in a valid state.
   *
   * @sa is)valid()
   */
  bool _valid = false;

  /// dummy constructor;
  Validating() {}

public:

  /**
   * Returns the previously set 'valid' stae of the implementing instance. Generally 'valid' means
   * that the class has all fields defined with sane values, such as floating-point values that
   * are not NAN, and object fields that are themselves 'valid'. Some implementing classes may
   * also apply additional range checking for values, for example, ensuring that a velocity does
   * not exceed the speed of light, etc.
   *
   * @return    `true` if the instance is in a 'valid' state, or else `false`.
   */
  bool is_valid() const { return _valid; }
};

/**
 * %Equatorial (RA/Dec) coordinate system. This class does not include the Earth-rotating systems
 * TIRS and ITRS.
 *
 * @sa CatalogEntry, Equatorial, Ecliptic, Apparent, Geometric
 * @ingroup source
 */
class EquatorialSystem : public Validating {
private:
  std::string _name;    ///< name of the catalog system, e.g. 'ICRS' or 'J2000'
  enum novas::novas_reference_system _system; ///< Coordinate reference system.
  double _jd;           ///< [day] Julian date of the dynamical equator (or closest to it) that
  ///< matches the system

  EquatorialSystem(const std::string& name, double jd_tt);

  explicit EquatorialSystem(enum novas::novas_reference_system system, double jd_tt = NOVAS_JD_J2000);

public:

  bool operator==(const EquatorialSystem& system) const;

  double jd() const;

  double mjd() const;

  double epoch() const;

  const std::string& name() const;

  enum novas::novas_reference_system reference_system() const;

  bool is_icrs() const;

  bool is_mod() const;

  bool is_true() const;

  std::string to_string() const;

  static std::optional<EquatorialSystem> from_string(const std::string& name);

  static std::optional<EquatorialSystem> for_reference_system(enum novas::novas_reference_system system, double jd_tt = NOVAS_JD_J2000);

  static EquatorialSystem mod(double jd_tt);

  static EquatorialSystem mod_at_besselian_epoch(double year);

  static EquatorialSystem tod(double jd_tt);

  static EquatorialSystem cirs(double jd_tt);

  static const EquatorialSystem& icrs();

  static const EquatorialSystem& j2000();

  static const EquatorialSystem& hip();

  static const EquatorialSystem& b1950();

  static const EquatorialSystem& b1900();
};


/**
 * A scalar separation between two points in space. It may be signed, such that that the distance
 * from __A__ to __B__ is the negative of the distance __B__ to __A__, i.e.:
 *
 *  \| __A__ - __B__ \| = -\| __B__ - __A__ \|
 *
 * for two vector locations __A__ and __B__.
 *
 * @sa Position
 * @ingroup util
 */
class Distance : public Validating {
private:
  double _meters;         ///< [m] stored distance

public:
  explicit Distance(double meters);

  /**
   * Returns the magnitude of this distance, as a unsigned distance.
   *
   * @return    The absolute value of the (possibly signed) distance value represented by this instance.
   */
  Distance abs() const;

  double m() const;

  double km() const;

  double au() const;

  double lyr() const;

  double pc() const;

  double kpc() const;

  double Mpc() const;

  double Gpc() const;

  Angle parallax() const;

  std::string to_string() const;

  static Distance from_parallax(const Angle& parallax);

  static const Distance& at_Gpc();
};

/**
 * A signed time interval between two instants of time, in the astronomical timescale of choice.
 *
 * @sa Time, TimeAngle
 * @ingroup util
 */
class Interval : public Validating {
private:

  double _seconds;                      ///< [s] stored time of the interval
  enum novas::novas_timescale _scale;   ///< store timescale of the interval

public:

  explicit Interval(double seconds, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Interval operator+(const Interval& r) const;

  Interval operator-(const Interval& r) const;

  Distance operator*(const Speed& v) const;

  Position operator*(const Velocity& v) const;

  bool equals(const Interval& interval, double precision = Unit::us) const;

  bool operator==(const Interval& interval) const {
    return equals(interval);
  }

  bool operator!=(const Interval& interval) const {
    return !equals(interval);
  }

  enum novas::novas_timescale timescale() const;

  Interval inv() const;

  double milliseconds() const;

  double seconds() const;

  double minutes() const;

  double hours() const;

  double days() const;

  double weeks() const;

  double years() const;

  double julian_years() const;

  double julian_centuries() const;

  Interval to_timescale(enum novas::novas_timescale scale) const;

  std::string to_string() const; // TODO

  static const Interval& zero();
};

/**
 * A representation of a regularized angle, which can be expressed in various commonly used
 * angular units as needed. It can also be used to instantiate angles from decimal or
 * [+]DDD:MM:SS.SSS string representations of the angle in degrees.
 *
 * @sa TimeAngle
 * @ingroup util
 */
class Angle : public Validating {
protected:
  double _rad;      ///< [rad] stored angle value, usually [-&pi;:&pi;), but can be different for subclasses.

public:

  virtual ~Angle() {};

  explicit Angle(double radians);

  explicit Angle(const std::string& str);

  virtual Angle operator+(const Angle& r) const;

  virtual Angle operator-(const Angle& r) const;

  bool equals(const Angle& angle, double precision = Unit::uas) const;

  bool operator==(const Angle& angle) {
    return equals(angle);
  }

  bool operator!=(const Angle& angle) {
    return !equals(angle);
  }

  double rad() const;

  double deg() const;

  double arcmin() const;

  double arcsec() const;

  double mas() const;

  double uas() const;

  double fraction() const;

  virtual std::string to_string(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;

  static constexpr int east = 1;      ///< East direction sign, e.g `19.5 * Unit::deg * Angle::east` for 19.5 deg East.

  static constexpr int west = -1;     ///< West direction sign, e.g `155.1 * Unit::deg * Angle::west` for 155.1 deg West.

  static constexpr int north = 1;     ///< North direction sign, e.g `33.4 * Unit::deg * Angle::north` for 33.4 deg North.

  static constexpr int south = -1;    ///< South direction sign, e.g `90.0 * Unit::deg * Angle::south` for the South pole.

};


/**
 * A representation of a regularized angle, which can also be represented as a time value in the 0
 * to 24h range. It can be expressed both in terms various commonly used angular units, or in terms
 * of time units, as needed.  It may also be used to instantiate time-angles from decimal or
 * from HH:MM:SS.SSS string representations of time in hours.
 *
 * @sa Time, Interval
 * @ingroup util
 */
class TimeAngle : public Angle {
public:

  explicit TimeAngle(double radians);

  explicit TimeAngle(const std::string& str);

  explicit TimeAngle(const Angle& angle);

  Angle operator+(const Angle& r) const override;

  Angle operator-(const Angle& r) const override;

  TimeAngle operator+(const Interval& other) const;

  TimeAngle operator-(const Interval& other) const;

  double hours() const;

  double minutes() const;

  double seconds() const;

  std::string to_string(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const TimeAngle& zero();
};

/**
 * A generic 3D spatial vector, expressed in arbitrary units.
 *
 * @sa Position, Velocity
 * @ingroup util
 */
class Vector : public Validating {
protected:
  double _component[3];       ///< [arb.u] Array containing the x, y, z components.

  explicit Vector(double x = 0.0, double y = 0.0, double z = 0.0);

  explicit Vector(const double v[3], double unit = 1.0);

public:

  virtual ~Vector() {}; // something virtual to make class polymorphic for dynamic casting.

  Vector operator*(double factor) const;

  double x() const;

  double y() const;

  double z() const;

  bool equals(const Vector& v, double precision) const;

  Vector scaled(double factor) const;

  const double *_array() const;

  double abs() const;

  double dot(const Vector& v) const;

  double projection_on(const Vector& v) const;

  Vector unit_vector() const;

  virtual std::string to_string() const;
};

/**
 * A 3D physical location vector in space.
 *
 * @sa Velocity, Geometric
 * @ingroup geometric
 */
class Position : public Vector {
public:

  explicit Position(double x_m = 0.0, double y_m = 0.0, double z_m = 0.0);

  explicit Position(const double pos[3], double unit = Unit::m);

  Position operator+(const Position &r) const;

  Position operator-(const Position &r) const;

  Distance distance() const;

  Position inv() const;

  Spherical as_spherical() const;

  std::string to_string() const override;

  static const Position& origin();

  static const Position& invalid();
};

/**
 * A 3D physical velocity vector in space.
 *
 * @sa Velocity, Geometric
 * @ingroup geometric
 */
class Velocity : public Vector {
public:
  explicit Velocity(double x_ms = 0.0, double y_ms = 0.0, double z_ms = 0.0);

  explicit Velocity(const double vel[3], double unit = 1.0);

  Velocity operator+(const Velocity& r) const;

  Velocity operator-(const Velocity& r) const;

  bool operator==(const Velocity& v) const {
    return equals(v, Unit::mm / Unit::sec);
  }

  bool operator!=(const Velocity& v) const {
    return !equals(v, Unit::mm / Unit::sec);
  }

  Speed speed() const;

  Velocity inv() const;

  Speed along(const Vector& direction) const;

  Position travel(double seconds) const;

  Position travel(const Interval& t) const;

  Position operator*(const Interval& t) const { return travel(t); }

  std::string to_string() const override;

  static const Velocity& stationary();

  static const Velocity& invalid();
};

/**
 * A scalar velocity (if signed) or speed (unsigned).
 *
 * @sa Position
 * @ingroup util, spectral
 */
class Speed : public Validating {
private:
  double _ms;       ///< [m/s] stored speed

public:
  explicit Speed(double m_per_s);

  Speed(const Distance& d, const Interval& time);

  Speed operator+(const Speed& r) const;

  Speed operator-(const Speed& r) const;

  bool equals(const Speed& speed, double m_per_s = Unit::mm / Unit::sec) const;

  bool equals(const Speed& speed, const Speed& tolerance) const;

  bool operator==(const Speed& speed) const;

  bool operator!=(const Speed& speed) const;

  /**
   * Returns the magnitude of this speed, as a unsigned speed.
   *
   * @return    The absolute value of the (possibly signed) speed value represented by this instance.
   */
  Speed abs() const;

  double m_per_s() const;

  double km_per_s() const;

  double au_per_day() const;

  double beta() const;

  double Gamma() const;

  double redshift() const;

  Distance travel(double seconds) const;

  Distance travel(const Interval& time) const;

  Distance operator*(const Interval& time) const { return travel(time); }

  Velocity in_direction(const Vector& direction) const;

  std::string to_string() const;

  static Speed from_redshift(double z);

  static const Speed& stationary();
};

/**
 * %Spherical coordinates (longitude, latitude, and distance), representing a direction on sky /
 * location in space.
 *
 * @sa Position, Equatorial, Ecliptic, Galactic, Horizontal
 * @ingroup util
 */
class Spherical : public Validating {
private:
  Angle _lon;           ///< [rad] stored longitude value
  Angle _lat;           ///< [rad] stored latitude value
  Distance _distance;   ///< [m] stored distance

public:
  virtual ~Spherical() {}; // something virtual to make class polymorphic for dynamic casting.

  Spherical(double longitude_rad, double latitude_rad, double distance_m = NOVAS_DEFAULT_DISTANCE);

  Spherical(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  explicit Spherical(const Position& pos);

  Position xyz() const;

  const Angle& longitude() const;

  const Angle& latitude() const;

  const Distance& distance() const;

  virtual std::string to_string(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

/**
 * %Equatorial coordinates (RA, Dec = &alpha;, &delta;) and distance, representing the direction on
 * the sky, or location in space, for a particular type of equatorial coordinate reference system,
 * relative to the equator and equinox on that system.
 *
 * @ingroup equatorial
 */
class Equatorial : public Spherical {
private:
  EquatorialSystem _sys;

  void validate();

public:
  Equatorial(double ra_rad, double dec_rad, const EquatorialSystem &system = EquatorialSystem::icrs(), double distance_m = NOVAS_DEFAULT_DISTANCE);

  Equatorial(const Angle& ra, const Angle& dec, const EquatorialSystem& system = EquatorialSystem::icrs(), const Distance& distance = Distance::at_Gpc());

  explicit Equatorial(const Position& pos, const EquatorialSystem& system = EquatorialSystem::icrs());

  TimeAngle ra() const;

  const Angle& dec() const;

  const EquatorialSystem& system() const;

  enum novas::novas_reference_system reference_system() const;

  Equatorial to_system(const EquatorialSystem& system) const;

  Equatorial to_icrs() const;

  Equatorial to_j2000() const;

  Equatorial to_hip() const;

  Equatorial to_mod(double jd_tt) const;

  Equatorial to_mod_at_besselian_epoch(double year) const;

  Equatorial to_tod(double jd_tt) const;

  Equatorial to_cirs(double jd_tt) const;

  /// @ingroup nonequatorial
  Ecliptic as_ecliptic() const;

  /// @ingroup nonequatorial
  Galactic as_galactic() const;

  std::string to_string(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Equatorial& invalid();
};

/**
 * %Ecliptic coordinates (_l_, _b_ or &lambda;, &beta;) and distance, representing the direction on
 * the sky, or location in space, for a particular type of equatorial coordinate reference system,
 * relative to the ecliptic and equinox of that system.
 *
 * @ingroup nonequatorial
 */
class Ecliptic : public Spherical {
private:
  enum novas::novas_equator_type _equator;
  double _jd;

  void validate();

  Ecliptic(double longitude_rad, double latitude_rad, enum novas::novas_equator_type equator, double jd_tt = NOVAS_JD_J2000, double distance_m = NOVAS_DEFAULT_DISTANCE);

public:
  Ecliptic(const Position& pos, enum novas::novas_equator_type equator, double jd_tt = NOVAS_JD_J2000);

  enum novas::novas_equator_type equator() const;

  double jd() const;

  double mjd() const;

  Ecliptic to_icrs() const;

  Ecliptic to_j2000() const;

  Ecliptic to_hip() const;

  Ecliptic to_mod(double jd_tt) const;

  Ecliptic to_tod(double jd_tt) const;

  /// @ingroup equatorial
  Equatorial as_equatorial() const;

  /// @ingroup nonequatorial
  Galactic as_galactic() const;

  std::string to_string(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static Ecliptic icrs(double longitude_rad, double latitude_rad, double distance = NOVAS_DEFAULT_DISTANCE);

  static Ecliptic icrs(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  static Ecliptic j2000(double longitude_rad, double latitude_rad, double distance = NOVAS_DEFAULT_DISTANCE);

  static Ecliptic j2000(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  static Ecliptic mod(double jd_tt, double longitude_rad, double latitude_rad, double distance = NOVAS_DEFAULT_DISTANCE);

  static Ecliptic mod(const Time& time, const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  static Ecliptic tod(double jd_tt, double longitude_rad, double latitude_rad, double distance = NOVAS_DEFAULT_DISTANCE);

  static Ecliptic tod(const Time& time, const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  static const Ecliptic& invalid();
};

/**
 * Galactic coordinates (_l_, _b_) and distance, representing the direction on the sky, or
 * location in space relative to the galactic plane and the nominal Galactic center location.
 *
 * @ingroup nonequatorial
 */
class Galactic : public Spherical {
public:
  Galactic(double longitude_rad, double latitude_rad, double distance_m = NOVAS_DEFAULT_DISTANCE);

  Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  explicit Galactic(const Position& pos);

  /// @ingroup equatorial
  Equatorial as_equatorial() const;

  /// @ingroup nonequatorial
  Ecliptic as_ecliptic() const;

  std::string to_string(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Galactic& invalid();
};

/**
 * A physical temperature value, which can be instantiated, and then expressed, in different
 * commonly used temperature units (C, K, or F). Within SuperNOVAS it is normally used to
 * express ambient temperatures at an observing location, but users may utilize it in any
 * other context also.
 *
 * @sa Weather
 * @ingroup util
 */
class Temperature : public Validating {
private:
  double _deg_C;        ///< [C] stored temperature

  explicit Temperature(double deg_C);

public:
  double celsius() const;

  double C() const {
    return celsius();
  }

  double kelvin() const;

  double K() const {
    return kelvin();
  }

  double farenheit() const;

  double F() const {
    return farenheit();
  }

  std::string to_string() const;

  static Temperature celsius(double value);

  static Temperature C(double value) {
    return celsius(value);
  }

  static Temperature kelvin(double value);

  static Temperature K(double value) {
    return kelvin(value);
  }

  static Temperature farenheit(double value);

  static Temperature F(double value) {
    return farenheit(value);
  }
};

/**
 * A physical pressure value, which can be instantiated, and then expressed, in different
 * commonly used pressure units (kPa, mbar, torr, atm, and more). Within SuperNOVAS it is
 * normally used to express atmospheric pressure at an observing location, but users may
 * utilize it in any other context also.
 *
 * @sa Weather
 * @ingroup util
 */
class Pressure : public Validating {
private:
  double _pascal;     ///< [Pa] stored pressure

  explicit Pressure(double value);

public:
  double Pa() const;

  double hPa() const;

  double kPa() const;

  double mbar() const;

  double bar() const;

  double torr() const;

  double atm() const;

  std::string to_string() const;

  static Pressure Pa(double value);

  static Pressure hPa(double value);

  static Pressure kPa(double value);

  static Pressure mbar(double value);

  static Pressure bar(double value);

  static Pressure torr(double value);

  static Pressure atm(double value);
};

/**
 * %Weather data, mainly for atmopsheric refraction correction for Earth-based (geodetic)
 * observers.
 *
 * @sa Horizontal::to_refracted(), Horizontal::to_unrefracted(), Site
 * @ingroup refract
 */
class Weather : public Validating {
private:
  Temperature _temperature;   ///< stored temperature value
  Pressure _pressure;         ///< stored pressure value
  double _humidity;           ///< [%] stored humidity value

  void validate();

public:
  Weather(const Temperature& T, const Pressure& p, double humidity_percent);

  Weather(double celsius, double pascal, double humidity_percent);

  const Temperature& temperature() const;

  const Pressure& pressure() const;

  double humidity() const;

  double humidity_fraction() const;

  std::string to_string() const;

  static Weather guess(const Site& site);
};

/**
 * Mean (inerpolated) IERS Earth Orientation Parameters (%EOP), without diurnal variations. IERS
 * publishes daily values, short-term and medium term forecasts, and historical data for the
 * measured, unmodelled (by the IAU 2006 precession-nutation model), _x_<sub>p</sub>,
 * _y_<sub>p</sub> pole offsets, leap-seconds (UTC - TAI difference), and the current UT1 - UTC
 * time difference.
 *
 * The _x_<sub>p</sub>, _y_<sub>p</sub> pole offsets define the true rotational pole of Earth vs
 * the dynamical equator of date, while the leap_seconds and UT1 - UTC time difference trace the
 * variations in Earth's rotation.
 *
 * %EOP are necessary both for defining or accessing astronomical times of the UT1 timescale (e.g.
 * for sidereal time or Earth-rotation angle (ERA) calculations), or for converting coordinates
 * between the preudo Earth-fixed Terrestrial Intermediate Reference System (TIRS) on the
 * dynamical equator of date, and the Earth-fixed International Terrestrial Reference System
 * (ITRS) on the true rotational equator.
 *
 * NOTES:
 * <ol>
 * <li>Corrections for diurnal variations are automatically applied in the constructors of
 * Time (for dUT1) and Frame (for _x_<sub>p</sub> and _y_<sub>p</sub> for geodetic observers),
 * and in Geometric::to_itrs(), as appropriate.</li>
 * </ol>
 *
 * @sa Time, GeodeticObserver, Apparent::to_itrs(), Geometric::to_itrs(), Horizontal::to_apparent()
 * \ingroup earth
 */
class EOP : public Validating {
private:
  int _leap;          ///< [s] store leap seconds (UTC - TAI time difference).
  Angle _xp;          ///< stored x pole offset (at midhight UTC).
  Angle _yp;          ///< stored y pole offset (at midnight UTC).
  double _dut1;       ///< [s] stored UT1 - UTC time difference.
  void validate();

public:
  explicit EOP(int leap_seconds, double dut1_sec = 0.0, double xp_rad = 0.0, double yp_rad = 0.0);

  EOP(int leap_seconds, const Interval& dut1, const Angle& xp, const Angle& yp);

  int leap_seconds() const;

  const Angle& xp() const;

  const Angle& yp() const;

  Interval dUT1() const;

  EOP itrf_transformed(int from_year, int to_year) const;

  std::string to_string() const;

  static const EOP& invalid();
};

/**
 * An Earth-based (geodetic) observer site location, or airborne observer location. Positions may
 * be defined as GPS / WGS84 or else as ITRF / GRS80 geodetic locations, or as Cartesian geocentric
 * _xyz_ positions in the International Terrestrial Reference Frame (ITRF).
 *
 * The class provides the means to convert between ITRF realizations, e.g. to match the ITRF
 * realization used for the Eath Orientation Parameters (%EOP) obtained from IERS, for &mu;as
 * precision. (This is really only necessary for VLBI interferometry). Alternatively, one may also
 * transform the %EOP values to match the ITRF realization of the site.
 *
 * @sa GeodeticObserver, EOP
 * @ingroup observer
 */
class Site : public Validating {
private:
  novas::on_surface _site = {};    ///< stored site information

  Site();

public:

  Site(double longitude_rad, double latitude_rad, double altitude_m = 0.0, enum novas::novas_reference_ellipsoid ellipsoid = novas::NOVAS_GRS80_ELLIPSOID);

  Site(const Angle& longitude, const Angle& latitude, const Distance& altitude, enum novas::novas_reference_ellipsoid ellipsoid = novas::NOVAS_GRS80_ELLIPSOID)
  : Site(longitude.rad(), latitude.rad(), altitude.m(), ellipsoid) {}

  explicit Site(const Position& xyz);

  const novas::on_surface *_on_surface() const;

  const Angle longitude() const;

  const Angle latitude() const;

  const Distance altitude() const;

  const Position xyz(enum novas::novas_reference_ellipsoid ellipsoid = novas::NOVAS_GRS80_ELLIPSOID) const;

  Site itrf_transformed(int from_year, int to_year) const;

  Position xyz() const;

  std::string to_string(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;

  static Site from_GPS(double longitude, double latitude, double altitude = 0.0);

  static Site from_xyz(const Position& xyz);
};

/**
 * An observer location. Both Earth-bound (geodetic sites, airborne, or Earth-orbit), and locations elsewhere in
 * the Solar-system are supported.
 *
 * @sa Frame
 * @ingroup observer
 */
class Observer : public Validating {
protected:
  novas::observer _observer = {};   ///< stored observer data

  Observer() {};

public:

  virtual ~Observer() {}; // something virtual to make class polymorphic for dynamic casting.

  const novas::observer * _novas_observer() const;

  enum novas::novas_observer_place type() const;

  virtual bool is_geodetic() const { return false; }

  virtual bool is_geocentric() const { return false; }

  virtual std::string to_string() const;

  static GeodeticObserver on_earth(const Site& site, const EOP& eop);

  static GeodeticObserver on_earth(const Site& geodetic, const Velocity& vel, const EOP& eop);

  static GeocentricObserver in_earth_orbit(const Position& pos, const Velocity& vel);

  static GeocentricObserver at_geocenter();

  static SolarSystemObserver in_solar_system(const Position& pos, const Velocity& vel);

  static SolarSystemObserver at_ssb();

  static const Observer& invalid();
};

/**
 * An observer location at a geodetic (longitude, latitude, altitude) location at the surface or
 * above it (e.g. in an aircraft or balloon). The observer may be fixed at that location, or else
 * moving with some velocity over the ground.
 *
 * @sa GeocentricObserver
 * @ingroup observer
 */
class GeodeticObserver : public Observer {
private:
  EOP _eop;     ///< stored Earth orientation parameters

  void diurnal_correct();

public:
  GeodeticObserver(const Site& site, const EOP& eop);

  GeodeticObserver(const Site& site, const Velocity& vel, const EOP& eop);

  bool is_geodetic() const override { return true; }

  Site site() const;

  const EOP& eop() const;

  std::string to_string() const override;
};

/**
 * An observer location and motion, defined relative to the geocenter, such as for an Earth-orbit
 * sattelite, or for a virtual observer located at the geocenter itself.
 *
 * @sa GeodeticObserver
 * @ingroup observer
 */
class GeocentricObserver : public Observer {
public:
  GeocentricObserver();

  GeocentricObserver(const Position& pos, const Velocity& vel);

  bool is_geocentric() const override { return true; }

  Position geocentric_position() const;

  Velocity geocentric_velocity() const;

  std::string to_string() const override;
};

/**
 * An observer location anywhere in the Solar System, defined by its momentary barycentric
 * position and velocity vectors.
 *
 * @ingroup observer
 */
class SolarSystemObserver : public Observer {
public:

  SolarSystemObserver();

  SolarSystemObserver(const Position& pos, const Velocity& vel);

  Position ssb_position() const;

  Velocity ssb_velocity() const;
};


/**
 * Type of calendar used for representing dates, such as Gregorian, Roman, or astronomical.
 *
 * @ingroup time
 */
class Calendar : public Validating {
private:
  enum novas::novas_calendar_type _type;

  explicit Calendar(enum novas::novas_calendar_type type);
public:

  enum novas::novas_calendar_type type() const;

  CalendarDate date(int year, int month, int day, const TimeAngle& time = TimeAngle::zero()) const;

  CalendarDate date(double jd) const;

  CalendarDate date(time_t t, long nanos = 0) const;

  CalendarDate date(struct timespec ts) const;

  static Calendar gregorian();

  static Calendar roman();

  static Calendar astronomical();

  std::optional<CalendarDate> parse_date(const std::string& str, enum novas::novas_date_format fmt = novas::NOVAS_YMD) const; // TODO

  std::string to_string() const; // TODO
};


/**
 * A time specified in a specific type of calendar (Gregorian, Roman, or astronomical). Unike the
 * Time class, calendar dates allow for broken-down (year, month, day-of-month, day-of-week,
 * day-of-year, and time-of-day) representation, It has a precision at the 100 &mu;s level,
 * limited by the double-precision representation of Julian dates. However, that level of
 * precision is sufficient for most applications.
 *
 * @sa Time
 * @ingroup time
 */
class CalendarDate : public Validating {
private:
  Calendar _calendar;
  int _year;
  int _month;
  int _mday;
  TimeAngle _time_of_day;
  double _jd;

public:
  CalendarDate(const Calendar& calendar, int year, int month, int day, const TimeAngle& time = TimeAngle::zero());

  CalendarDate(const Calendar& calendar, double jd);

  const Calendar& calendar() const { return _calendar; }

  double jd() const;

  double mjd() const;

  int year() const;

  int month() const;

  int day_of_month() const;

  int day_of_year() const;

  int day_of_week() const;

  CalendarDate operator+(const Interval& interval) const;

  CalendarDate operator-(const Interval& interval) const;

  Interval operator-(const CalendarDate& date) const;

  bool operator<(const CalendarDate& date) const;

  bool operator>(const CalendarDate& date) const;

  bool operator<=(const CalendarDate& date) const;

  bool operator>=(const CalendarDate& date) const;

  bool equals(const CalendarDate& date, double seconds = Unit::ms) const;

  bool equals(const CalendarDate& date, const Interval& precision) const {
    return equals(date, precision.seconds());
  }

  bool operator==(const CalendarDate& date) const {
    return equals(date);
  }

  bool operator!=(const CalendarDate& date) const {
    return !equals(date);
  }

  const TimeAngle& time_of_day() const;

  const std::string& month_name() const;

  const std::string& short_month_name() const;

  const std::string& day_name() const;

  const std::string& short_day_name() const;

  Time to_time(int leap_seconds, double dut1, novas::novas_timescale timescale = novas::NOVAS_UTC) const;

  Time to_time(const EOP& eop, novas::novas_timescale timescale = novas::NOVAS_UTC) const;

  std::string to_date_string(enum novas::novas_date_format fmt = novas::NOVAS_YMD) const;

  std::string to_long_date_string() const;

  CalendarDate in_calendar(const Calendar& calendar) const;

  std::string to_string(enum novas::novas_date_format fmt = novas::NOVAS_YMD, int decimals = 0) const; // TODO

  std::string to_string(int decimals) const; // TODO
};

/**
 * Precise astronomical time specification, supporting all relevant astronomical timescales (UT1,
 * UTC, TAI, GPS, TT, TDB, TCG, and TCB).
 *
 * @sa Interval, TimeAngle, Observer
 * @ingroup time
 */
class Time : public Validating {
private:
  novas::novas_timespec _ts = {};    ///< stored astronomical time specification

  Time() {};

  bool is_valid_parms(double dUT1, enum novas::novas_timescale timescale) const;

  void diurnal_correct();

public:

  Time(double jd, int leap_seconds, double dUT1, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(double jd, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(long ijd, double fjd, int leap_seconds, double dUT1, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(long ijd, double fjd, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(const std::string& timestamp, int leap_seconds, double dUT1, enum novas::novas_timescale timescale = novas::NOVAS_UTC);

  Time(const std::string& timestamp, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_UTC);

  Time(const struct timespec *t, int leap_seconds, double dUT1);

  Time(const struct timespec *t, const EOP& eop);

  explicit Time(const novas::novas_timespec *t);

  Interval operator-(const Time& other) const;

  Interval operator-(const CalendarDate& other) const;

  Time operator+(const Interval& delta) const;

  Time operator-(const Interval& delta) const;

  bool operator<(const Time& other) const;

  bool operator>(const Time& other) const;

  bool operator<=(const Time& other) const;

  bool operator>=(const Time& other) const;

  bool equals(const Time& time, double seconds = Unit::ms) const;

  bool equals(const Time& time, const Interval& precision) const {
    return equals(time, precision.seconds());
  }

  bool operator==(const Time& time) const {
    return equals(time);
  }

  bool operator!=(const Time& time) const {
    return !equals(time);
  }

  const novas::novas_timespec * _novas_timespec() const;

  double jd(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  long jd_day(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  TimeAngle jd_time_of_day(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  double mjd(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  int leap_seconds() const;

  Interval dUT1() const;

  TimeAngle time_of_day(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  int day_of_week(enum novas::novas_timescale timescale) const;

  double epoch() const;

  TimeAngle gst(enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;

  TimeAngle gmst() const;

  TimeAngle lst(const Site& site, enum novas::novas_accuracy accuracy) const;

  TimeAngle era() const;

  double moon_phase() const;

  Time next_moon_phase(const Angle& phase) const;

  std::string to_string(enum novas::novas_timescale timescale = novas::NOVAS_UTC) const;

  std::string to_iso_string() const;

  std::string to_epoch_string() const;

  Time shifted(double seconds) const;

  Time shifted(const Interval& offset) const;

  CalendarDate as_calendar_date(enum novas::novas_timescale timescale = novas::NOVAS_UTC) const;

  static Time from_mjd(double mjd, int leap_seconds, double dUT1, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  static Time from_mjd(double mjd, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  static Time now(const EOP& eop);

  static const Time& j2000();

  static const Time& hip();

  static const Time& b1950();

  static const Time& b1900();

  static const Time& invalid();
};

/**
 * An observing frame, defined by an observer location and precise time of observation. Frames can
 * be created with full (default) and reduced accuracy, supporting calculations with mas, or &mu;as
 * precisions typically. However note that full accuracy frames require SuperNOVAS to be configured
 * with an appropriate high-precision planet ephemeris provider (see e.g. `novas_use_calceph()` or
 * `novas_use_cspice()`), or else the resulting full-accuracy frame will be invalid.
 *
 * Reduced accuracy frames may also be invalid if the low precision planet ephemeris provider (
 * which, by default, calculates approximate positions for the Earth and Sun only) cannot provide
 * position for the Earth, Sun, the observer.
 *
 * Therefore, one is strongly advised to check the validity of an observing frame after instantiation
 * (using the is_valid() method), or else use the static Frame::create() function to return an
 * optional.
 *
 * @sa Source
 * @ingroup frame
 */
class Frame : public Validating {
private:
  novas::novas_frame _frame = {}; ///< Stored frame data
  Observer _observer;             ///< stored observer data
  Time _time;                     ///< stored time data

  void diurnal_correct();

public:
  Frame(const Observer& obs, const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY);

  const novas::novas_frame *_novas_frame() const;

  const Observer& observer() const;

  const Time& time() const;

  enum novas::novas_accuracy accuracy() const;

  /// @ingroup apparent
  Apparent approx_apparent(const Planet& planet) const;

  bool has_planet_data(enum novas::novas_planet planet) const;

  bool has_planet_data(const Planet& planet) const;

  /// @ingroup solar-system
  std::optional<Position> ephemeris_position(enum novas::novas_planet planet) const;

  /// @ingroup solar-system
  std::optional<Position> ephemeris_position(const Planet& planet) const;

  /// @ingroup solar-system
  std::optional<Velocity> ephemeris_velocity(enum novas::novas_planet planet) const;

  /// @ingroup solar-system
  std::optional<Velocity> ephemeris_velocity(const Planet& planet) const;

  double clock_skew(enum novas::novas_timescale = novas::NOVAS_TT) const;

  std::string to_string() const; // TODO

  static std::optional<Frame> create(const Observer& obs, const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY);

  static const Frame& invalid();
};


/**
 * An astronomical source, or target of observation.
 *
 * @sa CatalogSource, Planet, EphemerisSource, OrbitalSource
 * @ingroup source
 */
class Source : public Validating {
protected:
  struct novas::novas_object _object = {};     /// stored data on source

  Source() {}

public:
  virtual ~Source() {}; // something virtual to make class polymorphic for dynamic casting.

  const struct novas::novas_object *_novas_object() const;

  std::string name() const;

  /// @ingroup apparent
  Apparent apparent(const Frame &frame) const;

  /// @ingroup geometric
  Geometric geometric(const Frame &frame, enum novas::novas_reference_system system = novas::NOVAS_TOD) const;

  /// @ingroup apparent
  Angle sun_angle(const Frame &frame) const;

  /// @ingroup apparent
  Angle moon_angle(const Frame &frame) const;

  /// @ingroup apparent
  Angle angle_to(const Source& source, const Frame& frame) const;

  /// @ingroup time
  std::optional<Time> rises_above(double el, const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  /// @ingroup time
  std::optional<Time> transits(const Frame &frame) const;

  /// @ingroup time
  std::optional<Time> sets_below(double el, const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  /// @ingroup tracking
  std::optional<EquatorialTrack> equatorial_track(const Frame &frame, double range_seconds = Unit::hour) const;

  /// @ingroup tracking
  std::optional<HorizontalTrack> horizontal_track(const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  virtual std::string to_string() const; // TODO

  static void set_case_sensitive(bool value);
};

/**
 * Defines the cataloged parameters of a sidereal source, such as a star, a Galactic cloud, a distant
 * galaxy or a quasar.
 *
 * @sa CatalogSource
 * @ingroup source spectral
 */
class CatalogEntry : public Validating {
private:
  novas::cat_entry _entry = {};  ///< stored catalog entry
  EquatorialSystem _sys;            ///< stored catalog system

public:
  CatalogEntry(const std::string &name, const Equatorial& coords);

  CatalogEntry(const std::string &name, const Ecliptic& coords);

  CatalogEntry(const std::string &name, const Galactic& coords);

  explicit CatalogEntry(novas::cat_entry e, const EquatorialSystem& system = EquatorialSystem::icrs());

  const novas::cat_entry* _cat_entry() const;

  const EquatorialSystem& system() const;

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

  CatalogEntry& parallax(double radians);

  CatalogEntry& parallax(const Angle& angle);

  CatalogEntry& distance(double meters);

  CatalogEntry& distance(const Distance& dist);

  CatalogEntry& v_lsr(double v_ms);

  CatalogEntry& v_lsr(const Speed& v);

  CatalogEntry& radial_velocity(double v_ms);

  CatalogEntry& radial_velocity(const Speed& v);

  CatalogEntry& redshift(double z);

  CatalogEntry& catalog(const std::string& name, long number);

  std::string to_string() const; // TODO
};

/**
 * A sidereal source, defined by its catalog coordinates and other catalog parameters.
 *
 * @ingroup source
 */
class CatalogSource : public Source {
private:
  EquatorialSystem _system;      ///< stored catalog system

public:
  explicit CatalogSource(const CatalogEntry& e);

  const novas::cat_entry * _cat_entry() const;

  CatalogEntry catalog_entry() const;

  std::string to_string() const override;
};

/**
 * An abstract class of a source in the Solar-system.
 *
 * @sa Planet, EphemerisSource, OrbitalSource
 * @ingroup source
 */
class SolarSystemSource : public Source {
protected:
  SolarSystemSource() {}

public:
  double solar_illumination(const Frame& frame) const;

  double helio_distance(const Time& time, double *rate = NULL) const;

  double solar_power(const Time& time) const;
};

/**
 * A major planet (including Pluto), or the Sun, Moon, Solar-system Barycenter (SSB), Earth-Moon
 * Barycenter (EMB), or the Pluto-system barycenter. Planet positions are usually provided
 * by the JPL DE ephemeris files, such as DE440 or DE440s. By default SuperNOVAS calculates
 * approximate position for the Earth and Sun only. Thus to provide ephemeris positions for
 * all planet-type osurces, you will have to configure a Solar-system ephemeris provider, e.g.
 * via `novas::novas_use_calceph()` or `novas::novas_use_cspice()`.
 *
 * @sa EphemerisSource, OrbitalSource, novas::novas_use_calceph(), novas::novas_use_cspice().
 * @ingroup source
 */
class Planet : public SolarSystemSource {
public:
  explicit Planet(enum novas::novas_planet number);

  enum novas::novas_planet novas_id() const;

  int naif_id() const;

  int de_number() const;

  double mean_radius() const;

  double mass() const;

  static std::optional<Planet> for_naif_id(long naif);

  static std::optional<Planet> for_name(const std::string& name);

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

  std::string to_string() const override;
};

/**
 * A Solar-system source, whose positions / velocities are provided from ephemeris data. SuperNOVAS
 * does not support ephemeris data by itself, but can interface to other libraries (e.g. CALCEPH or
 * CSPICE), or provide them via user-selected function(s). Depending on the external implementation
 * that provides ephemeris data, sources may be looked up by name or ID number. Name-based lookup
 * may be case-sensitive (in which case you may want to use `novas::novas_case_sensitive()` to enable
 * case-sensitive source names in SuperNOVAS). ID-based lookup may use NAIF IDs, or else some other
 * numbering convention. In any case, you should construct your ephemeris source to match the
 * lookup method used by the ephemeris provider function(s) or library you will be using.
 *
 * @sa Planet, OrbitalSource, novas::novas_use_calceph(), novas::novas_use_cspice(),
 *     novas::novas_case_sensitive()
 * @ingroup source
 */
class EphemerisSource : public SolarSystemSource {
public:
  EphemerisSource(const std::string &name, long number);

  long number() const;

  std::string to_string() const override;
};

/**
 * Orbital system for Keplerian orbitals, defining the orbital plane and orientation, and the
 * central body (such as the Sun or a planet), around which the Keplerian orbital is to be
 * defined.
 *
 * @sa Orbital
 * @ingroup source
 */
class OrbitalSystem : public Validating {
private:
  novas::novas_orbital_system _system = {};

  OrbitalSystem(enum novas::novas_reference_plane plane, const Planet& center);

  explicit OrbitalSystem(const novas::novas_orbital_system *system);

public:
  const novas::novas_orbital_system * _novas_orbital_system() const { return &_system; }

  Planet center() const;

  Angle obliquity() const;

  Angle ascending_node() const;

  OrbitalSystem& orientation(double obliquity_rad, double node_rad, const EquatorialSystem& system = EquatorialSystem::icrs());

  OrbitalSystem& orientation(const Angle& obliquity, const Angle& node, const EquatorialSystem& system = EquatorialSystem::icrs());

  static OrbitalSystem equatorial(const Planet& center = Planet::sun());

  static OrbitalSystem ecliptic(const Planet& center = Planet::sun());

  static OrbitalSystem from_novas_orbital_system(const novas::novas_orbital_system *system);

  std::string to_string() const; // TODO
};

/**
 * Keplerian orbital elements, for example, for a comet using parameters published by the IAU
 * Minor Planet Center. While Keplerian orbitals cannot provide accurate positions or velocities
 * for Solar-system bodies over the long term (for that you need ephemeris data), they can be
 * sufficiently accurate on the short term. And, in case of recently discovered objects, such
 * as Near-Earth Objects (NEOs), orbital elements may be the only source of up-to-date
 * positional data.
 *
 * @sa EphemerisSource, Planet
 * @ingroup source
 */
class Orbital : public Validating {
private:
  novas::novas_orbital _orbit = {};

  explicit Orbital(const novas::novas_orbital *orbit);

public:
  Orbital(const OrbitalSystem& system, double jd_tdb, double semi_major_m, double mean_anom_rad, double period_s);

  Orbital(const OrbitalSystem& system, const Time& ref_time, const Distance& semi_major, const Angle& mean_anom,
          const Interval& periodT);

  static Orbital with_mean_motion(const OrbitalSystem& system, double jd_tdb, double a, double M0, double rad_per_s);

  static Orbital with_mean_motion(const OrbitalSystem& system, const Time& time, const Angle& a, const Angle& M0, double rad_per_s);

  const novas::novas_orbital * _novas_orbital() const { return &_orbit; }

  OrbitalSystem system() const;

  double reference_jd_tdb() const;

  Distance semi_major_axis() const;

  Angle reference_mean_anomaly() const;

  Interval period() const;

  double mean_motion() const;

  double eccentricity() const;

  Angle periapsis() const;

  Angle inclination() const;

  Angle ascending_node() const;

  Interval apsis_period() const;

  Interval node_period() const;

  double apsis_rate() const;

  double node_rate() const;

  Position position(const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;

  Velocity velocity(const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;

  Orbital& eccentricity(double e, double periapsis_rad);

  Orbital& eccentricity(double e, const Angle& periapsis_angle);

  Orbital& inclination(double angle_rad, double ascending_node_rad);

  Orbital& inclination(const Angle& angle, const Angle& ascending_node_angle);

  Orbital& apsis_period(double seconds);

  Orbital& apsis_period(const Interval& periodT);

  Orbital& apsis_rate(double rad_per_sec);

  Orbital& node_period(double seconds);

  Orbital& node_period(const Interval& periodT);

  Orbital& node_rate(double rad_per_sec);

  std::string to_string() const; // TODO

  static Orbital from_novas_orbit(const novas::novas_orbital *orbit);
};

/**
 * A Solar-system source, whose position and velocity can be calculated using Keplerian orbital
 * elements. While Keplerian orbitals are not typically accurate for long-term predictions, they
 * can be accurate in the short term, provided that one uses appropriate up-to-date orbital
 * elements, e.g. such as published by the Minor Planet Center (daily or otherwise regularly)
 * for asteroids, comets, an Near-Earth Objects (NEOs). For newly discovered objects, the
 * Keplerian orbital elements by the MPC may be the most accurate, or the only, source of
 * information.
 *
 * @sa EphemerisSource, Planet
 * @ingroup source
 */
class OrbitalSource : public SolarSystemSource {
public:
  OrbitalSource(const std::string& name, long number, const Orbital& orbit);

  const novas::novas_orbital *_novas_orbital() const;

  Orbital orbital() const;

  std::string to_string() const override;
};


/**
 * %Apparent position on sky as seen by an observer at a specific time of observation. Apparent
 * positions are corrected for aberration for a movig observer, and gravitational deflection
 * around the major Solar-system bodies along the path of visibility.
 *
 * The apparent position of a source is where it appears to the observer on the celestial sphere.
 * As such it is mainly a direction on sky, which is corrected for light-travel time (i.e. where
 * the source was at the time light originated from the Solar-system body, or the differential
 * light-travel time between the Solar-system barycenter and the observer location for sidereal
 * sources).
 *
 * Unlike geometric positions, the apparent location is also corrected for the observer's motion
 * (aberration), as well as gravitational deflection around the major Solar-system bodies. Also,
 * the radial valocity _v_<sub>r</sub> is calculated to reflect a spectroscopic measure, defined
 * by the relation:
 *
 * &lambda;<sub>obs</sub> / &lambda;<sub>rest</sub> = ( (1.0 + _v_<sub>r</sub> / _c_) / (1.0 - _v_<sub>r</sub> / _c_) )<sup>1/2</sup>
 *
 * As such it contains appropriate relativistic corrections for the observer's relative motion
 * as well as gravitational redshift corrections for the surface of major Solar-system bodies,
 * where light is assumed to originate, and at the observer location. It is also corrected for
 * the viewing angle difference when light is gravitationally deflected around major Solar-system
 * bodies.
 *
 * %Apparent positions can also come directly from observations, such as from unrefracted
 * horizontal coordinates.
 *
 * @sa Source::apparent(), Horizontal::to_apparent()
 * \ingroup apparent spectral
 */
class Apparent : public Validating {
private:
  EquatorialSystem _sys;                      ///< stored coordinate system type
  Frame _frame;                               ///< stored frame data

  novas::sky_pos _pos;                        ///< stored apparent position data

  Apparent(const EquatorialSystem& system, const Frame& frame);

  Apparent(const EquatorialSystem& system, const Frame& frame, novas::sky_pos p);

  Apparent(const EquatorialSystem& system, const Frame& frame, double ra_rad, double dec_rad, double rv_ms = 0.0);


public:
  const novas::sky_pos *_sky_pos() const;

  const Frame& frame() const;

  const EquatorialSystem& system() const;

  Position xyz() const;

  Speed radial_velocity() const;

  double redshift() const;

  Distance distance() const;

  /// @ingroup equatorial
  Equatorial equatorial() const;

  /// @ingroup nonequatorial
  Ecliptic ecliptic() const;

  /// @ingroup nonequatorial
  Galactic galactic() const;

  /// @ingroup nonequatorial
  std::optional<Horizontal> horizontal() const;

  std::string to_string() const; // TODO

  static Apparent cirs(double ra_rad, double dec_rad, const Frame& frame, double rv_ms = 0.0);

  static Apparent cirs(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv);

  static Apparent tod(double ra_rad, double dec_rad, const Frame& frame, double rv_ms = 0.0);

  static Apparent tod(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv);

  static Apparent from_tod_sky_pos(novas::sky_pos p, const Frame& frame);

  static Apparent from_cirs_sky_pos(novas::sky_pos p, const Frame& frame);

  static const Apparent& invalid();
};

/**
 * The geometric (3D) position and velocity of a source relative to an observer location. It
 * denotes spatial location and velocity of the source at the time light originated from it, prior
 * to detection by the observer. As such, geometric positions are necessarily antedated for light
 * travel time (for Solar-system sources) or corrected for the differential light-travel between
 * the Solar-system barycenter and the observer location (for sidereal sources).
 *
 * In other words, geometric positions are not the same as ephemeris positions for the equivalent
 * time for Solar-system bodies. Rather, geometric positions match the ephemeris positions for
 * an earlier time, when the observed light originated from the source.
 *
 * @sa apparent
 * @ingroup geometric
 */
class Geometric : public Validating {
private:
  Frame _frame;                             ///< stored frame data
  Position _pos;                            ///< stored geometric position w.r.t. observer
  Velocity _vel;                            ///< stored geometric velocity w.r.t. observer
  enum novas::novas_reference_system _sys;  ///< stored coordinate reference system type

  Geometric in_system(const novas::novas_frame *f, enum novas::novas_reference_system system) const;

public:
  Geometric(const Position& p, const Velocity& v, const Frame& frame, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  const Frame& frame() const;

  enum novas::novas_reference_system system() const;

  const Position& position() const;

  const Velocity& velocity() const;

  /// @ingroup equatorial
  Equatorial equatorial() const;

  /// @ingroup nonequatorial
  Ecliptic ecliptic() const;

  /// @ingroup nonequatorial
  Galactic galactic() const;

  Geometric in_system(enum novas::novas_reference_system system) const;

  Geometric in_icrs() const { return in_system(novas::NOVAS_ICRS); }

  Geometric in_j2000() const { return in_system(novas::NOVAS_J2000); }

  Geometric in_mod() const { return in_system(novas::NOVAS_MOD); }

  Geometric in_tod() const { return in_system(novas::NOVAS_TOD); }

  Geometric in_cirs() const { return in_system(novas::NOVAS_CIRS); }

  Geometric in_tirs() const { return in_system(novas::NOVAS_TIRS); }

  std::optional<Geometric> in_itrs(const EOP& eop = EOP::invalid()) const;

  std::string to_string() const; // TODO

  static const Geometric& invalid();
};

/**
 * %Horizontal (azimuth, elevation = Az/El) sky coordinates at a geodetic observing location, such
 * as an observatory site, an aircraft, or a balloon. These represent positions relative to the
 * local horizon and meridian, and can be used for both unrefracted (astrometric) or refracted
 * (observed) values or for conbverting between those two.
 *
 * @sa Apparent, Site, Weather
 * @ingroup nonequatorial refract
 */
class Horizontal : public Spherical {
private:
  Horizontal();

public:
  Horizontal(double azimuth, double elevation, double distance = NOVAS_DEFAULT_DISTANCE);

  Horizontal(const Angle& azimuth, const Angle& elevation, const Distance& distance = Distance::at_Gpc());

  const Angle& azimuth() const;

  const Angle& elevation() const;

  const Angle zenith_angle() const;

  /// @ingroup refract
  Horizontal to_refracted(const Frame &frame, novas::RefractionModel ref, const Weather& weather);

  /// @ingroup refract
  Horizontal to_unrefracted(const Frame &frame, novas::RefractionModel ref, const Weather& weather);

  /// @ingroup apparent
  std::optional<Apparent> to_apparent(const Frame& frame, double rv = 0.0, double distance = NOVAS_DEFAULT_DISTANCE) const;

  /// @ingroup apparent
  std::optional<Apparent> to_apparent(const Frame& frame, const Speed& rv = Speed::stationary(), const Distance& distance = Distance::at_Gpc()) const;

  std::string to_string(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Horizontal& invalid();
};

/**
 * Evolution of position vs. time in one dimension, based on a local quadratic approximation.
 *
 * @sa Track
 */
class Evolution : public Validating {
private:
  double _value;
  double _rate;
  double _accel;

public:
  Evolution(double pos, double vel, double accel = 0.0);

  double value(const Interval& offset = Interval::zero()) const;

  double rate(const Interval& offset = Interval::zero()) const;

  double acceleration() const;

  static const Evolution& zero();

  static const Evolution stationary(double value);

};

/**
 * Approximate trajectory of a source in spherical coordinates, using a local quadratic
 * approximation around a time instant, in some (unspecified) coordinate system.
 *
 * @sa HorizontalTrack, EquatorialTrack
 */
class Track : public Validating {
private:
  Time _ref_time;
  Interval _range;
  Evolution _lon;
  Evolution _lat;
  Evolution _r;

protected:

  Track(const Time& ref_time, const Interval& range, const Evolution& lon, const Evolution& lat, const Evolution& r = Evolution::stationary(NOVAS_DEFAULT_DISTANCE));

  Track(const novas::novas_track *track, const Interval& range);

public:
  Time& reference_time() const;

  bool is_valid(const Time& time) const;

  const Interval& range() const;

  Angle longitude(const Time& time) const;

  Angle latitude(const Time& time) const;

  Distance distance(const Time& time) const;

  Speed radial_velocity(const Time& time) const;

  double redshift(const Time& time) const;
};

/**
 * Approximate trajectory of a source in horizontal coordinates, using a local quadratic
 * approximation around a time instant. This may be used e.g., to control telescope drive systems
 * in horizontal mounts, by providing instantaneous porisitons, rate and acceletation along the
 * azimuth and elevation axes. Or, one may use the trajectory to obtain interpolated instantaneous
 * Az/El positions, within the interval of validity, at very low computational cost.
 *
 * @sa Apparent::horizontal(), EquatorialTrack
 * @ingroup tracking nonequatorial
 */
class HorizontalTrack : public Track {

  HorizontalTrack(const novas::novas_track *track, const Interval& range)
  : Track(track, range) {}

public:
  HorizontalTrack(const Time& ref_time, const Interval& range,
          const Evolution& lon, const Evolution& lat, const Evolution& r = Evolution::stationary(NOVAS_DEFAULT_DISTANCE))
  : Track(ref_time, range, lon, lat, r) {}

  Horizontal projected(const Time& time) const;

  static HorizontalTrack from_novas_track(const novas::novas_track *track, const Interval& range);
};

/**
 * Approximate trajectory of a source in equatorial coordinates, using a local quadratic
 * approximation around a time instant. This may be used e.g., to control telescope drive systems
 * in equatorial mounts, by providing instantaneous porisitons, rate and acceletation along the
 * R.A. and declination axes. Or, one may use the trajectory to obtain interpolated instantaneous
 * R.A./Dec positions, within the interval of validity, at very low computational cost.
 *
 * @sa Apparent::equatorial(), HorizontalTrack
 * @ingroup tracking apparent
 */
class EquatorialTrack : public Track {
private:
  EquatorialSystem _system;

  EquatorialTrack(const EquatorialSystem& system, const novas::novas_track *track, const Interval& range)
  : Track(track, range), _system(system) {}

public:
  EquatorialTrack(const EquatorialSystem& system, const Interval& range, const Time& ref_time,
          const Evolution& lon, const Evolution& lat, const Evolution& r = Evolution::stationary(NOVAS_DEFAULT_DISTANCE))
  : Track(ref_time, range, lon, lat, r), _system(system) {}

  Equatorial projected(const Time& time) const;

  static EquatorialTrack from_novas_track(const EquatorialSystem& system, const novas::novas_track *track, const Interval& range);
};

} // namespace supernovas

#  endif /* cplusplus */
#endif /* INCLUDE_SUPERMOVAS_H_ */
