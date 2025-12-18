/**
 * @file
 *
 * @date Created  on Dec 11, 2025
 * @author Attila Kovacs
 *
 *  This module implements self-contained calculations for the Moon's position, such as via
 *  Keplerian orbital approximation, and through a semi-analytical model by Chapront-Touze &amp;
 *  Chapront 1988 / Chapront &amp; Francou 2002, 2003.
 *
 *  In principle, the latter can predict the Moon's position to the 10-m level precision, but is quite
 *  expensive to calculate with around 35,000 sinusoidal terms. In SuperNOVAS we offer only a truncated
 *  version, with 100-m level precision (typically), using up to 3408 terms. And, one may opt to truncate
 *  further to obtain less precises results faster if needed.
 *
 *  REFERENCES:
 *  <ol>
 *   <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 *   <li>Chapront, J., Francou G., 2003, A&amp;A, 404, 735</li>
 *   <li>Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 *       https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 *  </ol>
 *
 *  @sa solsys-calceph.c, solsys-cspice.c, ephemeris.c
 */

#include <string.h>
#include <stdint.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif

#define ELP_DELTA       0.01      ///< [day] for cord estimate

typedef struct {
  double A;             ///< [arcsec|km] amplitude
  int8_t D;             ///< Mulitple for D
  int8_t F;             ///< Multiple for F
  int8_t l;             ///< Multiple for l
  int8_t l1;            ///< Multiple for l1
} elp_main_term;

typedef struct {
  float A;              ///< [arcsec|km] amplitude
  float phi;            ///< [deg] phase
  struct {
    int8_t D;           ///< Multiple for D
    int8_t F;           ///< Multiple for F
    int8_t l;           ///< Multiple for l
    int8_t l1;          ///< Multiple for l1
  } delaunay;
  int8_t planets[5];    ///< Multiples for Venus -> Saturn
  int8_t zeta;          ///< Multiple for &zeta;
} elp_pert_term;

typedef struct {
  double W1;            /// [rad] Moon mean ecliptic longitude (ELP2000)
  double W2;            /// [rad] Mean ecliptic longitude of Moon's periapsis (ELP2000)
  double W3;            /// [rad] Mean longitude of Moon's ascending node (ELP2000)
  double T;             /// [rad] Mean ecliptic longitude of Earth
  double omega1;        /// [rad] Mean ecliptic longitude of Earth perihelion (ELP2000)
} elp_mean_args;

// @formatter:off
/**
 * Table 1, from Chapront &amp; Francou (2002)
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/elpmpp02.pdf
 *
 * with corrections for DE406 (long term).
 */
static const double secular[5][5] = { //
        { 218.3166348972,  481266.4842736056,    -0.001901619444,       1.782125e-06,   -1.164722222e-08 }, // W1
        {  83.3533007472,    4067.6167758810,     -0.01062733166,   -1.258309167e-05,    5.916944444e-08 }, // W2
        { 125.0445350028,   -1935.5332170747,     0.001765663694,         2.0883e-06,   -9.961111111e-09 }, // W3
        { 100.4664273667,   35999.3728612000,   -5.611111111e-06,            2.5e-09,    4.166666667e-11 }, // T
        { 102.9373472694,       0.3225676167,    0.0001470180556,   -3.281666667e-08,    3.160833333e-09 }, // omega1
};
// @formatter:on

/// \endcond

/**
 * Returns the _k_<sup>th</sup> secular parameter at the specified time.
 *
 * @param k   [0:4] The index of the secular parameters (W1, W2, W3, T, omega)
 * @param t   [cy] Julian centuries since J2000
 * @return    [rad] The value of the secular parameter at the specified time.
 */
static double elp_arg(int k, double t) {
  int i;
  double sum = 0.0;

  for(i = 5; --i >= 0; ) {
    sum *= t;
    sum += secular[k][i];
  }

  // [deg] -> [rad].
  return sum * DEGREE;
}

/**
 * Returns the time derivative of the _k_<sup>th</sup> secular parameter at the specified
 * time.
 *
 * @param k   [0:4] The index of the secular parameters (W1, W2, W3, T, omega)
 * @param t   [cy] Julian centuries since J2000
 * @return    [rad/cy] The derivative of the secular parameter at the specified time.
 */
static double elp_prime(int k, double t) {
  int i;
  double sum = 0.0;

  for(i = 5; --i > 0; ) {
    sum *= t;
    sum += i * secular[k][i];
  }

  // [deg] -> [rad].
  return sum * DEGREE;
}

/**
 * Calculates the P,Q parameters for transforming position in the ELP of date to the mean inertial
 * ecliptic and equinox of J2000.
 *
 * @param t       [cy] Julian centuries since J2000
 * @param[out] P  Calculated P value
 * @param[out] Q  Calculated Q value
 */
static void get_PQ(double t, double *P, double *Q) {
  // @formatter:off
  *P = t * (  0.10180391e-4 + t * ( 0.47020439e-6 + t * (-0.5417367e-9 + t * (-0.2507948e-11 + t * 0.463486e-14))));
  *Q = t * (-0.113469002e-3 + t * ( 0.12372674e-6 + t * ( 0.1265417e-8 + t * (-0.1371808e-11 - t * 0.320334e-14))));
  // @formatter:on
}

/**
 * Calculates ELP2000 corrected secular parameters (W<sub>1</sub>, W<sub>2</sub>, W<sub>3</sub>, T, and
 * &omega;&prime;) from Chapront &amp; Francou 2002.
 *
 * @param t         [cy] Julian centuries from J2000.
 * @param elp       ELP2000 secular parameters, corected for DE405 fitted values.
 * @param delaunay  Delaunay arguments, corrected for ELP2000 / DE405 fitted values.
 */
static void elp_args(double t, elp_mean_args *restrict elp, novas_delaunay_args *restrict delaunay) {
  // From Chapront, J., & Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
  // https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
  elp->W1      = elp_arg(0, t);
  elp->W2      = elp_arg(1, t);
  elp->W3      = elp_arg(2, t);
  elp->T       = elp_arg(3, t);
  elp->omega1  = elp_arg(4, t);

  // Chapront & Francou (2002), Eq. 3.
  delaunay->D = elp->W1 - elp->T + M_PI;
  delaunay->F = elp->W1 - elp->W3;
  delaunay->l = elp->W1 - elp->W2;
  delaunay->l1 = elp->T - elp->omega1;

  delaunay->Omega = elp->W3;
}

/**
 * Calculates the ELP main problem sine series for a coordinate.
 *
 * @param args      Delaunay args, corrected for ELP2000 / DE405 fitted values.
 * @param coeffs    ELP2000 Fourier series (multiples and sine coefficient).
 * @param n         Number of terms in Fourier series.
 * @param limit     [arcsec|km] limiting term amplitude for truncated series, or 0.0 for all
 *                  available terms.
 * @return          [arcsec|km] The calculated coordinate dU, V, or r.
 */
static double elp_sin(const novas_delaunay_args *restrict args, const elp_main_term *restrict coeffs, int n, double limit) {
  int i;
  double sum = 0.0;

  for(i = 0; i < n; i++) {
    const elp_main_term *c = &coeffs[i];
    double arg;

    if(fabs(c->A) < limit)
      break;

    arg = c->D * args->D + c->F * args->F + c->l * args->l + c->l1 * args->l1;
    sum += c->A * sin(arg);
  }

  return sum;
}

/**
 * Calculates the ELP main problem cosine series for a coordinate.
 *
 * @param args      Delaunay args, corrected for ELP2000 / DE405 fitted values.
 * @param coeffs    ELP2000 Fourier series (multiples and cosine coefficient).
 * @param n         Number of terms in Fourier series.
 * @param limit     [arcsec|km] limiting term amplitude for truncated series, or 0.0 for all
 *                  available terms.
 * @return          [arcsec|km] The calculated coordinate dU, V, or r.
 */
static double elp_cos(const novas_delaunay_args *restrict args, const elp_main_term *restrict coeffs, int n, double limit) {
  int i;
  double sum = 0.0;

  for(i = 0; i < n; i++) {
    const elp_main_term *c = &coeffs[i];
    double arg;

    if(fabs(c->A) < limit)
      break;

    arg = c->D * args->D + c->F * args->F + c->l * args->l + c->l1 * args->l1;
    sum += c->A * cos(arg);
  }

  return sum;
}

/**
 * Calculates the ELP2000 Poisson perturbation series for a coordinate.
 *
 * @param t         [cy] Julian centuries from J2000.
 * @param args      Delaunay arguments, corrected for ELP2000 / DE405 values.
 * @param planets   [rad] Planet longitudes from Venus [2] through Saturn [6].
 * @param zeta      [rad] &zeta;, see Chapront &amp; Francou 2002.
 * @param coeffs    Perturbation series (multiples and sine coefficient and phase).
 * @param n         Number of terms in perturbation series.
 * @param limit     [arcsec|km] limiting term amplitude for truncated series, or 0.0 for all
 *                  available terms.
 * @return          [arcsec|km] The calculate perturbation correction.
 */
static double elp_pert(double t, const novas_delaunay_args *restrict args, const double *restrict planets, double zeta,
        const elp_pert_term **restrict coeffs, const int *restrict n, double limit) {
  double sum = 0.0;
  double tn = 1.0;
  int k;

  for(k = 0; n[k] > 0; k++) {
    const elp_pert_term *ck = coeffs[k];
    double sum1 = 0.0;
    int i;

    for(i = 0; i < n[k]; i++) {
      const elp_pert_term *c = &ck[i];
      double arg;
      int m;

      if(fabs(tn * c->A) < limit)
        break;

      arg = c->delaunay.D * args->D + c->delaunay.F * args->F + c->delaunay.l * args->l + c->delaunay.l1 * args->l1;
      for(m = NOVAS_VENUS; m <= NOVAS_SATURN; m++)
        arg += c->planets[m - NOVAS_VENUS] * planets[m];
      arg += c->zeta * zeta;

      sum1 += c->A * sin(arg + c->phi * DEGREE);
    }

    sum += tn * sum1;
    tn *= t;
  }

  return sum;
}

#if !CPPCHECK
/**
 * Calculates the Moon's geocentric position using the ELP/MPP02 model by Chapront &amp; Francou
 * (2003), in the ELP2000 reference plane (i.e. the inertial ecliptic and equinox of J2000), down
 * to the specified limiting term amplitude.
 *
 * NOTES:
 * <ol>
 * <li>The initial implementation (in v1.6) truncates the full series, keeping only terms with
 * amplitudes larger than 1 mas (around 3400 harmonic terms in total), resulting in a limiting
 * accuracy below 1 km level (and less than 100 meter error typically for 1900 -- 2100).
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront, J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param limit     [arcsec|km] Sum only the harmonic terms with amplitudes larger than this
 *                  limit.
 * @param[out] pos  [AU] Output geocentric position vector w.r.t. the intertial ecliptic and equinox
 *                  of J2000.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_posvel(), novas_moon_elp_sky_pos()
 * @sa novas_make_moon_orbit()
 */
int novas_moon_elp_ecl_pos(double jd_tdb, double limit, double *pos) {
  static THREAD_LOCAL double last_tdb = NAN, last_limit = NAN, last_pos[3];

  /// \cond PRIVATE
#  include "elp2000/elp-lat.tab.c"
#  include "elp2000/elp-dis.tab.c"
#  include "elp2000/elp-lon.tab.c"
#  include "elp2000/elp-pert-lon.tab.c"
#  include "elp2000/elp-pert-lat.tab.c"
#  include "elp2000/elp-pert-dis.tab.c"
  /// \endcond

  const elp_pert_term *leading = &elp_plon0[0];
  const double t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;

  elp_mean_args elp = {};
  novas_delaunay_args args = {};
  double lon, lat, dis;
  double clat, p[3];
  double P = 0.0, Q = 0.0, S;
  const double s2 = sqrt(2.0);

  if(!pos)
    return novas_error(-1, EINVAL, "novas_moon_elp_ecl_pos", "output position vector is NULL");

  if(novas_time_equals(jd_tdb, last_tdb) && limit == last_limit) {
    memcpy(pos, last_pos, sizeof(last_pos));
    return 0;
  }

  // Delaunay args of ELP2000 (Chapront & Francou 2003).
  elp_args(t, &elp, &args);

  lon = elp.W1 + elp_sin(&args, elp_lon, elp_n_lon, limit) * ARCSEC;
  lat = elp_sin(&args, elp_lat, elp_n_lat, limit) * ARCSEC;
  dis = elp_cos(&args, elp_dis, elp_n_dis, limit) * NOVAS_KM / NOVAS_AU;

  if(limit < fabs(leading->A)) {
    double planets[NOVAS_NEPTUNE + 1] = {0.0};
    double zeta = elp.W1 + (5029.0966 - 0.29965) * ARCSEC * t;
    int i;

    // Perturbation only for Venus through Saturn.
    for(i = NOVAS_VENUS; i <= NOVAS_SATURN; i++)
      planets[i] = (i == NOVAS_EARTH) ? elp.T  : planet_lon(t, (enum novas_planet) i);

    lon += elp_pert(t, &args, planets, zeta, elp_plon, elp_n_plon, limit) * ARCSEC;
    lat += elp_pert(t, &args, planets, zeta, elp_plat, elp_n_plat, limit) * ARCSEC;
    dis += elp_pert(t, &args, planets, zeta, elp_pdis, elp_n_pdis, limit) * NOVAS_KM / NOVAS_AU;
  }

  // rescaling of ELP/MPP02
  dis *= 384747.961370173 / 384747.980674318;

  clat = cos(lat);

  // ELP of date coordinates
  p[0] = dis * clat * cos(lon);
  p[1] = dis * clat * sin(lon);
  p[2] = dis * sin(lat);

  // Transform to the mean ecliptic of J2000
  // Laskar 1986, A&A, 157, 59
  get_PQ(t, &P, &Q);

  // Include sqrt(2) for all terms, so products implicitly include the factor of 2
  // in all matrix elements.
  S = s2 * sqrt(1.0 - P * P - Q * Q);
  P *= s2;
  Q *= s2;

  memcpy(pos, p, sizeof(p));

  // @formatter:off
  pos[0] += (-P * P) * p[0] + (P * Q) * p[1]         + (P * S) * p[2];
  pos[1] +=  (P * Q) * p[0] - (Q * Q) * p[1]         - (Q * S) * p[2];
  pos[2] += (-P * S) * p[0] + (Q * S) * p[1] - (P * P + Q * Q) * p[2];
  // @formatter:on

  memcpy(last_pos, pos, sizeof(last_pos));
  last_tdb = jd_tdb;
  last_limit = limit;

  return 0;
}
#endif // CPPCHECK

/**
 * Calculates the Moon's geocentric velocity using the ELP/MPP02 model by Chapront &amp; Francou
 * (2003), in the ELP2000 reference plane (i.e. the inertial ecliptic and equinox of J2000), down
 * to the specified limiting term amplitude.
 *
 * NOTES:
 * <ol>
 * <li>The initial implementation (in v1.6) truncates the full series, keeping only terms with
 * amplitudes larger than 1 mas (around 3400 harmonic terms in total).
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront, J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param limit     [arcsec|km] Sum only the harmonic terms with amplitudes larger than this
 *                  limit.
 * @param[out] vel  [AU/day] Output geocentric velocity vector w.r.t. the intertial ecliptic and
 *                  equinox of J2000.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_posvel(), novas_moon_elp_sky_pos()
 * @sa novas_make_moon_orbit()
 */
int novas_moon_elp_ecl_vel(double jd_tdb, double limit, double *vel) {
  static THREAD_LOCAL double last_tdb = NAN, last_limit = NAN, last_vel[3];

  double p1[3] = {0.0}, p2[3] = {0.0};
  int k;

  if(!vel)
    return novas_error(-1, EINVAL, "novas_moon_elp_ecl_vel", "output velocity vector is NULL");

  novas_moon_elp_ecl_pos(jd_tdb - ELP_DELTA, limit, p1);
  novas_moon_elp_ecl_pos(jd_tdb + ELP_DELTA, limit, p2);

  // calculate velocity on cord
  for(k = 3; --k >= 0; )
    vel[k] = (p2[k] - p1[k]) / (2.0 * ELP_DELTA);

  if(novas_time_equals(jd_tdb, last_tdb) && limit == last_limit) {
    memcpy(vel, last_vel, sizeof(last_vel));
    return 0;
  }

  memcpy(last_vel, vel, sizeof(last_vel));
  last_tdb = jd_tdb;
  last_limit = limit;

  return 0;
}

/**
 * Convers an ICRS equatorial position vector to a vector in the specified celestial coordinate
 * reference system, at the specified time of observation
 *
 * @param tdb           [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param[in, out] v    [arb.u.] Vector to transform
 * @param sys           The desired output reference system. It may not be Earth-bound TIRS or
 *                      ITRS.
 * @return              0 if successful, or else -1 if the coordinate system is invalid (errno
 *                      is set to EINVAL).
 */
static int icrs_to_sys(double tdb, double *v, enum novas_reference_system sys) {
  static const char *fn = "icrs_to_sys";

  switch(sys) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      break;
    case NOVAS_J2000:
      gcrs_to_j2000(v, v);
      break;
    case NOVAS_MOD:
      gcrs_to_mod(tdb, v, v);
      break;
    case NOVAS_TOD:
      gcrs_to_tod(tdb, NOVAS_REDUCED_ACCURACY, v, v);
      break;
    case NOVAS_CIRS: {
      gcrs_to_cirs(tdb, NOVAS_REDUCED_ACCURACY, v, v);
      break;
    }
    default:
      return novas_error(-1, EINVAL, fn, "unsupported celestial coordinate reference system: %d.", (int) sys);
  }
  return 0;
}

/**
 * Checks that a frame is valid and is for an earth-bound observer (geocentric, on-Earth, or
 * airborne observer).
 *
 * @param frame     The frame to check
 * @return          0 if the frame is valid and is for an Earth-bound observer, or else -1
 *                  (errno set to EINVAL).
 */
static int check_earth_bound(const novas_frame *frame) {
  static const char *fn = "check_earth_bound";
  enum novas_observer_place where;

  if(!frame)
    return novas_error(-1, EINVAL, fn, "input frame is NULL");

  if(!novas_frame_is_initialized(frame))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  where = frame->observer.where;

  if(where != NOVAS_OBSERVER_AT_GEOCENTER && where != NOVAS_OBSERVER_ON_EARTH && where != NOVAS_AIRBORNE_OBSERVER)
    return novas_error(-1, EINVAL, fn, "observer type %d is not Earth-bound", (int) where);

  return 0;
}

/**
 * Returns the Moon's geometric position and velocity, relative to an Earth-based observer (or the
 * geocenter), using the ELP/MPP02 model by Chapront &amp; Francou (2003). Only terms larger than
 * the specified limit are used to provide a result with the desired precision.
 *
 * NOTES:
 * <ol>
 * <li>The initial implementation (in v1.6) truncates the full series, keeping only terms with
 * amplitudes larger than 1 mas (around 3400 harmonic terms in total), resulting in a limiting
 * accuracy below the 1 km level (and less than 100 m error typically for 1900 -- 2100).
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront, J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param time      Astrometric time of observation.
 * @param obs       Earth-based observer location, or NULL for geocentric.
 * @param limit     [arcsec|km] Sum only terms with amplitudes larger than this limit. The
 *                  resulting accuracy is typically an order-of-magnitude above the set limiting
 *                  amplitude.
 * @param sys       The celestial coordinate reference system in which to return the result. (It
 *                  may not be Earth-based TIRS or ITRS).
 * @param[out] pos  [AU] The Moon's position vector relative to the observer (or geocenter), in
 *                  the specified coordinate reference system, or NULL if not required.
 * @param[out] vel  [AU/day] The Moon's ICRS velocity vector relative to the observer (or
 *                  geocenter), in the specified coordinate reference system, or NULL if not
 *                  required.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_posvel()
 * @sa novas_moon_elp_sky_pos_fp()
 * @sa novas_make_moon_orbit()
 * @sa novas_geom_posvel()
 */
int novas_moon_elp_posvel_fp(const novas_timespec *restrict time, const on_surface *restrict obs, double limit,
        enum novas_reference_system sys, double *restrict pos, double *restrict vel) {
  static const char *fn = "novas_moon_elp_posvel_fp";

  double tdb;
  double opos[3] = {0.0}, ovel[3] = {0.0};
  enum novas_accuracy acc = limit < 1e-3 ? NOVAS_FULL_ACCURACY : NOVAS_REDUCED_ACCURACY;

  if(!time)
    return novas_error(-1, EINVAL, fn, "input time specification is NULL");

  if(!pos && !vel)
    return novas_error(-1, EINVAL, fn, "both output pos and vel are NULL");

  tdb = novas_get_time(time, NOVAS_TDB);
  if(isnan(tdb))
    novas_trace(fn, -1, 0);

  if(obs) {
    terra(obs, novas_time_gst(time, NOVAS_REDUCED_ACCURACY), opos, ovel);
    tod_to_gcrs(tdb, NOVAS_REDUCED_ACCURACY, opos, opos);
    tod_to_gcrs(tdb, NOVAS_REDUCED_ACCURACY, ovel, ovel);
  }

  if(pos) {
    int k;

    novas_moon_elp_ecl_pos(tdb, limit, pos);
    ecl2equ_vec(NOVAS_JD_J2000, NOVAS_GCRS_EQUATOR, acc, pos, pos);

    // w.r.t. observer
    for(k = 3; --k >= 0; )
      pos[k] -= opos[k];

    prop_error(fn, icrs_to_sys(tdb, pos, sys), 0);
  }

  if(vel) {
    int k;

    novas_moon_elp_ecl_vel(tdb, limit, vel);
    ecl2equ_vec(NOVAS_JD_J2000, NOVAS_GCRS_EQUATOR, acc, vel, vel);

    // w.r.t. observer
    for(k = 3; --k >= 0; )
      vel[k] -= ovel[k];

    prop_error(fn, icrs_to_sys(tdb, vel, sys), 0);
  }

  return 0;
}

/**
 * Returns the Moon's geometric position and velocity, relative to an Earth-based observer
 * (or the geocenter), using the ELP/MPP02 model by Chapront &amp; Francou (2003).
 *
 * NOTES:
 * <ol>
 * <li>The initial implementation (in v1.6) truncates the full series, keeping only terms with
 * amplitudes larger than 1 mas (around 3400 harmonic terms in total), resulting in a limiting
 * accuracy below the 1 km level (and less than 100 m error typically for 1900 -- 2100).
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront, J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param frame     Earth-based observing frame
 * @param sys       The celestial coordinate reference system in which to return the result. (It
 *                  may not be Earth-based TIRS or ITRS).
 * @param[out] pos  [AU] The Moon's position vector relative to the observer (or geocenter), in
 *                  the specified coordinate reference system, or NULL if not required.
 * @param[out] vel  [AU/day] The Moon's ICRS velocity vector relative to the observer (or
 *                  geocenter), in the specified coordinate reference system, or NULL if not
 *                  required.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error).
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_posvel_fp()
 * @sa novas_moon_elp_sky_pos()
 * @sa novas_make_moon_orbit()
 * @sa novas_geom_posvel()
 */
int novas_moon_elp_posvel(const novas_frame *restrict frame, enum novas_reference_system sys, double *restrict pos, double *restrict vel) {
  static const char *fn = "novas_moon_elp_posvel";

  const on_surface *loc = NULL;
  double limit;

  prop_error(fn, check_earth_bound(frame), 0);

  if(frame->observer.where !=  NOVAS_OBSERVER_AT_GEOCENTER)
    loc = &frame->observer.on_surf;

  limit = (frame->accuracy == NOVAS_REDUCED_ACCURACY) ? 1e-2 : 0.0;
  prop_error("novas_moon_elp_posvel", novas_moon_elp_posvel_fp(&frame->time, loc, limit, sys, pos, vel), 0);

  // For airborne observers subtract the ground velocity....
  if(frame->observer.where ==  NOVAS_AIRBORNE_OBSERVER) {
    int k;
    for(k = 3; --k >= 0; )
      vel[k] -= frame->observer.near_earth.sc_vel[k] * NOVAS_KMS / (NOVAS_AU / NOVAS_DAY);
  }

  return 0;
}

/**
 * Corrects the Moon's position for aberration for an Earth-based observer.
 *
 * @param time          Astrometric time (not NULL)
 * @param obs           Geodetic observer location (may be NULL).
 * @param v_ground      [km/s] Observer's velocity over the ground in the ITRS, or NULL if fixed
 *                      site location. It is unused if `obs` is NULL.
 * @param sys           Celestial coordinate reference system in which position is given.
 * @param[in,out] pos   [AU] Moon's position (in: geometric, out: aberration corrected).
 * @return              0
 */
static int moon_aberration(const novas_timespec *restrict time, const on_surface *restrict obs, const double *restrict v_ground,
        enum novas_reference_system sys, double *restrict pos) {

  const double pos0[3] = { pos[0], pos[1], pos[2] };
  const double kms_to_auday = NOVAS_KMS / (NOVAS_AU / NOVAS_DAY);

  double tdb = novas_get_time(time, NOVAS_TDB);
  double ovel[3] = {0.0}; // observer movement w.r.t. geocenter
  double d, vobs, beta, gamma, p, q, r;
  int k;

  if(!obs)
    return 0;

  // Earth rotation at observer location
  terra(obs, novas_time_gst(time, NOVAS_REDUCED_ACCURACY), NULL, ovel);

  // Add observer ground motion (in TOD).
  if(v_ground) {
    double vg[3] = {0.0};

    itrs_to_tod(tdb, 0.0, time->ut1_to_tt, NOVAS_REDUCED_ACCURACY, 0.0, 0.0, v_ground, vg);
    for(k = 3; --k >= 0; )
      ovel[k] += vg[k] * kms_to_auday;
  }

  if(sys != NOVAS_TOD) {
    // observer velocity in the desired coordinate system
    tod_to_gcrs(tdb, NOVAS_REDUCED_ACCURACY, ovel, ovel);
    icrs_to_sys(tdb, ovel, sys);
  }

  d = novas_vlen(pos);

  vobs = novas_vlen(ovel);
  beta = vobs / C_AUDAY;
  gamma = sqrt(1.0 - beta * beta);

  p = beta * novas_vdot(pos, ovel) / (d * vobs);
  q = (1.0 + p / (1.0 + gamma)) * d / C_AUDAY;
  r = 1.0 + p;

  // Geometric to apparent
  for(k = 3; --k >= 0; )
    pos[k] = (gamma * pos0[k] + q * ovel[k]) / r;

  return 0;
}

/**
 * Returns the Moon's apparent place, relative to an Earth-based observer (or the geocenter),
 * using the ELP/MPP02 model by Chapront &amp; Francou (2003). Only terms larger than the
 * specified limit are used to provide a result with the desired precision.
 *
 * NOTES:
 * <ol>
 * <li>The initial implementation (in v1.6) truncates the full series, keeping only terms with
 * amplitudes larger than 1 mas (around 3400 harmonic terms in total), resulting in a limiting
 * accuracy below the 1 arcsec level (and less than 0.1 arcsec or 100 m error typically for
 * 1900 -- 2100).
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront, J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param time      Astrometric time of observation.
 * @param obs       Earth-based observer location, or NULL for geocentric.
 * @param v_ground  [km/s] Observer's velocity over the ground in the ITRS, or NULL if fixed site
 *                  location. It is unused if `obs` in NULL.
 * @param limit     [arcsec|km] Sum only terms with amplitudes larger than this limit. The
 *                  resulting accuracy is typically an order-of-magnitude above the set limiting
 *                  amplitude.
 * @param sys       The celestial coordinate reference system in which to return the result. (It
 *                  may not be Earth-based TIRS or ITRS).
 * @param[out] pos  The Moon's position, relative to the true equator and equinox of date.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_sky_pos_fp()
 * @sa novas_moon_elp_posvel()
 * @sa novas_make_moon_orbit()
 * @sa novas_sky_pos()
 */
int novas_moon_elp_sky_pos_fp(const novas_timespec *restrict time, const on_surface *restrict obs, const double *restrict v_ground,
        double limit, enum novas_reference_system sys, sky_pos *restrict pos) {
  static const char *fn = "novas_moon_elp_skypos_fp";

  const double kms_to_auday = NOVAS_KMS / (NOVAS_AU / NOVAS_DAY);
  double p[3] = {0.0}, v[3] = {0.0};
  int k;

  if(!pos)
    return novas_error(-1, EINVAL, fn, "output position is NULL.");

  prop_error(fn, novas_moon_elp_posvel_fp(time, obs, limit, sys, p, v), 0);

  // Aberration correction
  moon_aberration(time, obs, v_ground, sys, p);

  vector2radec(p, &pos->ra, &pos->dec);
  pos->dis = novas_vlen(p);

  for(k = 3; --k >= 0; )
    pos->r_hat[k] = p[k] / pos->dis;

  pos->rv = novas_vdot(pos->r_hat, v) / kms_to_auday;

  return 0;
}

/**
 * Returns the Moon's apparent place, relative to an Earth-based observer (or the geocenter),
 * using the ELP/MPP02 model by Chapront &amp; Francou (2003).
 *
 * NOTES:
 * <ol>
 * <li>The initial implementation (in v1.6) truncates the full series, keeping only terms with
 * amplitudes larger than 1 mas (around 3400 harmonic terms in total), resulting in a limiting
 * accuracy below the 1 arcsec level (and less than 0.1 arcsec or 100 m error typically for
 * 1900 -- 2100).
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront, J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront, J., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param frame     Earth-based observing frame.
 * @param sys       The celestial coordinate reference system in which to return the result. (It
 *                  may not be Earth-based TIRS or ITRS).
 * @param[out] pos  The Moon's position, relative to the true equator and equinox of date.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_sky_pos_fp()
 * @sa novas_moon_elp_posvel()
 * @sa novas_make_moon_orbit()
 * @sa novas_sky_pos()
 */
int novas_moon_elp_sky_pos(const novas_frame *restrict frame, enum novas_reference_system sys, sky_pos *restrict pos) {
  static const char *fn = "novas_moon_elp_sky_pos";

  double limit = (frame->accuracy == NOVAS_REDUCED_ACCURACY) ? 1e-2 : 0.0;
  const on_surface *loc = NULL;
  const double *vg = NULL;

  prop_error(fn, check_earth_bound(frame), 0);

  if(frame->observer.where !=  NOVAS_OBSERVER_AT_GEOCENTER) {
    loc = &frame->observer.on_surf;
    if(frame->observer.where == NOVAS_AIRBORNE_OBSERVER)
      vg = frame->observer.near_earth.sc_vel;
  }

  prop_error("novas_moon_elp_skypos", novas_moon_elp_sky_pos_fp(&frame->time, loc, vg, limit, sys, pos), 0);
  return 0;
}

/**
 * Gets mean orbital elements for the Moon relative to the geocenter for the specified epoch
 * of observation. It is based on the secular parameters of the  ELP2000-85 model, not including
 * the harmonic series the perturbation terms. As such it has accuracy at the few degrees level
 * only, however it is 'valid' for long-term projections (i.e. for years around the orbit's
 * reference epoch) at that coarse level.
 *
 * For the short-term , `novas_make_moon_orbit()` can provide somewhat more accurate
 * predictions for up to a day or so around the reference epoch of the orbit.
 *
 * REFERENCES:
 * <ol>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M, and Chapront, J. 1988, Astronomy and Astrophysics,
 *      vol. 190, p. 342-352.</li>
 *  <li>Chapront J., &amp; Francou G., 2003, A&amp;A, 404, 735</li>
 *  <li>Laskar J., 1986, A&amp;A, 157, 59</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian Date.
 * @param[out] orbit  Orbital elements data structure to populate.
 * @return            0 if successful, or else -1 (`errno` set to `EINVAL`).
 *
 * @since 1.4
 * @author Attila Kovacs
 *
 * @sa novas_make_moon_orbit(), novas_make_planet_orbit(), make_orbital_object()
 * @sa novas_moon_elp_posvel(), novas_moon_elp_sky_pos()
 */
int novas_make_moon_mean_orbit(double jd_tdb, novas_orbital *restrict orbit) {
  novas_orbital def = NOVAS_ORBIT_INIT;
  double W1, t;
  double P, Q, pole[3] = {0.0}, dOmega;

  if(!orbit)
    return novas_error(-1, EINVAL, "novas_make_moon_orbit", "output orbital is NULL");

  // Default ecliptic orbital...
  memcpy(orbit, &def, sizeof(novas_orbital));

  orbit->system.center = NOVAS_EARTH;
  orbit->system.plane = NOVAS_ECLIPTIC_PLANE;
  orbit->system.type = NOVAS_J2000;

  // Values expressed for instant
  orbit->jd_tdb = jd_tdb;

  t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;

  // Mean inclination (leading latitude term of ELP02 series)
  orbit->i = 18461.24038 / 3600.0;

  // eccentricity (from the leading term of the ELP03 series)
  orbit->e = 20905.35494 / 385000.52906;

  // Chapront & Francou 2003
  // ELP/MPP02
  // https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/elpmpp02.pdf
  W1           = elp_arg(0, t) / DEGREE; // W1
  orbit->omega = elp_arg(1, t) / DEGREE; // W2
  orbit->Omega = elp_arg(2, t) / DEGREE; // W3

  orbit->M0 = W1 - orbit->omega;

  // apsis from rising node (omega = Omega - omega_bar)
  orbit->omega -= orbit->Omega;

  // [deg/cy] differentiate M0 above to get mean motion
  orbit->n = (elp_prime(0, t) - elp_prime(1, t)) / DEGREE;

  // From Chapront-Touze, M, and Chapront, J. 1983, A&A, 124, 1, p. 50-62.
  // (n^2 a^3 = constant).
  orbit->a = 3.84747980645e8 / NOVAS_AU * pow(secular[0][1] / orbit->n, 2.0 / 3.0);

  // [deg/cy] -> [deg/day]
  orbit->n /= JULIAN_CENTURY_DAYS;

  // differentiate omega above to get apsis motion
  orbit->apsis_period = JULIAN_CENTURY_DAYS * TWOPI / elp_prime(1, t);

  // differentiate Omega above to get node motion
  orbit->node_period = JULIAN_CENTURY_DAYS * TWOPI / elp_prime(2, t);

  // apsis w.r.t. the node.
  orbit->apsis_period -= orbit->node_period;

  // Transform from the mean ecliptic of date to the mean ecliptic of J2000
  // Laskar 1986, A&A, 157, 59
  pole[1] = -sin(orbit->i * DEGREE);
  pole[2] =  cos(orbit->i * DEGREE);

  get_PQ(t, &P, &Q);
  novas_Rx(-P, pole);
  novas_Ry(Q, pole);

  orbit->i = atan2(sqrt(pole[0] * pole[0] + pole[1] * pole[1]), pole[2]) / DEGREE;
  dOmega = atan2(pole[0], -pole[1]) / DEGREE;

  orbit->Omega += dOmega;
  orbit->omega -= dOmega;

  return 0;
}

/**
 * Gets an approximation of the `current` Keplerian orbital elements for the Moon relative to the
 * geocenter for the specified epoch of observation. The orbit includes the most dominant Solar
 * perturbation terms to produce results with an accuracy at the few arcmin level near (+- 0.5 days)
 * the reference time argument of the orbit. The perturbed orbit is based on the ELP/MPP02 model.
 *
 * While, the ELP/MPP02 model itself can be highly precise, the Moon's orbit is strongly
 * non-Keplerian, and so any attempt to describe it in purely Keplerian terms is inherently flawed,
 * which is the reason for the generally poor accuracy of this model.
 *
 * REFERENCES:
 * <ol>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M, and Chapront, J. 1988, Astronomy and Astrophysics, vol. 190, p. 342-352.</li>
 *  <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian Date.
 * @param[out] orbit  Orbital elements data structure to populate.
 * @return            0 if successful, or else -1 (`errno` set to `EINVAL`).
 *
 * @since 1.5.1
 * @author Attila Kovacs
 *
 * @sa novas_make_moon_mean_orbit(), novas_make_planet_orbit(), make_orbital_object()
 * @sa novas_moon_elp_posvel(), novas_moon_elp_sky_pos()
 */
int novas_make_moon_orbit(double jd_tdb, novas_orbital *restrict orbit) {
  typedef struct {
    int8_t iD;      // multiple of D
    int8_t il1;     // multiple of l1
    int8_t il;      // multiple of l
    int8_t iF;      // multiple of F
    float A;        // [arcsec,km] amplitude
  } elp_coeffs;


  // From ELP01: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP01
  static const elp_coeffs clon[7] = {
          {  0,  0,  1,  2,      -45.10032 }, //
          {  0,  0,  1, -2,       39.53393 }, //
          {  1,  0, -1,  0,      -18.58467 }, //
          {  2,  0,  0, -2,       55.17801 }, //
          {  2,  0,  2,  0,       14.37964 }, //
          {  4,  0, -2,  0,       30.77247 }, //
          {  2,  2, -1,  0,       -9.36601 }, //

          // Principal terms not included
          // (These degrade the Keplerian model)
          //{  0,  0,  0,  2,     -411.60287 }, //
          //{  2,  0, -2,  0,      211.65487 }, // T
          //{  2,  0, -3,  0,       13.19400 }, //

          // The following elongate the orbit, and are at least partly degenerate with eccentric
          // deformation...
          //{  0,  1,  1,  0,     -109.38419 }, // E
          //{  0,  1, -1,  0,     -147.32654 }, // E-
          //{  2,  0, -1,  0,     4586.43061 }, // E
          //{  2,  0,  1,  0,      191.95575 }, // E
          //{  2,  1, -1,  0,      -28.39810 }  // E?
          //{  2, -1,  1,  0,       14.53078 }, // E
          //{  2, -1, -1,  0,      205.44315 }, // E
          //{  4,  0, -1,  0,       38.42974 }, // E
  };

  // From ELP01: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP01
  static const elp_coeffs comega[7] = {
          {  0,  1,  0,  0,     -666.44186 },
          {  1,  0,  0,  0,     -124.98806 }, //
          {  1,  1,  0,  0,       17.95512 }, //
          {  2,  0,  0,  0,     2369.91227 }, //
          {  2,  1,  0,  0,      -24.35910 }, //
          {  2, -1,  0,  0,      164.73458 }, //
          {  4,  0,  0,  0,       13.89903 }, //
  };

  // From ELP02: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP02
  static const elp_coeffs clat[8] = {
          {  0,  0,  2, -1,       31.75985 }, //
          {  2,  0,  0, -1,      623.65783 }, //
          {  2,  0,  1, -1,       33.35743 }, // *
          {  2,  1,  0, -1,      -12.09470 }, //
          {  0,  1,  1, -1,       -5.07614 }, // *
          {  0,  1,  1,  1,       -5.31151 }, //
          {  2,  0,  1,  1,       15.12165 }, //
          {  2, -1,  0, -1,       29.57794 }  //

          // Principal terms not included
          // (These degrade the Keplerian model)
          //{  0,  0,  1, -1,      999.70079 }, // *
          //{  0,  0,  1,  1,     1010.17430 }, //
          //{  0,  0,  2,  1,       61.91229 }, //
          //{  2,  0, -1, -1,      166.57528 }, //
          //{  2,  0, -1,  1,      199.48515 }, // *
          //{  2,  0,  0,  1,      117.26161 }, //
  };


  // From ELP03: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP03
  static const elp_coeffs ce[11] = {
          {  0,  1,  1,  0,      104.75896 }, // E
          {  2,  0, -1,  0,    -3699.10468 }, // E
          {  2,  0,  1,  0,     -170.73274 }, // E
          {  2,  1, -1,  0,       24.20935 }, // E
          {  2, -1,  1,  0,      -12.83185 }, // E
          {  2, -1, -1,  0,     -152.14314 }, // E
          {  4,  0, -1,  0,      -34.78245 }, // E

          // Tidal terms, which we crudely approximate with elliptical flattening
          {  2, -1, -2,  0,       10.05654 }, // T
          {  2,  0, -2,  0,      246.15768 }, // T
          {  2,  0,  2,  0,      -10.44472 }, // T
          {  4,  0, -2,  0,      -21.63627 }  // T

          // Principal terms not included
          // (These degrade the Keplerian model)
          //{  0,  1, -1,  0,     -129.62476 }, // E
  };

  // From ELP03: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP03
  static const elp_coeffs cdis[7] = {
          {  0,  1,  0,  0,       48.89010 }, //
          {  1,  0,  0,  0,      108.74265 }, //
          {  1,  1,  0,  0,      -16.67533 }, //
          {  2, -1,  0,  0,     -204.59357 }, //
          {  2,  0,  0,  0,    -2955.96651 }, //
          {  2,  1,  0,  0,       30.82498 }, //
          {  4,  0,  0,  0,      -11.64993 },  //

          // Principal terms not included
          // These terms are not consistent with a Keplerian orbit, with |iL| != 1
          //{  2,  0,  0, -2,       10.32129 }, //
  };

  // [arcsec] Eccentric series for ecliptic longitude vs mean anomaly.
  static const float AE[8] = { 22639.55000, 769.02326, 36.12364, 1.93367, 0.11100, 0.00665, 0.00041, 0.00003 };

  const double t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;
  double M1, dE = 0.0, dL = 0.0, dL1 = 0.0, dY = 0.0, dOmega;
  double pole[3] = {0.0}, E[2] = {0.0}, domega;
  int i;

  elp_mean_args elp = {};
  novas_delaunay_args args = {};

  prop_error("novas_make_moon_orbit", novas_make_moon_mean_orbit(jd_tdb, orbit), 0);

  // Delaunay args for Solar perturbations.
  // (Chapront & Francou 2003).
  elp_args(t, &elp, &args);

  // Perturb longitude...
  for(i = 0; i < 7; i++) {
    const elp_coeffs *c = &clon[i];
    const double arg = (c->iD * args.D + c->il1 * args.l1 + c->il * args.l + c->iF * args.F);
    dL += c->A * sin(arg);
    dL1 = c->il * c->A * cos(arg);   // dL/dl * dl/dt = dL / dt
  }

  // Calculate the ecliptic vs mean longitude differential for eccentric orbit.
  for(i = 0; i < 8; i++)
    dE += (i + 1) * AE[i] * cos((i + 1) * args.l);
  dE *= ARCSEC;

  // Project longitude perturbation into orbital mean anomaly.
  orbit->M0 += (dL / 3600.0) * (1.0 + dE);

  // dl / dt - d(W1 - W2) / dt
  M1      = (elp_prime(0, t) - elp_prime(1, t)) / DEGREE;

  // Projected local current mean motion.
  orbit->n += (dL1 * ARCSEC) * (1.0 + dE) * M1 / JULIAN_CENTURY_DAYS;

  // Perturb omega (apsis location vs node)
  dL = 0.0;
  for(i = 0; i < 7; i++) {
    const elp_coeffs *c = &comega[i];
    double arg = (c->iD * args.D + c->il1 * args.l1);
    dL += c->A * sin(arg);
  }

  // Project longitude perturbation into shift in periapsis.
  orbit->omega += (dL / 3600.0);

  pole[1] = -sin(orbit->i * DEGREE);
  pole[2] =  cos(orbit->i * DEGREE);

  // Perturb pole...
  for(i = 0; i < 8; i++) {
    const elp_coeffs *c = &clat[i];
    double arg = (c->iD * args.D + c->il1 * args.l1 + c->il * args.l + c->iF * args.F);
    dY += c->A * sin(arg);
  }

  novas_Rz(args.F, pole);
  novas_Ry(dY * ARCSEC, pole);
  novas_Rz(-args.F, pole);

  orbit->i = atan2(sqrt(pole[0] * pole[0] + pole[1] * pole[1]), pole[2]) / DEGREE;
  dOmega = atan2(pole[0], -pole[1]) / DEGREE;

  orbit->Omega += dOmega;
  orbit->omega -= dOmega;

  // Perturb eccentricity
  E[0] = -orbit->e * orbit->a * NOVAS_AU;

  for(i = 0; i < 11; i ++) {
    const elp_coeffs *c = &ce[i];
    double arg = -(c->iD * args.D + c->il1 * args.l1);
    double A = c->A * NOVAS_KM;

    if(abs(c->il) == 2)
      A = -2.0 * A;       // Tidal terms as excess eccentricity (a crude approximation...)

    E[0] -= A * cos(arg);
    E[1] -= A * sin(arg);
  }

  orbit->e = sqrt(E[0] * E[0] + E[1] * E[1]) / (orbit->a * NOVAS_AU);
  domega = -atan2(-E[1], -E[0]) / DEGREE;

  orbit->omega += domega;
  orbit->M0 -= domega;

  // Perturb mean distance
  for(i = 0; i < 7; i++) {
    const elp_coeffs *c = &cdis[i];
    double arg = (c->iD * args.D + c->il1 * args.l1);
    orbit->a += c->A * NOVAS_KM / NOVAS_AU * cos(arg);
  }

  return 0;
}

/**
 * Calculates the Moon's phase at a given time. It uses orbital models for Earth (E.M. Standish
 * and J.G. Williams 1992), and the ELP2000/MPP02 semi-analytical model for the Moon (Chapront
 * &amp; Francou, 2002, 2003), and takes into account the slightly eccentric nature of both orbits.
 *
 * NOTES:
 * <ol>
 * <li>The Moon's phase here follows the definition by the Astronomical Almanac, as the excess
 * ecliptic longitude of the Moon over that of the Sun seen from the geocenter.</li>
 * <li>There are other definitions of the phase too, depending on which you might find slightly
 * different answers, but regardless of the details most phase calculations should match to within
 * a few degrees.</li>
 * <li>As of version 1.6, this function relies on the ELP2000/MM02 semi-analytical model of the
 * Moon by Chapront &amp; Francou (2003).</li>
 * </ol>
 *
 * NOTES:
 * <ol>
 * <li>This function caches the result of the last calculation.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>The Explanatory Supplement to the Astronomical Almanac, University Science Books, 3rd ed.,
 *      p. 507</li>
 *  <li>E.M. Standish and J.G. Williams 1992.</li>
 *  <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M, and Chapront, J. 1983, Astronomy and Astrophysics (ISSN 0004-6361),
 *      vol. 124, no. 1, July 1983, p. 50-62.</li>
 *  <li>Chapront J., &amp; Francou G., 2003, A&amp;A, 404, 735</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian Date.
 * @return            [deg] The Moon's phase, or more precisely the ecliptic longitude difference
 *                    between the Sun and the Moon, as seen from the geocenter. 0: New Moon, 90:
 *                    1st quarter, +/- 180 Full Moon, -90: 3rd quarter or NAN if the solution
 *                    failed to converge (errno will be set to ECANCELED), or if the JD date is
 *                    outside the range of the orbital model (errno set to EINVAL).
 *
 * @since 1.4
 * @author Attila Kovacs
 *
 * @sa novas_next_moon_phase(), novas_make_moon_orbit(), novas_solar_illum()
 */
double novas_moon_phase(double jd_tdb) {
  static const char *fn = "novas_moon_phase";

  static THREAD_LOCAL double last_tdb = NAN, last_phase;

  if(novas_time_equals(jd_tdb, last_tdb))
    return last_phase;

  novas_orbital orbit = NOVAS_ORBIT_INIT;
  double pos[3] = {0.0};
  double he, hm;

  // EMB ecliptic pos around Sun
  prop_nan(fn, novas_make_planet_orbit(NOVAS_EMB, jd_tdb, &orbit));
  prop_nan(fn, novas_orbit_native_posvel(jd_tdb, &orbit, pos, NULL));
  vector2radec(pos, &he, NULL);

  // Moon ecliptic pos around Earth
  //novas_make_moon_orbit(jd_tdb, &orbit);
  //prop_nan(fn, novas_orbit_native_posvel(jd_tdb, &orbit, pos, NULL));

  prop_nan(fn, novas_moon_elp_ecl_pos(jd_tdb, 0.0, pos));
  vector2radec(pos, &hm, NULL);

  last_phase = remainder(12.0 + hm - he, 24.0) * 15.0;
  last_tdb = jd_tdb;

  return last_phase;
}

/**
 * Calculates the date / time at which the Moon will reach the specified phase next, _after_ the
 * specified time. It uses orbital models for Earth (E.M. Standish and J.G. Williams 1992), and
 * the ELP2000/MPP02 semi-analytical model for the Moon (Chapront &amp; Francou, 2002, 2003), and
 * takes into account the slightly eccentric nature of both orbits.
 *
 * NOTES:
 * <ol>
 * <li>The Moon's phase here follows the definition by the Astronomical Almanac, as the excess
 * ecliptic longitude of the Moon over that of the Sun seen from the geocenter.</li>
 * <li>There are other definitions of the phase too, depending on which you might find slightly
 * different answers, but regardless of the details most phase calculations should match give or
 * take a few hours.</li>
 * <li>As of version 1.6, this function relies on the ELP2000/MM02 semi-analytical model of the
 * Moon by Chapront &amp; Francou (2003).</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>The Explanatory Supplement to the Astronomical Almanac, University Science Books, 3rd ed.,
 *      p. 507</li>
 *  <li>E.M. Standish and J.G. Williams 1992.</li>
 *  <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 *  <li>Chapront, J., &amp; Francou, G., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M, and Chapront, J. 1983, Astronomy and Astrophysics (ISSN 0004-6361),
 *      vol. 124, no. 1, July 1983, p. 50-62.</li>
 * </ol>
 *
 * @param phase   [deg] The Moon's phase, or more precisely the ecliptic longitude difference
 *                between the Sun and the Moon, as seen from the geocenter. 0: New Moon, 90: 1st
 *                quarter, +/- 180 Full Moon, -90: 3rd quarter.
 * @param jd_tdb  [day] The lower bound date for the phase, as a Barycentric Dynamical Time (TDB)
 *                based Julian Date.
 * @return        [day] The Barycentric Dynamical Time (TDB) based Julian Date when the Moon will
 *                be in the desired phase next after the input date; or NAN if the solution failed
 *                to converge (errno will be set to ECANCELED).
 *
 * @since 1.4
 * @author Attila Kovacs
 *
 * @sa novas_moon_phase(), novas_make_moon_orbit()
 */
double novas_next_moon_phase(double phase, double jd_tdb) {
  static const char *fn = "novas_next_moon_phase";
  int i;

  double t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;

  // [deg/day] Differential motion of the Moon w.r.t. Earth.
  // Moon motion from Chapront-Touze, M, and Chapront, J. 1983, A&A, 124, 1, p. 50-62.
  // Earth motion from E.M. Standish and J.G. Williams 1992. Table 8.10.3. Valid for 3000 BC to 3000 AD.
  double rate = 445266.793243221 + t * (0.021258 + t * (3.75393e-05 - t * 2.366776e-07));
  rate /= JULIAN_CENTURY_DAYS;

  for(i = 0; i < novas_inv_max_iter; i++) {
    double phi = novas_moon_phase(jd_tdb);

    if(isnan(phi))
      return novas_trace_nan(fn);

    phi = remainder(phase - phi, DEG360); // [deg]
    if(fabs(phi) < 1e-6)
      return jd_tdb;

    if(i == 0 && phi < 0.0)
      phi += DEG360;  // initial phase shift must be positive to ensure it is after input date.

    // Date when mean elongation changes by phi...
    jd_tdb += phi / rate;
  }

  novas_error(-1, ECANCELED, fn, "Failed to converge");
  return NAN;
}

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif


#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif

