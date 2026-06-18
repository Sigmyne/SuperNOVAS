/**
 * @file
 *
 * @date Created  on May 8, 2026
 * @author Attila Kovacs
 * @since 1.7
 *
 *  Set of functions to check for effective equality between SuperNOVAS data sturctures within
 *  typical tolerances.
 */

#include <math.h>
#include <string.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"

/// \cond PRIVATE
#define CMP_DEG                   (1e-6 / 3600.0)        ///< [deg] Compare degrees to 1 &mu;as precision.
#define CMP_RAD                   ( CMP_DEG * DEGREE )   ///< [rad] Compare radians to 1 &mu;as precision.
/// \endcond


static int novas_equals_double(double a, double b, double tol) {
  return fabs(a - b) <= fabs(tol);
}

static int novas_equals_deg(double a, double b) {
  return fabs(remainder(a - b, DEG360)) <= fabs(CMP_DEG);
}


/**
 * Checks if two 3D vectors are effectively the same, within the specified absolute tolerance. Two
 * vectors are equal if the distance between them is less than or equal to the magnitude of the
 * specified tolerance.
 *
 * NOTES:
 *
 *  - Two NULL (undefined) vectors are considered not equal.
 *
 *  - A vector may not equal itself if it contains NAN or infinite components, since
 *    `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a       one of the vectors
 * @param b       the other vector
 * @param tol     absolute tolerance (sign is ignored).
 * @return        TRUE (1) if all the two vectors match within the specified tolerance, or else FALSE
 *                (0)
 *
 * @since 1.7
 * @author Attila Kovacs
 */
int novas_equals_vector(const double *a, const double *b, double tol) {
  double d2 = 0.0;
  int i;

  if(!a || !b)
    return 0;

  for(i = 0; i < 3; i++) {
    double d = a[i] - b[i];
    d2 += d * d;
  }

  return d2 <= tol * tol;
}

/**
 * Checks if two time specifications are the same within 10<sup>-7</sup> days (~100 &mu;s).
 *
 * NOTES:
 *
 *  - Two NULL (undefined) time specifications are considered not equal.
 *
 *  - A time specification may not equal itself if it contains NAN or infinite components,
 *    since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   one of the time specs
 * @param b   the other time spec
 * @return    TRUE (1) if the two time specifications match to within 100 &mu;s, or else FALSE
 *            (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 */
int novas_equals_timespec(const novas_timespec *a, const novas_timespec *b) {
  if(!a || !b)
    return 0;

  if(!novas_time_equals(remainder(a->fjd_tt, 1.0), remainder(b->fjd_tt, 1.0))) // [0.1 * us]
    return 0;
  if(!novas_time_equals(a->ijd_tt + a->fjd_tt, b->ijd_tt + b->fjd_tt))
    return 0;
  if(!novas_equals_double(a->ut1_to_tt, b->ut1_to_tt, 1e-7)) // [0.1 * us]
    return 0;
  if(!novas_equals_double(a->dut1, b->dut1, 1e-7)) // [0.1 * us]
    return 0;
  if(!novas_equals_double(a->tt2tdb, b->tt2tdb, 1e-7)) // [0.1 * us]
    return 0;

  return 1;
}

/**
 * Checks if two geodetic locations, and the weather parameters defined for them, match. For two
 * `on_surface` structures to be equal, they must have matching:
 *
 *  - longitude and latitude angles to within 1 &mu;as.
 *  - altitudes within 1 mm.
 *  - temperatures within 1 mK.
 *  - pressures within 1 &mu;bar
 *  - humidities within 0.001 %
 *
 * NOTES:
 *
 *  - Two NULL (undefined) structures are considered not equal.
 *
 *  - A geodetic location + local weather structure may not equal itself if it contains
 *    NAN or infinite components, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   one geodetic location (with weather)
 * @param b   another geodetic location (with weather)
 * @return    TRUE (1) if the two locations and their weather parameters match within the
 *            described tolerances, or else FALSE (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_equals_observer()
 */
int novas_equals_on_surface(const on_surface *a, const on_surface *b) {
  if(!a || !b)
    return 0;

  if(!novas_equals_deg(a->longitude, b->longitude))
    return 0;
  if(!novas_equals_double(a->latitude, b->latitude, CMP_DEG))
    return 0;
  if(!novas_equals_double(a->height, b->height, 1e-3))
    return 0;
  if(!novas_equals_double(a->temperature, b->temperature, 1e-3))
    return 0;
  if(!novas_equals_double(a->pressure, b->pressure, 1e-3))
    return 0;
  if(!novas_equals_double(a->humidity, b->humidity, 1e-3))
    return 0;

  return 1;
}

/**
 * Checks if two near-Earth locations and motions match within 1 mm and 1 mm/s, respectively. Such
 * near-Earth data structures are used by airborne observers or observers in Earth orbit.
 *
 * NOTES:
 *
 *  - Two NULL (undefined) structures are considered not equal.
 *
 *  - A near-Earth position/velocity structure may not equal itself if it contains NAN or
 *    infinite components, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   one near-Earth position/velocity data structure
 * @param b   another near-Earth position/velocity data structure
 * @return    TRUE (1) if the two near-Earth positions and velocities match within 1 mm and
 *            1 mm/s, respectively.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_equals_ssb_posvel(), novas_equals_observer()
 */
int novas_equals_near_earth(const in_space *a, const in_space *b) {
  if(!a || !b)
    return 0;

  if(!novas_equals_vector(a->sc_pos, b->sc_pos,  1e-6))         // [mm]
    return 0;
  if(!novas_equals_vector(a->sc_vel, b->sc_vel,  1e-6))         // [mm/s]
    return 0;

  return 1;
}

/**
 * Checks if two Solar-system locations and motions match within 1 m and ~1 mm/s, respectively.
 * Such near-Earth data structures are used by observers defined relative to the Solar-System
 * Barycenter (SSB).
 *
 * NOTES:
 *
 *  - Two NULL (undefined) structures are considered not equal.
 *
 *  - A Solar-system position / velocity structure may not equal itself if it contains NAN
 *    or infinite components, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   one Solar-system position/velocity data structure
 * @param b   another Solar-system position/velocity data structure
 * @return    TRUE (1) if the two SSB positions and velocities match within 1 m and ~1 mm/s,
 *            respectively.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_equals_near_earth(), novas_equals_observer()
 */
int novas_equals_ssb_posvel(const in_space *a, const in_space *b) {
  if(!a || !b)
    return 0;

  if(!novas_equals_vector(a->sc_pos, b->sc_pos, 1.0 / NOVAS_AU))          // [m]
    return 0;
  if(!novas_equals_vector(a->sc_vel, b->sc_vel, 10.0 / NOVAS_AU))         // [~mm/s] 1e-4 km/s * 1e5 s / AU[m]
    return 0;

  return 1;
}

/**
 * Checks if two observers are essentially the same within the tolerances associated to their
 * defining components.
 *
 * NOTES:
 *
 *  - Two NULL (undefined) observers are considered not equal.
 *
 *  - An observer structure may not equal itself if it contains NAN or infinite
 *    components, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   an observer data structure
 * @param b   another observer data structure
 * @return    TRUE (1) if the two data structures describe effectively the same observer location,
 *            within the typical tolerances associated to them, or else FALSE (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 */
int novas_equals_observer(const observer *a, const observer *b) {
  if(!a || !b)
    return 0;

  if(a->where != b->where)
    return 0;

  switch(a->where) {
    case NOVAS_OBSERVER_AT_GEOCENTER:
      return 1;
    case NOVAS_OBSERVER_ON_EARTH:
      return novas_equals_on_surface(&a->on_surf, &b->on_surf);
    case NOVAS_AIRBORNE_OBSERVER:
      return novas_equals_on_surface(&a->on_surf, &b->on_surf) && novas_equals_near_earth(&a->near_earth, &b->near_earth);
    case NOVAS_OBSERVER_IN_EARTH_ORBIT:
      return novas_equals_near_earth(&a->near_earth, &b->near_earth);
    case NOVAS_SOLAR_SYSTEM_OBSERVER:
      return novas_equals_ssb_posvel(&a->near_earth, &b->near_earth);
    default:
      return 0;
  }

  return 1;
}

/**
 * Checks if two catalog entries define the same parameters, within the typical tolerances
 * associated to these. For two catalog entries to be equals, they must have matching
 *
 *  - names (case sensitive)
 *  - catalog IDs (case sensitive)
 *  - numerical IDs
 *  - coordinates within 1 &mu;as.
 *  - proper motions to within 1 &mu;as / century.
 *  - parallaxes to within 1 ppm of their geometric mean.
 *  - radial velocities to withing 1 mm/s.
 *
 * NOTES:
 *
 *  - Two NULL (undefined) catalog entries are considered not equal.
 *
 *  - A catalog entry structure may not equal itself if it contains NAN or infinite
 *    components, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   one of the catalog entries
 * @param b   another catalog entry
 * @return    TRUE (1) if the two catalog entries define the same sidereal source effectively,
 *            or else FALSE (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_equals_object()
 */
int novas_equals_cat_entry(const cat_entry *a, const cat_entry *b) {
  if(!a || !b)
    return 0;

  if(strncmp(a->starname, b->starname, SIZE_OF_OBJ_NAME))
    return 0;
  if(strncmp(a->catalog, b->catalog, SIZE_OF_CAT_NAME))
    return 0;
  if(a->starnumber != b->starnumber)
    return 0;
  if(!novas_equals_deg(15.0 * a->ra, 15.0 * b->ra))
    return 0;
  if(!novas_equals_double(a->dec, b->dec, CMP_DEG))
    return 0;
  if(!novas_equals_double(a->promora, b->promora, 1e-6))    // [uas / cy]
    return 0;
  if(!novas_equals_double(a->promodec, b->promodec, 1e-6))  // [uas / cy]
    return 0;
  if(!novas_equals_double(a->parallax, b->parallax, 1e-6 * sqrt(fabs(a->parallax * b->parallax))))  // 1 ppm
    return 0;
  if(!novas_equals_double(a->radialvelocity, b->radialvelocity, 1e-6))  // [mm/s]
    return 0;

  return 1;
}

/**
 * Checks if two orbital systems match within typical tolerances. Two orbital system are
 * considered equal, if they are defined with respect to the same reference plane, around the same
 * major planet (or Solar-system position), and have the obliquity and ascending node (if
 * obliquity is non-zero) to within 1 &mu;as.
 *
 * NOTES:
 *
 *  - Two NULL (undefined) orbital systems are considered not equal.
 *
 *  - An orbital system structure may not equal itself if it contains NAN or infinite
 *    components, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   one of the orbital systems
 * @param b   another orbital system
 * @return    TRUE (1) if the two systems are effectively the same, within tolerances,
 *            or else FALSE (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_equals_orbital(), novas_equals_object()
 */
int novas_equals_orbsys(const novas_orbital_system *a, const novas_orbital_system *b) {
  if(!a || !b)
    return 0;

  if(a->center != b->center)
    return 0;
  if(a->plane != b->plane)
    return 0;
  if(a->type != b->type)
    return 0;
  if(!novas_equals_deg(a->obl, b->obl))
    return 0;
  if(sqrt(fabs(a->obl * b->obl)) > CMP_DEG && !novas_equals_deg(a->Omega, b->Omega))
    return 0;

  return 1;
}

/**
 * Checks if two Keplerian orbitals match, within typical tolerances. For two orbitals to be
 * considered equal, they must have matching orbital systems, have the same reference dates
 * to within ~10 ms (see `novas_time_equals()`), and:
 *
 *   - angular parameters must match to within 1 &mu;as.
 *   - semi-major axis must match to within 1 m.
 *   - eccentricity must match to within 10<sup>-12</sup> time the geometric mean of the two
 *     semi-major axes.
 *   - mean motion must match to within 1 &mu;as / cy.
 *   - apsis and node periods must match to within ~10 ms (see `novas_time_equals()`)
 *
 * NOTES:
 *
 *  - Two NULL (undefined) orbitals are considered not equal.
 *
 *  - Sn orbital elements structure may not equal itself if it contains NAN or infinite
 *    components, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   one set of Keplerian orbital parameters
 * @param b   another set of Keplerian orbital parameters
 * @return    TRUE (1) if the two Keplerian orbitals are effectively the same, within
 *            tolerances, or else FALSE (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_equals_object()
 */
int novas_equals_orbital(const novas_orbital *a, const novas_orbital *b) {
  if(!a || !b)
    return 0;

  if(!novas_equals_orbsys(&a->system, &b->system))
    return 0;
  if(!novas_time_equals(a->jd_tdb, b->jd_tdb))
    return 0;
  if(!novas_equals_double(a->a, b->a, 1.0 / NOVAS_AU))    // [m]
    return 0;
  if(!novas_equals_deg(a->M0, b->M0))
    return 0;
  if(!novas_equals_double(a->n, b->n, 1e-3 * CMP_DEG))    // [< uas/yr]
    return 0;
  if(!novas_equals_double(a->e, b->e, 1e-12 * sqrt(a->a * b->a)))
    return 0;
  if(sqrt(fabs(a->e * b->e)) > 1e-12 && !novas_equals_deg(a->omega, b->omega))
    return 0;
  if(!novas_equals_deg(a->i, b->i))
    return 0;
  if(sqrt(fabs(a->i * b->i)) > CMP_DEG && !novas_equals_deg(a->Omega, b->Omega))
    return 0;
  if(!novas_time_equals(a->apsis_period, b->apsis_period))
    return 0;
  if(!novas_time_equals(a->node_period, b->node_period))
    return 0;

  return 1;
}

/**
 * Checks if two astronomical targets are the same within typical tolerances. Two targets are
 * considered equals only if their types, names (case sensitive), numerical IDs, and defining
 * parameters match within typical tolerances.
 *
 * NOTES:
 *
 *  - Two NULL (undefined) objects are considered not equal.
 *
 *  - An astronomical target structure may not equal itself if it contains NAN or infinite
 *    components, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a   an astronomical target
 * @param b   another astronomical target
 * @return    TRUE (1) if the two targets are effectively the same, within tolerances, or else
 *            FALSE (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 */
int novas_equals_object(const object *a, const object *b) {
  if(!a || !b)
    return 0;

  if(a->type != b->type)
    return 0;

  if(strncmp(a->name, b->name, SIZE_OF_OBJ_NAME))
    return 0;

  if(a->number != b->number)
    return 0;

  switch(a->type) {
    case NOVAS_PLANET:
    case NOVAS_EPHEM_OBJECT:
      return 1;
    case NOVAS_CATALOG_OBJECT:
      return novas_equals_cat_entry(&a->star, &b->star);
    case NOVAS_ORBITAL_OBJECT:
      return novas_equals_orbital(&a->orbit, &b->orbit);
    default:
      return 0;
  }

  return 1;
}

/**
 * Checks if two apparent positions on the observer's sky are the same within typical tolerances.
 * Two sky positions are considered to be equal if their:
 *
 *  - coordinates match within 1 &mu;as,
 *  - distances match within 1 ppm.
 *  - radial velocities match within 1 mm/s.
 *  - unit vectors match to 10<sup>-12</sup> (~1 &mu;as).
 *
 * NOTES:
 *
 *  - Two NULL (undefined) sky positions are considered not equal.
 *
 *  - A sky position may not equal itself if it contains NAN or infinite components, since
 *    `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a     a position on the observer's sky
 * @param b     another sky position
 * @return      TRUE (1) if the two positions are essentially the same within typical tolerances,
 *              or else FALSE (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 */
int novas_equals_sky_pos(const sky_pos *a, const sky_pos *b) {
  if(!a || !b)
    return 0;

  if(!novas_equals_deg(15.0 * a->ra, 15.0 * b->ra))
    return 0;
  if(!novas_equals_double(a->dec, b->dec, CMP_DEG))
    return 0;
  if(!novas_equals_double(a->dis, b->dis, 1e-6 * sqrt(fabs(a->dis * b->dis))))  // [1 ppm]
    return 0;
  if(!novas_equals_double(a->rv, b->rv, 1e-6))  // [1 mm/s]
    return 0;
  if(!novas_equals_vector(a->r_hat, b->r_hat, 1e-12))  // [~1 &mu;as]
    return 0;

  return 1;
}

/**
 * Checks if two bundles containing Solar-system baricentric planet positions and velocities are
 * effectively the same. The two bundles are considered equals if:
 *
 *  - they contain data for the same set of planets.
 *  - the planets with data have positions matching within 1 m.
 *  - the planets with data have velocities matching within 1 mm/s.
 *
 * NOTES:
 *
 *  - Two NULL (undefined) planet bundles are considered not equal.
 *
 *  - A planet bundle may not equal itself if it contains NAN or infinite components for the
 *    planets they are supposed to have data for, since `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a     a planet position / velocity bundle
 * @param b     another planet position / velocity bundle
 * @return      TRUE (1) if the two bundles are essentially the same within tolerances, or else
 *              FALSE (0).
 *
 * @since 1.7
 * @author Attila Kovacs
 */
int novas_equals_planet_bundle(const novas_planet_bundle *a, const novas_planet_bundle *b) {
  int i;

  if(!a || !b)
    return 0;

  for(i = 0; i < NOVAS_PLANETS; i++) {
    if((a->mask & (1 << i)) != (b->mask & (1 << i)))
      return 0;
  }

  for(i = 0; i < NOVAS_PLANETS; i++)
    if(a->mask & (1 << i)) {
      if(!novas_equals_vector(&a->pos[i][0], &b->pos[i][0], 1.0 / NOVAS_AU))     // [m]
        return 0;
      if(!novas_equals_vector(&a->vel[i][0], &b->vel[i][0], 10.0 / NOVAS_AU))    // [~mm/s] 1e-4 km/s * 1e5 s / AU[m]
        return 0;
    }

  return 1;
}

static int is_geodetic(const observer *obs) {
  return obs->where == NOVAS_OBSERVER_ON_EARTH || obs->where == NOVAS_AIRBORNE_OBSERVER;
}

/**
 * Checks if two observing frames are essentially the same, within typical tolerances. Two frames
 * are considered equal, if they are both:
 *
 *  - initialized,
 *  - represent the same accuracy,
 *  - for the same observer,
 *  - at the same time,
 *  - same polar offsets (for geodetic observers or if defined),
 *  - and contain the same planetary ephemeris data
 *
 * all within the typical tolerances associated to these. For non-geodetic observers two frames
 * are considered equal also if both have all NAN (undefined) polar offsets -- since polar offsets
 * are not generally required for non-geodetic observing frames.
 *
 * NOTES:
 *
 *  - this function does not check the calculated fields of the observing frames, which are
 *    assumed to have been left untouched after the called to `novas_make_frame()`. If the user
 *    modifies the calculated fields, they may need additional checks to ensure 'equality'
 *    accordingly.
 *
 *  - Two NULL (undefined) observing frames are considered not equal.
 *
 *  - An observing frame may not equal itself if it contains NAN or infinite components, since
 *    `NAN != NAN` and `INFINITE != INFINITE`.
 *
 * @param a     an observing frame
 * @param b     another observing frame
 * @return      TRUE (1) if the two observing frames are effectively the same, given the
 *              tolerances, or else false.
 *
 * @since 1.7
 * @author Attila Kovacs
 */
int novas_equals_frame(const novas_frame *a, const novas_frame *b) {
  if(!a || !b)
    return 0;

  if(!novas_frame_is_initialized(a) || !novas_frame_is_initialized(b))
    return 0;
  if(a->accuracy != b->accuracy)
    return 0;
  if(!novas_equals_timespec(&a->time, &b->time))
    return 0;
  if(!novas_equals_observer(&a->observer, &b->observer))
    return 0;
  if(!novas_equals_planet_bundle(&a->planets, &b->planets))
    return 0;

  // Allow all NAN dx/dy for non-geodetic observers
  if(is_geodetic(&a->observer) || !isnan(a->dx) || !isnan(b->dx) || !isnan(a->dy) || !isnan(b->dy)) {
    if(!novas_equals_double(a->dx, b->dx, 1e-3))    // [uas]
      return 0;
    if(!novas_equals_double(a->dy, b->dy, 1e-3))    // [uas]
      return 0;
  }

  return 1;
}
