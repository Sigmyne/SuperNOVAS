/**
 * @file
 *
 * Functions to calculate or access the Celestial Intermediate Origin (CIO) location. The CIO is
 * the origin of the Celestial Intermediate Reference System (CIRS), which is the IAU 2000 dynamic
 * equatorial system of date. CIRS and the old equivalent True of Date (TOD) systems share the
 * same dynamical equator, but differ in where the origin lies on the equator: CIO vs the true
 * equinox of date.
 *
 * The IAU2000 standard for position calculations typically involves converting between GCRS and
 * CIRS systems, and the IERS Conventions 2010, Chapter 5 describes two equivalent methods to do
 * so.
 *
 * - __Method 1__ is the more direct method and involves calculating the position of the Celestial
 *   Intermediate Pole (CIP) of date in GCRS, using a harmonic series containing some 2825
 *   lunisolar and planetary terms (IERS Conventions 2010, Tables
 *   [5.2a](https://iers-conventions.obspm.fr/content/chapter5/additional_info/tab5.2a.txt) and
 *   [5.2b](https://iers-conventions.obspm.fr/content/chapter5/additional_info/tab5.2b.txt)).
 *
 * - __Method 2__ is more roundabout, transforming GCRS to J2000 first, then using the IAU 2006
 *   (P03) precession-nutation model to calculate True-of-Date coordinates, which are then
 *   transformed to CIRS by a simple rotation with the CIO's position relative to the true-equinox
 *   of date. The IAU2006 nutation series uses 2414 lunisolar and planetary terms (IERS Conventions
 *   2010, Tables
 *   [5.3a](https://iers-conventions.obspm.fr/content/chapter5/additional_info/tab5.3a.txt) and
 *   [5.3b](https://iers-conventions.obspm.fr/content/chapter5/additional_info/tab5.3b.txt)).
 *
 * The two methods are equivalent both in terms of accuracy, down to the &mu;as level, and in
 * terms of computational cost. Neither is 'superior' to the other in any measurable way. In
 * SuperNOVAS we choose to follow Method 2, since its implementation is more readily given with
 * the existing framework of related functions.
 *
 * REFERENCES:
 * <ol>
 * <li>IERS Conventions 2010, Chapter 5, especially Section 5.9</li>
 * <li>Capitaine, N. et al. (2003), Astronomy And Astrophysics 412, pp. 567-586.</li>
 * </ol>
 *
 * @date Created  on Mar 6, 2025
 * @author G. Kaplan and Attila Kovacs
 *
 * @sa equator.c
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
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

/// \cond PRIVATE
#define CIO_INTERP_POINTS   6     ///< Number of points to load from CIO interpolation table at once.

#define CIO_ARRAY_STEP      1.2   ///< [day] Interval between CIO vs GCRS locator lookup entries.
/// \endcond


/**
 * Computes the true right ascension of the celestial intermediate origin (CIO) vs the equinox of
 * date on the true equator of date for a given TT Julian date. This is simply the negated return
 * value ofira_equinox() for the true equator of date.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra_cio [h] Right ascension of the CIO, with respect to the true equinox of date (+
 *                    or -), or NAN when returning with an error code.
 * @return            0 if successful, -1 if the output pointer argument is NULL,
 *                    1 if 'accuracy' is invalid, 10--20: 10 + error code from cio_location(), or
 *                    else 20 + error from cio_basis()
 *
 * @sa ira_equinox()
 */
short cio_ra(double jd_tt, enum novas_accuracy accuracy, double *restrict ra_cio) {
  static const char *fn = "cio_ra";

  if(!ra_cio)
    return novas_error(-1, EINVAL, fn, "NULL output array");

  // Default return value.
  *ra_cio = NAN;

  // Check for valid value of 'accuracy'.
  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // For these calculations we can assume TDB = TT (< 2 ms difference)
  *ra_cio = -ira_equinox(jd_tt, NOVAS_TRUE_EQUINOX, accuracy);
  return 0;
}

/**
 * @deprecated    SuperNOVAS no longer uses a NOVAS-type CIO locator file, or accesses one in any
 *                way. This function is now a dummy.
 *
 * It used to set the CIO interpolaton data file to use to interpolate CIO locations vs the GCRS.
 * As of version 1.5, this call does exactly nothing. It simply returns 0.
 *
 * @param filename    (<i>unused</i>) Used to be the path (preferably absolute path) `CIO_RA.TXT`
 *                    or else to the binary `cio_ra.bin` data, or NULL to disable using a CIO
 *                    locator file altogether.
 * @return            0
 *
 * @sa cio_location()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int set_cio_locator_file(const char *restrict filename) {
  (void) filename; // unused
  return 0;
}

/**
 * @deprecated This function is no longer used internally in the library. Given that the CIO is
 *             defined on the dynamical equator of date, it is not normally meaningful to provide
 *             an R.A. coordinate for it in GCRS. Instead, you might use `cio_ra()` to get the
 *             CIO location w.r.t. the equinox of date (on the same dynamical equator), or
 *             equivalently `ira_equinox()` to return the negated value of the same.
 *
 * Returns the location of the celestial intermediate origin (CIO) for a given Julian date, as a
 * right ascension with respect to the true equinox of date. The CIO is always located on the true
 * equator (= intermediate equator) of date.
 *
 * NOTES:
 * <ol>
 * <li>
 *   Unlike the NOVAS C version of this function, this version will always return a CIO
 *   location as long as the pointer arguments are not NULL.
 * </li>
 * <li>
 *  This function caches the results of the last calculation in case it may be re-used at no extra
 *  computational cost for the next call.
 * </li>
 * <li>
 *  As of version 1.5, this function always returns the CIO location w.r.t. the true equinox of
 *  date, on the true equator of date, i.e. the R.A. of the CIO on the true equator of date, meaured
 *  from the true equinox of date.
 * </li>
 * </ol>
 *
 * @param jd_tdb           [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param accuracy         NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra_cio      [h] Right ascension of the CIO, in hours, or NAN if returning with an
 *                         error.
 * @param[out] loc_type    Pointer in which to return the reference system in which right
 *                         ascension is given, which is either CIO_VS_GCRS (1) if the location was
 *                         obtained via interpolation of the available data file, or else
 *                         CIO_VS_EQUINOX (2) if it was calculated locally. It is set to -1 if
 *                         returning with an error.
 *
 * @return            0 if successful, -1 if one of the pointer arguments is NULL or the
 *                    accuracy is invalid.
 *
 * @sa cio_ra(), ira_equinox()
 */
short cio_location(double jd_tdb, enum novas_accuracy accuracy, double *restrict ra_cio, short *restrict loc_type) {
  static const char *fn = "cio_location";

  static THREAD_LOCAL enum novas_accuracy acc_last = (enum novas_accuracy) -1;
  static THREAD_LOCAL double t_last = NAN, ra_last = NAN;

  // Default return values...
  if(ra_cio)
    *ra_cio = NAN;
  if(loc_type)
    *loc_type = CIO_VS_EQUINOX;

  if(!ra_cio || !loc_type)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: ra_cio=%p, loc_type=%p", ra_cio, loc_type);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Check if previously computed RA value can be used.
  if(!novas_time_equals(jd_tdb, t_last) || accuracy != acc_last) {
    // Calculate the equation of origins.
    *ra_cio = -ira_equinox(jd_tdb, NOVAS_TRUE_EQUINOX, accuracy);

    t_last = jd_tdb;
    acc_last = accuracy;
    ra_last = *ra_cio;
  }

  *ra_cio = ra_last;
  return 0;
}

/**
 * @deprecated This function is no longer used internally in the library, and users are
 *             recommended against using it themselves, since SuperNOVAS provides better ways to
 *             convert between GCRS and CIRS using frames or via gcrs_to_cirs() / cirs_to_gcrs()
 *             or novas_transform() functions.
 *
 * Computes the CIRS basis vectors, with respect to the GCRS (geocentric ICRS), of the
 * celestial intermediate system defined by the celestial intermediate pole (CIP) (in the z
 * direction) and the celestial intermediate origin (CIO) (in the x direction).
 *
 * This function effectively constructs the CIRS to GCRS transformation matrix C in eq. (3) of the
 * reference.
 *
 * NOTES:
 * <ol>
 * <li>This function caches the results of the last calculation in case it may be re-used at
 * no extra computational cost for the next call.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param ra_cio      [h] Right ascension of the CIO at epoch (hours).
 * @param loc_type    CIO_VS_GCRS (1) if the cio location is relative to the GCRS or else
 *                    CIO_VS_EQUINOX (2) if relative to the true equinox of date.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] x      Unit 3-vector toward the CIO, equatorial rectangular coordinates,
 *                    referred to the GCRS.
 * @param[out] y      Unit 3-vector toward the y-direction, equatorial rectangular
 *                    coordinates, referred to the GCRS.
 * @param[out] z      Unit 3-vector toward north celestial pole (CIP), equatorial
 *                    rectangular coordinates, referred to the GCRS.
 * @return            0 if successful, or -1 if any of the output vector arguments are NULL
 *                    or if the accuracy is invalid, or else 1 if 'ref-sys' is invalid.
 *
 * @sa gcrs_to_cirs(), cirs_to_gcrs(), novas_make_transform()
 */
short cio_basis(double jd_tdb, double ra_cio, enum novas_cio_location_type loc_type, enum novas_accuracy accuracy,
        double *restrict x, double *restrict y, double *restrict z) {
  static const char *fn = "cio_basis";
  static THREAD_LOCAL enum novas_accuracy acc_last = (enum novas_accuracy) -1;
  static THREAD_LOCAL double t_last = NAN;
  static THREAD_LOCAL double xx[3], yy[3], zz[3];

  if(!x || !y || !z)
    return novas_error(-1, EINVAL, fn, "NULL output 3-vector: x=%p, y=%p, z=%p", x, y, z);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Compute unit vector z toward celestial pole.
  if(!novas_time_equals(jd_tdb, t_last) || (accuracy != acc_last)) {
    const double z0[3] = { 0.0, 0.0, 1.0 };
    tod_to_gcrs(jd_tdb, accuracy, z0, zz);
    t_last = jd_tdb;
    acc_last = accuracy;

    // Now compute unit vectors x and y.  Method used depends on the
    // reference system in which right ascension of the CIO is given.
    ra_cio *= HOURANGLE;

    switch(loc_type) {
      case CIO_VS_GCRS: {

        // Compute vector x toward CIO in GCRS.
        const double sinra = sin(ra_cio);
        const double cosra = cos(ra_cio);
        double l;

        xx[0] = zz[2] * cosra;
        xx[1] = zz[2] * sinra;
        xx[2] = -zz[0] * cosra - zz[1] * sinra;

        // Normalize vector x.
        l = novas_vlen(xx);
        xx[0] /= l;
        xx[1] /= l;
        xx[2] /= l;

        break;
      }

      case CIO_VS_EQUINOX: {
        // Construct unit vector toward CIO in equator-and-equinox-of-date
        // system.
        xx[0] = cos(ra_cio);
        xx[1] = sin(ra_cio);
        xx[2] = 0.0;

        // Rotate the vector into the GCRS to form unit vector x.
        tod_to_gcrs(jd_tdb, accuracy, xx, xx);
        break;
      }

      default:
        // Invalid value of 'ref_sys'.
        memset(x, 0, XYZ_VECTOR_SIZE);
        memset(y, 0, XYZ_VECTOR_SIZE);
        memset(z, 0, XYZ_VECTOR_SIZE);

        return novas_error(1, EINVAL, fn, "invalid input CIO location type: %d", loc_type);
    }

    // Compute unit vector y orthogonal to x and z (y = z cross x).
    yy[0] = zz[1] * xx[2] - zz[2] * xx[1];
    yy[1] = zz[2] * xx[0] - zz[0] * xx[2];
    yy[2] = zz[0] * xx[1] - zz[1] * xx[0];
  }

  memcpy(x, xx, sizeof(xx));
  memcpy(y, yy, sizeof(yy));
  memcpy(z, zz, sizeof(zz));

  return 0;
}

/// \cond PRIVATE

double novas_cio_gcrs_ra(double jd_tdb) {
  double RA, pos[3] = {0.0};

  // CIO's R.A. in TOD.
  RA = -ira_equinox(jd_tdb, NOVAS_TRUE_EQUINOX, NOVAS_FULL_ACCURACY);
  radec2vector(RA, 0.0, 1.0, pos);

  // TOD -> GCRS
  tod_to_gcrs(jd_tdb, NOVAS_FULL_ACCURACY, pos, pos);

  // Get the GCRS R.A. of the CIO
  vector2radec(pos, &RA, NULL);

  return RA;
}

/// \endcond

/**
 * @deprecated    This function is no longer used within SuperNOVAS. It is still provided,
 *                however, in order to retain 100% API compatibility with NOVAS C.
 *
 * Given an input TDB Julian date and the number of data points desired, this function returns
 * a set of Julian dates and corresponding values of the GCRS right ascension of the celestial
 * intermediate origin (CIO).  The range of dates is centered on the requested date.
 *
 * NOTES:
 * <ol>
 * <li>
 *   This function has been completely re-written by A. Kovacs to provide much more efficient
 *   caching and I/O.
 * </li>
 * <li>
 *  The CIO locator file that is bundled was prepared with the original IAU2000A nutation model,
 *  not with the newer R06 (a.k.a. IAU2006) nutation model, resulting in an error up to the few
 *  tens of micro-arcseconds level for dates between 1900 and 2100, and larger errors further away
 *  from the current epoch.
 * </li>
 *
 * <li>
 *  Prior to version 1.5, this function relied on a CIO locator file (`CIO_RA.TXT` or
 *  `cio_ra.bin`). The current version no longer does, and instead generates the requested data on
 *  the fly.
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param n_pts     Number of Julian dates and right ascension values requested (not less than 2
 *                  or more than NOVAS_CIO_CACHE_SIZE).
 * @param[out] cio  A time series (array) of the right ascension of the Celestial Intermediate
 *                  Origin (CIO) with respect to the GCRS.
 * @return          0 if successful, -1 if the output array is NULL or there
 *                  was an I/O error accessing the CIO location data file. Or else 1 if no
 *                  locator data file is available, 2 if 'jd_tdb' not in the range of the CIO
 *                  file, 3 if 'n_pts' out of range, or 6 if 'jd_tdb' is too close to either end
 *                  of the CIO file do we are unable to put 'n_pts' data points into the output
 *
 * @sa set_cio_locator_file(), cio_location()
 */
short cio_array(double jd_tdb, long n_pts, ra_of_cio *restrict cio) {
  static const char *fn = "cio_array";

  int i;

  if(cio == NULL)
    return novas_error(-1, EINVAL, fn, "NULL output array");

  if(n_pts < 2)
    return novas_error(3, ERANGE, fn, "n_pts=%ld is out of bounds [2:*]", n_pts);

  // Center to starting date
  jd_tdb -= 0.5 * n_pts * CIO_ARRAY_STEP;

  for(i = 0; i < n_pts; i++) {
    ra_of_cio *p = &cio[i];
    p->jd_tdb = jd_tdb + i * CIO_ARRAY_STEP;
    p->ra_cio = remainder(novas_cio_gcrs_ra(p->jd_tdb), 24.0) * NOVAS_HOURANGLE / NOVAS_ARCSEC;
  }

  return 0;
}

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif
