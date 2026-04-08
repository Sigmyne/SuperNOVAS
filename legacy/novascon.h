/**
 * @file
 *
 * @deprecated Use your own version for the selection of the constant you need, expressed in
 *              whatever units your application desires. We should not force you to adopt the
 *              internally used convention of NOVAS, not to mention the high chance of namespace
 *              conflicts with the super-simplistic naming scheme here. You are better off
 *              without this.
 *
 *  SuperNOVAS header for numerical constants that used to be used internally in novas.c.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  http://www.usno.navy.mil/USNO/astronomical-applications
 *
 *  @author G. Kaplan and Attila Kovacs
 */

#ifndef _CONSTS_
#define _CONSTS_

/// @deprecated Definition was already ommitted in NOVAS C 3.1.
extern const short FN1;

/// @deprecated Definition was already ommitted in NOVAS C 3.1.
extern const short FN0;

/// @deprecated TDB Julian date of epoch J2000.0. Use NOVAS_JD_J2000 instead.
extern const double T0;

/// @deprecated Speed of light in meters/second is a defining physical constant. Use NOVAS_C instead.
extern const double C;

/// @deprecated Light-time for one astronomical unit (AU) in seconds, from DE-405. Use NOVAS_AU_SEC instead.
extern const double AU_SEC;

/// @deprecated Speed of light in AU/day. Use NOVAS_AUDAY instead.
extern const double C_AUDAY;

/// @deprecated Astronomical unit in meters. Use NOVAS_AU instead.
extern const double AU;

/// @deprecated Astronomical Unit in kilometers. Use NOVAS AU_KM instead.
extern const double AU_KM;

/// @deprecated Heliocentric gravitational constant in meters^3 / second^2, from DE-405. Use NOVAS_G_SUN instead.
extern const double GS;

/// @deprecated Geocentric gravitational constant in meters^3 / second^2, from DE-405. Use NOVAS_G_EARTH instead.
extern const double GE;

/// @deprecated Radius of Earth in kilometers from IERS Conventions (2003). Use NOVAS_EARTH_RADIUS instead
extern const double ERAD;

/// @deprecated Earth ellipsoid flattening from IERS Conventions (2003). Value is 1 / 298.25642.
///             Use NOVAS_EARTH_FLATTENING instead.
extern const double F;

/// @deprecated Rotational angular velocity of Earth in radians/sec from IERS Conventions (2003).
///             Use NOVAS_EARTH_ANGVEL instead
extern const double ANGVEL;

/// @deprecated Reciprocal masses of solar system bodies, from DE-405 (Sun mass / body mass).
///             MASS[0] = Earth/Moon barycenter, MASS[1] = Mercury, ..., MASS[9] = Pluto, MASS[10] = Sun,
///             MASS[11] = Moon. Use NOVAS_RMASS_INIT instead.
extern const double RMASS[12];


#endif /* _CONSTS_ */
