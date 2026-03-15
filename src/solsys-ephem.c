/**
 * @file
 *
 * Major planet ephemeris lookup via the function, which was configured by set_ephem_provider().
 *
 * @date Created  on Jan 29, 2024
 * @author Attila Kovacs
 *
 * @sa ephemeris.c, solsys-calceph.c, solsys-cspice.c, orbital.c
 */

#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif


/**
 * Major planet ephemeris data via the same generic ephemeris provider that is configured by
 * set_ephem_provider() prior to calling this routine. This is the highest precision version.
 *
 * @param jd_tdb         [day] Barycentric Dynamical Time (TDB) based Julian date, split into
 *                       high and low order components (e.g. integer and fractional parts) for
 *                       high-precision calculations.
 * @param body           Major planet number (or that for Sun, Moon, SSB...)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                       report positions and velocities.
 * @param[out] position  [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param[out] velocity  [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code of solarsystem_hp().
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa planet_ephem_provider(), set_ephem_provider()
 * @sa solarsystem_hp()
 */
short planet_ephem_provider_hp(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity) {
  static const char *fn = "planet_ephem_provider_hp";
  static const char *names[] = NOVAS_PLANET_NAMES_INIT;

  novas_ephem_provider ephem_call = get_ephem_provider();
  enum novas_origin o = NOVAS_BARYCENTER;

  if(!ephem_call)
    return novas_error(1, EAGAIN, fn, "No ephem provider. Use set_ephem_provider() to specify one.");

  if(origin != NOVAS_BARYCENTER && origin != NOVAS_HELIOCENTER)
    return novas_error(1, EINVAL, fn, "invalid origin: %d", origin);

  if((unsigned) body >= NOVAS_PLANETS)
    return novas_error(-1, EINVAL, fn, "planet number %d out of range [0:%d]", body, NOVAS_PLANETS - 1);

  prop_error(fn, ephem_call(names[body], body, jd_tdb[0], jd_tdb[1], &o, position, velocity), 0);

  if(o != origin) {
    double pos0[3] = {0.0}, vel0[3] = {0.0};
    int i;
    int ref = (o == NOVAS_BARYCENTER) ? NOVAS_SUN : NOVAS_SSB;

    prop_error(fn, ephem_call(names[ref], ref, jd_tdb[0], jd_tdb[1], &o, pos0, vel0), 0);

    for(i = 0; i < 3; i++) {
      if(position)
        position[i] -= pos0[i];
      if(velocity)
        velocity[i] -= vel0[i];
    }
  }

  return 0;
}

/**
 * Major planet ephemeris data via the same generic ephemeris provider that is configured by
 * set_ephem_provider() prior to calling this routine. This is the regular (reduced) precision
 * version, but in reality it's exactly the same as the high-precision version, except for the
 * way the TDB-based Julian date is specified.
 *
 * @param jd_tdb         [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param body           Major planet number (or that for Sun, Moon, SSB...)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                       report positions and velocities.
 * @param[out] position  [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param[out] velocity  [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code defined by novas_planet_provider.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa planet_ephem_provider_hp(), set_ephem_provider()
 */
short planet_ephem_provider(double jd_tdb, enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity) {
  const double jd_tdb2[2] = { jd_tdb };
  prop_error("planet_ephem_provider", planet_ephem_provider_hp(jd_tdb2, body, origin, position, velocity), 0);
  return 0;
}

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif
