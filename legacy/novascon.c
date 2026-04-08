/**
 * @file
 *
 *  @deprecated Use your own version for the selection of the constant you need, expressed in
 *              whatever units your application desires. We should not force you to adopt the
 *              internally used convention of NOVAS, not to mention the high chance of namespace
 *              conflicts with the super-simplistic naming scheme here. You are better off
 *              without this.
 *
 *  SuperNOVAS implementation for numerical constants that were used internally in novas.c. In
 *  SuperNOVAS it is no longer needed by novas.c, and you probably don't want to use it either
 *  with your application code.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  http://www.usno.navy.mil/USNO/astronomical-applications
 *
 * @author G. Kaplan and Attila Kovacs
 */

// We shall never use these in the internal API. We define precompiler constants instead...
#ifndef __NOVAS_INTERNAL_API__

#include "novas.h"
#include "novascon.h"

const double T0 = NOVAS_JD_J2000;
const double C = NOVAS_C;
const double AU_SEC = NOVAS_AU_SEC;
const double C_AUDAY = NOVAS_C_AU_PER_DAY;
const double AU = NOVAS_AU;
const double AU_KM = NOVAS_AU_KM;
const double GS = NOVAS_G_SUN;
const double GE = NOVAS_G_EARTH;
const double ERAD = NOVAS_EARTH_RADIUS;
const double F = NOVAS_EARTH_FLATTENING;
const double ANGVEL = NOVAS_EARTH_ANGVEL;
const double RMASS[12] = NOVAS_RMASS_INIT;


#endif /* __NOVAS_INTERNAL_API__ */







