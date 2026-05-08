/**
 * @file
 *
 * @date Created  on May 8, 2026
 * @author Attila Kovacs
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
#define CMP_DEG                   (1e-6 / 3600.0)   ///< [deg] Compare degrees to 1 &mu;as precison.
/// \endcond


static int novas_equals_double(double a, double b, double tol) {
  return fabs(a - b) <= fabs(tol);
}

/**
 * Checks if two 3D vectors are effectively the same, within the specified absolute tolerance. Two
 * NULL vectors are also considered equal.
 *
 * @param a       one of the vectors
 * @param b       the other vector
 * @param tol     absolute tolerance
 * @return        TRUE (1) if all components of the two vectors match within the specified
 *                tolerance, or else FALSE (0)
 *
 * @since 1.7
 * @author Attila Kovacs
 */
int novas_equals_vector(const double *a, const double *b, double tol) {
  int i;

  if(a == b)
    return 1;

  if(!a || !b)
    return 0;

  for(i = 0; i < 3; i++)
    if(!novas_equals_double(a[i], b[i], tol))
      return 0;

  return 1;
}

/**
 * Checks if two time specifications are the same within 100 &mu;s. Two NULL time specifications
 * are also considered equal.
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
  if(a == b)
    return 1;

  if(!a || !b)
    return 0;

  if(!novas_time_equals(a->ijd_tt + a->fjd_tt, b->ijd_tt + b->fjd_tt))
    return 0;
  if(!novas_time_equals(remainder(a->fjd_tt, 1.0), remainder(b->fjd_tt, 1.0))) // [0.1 * us]
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
 * In addition, two NULL structures are also considered equal.
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
  if(a == b)
    return 1;

  if(!a || !b)
    return 0;

  if(!novas_equals_double(remainder(a->longitude, DEG360), remainder(b->longitude, DEG360), CMP_DEG))
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
 * near-Earth data structures are used by airborne observers or observers in Earth orbit. Two NULL
 * structures are also considered equal.
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
  if(a == b)
    return 1;

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
 * Barycenter (SSB). Two NULL structures are also considered equal.
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
  if(a == b)
    return 1;

  if(!a || !b)
    return 0;

  if(!novas_equals_vector(a->sc_pos, b->sc_pos,  1.0 / NOVAS_AU))          // [m]
    return 0;
  if(!novas_equals_vector(a->sc_vel, b->sc_vel,  10.0 / NOVAS_AU))         // [~mm/s] 1e-4 km/s * 1e5 s / AU[m]
    return 0;

  return 1;
}

/**
 * Checks if two observers are essentially the same within the tolerances associated to their
 * defininf components. Two NULL observers are also considered equal.
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
  if(a == b)
    return 1;

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
 *  - parallaxes to within 10<sup>-4</sup> % of their geometric mean.
 *  - radial velocities to withing 1 mm/s.
 *
 * In addition, two NULL catalog entries are also considered equal.
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
  if(a == b)
    return 1;

  if(!a || !b)
    return 0;

  if(strncmp(a->starname, b->starname, SIZE_OF_OBJ_NAME))
    return 0;
  if(strncmp(a->catalog, b->catalog, SIZE_OF_CAT_NAME))
    return 0;
  if(a->starnumber != b->starnumber)
    return 0;
  if(!novas_equals_double(remainder(a->ra, 24.0), remainder(b->ra, 24.0), CMP_DEG / 15.0))
    return 0;
  if(!novas_equals_double(a->dec, b->dec, CMP_DEG))
    return 0;
  if(!novas_equals_double(a->promora, b->promora, 1e-6))    // [uas / cy]
    return 0;
  if(!novas_equals_double(a->promodec, b->promodec, 1e-6))  // [uas / cy]
    return 0;
  if(!novas_equals_double(a->parallax, b->parallax, 1e-5 * sqrt(fabs(a->parallax * b->parallax))))  // [0.1 %]
    return 0;
  if(!novas_equals_double(a->radialvelocity, b->radialvelocity, 1e-6))  // [mm/s]
    return 0;

  return 1;
}

/**
 * Checks if two orbital systems match within typical tolerances. Two orbital system are
   considered equal, if they are defined with respect to the same reference plane, around the same
   major planet (or Solar-system position), and have the obliquity and ascending node (if
   obliquity is non-zero) to within 1 &mu;as. Two NULL orbital systems are also considered equal.
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
  if(a == b)
    return 1;

  if(!a || !b)
    return 0;

  if(a->center != b->center)
    return 0;
  if(a->plane != b->plane)
    return 0;
  if(a->type != b->type)
    return 0;
  if(!novas_equals_double(remainder(a->obl, DEG360), remainder(b->obl, DEG360), CMP_DEG))
    return 0;
  if(fabs(a->obl) > CMP_DEG && !novas_equals_double(remainder(a->Omega, DEG360), remainder(b->Omega, DEG360), CMP_DEG))
    return 0;

  return 1;
}

#include <stdio.h>

/**
 * Checks if two Keplerian orbitals match, within typical tolerances. For two orbitals to be
 * considered equal, they must have matching orbital systems, have the same reference dates
 * to within ~10 ms (see `novas_time_equals()`), and:
 *
 *   - angular parameters must match to within 1 &mu;as.
 *   - semi-major axis must match to within 1 m.
 *   - eccentricity must match to within 10<sup>-12</sup> time the geometric mean of the two semi-major axes.
 *   - mean motion must match to within 1 &mu;as / cy.
 *   - apsis and node periods must match to within ~10 ms (see `novas_time_equals()`)
 *
 * In addition, two NULL orbitals are also considered equal.
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
  if(a == b)
    return 1;

  if(!a || !b)
    return 0;

  if(!novas_equals_orbsys(&a->system, &b->system))
    return 0;
  if(!novas_time_equals(a->jd_tdb, b->jd_tdb))
    return 0;
  if(!novas_equals_double(a->a, b->a, 1.0 / NOVAS_AU))    // [m]
    return 0;
  if(!novas_equals_double(remainder(a->M0, DEG360), remainder(b->M0, DEG360), CMP_DEG))
    return 0;
  if(!novas_equals_double(a->n, b->n, 1e-3 * CMP_DEG))    // [< uas/yr]
    return 0;
  if(!novas_equals_double(a->e, b->e, 1e-12 * sqrt(a->a * b->a)))
    return 0;
  if(fabs(a->e) > 1e-6 && !novas_equals_double(a->omega, b->omega, CMP_DEG))
    return 0;
  if(!novas_equals_double(remainder(a->i, DEG360), remainder(b->i, DEG360), CMP_DEG))
    return 0;
  if(fabs(a->i) > CMP_DEG && !novas_equals_double(remainder(a->Omega, DEG360), remainder(b->Omega, DEG360), CMP_DEG))
    return 0;
  if(!novas_time_equals(a->apsis_period, b->apsis_period))
    return 0;
  if(!novas_time_equals(a->node_period, b->node_period))
    return 0;

  return 1;
}

/**
 * Checks if two astronomical targets are the same within typical tolerances. Two targets are
 * considered equals only if their types, names (case sensitive), and defining parameters match
 * within tolerances. Two NULL objects are also considered equal.
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
  if(a == b)
    return 1;

  if(!a || !b)
    return 0;

  if(a->type != b->type)
    return 0;

  if(strncmp(a->name, b->name, SIZE_OF_OBJ_NAME))
    return 0;

  switch(a->type) {
    case NOVAS_PLANET:
    case NOVAS_EPHEM_OBJECT:
      return (a->number == b->number);
    case NOVAS_CATALOG_OBJECT:
      return novas_equals_cat_entry(&a->star, &b->star);
    case NOVAS_ORBITAL_OBJECT:
      return novas_equals_orbital(&a->orbit, &b->orbit);
    default:
      return 0;
  }

  return 1;
}


