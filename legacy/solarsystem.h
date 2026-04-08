/**
 * @file
 *
 * Legacy NOVAS C header, with the solarsystem() / solarsystem_hp() prototypes.
 *
 * @author Attila Kovacs and G. Kaplan
 */

#ifndef _SOLSYS_
#define _SOLSYS_

/// \cond PRIVATE
#if __STDC_VERSION__ < 199901L
#  define restrict                        ///< No 'restrict' keyword prior to C99
#endif
/// \endcond

#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated (legacy function</i>) Use `set_planet_provider()` instead to specify what
 *             function should be used to calculate ephemeris positions for major planets.
 *
 * Legacy NOVAS C function for obtaining planet position / velocity data.  This function can be
 * defined by external modules, as per the NOVAS C way, provided you compile SuperNOVAS with the
 * `SOLSYS_SOURCE` option set to specify the source(s) that implement it (in `config.mk` or the
 * environment). If `SOLSYS_SOURCE` in not defined during the build of SuperNOVAS, then this
 * function will be provided by the currently configured `novas_planet_provider` function, such as
 * `earth_sun_calc()` if not configured otherwise.
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param body          Major planet number (or that for the Sun, Moon, or an appropriate
 *                      barycenter), as defined by enum novas_planet, e.g. NOVAS_MARS
 *                      (4), NOVAS_SUN (10) or NOVAS_SSB (0). (For compatibility
 *                      with existing NOVAS C compatible user implementations, we keep the
 *                      original NOVAS C argument type here).
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing NOVAS
 *                      C compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), 1 if the input Julian date ('tjd') is out of
 *                      range, 2 if 'body' is invalid, or 3 if the ephemeris data cannot be
 *                      produced for other reasons.
 *
 * @sa novas_planet, solarsystem_hp(), set_planet_provider(), ephemeris()
 * @sa novas_sky_pos(), novas_geom_posvel()
 */
short solarsystem(double jd_tdb, short body, short origin, double *restrict position, double *restrict velocity);
#endif

#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated (_legacy function_) Use `set_planet_provider_hp()` instead to specify what
 *             function should be used to calculate high-precision ephemeris positions for major
 *             planets.
 *
 * Legacy NOVAS C function for obtaining high-precision planet position / velocity data. This
 * function can be defined by external modules, as per the NOVAS C way, provided you compile
 * SuperNOVAS with the `SOLSYS_SOURCE` option set to specify the source(s) that implement it (in
 * `config.mk` or the environment). If `SOLSYS_SOURCE` in not defined during the build of
 * SuperNOVAS, then this function will be provided by the currently configured
 * `novas_planet_provider_hp` function, such as `earth_sun_calc()_hp` if not configured otherwise.
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date, broken into
 *                      high and low order components, respectively. Typically, as the integer
 *                      and fractional parts for the highest precision.
 * @param body          Major planet number (or that for the Sun, Moon, or an appropriate
 *                      barycenter), as defined by enum novas_planet, e.g. NOVAS_MARS
 *                      (4), NOVAS_SUN (10) or NOVAS_SSB (0). (For compatibility with
 *                      existing NOVAS C compatible user implementations, we keep the original
 *                      NOVAS C argument type here).
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing NOVAS
 *                      C compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), or some other error code (NOVAS C was not very
 *                      consistent here...
 *
 * @sa solarsystem(), set_planet_provider_hp(), ephemeris()
 * @sa novas_sky_pos(), novas_geom_posvel(), grav_planets(), grav_undo_planets()
 */
short solarsystem_hp(const double jd_tdb[restrict 2], short body, short origin, double *restrict position, double *restrict velocity);
#endif

#endif /* _SOLSYS_ */
