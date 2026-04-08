/**
 * @file
 *
 *  Dummy readeph() implementation for SuperNOVAS to use when minor planet ephemeris is
 *  unavailable, but you want the functionality to be present in name only. It's not really
 *  useful as an implementation as is, but it can be used as a basis for an actual legacy
 *  NOVAS C module.
 *
 *  SuperNOVAS provides an improved method for setting more capable ephemeris provider
 *  functions dynamically, at runtime. So, unless you readily have a legacy readeph()
 *  implementation to use, we recommend you implement an novas_ephem_provider function, and
 *  activate it in your application with set_ephem_provider instead.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  http://www.usno.navy.mil/USNO/astronomical-applications
 *
 * @author G. Kaplan and Attila Kovacs
 *
 * @sa readeph(), set_ephem_provider(), novas_ephem_provider
 */
#include <stdlib.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"


/**
 * Returns a zeroed pv[] array, with an error code of 9 indicating that it's not real data.
 * This is basically the NOVAS C 3.1 readeph0.c implementation with added comments and
 * error handling.
 *
 * It can be used a a template for an actual implementation for minor planets, which are not
 * handled by the solarsystem() type calls. You can set the built-in implementation for the
 * library at build-time by setting the DEFAULT_READEPH variable in the `config.mk`.
 *
 * @param mp            The ID number of the solar-system body for which the position are
 *                      desired. An actual implementation might use this and/or the name to
 *                      identify the object.
 * @param name          The name of the solar-system body (usually upper-case). An actual
 *                      implementation might use this and/or `mp` to identify the object.
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date for which to
 *                      find the position and velocity.
 * @param[out] error    Pointer to integer to populate with the error status: 0 if successful,
 *                      -1 if any of the pointer arguments are NULL, or some non-zero value
 *                      if the was an error s.t. the position and velocity vector should not
 *                      be used, or else any non-zero value to indicate particular error
 *                      conditions.
 * @return              [AU, AU/day] A newly allocated 6-vector in rectangular equatorial
 *                      coordinates, containing the heliocentric position coordinates in AU,
 *                      followed by hte heliocentric velocity components in AU/day. The caller
 *                      is responsible for calling free() on the returned value when it is no
 *                      longer needed.
 *
 * @sa readeph()
 * @sa set_ephem_provider()
 * @sa novas_ephem_provider
 * @sa NOVAS_EPHEM_OBJECT
 *
 */
double *readeph(int mp, const char *restrict name, double jd_tdb, int *restrict error) {
  static const char *fn = "readeph_dummy";
  double *pv;

  // Check that pointers arguments are not NULL.
  if(!name || !error) {
    novas_set_errno(EINVAL, fn, "NULL parameter: name=%p, error=%p", name, error);
    if(error) *error = -1;
    return NULL;
  }

  // Check that TDB is not NAN.
  if(isnanf(jd_tdb)) {
    novas_set_errno(EINVAL, fn, "NaN jd_tdb");
    *error = -1;
    return NULL;
  }

  // Dynamically allocated return value
  pv = (double*) calloc(6, sizeof(double));
  if(pv) {
    // An actual implementation would populate the position and velocity components
    // of pv[] here, and set the value in error to 0 to indicate successful return
    // or else to another appropriate value if no valid position / velocity vector
    // is provided.

    // But since this is just a dummy example, with no really valid data, we'll
    // set the value in error to the NOVAS C 3.1 legacy error code for readeph0.c
    *error = 9;
  }
  else {
    novas_set_errno(errno, fn, "alloc error");
    *error = -1;       // allocation error.
  }

  return pv;
}
