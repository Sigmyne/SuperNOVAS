/**
 * @file
 *
 *  Functions that allow to define or access Solar-system ephemeris data. SuperNOVAS cannot, by
 *  itself, provide accurate positions for Solar-system sources. While it has a built-in
 *  calculator (see @ref solsys3.c) for the Earth and the Sun, it is suitable for obtaining
 *  approximate positions for the Sun and Earth only, and is limited to around 10 arcsec accuracy,
 *  typically.
 *
 *  To do astrometry right for Solar-system sources (major planets, and minor bodies like
 *  asteroids, comets, or moons), you will have to interface SuperNOVAS with ephemeris data,
 *  through a library or service. For example, NASA JPL provides short and long-term ephemeris
 *  data for all known Solar-system sources.
 *
 *  Besides providing accurate astrometric positions for Solar-system sources, planetary ephemeris
 *  data are also necessary for high-precision position calculations for sidereal
 *  (non-Solar-system) sources if one is to account for gravitational deflections around the
 *  major planets as light traverses the Solar-system on its way to the observer.
 *
 *  You may download various ephemeris data files (SPICE kernels) made available by NASA from the
 *  [JPL NAIF](https://naif.jpl.nasa.gov/naif/data.html) site or from the
 *  [JPL Horizons](https://ssd.jpl.nasa.gov/horizons/) system, and them
 *  access them via appropriate C libraries, such as JPL's own
 *  [NAIF CSPICE Toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html), or the more modern
 *  [CALCEPH](https://calceph.imcce.fr/) library by IMCCE. SuperNOVAS provides interfaces to both
 *  (see @ref solsys-cspice.c and @ref solsys-calceph.c), making it very easy to incorporate
 *  ephemeris data into SuperNOVAS that way.
 *
 *  Alternatively, you might use some other library or your own code to access ephemeris data,
 *  e.g. by interfacing directly to the [JPL Horizons](https://ssd.jpl.nasa.gov/horizons/) system,
 *  or to the [IAU Minor Planet Center (MPC)](https://www.minorplanetcenter.net/iau/mpc.html). You
 *  can always write your own `novas_ephem_provider` function and activate it for SuperNOVAS with
 *  `set_ephem_provider()`, and also configure it to use for planets with:
 *
 *  ```c
 *    set_planet_provider(planet_ephem_provider);
 *    set_planet_provider_hp(planet_ephem_provider_hp);
 *  ```
 *
 *  (Alternatively, you might define separate low and high-precision ephemeris provider functions
 *  for the major planets ,by writing your own `novas_ephem_provider` and `novas_ephem_provider_hp`
 *  interface functions, respectively, and activating these with `set_ephem_provider()` and
 *  `set_ephem_provider_hp()`.)
 *
 * @date Created  on Mar 6, 2025
 * @author Attila Kovacs and G. Kaplan
 *
 * @sa solsys-calceph.c, solsys-cspice.c, solsys3.c, orbital.c
 */

#include <string.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond



#if __cplusplus
#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif

// <---------- GLOBAL VARIABLES -------------->

/// \cond PRIVATE
#ifdef USER_SOLSYS
extern short solarsystem(double jd_tdb, short body, short origin, double *restrict position, double *restrict velocity);
extern short solarsystem_hp(const double jd_tdb[restrict 2], short body, short origin, double *restrict position, double *restrict velocity);

static short solarsystem_adapter(double jd_tdb, enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity) {
  return solarsystem(jd_tdb, (short) body, (short) origin, position, velocity);
}

static short solarsystem_hp_adapter(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity) {
  return solarsystem_hp(jd_tdb, (short) body, (short) origin, position, velocity);
}

novas_planet_provider planet_call = solarsystem_adapter;
novas_planet_provider_hp planet_call_hp = solarsystem_hp_adapter;
#else
// Unfortunately, these were exposed before, so we should keep them so.
// TODO make static for 2.0
novas_planet_provider planet_call = earth_sun_calc;
novas_planet_provider_hp planet_call_hp = earth_sun_calc_hp;

short solarsystem(double jd_tdb, short body, short origin, double *restrict position, double *restrict velocity) {
  prop_error("solarsystem", planet_call(jd_tdb, (enum novas_planet) body, (enum novas_origin) origin, position, velocity), 0);
  return 0;
}

short solarsystem_hp(const double jd_tdb[restrict 2], short body, short origin, double *restrict position, double *restrict velocity) {
  prop_error("solarsystem_hp", planet_call_hp(jd_tdb, (enum novas_planet) body, (enum novas_origin) origin, position, velocity), 0);
  return 0;
}
#endif /* USER_SOLSYS */
/// \endcond


/// function to use for reading ephemeris data for all types of solar system sources
static novas_ephem_provider readeph2_call = NULL;

/// Function to use for reduced-precision calculations. (The full IAU 2000A model is used always
/// for high-precision calculations)
static novas_nutation_provider nutate_lp = iau2000b;


/**
 * Sets the function to use for obtaining position / velocity information for minor planets, or
 * satellites.
 *
 * @param func   new function to use for accessing ephemeris data for minor planets or satellites.
 * @return       0 if successful, or else -1 if the function argument is NULL.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa get_ephem_provider(), set_planet_provider(), set_planet_provider_hp()
 * @sa novas_use_calceph(), novas_use_cspice()
 */
int set_ephem_provider(novas_ephem_provider func) {
  readeph2_call = func;
  return 0;
}

/**
 * Returns the user-defined ephemeris accessor function.
 *
 * @return    the currently defined function for accessing ephemeris data for minor planets
 *            or satellites, ot NULL if no function was set via set_ephem_provider() previously.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa set_ephem_provider(), get_planet_provider(), get_planet_provider_hp(), ephemeris()
 */
novas_ephem_provider get_ephem_provider() {
  return readeph2_call;
}

/**
 * Set the function to use for low-precision IAU 2000 nutation calculations instead of the default
 * nu2000k().
 *
 * @param func  the new function to use for low-precision IAU 2000 nutation calculations
 * @return      0 if successful, or -1 if the input argument is NULL
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa get_nutation_lp_provider(), nutation_angles(), nutation()
 */
int set_nutation_lp_provider(novas_nutation_provider func) {
  if(!func)
    return novas_error(-1, EINVAL, "set_nutation_lp_provider", "NULL 'func' parameter");

  nutate_lp = func;
  return 0;
}

/**
 * Returns the function configured for low-precision IAU 2000 nutation calculations instead of the
 * default nu2000k().
 *
 * @return   the function to use for low-precision IAU 2000 nutation calculations
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa set_nutation_lp_provider(), nutation_angles(), nutation()
 */
novas_nutation_provider get_nutation_lp_provider() {
  return nutate_lp;
}

/**
 * Set a custom function to use for regular precision (see NOVAS_REDUCED_ACCURACY) ephemeris
 * calculations instead of the default solarsystem() routine.
 *
 * @param func    The function to use for solar system position/velocity calculations.
 *                See solarsystem() for further details on what is required of this
 *                function.
 *
 * @author Attila Kovacs
 * @since 1.0
 *
 * @sa get_planet_provider(), set_planet_provider_hp(), NOVAS_REDUCED_ACCURACY
 * @sa novas_use_calceph(), novas_use_cspice()
 */
int set_planet_provider(novas_planet_provider func) {
  if(!func)
    return novas_error(-1, EINVAL, "set_planet_provider", "NULL 'func' parameter");

  planet_call = func;
  return 0;
}

/**
 * Returns the custom (low-precision) ephemeris provider function for major planets (and Sun,
 * Moon, SSB...), if any.
 *
 * @return    the custom (low-precision) planet ephemeris provider function.
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa set_planet_provider(), get_planet_provider_hp(), get_ephem_provider()
 */
novas_planet_provider get_planet_provider() {
  return planet_call;
}

/**
 * Set a custom function to use for high precision (see NOVAS_FULL_ACCURACY) ephemeris
 * calculations instead of the default solarsystem_hp() routine.
 *
 * @param func    The function to use for solar system position/velocity calculations.
 *                See solarsystem_hp() for further details on what is required of this
 *                function.
 *
 * @author Attila Kovacs
 * @since 1.0
 *
 * @sa get_planet_provider_hp(), set_planet_provider(), solarsystem_hp(), NOVAS_FULL_ACCURACY
 * @sa novas_use_calceph(), novas_use_cspice()
 */
int set_planet_provider_hp(novas_planet_provider_hp func) {
  if(!func)
    return novas_error(-1, EINVAL, "set_planet_provider_hp", "NULL 'func' parameter");

  planet_call_hp = func;
  return 0;
}

/**
 * Returns the custom high-precision ephemeris provider function for major planets (and Sun, Moon,
 * SSB...), if any.
 *
 * @return    the custom high-precision planet ephemeris provider function.
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa set_planet_provider_hp(), get_planet_provider(), get_ephem_provider()
 */
novas_planet_provider_hp get_planet_provider_hp() {
  return planet_call_hp;
}

/**
 * Retrieves the position and velocity of a solar system body using the currently configured
 * plugins that provide them.
 *
 * It is recommended that the input structure 'cel_obj' be created using make_object()
 *
 * @param jd_tdb    [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param body      Pointer to structure containing the designation of the body of interest
 * @param origin    NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1)
 * @param accuracy  NOCAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos  [AU] Pointer to structure containing the designation of the body of interest
 * @param[out] vel  [AU/day] Velocity vector of the body at 'jd_tdb'; equatorial rectangular
 *                  coordinates in AU/day referred to the ICRS.
 * @return          0 if successful, -1 if the 'jd_tdb' or input object argument is NULL, or
 *                  else 1 if 'origin' is invalid, 2 if `cel_obj->type` is invalid,
 *                  10 + the error code from the currently configured `novas_planet_provider_hp`
 *                  call, or 20 + the error code from `readeph()`.
 *
 * @sa set_planet_provider(), set_planet_provider_hp(), set_ephem_provider()
 * @sa make_planet(), make_ephem_object()
 */
short ephemeris(const double *restrict jd_tdb, const object *restrict body, enum novas_origin origin,
        enum novas_accuracy accuracy, double *restrict pos, double *restrict vel) {
  static const char *fn = "ephemeris";

  double posvel[6] = {0};
  int error = 0;

  if(!jd_tdb || !body)
    return novas_error(-1, EINVAL, fn, "NULL input pointer: jd_tdb=%p, body=%p", jd_tdb, body);

  if(!pos || !vel)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: pos=%p, vel=%p", pos, vel);

  // Check the value of 'origin'.
  if((unsigned) origin >= NOVAS_ORIGIN_TYPES)
    return novas_error(1, EINVAL, fn, "invalid origin type: %d", origin);

  // Invoke the appropriate ephemeris access software depending upon the
  // type of object
  switch(body->type) {

    case NOVAS_PLANET:
      // Get the position and velocity of a major planet, Pluto, Sun, or Moon.
      // When high accuracy is specified, use function 'solarsystem_hp' rather
      // than 'solarsystem'.

      if(accuracy == NOVAS_FULL_ACCURACY)
        error = planet_call_hp(jd_tdb, (enum novas_planet) body->number, (enum novas_origin) origin, pos, vel);
      else
        error = planet_call(jd_tdb[0] + jd_tdb[1], (enum novas_planet) body->number, (enum novas_origin) origin, pos, vel);

      prop_error("ephemeris:planet", error, 10);
      break;

    case NOVAS_EPHEM_OBJECT: {
      enum novas_origin eph_origin = NOVAS_HELIOCENTER;
      novas_ephem_provider ephem_call = get_ephem_provider();

      if(ephem_call) {
        // If there is a newstyle epehemeris access routine set, we will prefer it.
        error = ephem_call(body->name, body->number, jd_tdb[0], jd_tdb[1], &eph_origin, posvel, &posvel[3]);
      }
      else {
#  ifdef USER_READEPH
        // Use whatever readeph() was compiled or the equivalent user-defined call
        double *res = readeph(body->number, body->name, jd_tdb[0] + jd_tdb[1], &error);
        if(res == NULL) {
          error = 3;
          errno = ENOSYS;
        }
        else {
          memcpy(posvel, res, sizeof(posvel));
          free(res);
        }
#  else
        return novas_error(-1, errno, "ephemeris:ephem_object", "No ephemeris provider was defined. Call set_ephem_provider() prior.");
#  endif
      }

      prop_error("ephemeris:ephem_object", error, 20);

      // Check and adjust the origins as necessary.
      if(origin != eph_origin) {
        double pos0[3] = {0}, vel0[3] = {0};
        enum novas_planet refnum = (origin == NOVAS_BARYCENTER) ? NOVAS_SSB : NOVAS_SUN;
        int i;

        prop_error("ephemeris:origin", planet_call(jd_tdb[0] + jd_tdb[1], refnum, eph_origin, pos0, vel0), 10);

        for(i = 3; --i >= 0;) {
          posvel[i] -= pos[i];
          posvel[i + 3] = novas_add_vel(posvel[i + 3], vel[i]);
        }
      }

      // Break up 'posvel' into separate position and velocity vectors.
      memcpy(pos, posvel, XYZ_VECTOR_SIZE);
      memcpy(vel, &posvel[3], XYZ_VECTOR_SIZE);

      break;
    }

    case NOVAS_ORBITAL_OBJECT: {
      object center;
      double pos0[3] = {0}, vel0[3] = {0};
      int i;

      prop_error(fn, make_planet(body->orbit.system.center, &center), 0);
      prop_error(fn, ephemeris(jd_tdb, &center, origin, accuracy, pos0, vel0), 0);
      prop_error(fn, novas_orbit_posvel(jd_tdb[0] + jd_tdb[1], &body->orbit, accuracy, pos, vel), 0);

      for(i = 3; --i >= 0; ) {
        pos[i] += pos0[i];
        vel[i] += vel0[i];
      }

      break;
    }

    default:
      return novas_error(2, EINVAL, fn, "invalid Solar-system body type: %d", body->type);
  }

  return 0;
}

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif
