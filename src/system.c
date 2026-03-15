/**
 * @file
 *
 *  Various transformations between different coordinate systems used in astronomy, such as
 *  equatorial, ecliptic, Galactic, or local horizontal coordinate systems.
 *
 *  This module also provides functions otherwise relating to the observer locations.
 *
 *  For Earth-based geodetic locations, the module provides a set of functions to convert between
 *  various (local) coordinate systems. For example, calculating parallactic angles (a.k.a.
 *  vertical position angle or VPA), for converting between horizontal and equatorial offsets,
 *  and various functions for converting equatorial vectors to a line-of-sight, or _u_, _v_, _w_
 *  coordinate bases, and vice-versa.
 *
 * @date Created  on Mar 5, 2025
 * @author Attila Kovacs and G. Kaplan
 *
 * @sa frames.c, transform.c
 */

#include <string.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif



/**
 * Convert ecliptic longitude and latitude to right ascension and declination.  To convert GCRS
 * ecliptic coordinates (mean ecliptic and equinox of J2000.0), set 'coord_sys' to
 * NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set to anything, since J2000.0
 * is assumed. Otherwise, all input coordinates are dynamical at'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system of the coordinates. If 'coord_sys' is
 *                    NOVAS_GCRS_EQUATOR(2), the input GCRS coordinates are converted to
 *                    J2000 ecliptic coordinates.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param elon        [deg] Ecliptic longitude in degrees, referred to the specified ecliptic and
 *                    equinox of date.
 * @param elat        [deg] Ecliptic latitude in degrees, referred to the specified ecliptic and
 *                    equinox of date.
 * @param[out] ra     [h] Right ascension in hours, referred to specified equator and equinox
 *                    of date.
 * @param[out] dec    [deg] Declination in degrees, referred to specified equator and equinox
 *                    of date.

 * @return            0 if successful, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa ecl2equ_vec(), equ2ecl()
 */
int ecl2equ(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat,
        double *restrict ra, double *restrict dec) {
  static const char *fn = "ecl2equ";
  double coslat, pos[3], xyproj;

  if(!ra || !dec)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: ra=%p, dec=%p", ra, dec);

  // Form position vector in equatorial system from input coordinates.
  elon *= DEGREE;
  elat *= DEGREE;
  coslat = cos(elat);

  pos[0] = coslat * cos(elon);
  pos[1] = coslat * sin(elon);
  pos[2] = sin(elat);

  // Convert the vector from equatorial to ecliptic system.
  prop_error(fn, ecl2equ_vec(jd_tt, coord_sys, accuracy, pos, pos), 0);

  // Decompose ecliptic vector into ecliptic longitude and latitude.
  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
  *ra = xyproj ? atan2(pos[1], pos[0]) / HOURANGLE : 0.0;
  if(*ra < 0.0)
    *ra += DAY_HOURS;

  *dec = atan2(pos[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Converts galactic longitude and latitude to ICRS right ascension and declination.
 *
 * REFERENCES:
 * <ol>
 * <li>J.-C. Liu, Z. Zhu, and H. Zhang, A&amp;A, 526, A16 (2011), Eq. 18</li>
 * <li>Hipparcos and Tycho Catalogues, Vol. 1, Section 1.5.3.</li>
 * </ol>
 *
 * @param glon        [deg] Galactic longitude in degrees.
 * @param glat        [deg] Galactic latitude in degrees.
 * @param[out] ra     [h] ICRS right ascension in hours.
 * @param[out] dec    [deg] ICRS declination in degrees.

 *
 * @return            0 if successful, or -1 if either of the output pointer arguments
 *                    are NULL.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa equ2gal()
 */
int gal2equ(double glon, double glat, double *restrict ra, double *restrict dec) {
  double pos1[3], pos2[3], xyproj, coslat;

  // Rotation matrix A_g from Liu+2011 Eq. 18
  // AK: Transposed compared to NOVAS C 3.1 for dot product handling.
  static const double ag[3][3] = { //
          { -0.054875657707, +0.494109437203, -0.867666137554 }, //
          { -0.873437051953, -0.444829721222, -0.198076337284 }, //
          { -0.483835073621, +0.746982183981, +0.455983813693 } };

  if(!ra || !dec)
    return novas_error(-1, EINVAL, "gal2equ", "NULL output pointer: ra=%p, dec=%p", ra, dec);

  // Form position vector in equatorial system from input coordinates
  glon *= DEGREE;
  glat *= DEGREE;
  coslat = cos(glat);

  pos1[0] = coslat * cos(glon);
  pos1[1] = coslat * sin(glon);
  pos1[2] = sin(glat);

  // Rotate position vector to galactic system, using Hipparcos documentation eq. 1.5.13.
  pos2[0] = novas_vdot(ag[0], pos1);
  pos2[1] = novas_vdot(ag[1], pos1);
  pos2[2] = novas_vdot(ag[2], pos1);

  // Decompose galactic vector into longitude and latitude.
  xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);
  *ra = xyproj ? atan2(pos2[1], pos2[0]) / HOURANGLE : 0.0;
  if(*ra < 0.0)
    *ra += DAY_HOURS;

  *dec = atan2(pos2[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Converts a position vector in the Earth-fixed ITRS frame to astrometric (unrefracted) azimuth
 * and zenith angles at the specified observer location.
 *
 * @param location   Geodetic (ITRF / GRS80) observer location on Earth.
 * @param itrs       3-vector position in Earth-fixed ITRS frame
 * @param[out] az    [deg] astrometric (unrefracted) azimuth angle at observer location [0:360].
 *                   It may be NULL if not required.
 * @param[out] za    [deg] astrometric (unrefracted) zenith angle at observer location [0:180].
 *                   It may be NULL if not required.
 * @return           0 if successful, or else -1 if the location or the input vector is NULL.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa hor_to_itrs(), cirs_to_itrs(), tod_to_itrs(), refract_astro()
 */
int itrs_to_hor(const on_surface *restrict location, const double *restrict itrs, double *restrict az, double *restrict za) {
  double uze[3], une[3], uwe[3];
  double lat, lon, coslat, sinlat, coslon, sinlon;
  double pn, pw, pz, proj;

  // Default output values in case of error return.
  if(az)
    *az = NAN;
  if(za)
    *za = NAN;

  if(!location || !itrs)
    return novas_error(-1, EINVAL, "itrs_to_hor", "NULL input location=%p or ITRS pos=%p", location, itrs);

  lat = location->latitude * DEGREE;
  lon = location->longitude * DEGREE;

  coslat = cos(lat);
  sinlat = sin(lat);
  coslon = cos(lon);
  sinlon = sin(lon);

  // Define vector toward local north in Earth-fixed system (x axis).
  une[0] = -sinlat * coslon;
  une[1] = -sinlat * sinlon;
  une[2] = coslat;

  // Define vector toward local west in Earth-fixed system (y axis).
  uwe[0] = sinlon;
  uwe[1] = -coslon;
  uwe[2] = 0.0;

  // Define vector toward local zenith in Earth-fixed system (z axis).
  uze[0] = coslat * coslon;
  uze[1] = coslat * sinlon;
  uze[2] = sinlat;

  // Obtain vectors in celestial system.

  // Compute coordinates of object w.r.t orthonormal basis.

  // Compute components of 'p' - projections of 'p' onto rotated
  // Earth-fixed basis vectors.
  pn = novas_vdot(itrs, une);
  pw = novas_vdot(itrs, uwe);
  pz = novas_vdot(itrs, uze);

  // Compute azimuth and zenith distance.
  proj = sqrt(pn * pn + pw * pw);

  if(az) {
    *az = proj > 0.0 ? -atan2(pw, pn) / DEGREE : 0.0;
    if(*az < 0.0)
      *az += DEG360;
  }

  if(za)
    *za = atan2(proj, pz) / DEGREE;

  return 0;
}

/**
 * Converts astrometric (unrefracted) azimuth and zenith angles at the specified observer location
 * to a unit position vector in the Earth-fixed ITRS frame.
 *
 * @param location   Geodetic (ITRF / GRS80) observer location on Earth
 * @param az         [deg] astrometric (unrefracted) azimuth angle at observer location [0:360].
 *                   It may be NULL if not required.
 * @param za         [deg] astrometric (unrefracted) zenith angle at observer location [0:180]. It
 *                   may be NULL if not required.
 * @param[out] itrs  Unit 3-vector direction in Earth-fixed ITRS frame
 * @return           0 if successful, or else -1 if the location or the input vector is NULL.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa itrs_to_hor(), itrs_to_cirs(), itrs_to_tod(), refract()
 */
int hor_to_itrs(const on_surface *restrict location, double az, double za, double *restrict itrs) {
  double in[3], uze[3], une[3], uwe[3];
  double sinza;
  double lat, lon, coslat, sinlat, coslon, sinlon;

  if(!location || !itrs)
    return novas_error(-1, EINVAL, "hor_to_itrs", "NULL input location=%p or output itrs=%p position", location, itrs);

  az *= -DEGREE;
  za *= DEGREE;

  sinza = sin(za);
  in[0] = sinza * cos(az);
  in[1] = sinza * sin(az);
  in[2] = cos(za);

  lat = location->latitude * DEGREE;
  lon = location->longitude * DEGREE;

  coslat = cos(lat);
  sinlat = sin(lat);
  coslon = cos(lon);
  sinlon = sin(lon);

  // Define vector toward local zenith in Earth-fixed system (z axis).
  uze[0] = coslat * coslon;
  uze[1] = coslat * sinlon;
  uze[2] = sinlat;

  // Define vector toward local north in Earth-fixed system (x axis).
  une[0] = -sinlat * coslon;
  une[1] = -sinlat * sinlon;
  une[2] = coslat;

  // Define vector toward local west in Earth-fixed system (y axis).
  uwe[0] = sinlon;
  uwe[1] = -coslon;
  uwe[2] = 0.0;

  // Calculate ITRS positions from NWZ components at ITRS origin
  itrs[0] = une[0] * in[0] + uwe[0] * in[1] + uze[0] * in[2];
  itrs[1] = une[1] * in[0] + uwe[1] * in[1] + uze[1] * in[2];
  itrs[2] = une[2] * in[0] + uwe[2] * in[1] + uze[2] * in[2];

  return 0;
}

/**
 * Converts ICRS right ascension and declination to galactic longitude and latitude.
 *
 * REFERENCES:
 * <ol>
 * <li>J.-C. Liu, Z. Zhu, and H. Zhang, A&amp;A, 526, A16 (2011), Eq. 18</li>
 * <li>Hipparcos and Tycho Catalogues, Vol. 1, Section 1.5.3.</li>
 * </ol>
 *
 * @param ra          [h] ICRS right ascension in hours.
 * @param dec         [deg] ICRS declination in degrees.
 * @param[out] glon   [deg] Galactic longitude in degrees.
 * @param[out] glat   [deg] Galactic latitude in degrees.
 *
 * @return            0 if successful, or -1 if either of the output pointer arguments
 *                    are NULL.
 *
 * @sa gal2equ()
 */
int equ2gal(double ra, double dec, double *restrict glon, double *restrict glat) {
  double pos1[3], pos2[3], xyproj, cosd;

  // Rotation matrix A_g from Liu+2011 (Eq. 18)
  // AK: Transposed compared to NOVAS C 3.1 for dot product handling.
  static const double ag[3][3] = { //
          { -0.054875657707, -0.873437051953, -0.483835073621 }, //
          { +0.494109437203, -0.444829721222, +0.746982183981 }, //
          { -0.867666137554, -0.198076337284, +0.455983813693 } };

  if(!glon || !glat)
    return novas_error(-1, EINVAL, "equ2gal", "NULL output pointer: glon=%p, glat=%p", glon, glat);

  // Form position vector in equatorial system from input coordinates
  ra *= HOURANGLE;
  dec *= DEGREE;
  cosd = cos(dec);

  pos1[0] = cosd * cos(ra);
  pos1[1] = cosd * sin(ra);
  pos1[2] = sin(dec);

  // Rotate position vector to galactic system, using Hipparcos documentation eq. 1.5.13.
  pos2[0] = novas_vdot(ag[0], pos1);
  pos2[1] = novas_vdot(ag[1], pos1);
  pos2[2] = novas_vdot(ag[2], pos1);

  // Decompose galactic vector into longitude and latitude.
  xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);
  *glon = xyproj ? atan2(pos2[1], pos2[0]) / DEGREE : 0.0;
  if(*glon < 0.0)
    *glon += DEG360;

  *glat = atan2(pos2[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Convert right ascension and declination to ecliptic longitude and latitude.  To convert
 * GCRS RA and dec to ecliptic coordinates (mean ecliptic and equinox of J2000.0), set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set to
 * anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at
 * 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system of the coordinates. If 'coord_sys' is
 *                    NOVAS_GCRS_EQUATOR(2), the input GCRS coordinates are converted to
 *                    J2000 ecliptic coordinates.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param ra          [h] Right ascension in hours, referred to the specified equator and equinox
 *                    of date.
 * @param dec         [deg] Declination in degrees, referred to the specified equator and equinox
 *                    of date.
 * @param[out] elon   [deg] Ecliptic longitude in degrees, referred to specified ecliptic and
 *                    equinox of date.
 * @param[out] elat   [deg] Ecliptic latitude in degrees, referred to specified ecliptic and
 *                    equinox of date.
 * @return            0 if successful, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa equ2ecl_vec(), ecl2equ()
 */
short equ2ecl(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec,
        double *restrict elon, double *restrict elat) {
  static const char *fn = "equ2ecl";
  double cosd, pos[3], xyproj;

  if(!elon || !elat)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: elon=%p, elat=%p", elon, elat);

  // Form position vector in equatorial system from input coordinates.
  ra *= HOURANGLE;
  dec *= DEGREE;
  cosd = cos(dec);

  pos[0] = cosd * cos(ra);
  pos[1] = cosd * sin(ra);
  pos[2] = sin(dec);

  // Convert the vector from equatorial to ecliptic system.
  prop_error(fn, equ2ecl_vec(jd_tt, coord_sys, accuracy, pos, pos), 0);

  // Decompose ecliptic vector into ecliptic longitude and latitude.
  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);

  *elon = (xyproj > 0.0) ? atan2(pos[1], pos[0]) / DEGREE : 0.0;
  if(*elon < 0.0)
    *elon += DEG360;

  *elat = atan2(pos[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Converts an equatorial position vector to an ecliptic position vector. To convert
 * ICRS RA and dec to ecliptic coordinates (mean ecliptic and equinox of J2000.0), set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set
 * to anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical
 * at 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system type of the coordinates.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in          Position vector, referred to the specified equator and equinox of date.
 * @param[out] out    Position vector, referred to the specified ecliptic and equinox of date.
 *                    It can be the same vector as the input. If 'coord_sys' is
 *                    NOVAS_GCRS_EQUATOR(2), the input GCRS coordinates are converted to
 *                    J2000 ecliptic coordinates.
 * @return            0 if successful, -1 if either vector argument is NULL or the accuracy
 *                    is invalid, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa equ2ecl(), ecl2equ_vec()
 */
short equ2ecl_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy,
        const double *in, double *out) {
  static const char *fn = "equ2ecl_vec";
  double pos0[3], obl, c, s;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Get obliquity, depending upon the "system" of the input coordinates.
  switch(coord_sys) {
    case NOVAS_MEAN_EQUATOR:      // Input: mean equator and equinox of date
    case NOVAS_TRUE_EQUATOR: {    // Input: true equator and equinox of date
      double oblm = NAN, oblt = NAN;

      // For these calculations we can assume TDB = TT (< 2 ms difference)
      const double jd_tdb = jd_tt;

      memcpy(pos0, in, sizeof(pos0));

      e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, NULL, NULL);

      obl = (coord_sys == NOVAS_MEAN_EQUATOR ? oblm : oblt) * DEGREE;
      break;
    }

    case NOVAS_GCRS_EQUATOR: /* Input: GCRS */{
      static THREAD_LOCAL enum novas_accuracy acc_2000 = (enum novas_accuracy) -1;
      static THREAD_LOCAL double ob2000;

      frame_tie(in, ICRS_TO_J2000, pos0);

      if(accuracy != acc_2000) {
        ob2000 = mean_obliq(JD_J2000);
        acc_2000 = accuracy;
      }

      obl = ob2000 * ARCSEC;
      break;
    }

    default:
      return novas_error(1, EINVAL, fn, "invalid equator type: %d", coord_sys);
  }

  c = cos(obl);
  s = sin(obl);

  // Rotate position vector to ecliptic system.
  out[0] = pos0[0];
  out[1] = pos0[1] * c + pos0[2] * s;
  out[2] = -pos0[1] * s + pos0[2] * c;

  return 0;
}

/**
 * Converts an ecliptic position vector to an equatorial position vector. To convert
 * ecliptic coordinates (mean ecliptic and equinox of J2000.0) to GCRS RA and dec to, set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set to
 * anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at
 * 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system type of the coordinates
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in          Position vector, referred to the specified ecliptic and equinox of date.
 * @param[out] out    Position vector, referred to the specified equator and equinox of date. It
 *                    can be the same vector as the input.
 * @return            0 if successful, -1 if either vector argument is NULL or the accuracy
 *                    is invalid, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa ecl2equ(), equ2ecl_vec()
 */
short ecl2equ_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy,
        const double *in, double *out) {
  static const char *fn = "ecl2equ_vec";
  double obl = 0.0, c, s, x, y, z;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Get obliquity, depending upon the "system" of the input coordinates.
  switch(coord_sys) {

    case NOVAS_MEAN_EQUATOR:        // Output: mean equator and equinox of date
    case NOVAS_TRUE_EQUATOR: {      // Output: true equator and equinox of date
      double oblm = NAN, oblt = NAN;

      // For these calculations we can assume TDB = TT (< 2 ms difference)
      const double jd_tdb = jd_tt;

      e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, NULL, NULL);
      obl = (coord_sys == NOVAS_MEAN_EQUATOR ? oblm : oblt) * DEGREE;
      break;
    }

    case NOVAS_GCRS_EQUATOR: {      // Output: GCRS
      static THREAD_LOCAL double ob2000;

      if(ob2000 == 0.0) {
        ob2000 = mean_obliq(JD_J2000);
      }

      obl = ob2000 * ARCSEC;
      break;
    }

    default:
      return novas_error(1, EINVAL, fn, "invalid equator type: %d", coord_sys);
  }

  x = in[0];
  y = in[1];
  z = in[2];

  c = cos(obl);
  s = sin(obl);

  // Rotate position vector to ecliptic system.
  out[0] = x;
  out[1] = y * c - z * s;
  out[2] = y * s + z * c;

  // Case where output vector is to be in ICRS, rotate from dynamical
  // system to ICRS.
  if(coord_sys == NOVAS_GCRS_EQUATOR) {
    frame_tie(out, J2000_TO_ICRS, out);
  }

  return 0;
}

/**
 * @deprecated  You should use the frame-based `novas_app_to_hor()` instead, or else the
 *              more explicit (less ambiguous) `tod_to_itrs()` followed by `itrs_to_hor()`,
 *              and possibly following it with an atmospheric refraction correction if
 *              appropriate.
 *
 * Transforms topocentric (TOD) apparent right ascension and declination to zenith distance and
 * azimuth. This method should not be used to convert CIRS apparent coordinates (IAU 2000
 * standard) -- for those you should use cirs_to_itrs() followed by itrs_to_hor() instead.
 *
 * It uses a method that properly accounts for polar motion, which is significant at the
 * sub-arcsecond level. This function can also adjust coordinates for atmospheric
 * refraction.
 *
 * NOTES:
 * <ul>
 *  <li>'xp' and 'yp' can be set to zero if sub-arcsecond accuracy is not needed.</li>
 *  <li> The directions 'zd'= 0 (zenith) and 'az'= 0 (north) are here considered fixed in
 *  the terrestrial system. Specifically, the zenith is along the geodetic normal, and north
 *  is toward the ITRS pole.</li>
 *  <li>If 'ref_option' is NOVAS_STANDARD_ATMOSPHERE (1), then 'rar'='ra' and 'decr'='dec'.
 * </ul>
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2008). USNO/AA Technical Note of 28 Apr 2008, "Refraction as a Vector."</li>
 * </ol>
 *
 * @param jd_ut1      [day] UT1 based Julian date
 * @param ut1_to_tt   [s] TT - UT1 Time difference in seconds
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param xp          [arcsec] Conventionally-defined x coordinate of celestial intermediate
 *                    pole with respect to ITRS reference pole, e.g. from IERS Bulletin A.
 *                    If you have defined pole offsets to be incorporated into the TOD input
 *                    coordinates (pre-IAU2000 method) via `cel_pole()`, then you should set this
 *                    to 0.
 * @param yp          [arcsec] Conventionally-defined y coordinate of celestial intermediate
 *                    pole with respect to ITRS reference pole, e.g. from IERS Bulletin A. If you
 *                    have defined pole offsets to be incorporated into the TOD input coordinates
 *                    (pre-IAU2000 method) via `cel_pole()`, then you should set this to 0.
 * @param location    Geodetic (ITRF / GRS80) observer location on Earth
 * @param ra          [h] Topocentric apparent (TOD) right ascension of object of interest,
 *                    referred to true equator and equinox of date.
 * @param dec         [deg] Topocentric apparent (TOD) declination of object of interest, referred
 *                    to true equator and equinox of date.
 * @param ref_option  Refraction model to use. E.g., NOVAS_STANDARD_ATMOSPHERE (1), or
 *                    NOVAS_WEATHER_AT_LOCATION (2) if to use the weather.
 * @param[out] zd     [deg] Topocentric zenith distance in degrees (unrefracted).
 * @param[out] az     [deg] Topocentric azimuth (measured east from north) in degrees.
 * @param[out] rar    [h] Topocentric right ascension of object of interest, in hours, referred
 *                    to true equator and equinox of date, affected by refraction if 'ref_option'
 *                    is non-zero. (It may be NULL if not required)
 * @param[out] decr   [deg] Topocentric declination of object of interest, in degrees, referred
 *                    to true equator and equinox of date. (It may be NULL if not required)
 * @return            0 if successful, or -1 if one of the 'zd' or 'az' output pointers are NULL.
 *
 * @sa itrs_to_hor(), tod_to_itrs(), NOVAS_TOD
 */
int equ2hor(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp,
        const on_surface *restrict location, double ra, double dec, enum novas_refraction_model ref_option,
        double *restrict zd, double *restrict az, double *restrict rar, double *restrict decr) {

  double lon, lat, sinlat, coslat, sinlon, coslon, cosdec;
  double uze[3], une[3], uwe[3], uz[3], un[3], uw[3], p[3];
  double pz, pn, pw, proj, pr[3];

  // Default output values in case of error return;
  if(az)
    *az = NAN;
  if(zd)
    *zd = NAN;
  if(rar)
    *rar = ra;
  if(decr)
    *decr = dec;

  if(!location || !zd || !az)
    return novas_error(-1, EINVAL, "equ2hor", "NULL input observer location=%p, output az=%p, or zd=%p pointer", location, az, zd);

  lon = location->longitude * DEGREE;
  lat = location->latitude * DEGREE;

  sinlat = sin(lat);
  coslat = cos(lat);
  sinlon = sin(lon);
  coslon = cos(lon);

  ra *= HOURANGLE;
  dec *= DEGREE;

  cosdec = cos(dec);

  // Define unit vector 'p' toward object in celestial system
  // (wrt equator and equinox of date).
  p[0] = cosdec * cos(ra);
  p[1] = cosdec * sin(ra);
  p[2] = sin(dec);

  // Set up orthonormal basis vectors in local Earth-fixed system.

  // Define vector toward local zenith in Earth-fixed system (z axis).
  uze[0] = coslat * coslon;
  uze[1] = coslat * sinlon;
  uze[2] = sinlat;

  // Define vector toward local north in Earth-fixed system (x axis).
  une[0] = -sinlat * coslon;
  une[1] = -sinlat * sinlon;
  une[2] = coslat;

  // Define vector toward local west in Earth-fixed system (y axis).
  uwe[0] = sinlon;
  uwe[1] = -coslon;
  uwe[2] = 0.0;

  // Obtain vectors in celestial system.

  // Rotate Earth-fixed orthonormal basis vectors to celestial system
  // (wrt equator and equinox of date).
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, uze, uz);
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, une, un);
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, uwe, uw);

  // Compute coordinates of object w.r.t orthonormal basis.

  // Compute components of 'p' - projections of 'p' onto rotated
  // Earth-fixed basis vectors.
  pz = novas_vdot(p, uz);
  pn = novas_vdot(p, un);
  pw = novas_vdot(p, uw);

  // Compute azimuth and zenith distance.
  proj = sqrt(pn * pn + pw * pw);
  *az = proj ? -atan2(pw, pn) / DEGREE : 0.0;
  if(*az < 0.0)
    *az += DEG360;

  *zd = atan2(proj, pz) / DEGREE;

  // Apply atmospheric refraction if requested.
  if(ref_option) {
    // Get refraction in zenith distance.
    const double zd0 = *zd;
    const double refr = refract_astro(location, ref_option, zd0);

    // Apply refraction to celestial coordinates of object.
    if(refr) {
      // Shift position vector of object in celestial system to account
      // for refraction (see USNO/AA Technical Note 1998-09).
      const double sinzd = sin(*zd * DEGREE);
      const double coszd = cos(*zd * DEGREE);
      const double sinzd0 = sin(zd0 * DEGREE);
      const double coszd0 = cos(zd0 * DEGREE);

      int j;

      *zd -= refr;

      // Compute refracted position vector.
      for(j = 3; --j >= 0;)
        pr[j] = ((p[j] - coszd0 * uz[j]) / sinzd0) * sinzd + uz[j] * coszd;

      // Compute refracted right ascension and declination.
      proj = sqrt(pr[0] * pr[0] + pr[1] * pr[1]);

      if(rar) {
        *rar = proj ? atan2(pr[1], pr[0]) / HOURANGLE : 0.0;
        if(*rar < 0.0)
          *rar += DAY_HOURS;
      }

      if(decr)
        *decr = atan2(pr[2], proj) / DEGREE;
    }
  }
  return 0;
}

/**
 * Converts a 3D line-of-sight vector (&delta;&phi;, &delta;&theta; &delta;r) to a rectangular
 * equatorial (&delta;x, &delta;y, &delta;z) vector.
 *
 * @param los         [arb.u.] Line-of-sight 3-vector (&delta;&phi;, &delta;&theta; &delta;r).
 * @param lon         [deg] Line-of-sight longitude.
 * @param lat         [deg] Line-of-sight latitude.
 * @param[out] xyz    [arb.u.] Output rectangular equatorial 3-vector (&delta;x, &delta;y, &delta;z),
 *                    in the same units as the input. It may be the same vector as the input.
 * @return            0 if successful, or else -1 if either vector argument is NULL (errno will be
 *                    set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_xyz_to_los(), novas_uvw_to_xyz()
 */
int novas_los_to_xyz(const double *los, double lon, double lat, double *xyz) {
  static const char *fn = "novas_los_to_xyz";

  double slon, clon, slat, clat;
  double dlon, dlat, dr, clatdr_m_slatdlat;

  if(!xyz)
    return novas_error(-1, EINVAL, fn, "output xyz vector is NULL");

  if(xyz != los)
    memset(xyz, 0, XYZ_VECTOR_SIZE);

  if(!los)
    return novas_error(-1, EINVAL, fn, "input los vector is NULL");

  lon *= DEGREE;
  lat *= DEGREE;

  slon = sin(lon);
  clon = cos(lon);
  slat = sin(lat);
  clat = cos(lat);

  dlon = los[0];
  dlat = los[1];
  dr = los[2];

  clatdr_m_slatdlat = clat * dr - slat * dlat;

  // Transform motion vector to equatorial system.
  xyz[0] = clon * clatdr_m_slatdlat - slon * dlon;
  xyz[1] = clon * dlon + slon * clatdr_m_slatdlat;
  xyz[2] = clat * dlat + slat * dr;

  return 0;
}

/**
 * Converts a 3D rectangular equatorial (&delta;x, &delta;y, &delta;z) vector to a polar
 * (&delta;&phi;, &delta;&theta; &delta;r) vector along a line-of-sight.
 *
 * @param xyz         [arb.u.] Rectangular equatorial 3-vector (&delta;x, &delta;y, &delta;z).
 * @param lon         [deg] Line-of-sight longitude.
 * @param lat         [deg] Line-of-sight latitude.
 * @param[out] los    [arb.u.] Output line-of-sight 3-vector (&delta;&phi;, &delta;&theta;
 *                    &delta;r), in the same units as the input. It may be the same vector as the
 *                    input.
 * @return            0 if successful, or else -1 if either vector argument is NULL (errno will be
 *                    set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_los_to_xyz(), novas_xyz_to_uvw()
 */
int novas_xyz_to_los(const double *xyz, double lon, double lat, double *los) {
  static const char *fn = "novas_xyz_to_los";

  double slon, clon, slat, clat;
  double x, y, z, clonx_slony;

  if(!los)
    return novas_error(-1, EINVAL, fn, "output los vector is NULL");

  if(los != xyz)
    memset(los, 0, XYZ_VECTOR_SIZE);

  if(!xyz)
    return novas_error(-1, EINVAL, fn, "input xyz vector is NULL");

  lon *= DEGREE;
  lat *= DEGREE;

  slon = sin(lon);
  clon = cos(lon);
  slat = sin(lat);
  clat = cos(lat);

  x = xyz[0];
  y = xyz[1];
  z = xyz[2];

  clonx_slony = clon * x + slon * y;

  // Transform motion vector to equatorial system.
  los[0] = clon * y - slon * x;
  los[1] = clat * z - slat * clonx_slony;
  los[2] = clat * clonx_slony + slat * z;

  return 0;
}

/**
 * Converts rectangular telescope x,y,z (absolute or relative) coordinates (in ITRS) to equatorial
 * u,v,w projected coordinates for a specified line of sight.
 *
 * x,y,z are Cartesian coordinates w.r.t the Greenwich meridian, in the ITRS frame. The directions
 * are x: long=0, lat=0; y: long=90, lat=0; z: lat=90.
 *
 * u,v,w are Cartesian coordinates (u,v) along the local equatorial R.A. and declination
 * directions as seen from a direction on the sky (w). As such, they are effectively ITRS-based
 * line-of-sight (LOS) coordinates.
 *
 * @param xyz           [arb.u.] Absolute or relative x,y,z coordinates (double[3]).
 * @param ha            [h] Hourangle (LST - RA) i.e., the difference between the Local (apparent)
 *                      Sidereal Time and the apparent (true-of-date) Right Ascension of observed
 *                      source.
 * @param dec           [deg] Apparent (true-of-date) declination of source
 * @param[out] uvw      [arb.u.] Converted u,v,w coordinates (double[3]) in same units as xyz.
 *                      It may be the same vector as the input.
 *
 * @return              0 if successful, or else -1 if either vector argument is NULL (errno will
 *                      be set to EINVAL)
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_uvw_to_xyz()
 */
int novas_xyz_to_uvw(const double *xyz, double ha, double dec, double *uvw) {
  prop_error("novas_xyz_to_uvw", novas_xyz_to_los(xyz, -15.0 * ha, dec, uvw), 0);
  return 0;
}

/**
 * Converts equatorial u,v,w projected (absolute or relative) coordinates to rectangular telescope
 * x,y,z coordinates (in ITRS) to for a specified line of sight.
 *
 * u,v,w are Cartesian coordinates (u,v) along the local equatorial R.A. and declination
 * directions as seen from a direction on the sky (w). As such, they are effectively ITRS-based
 * line-of-sight (LOS) coordinates.
 *
 * x,y,z are Cartesian coordinates w.r.t the Greenwich meridian in the ITRS frame. The directions
 * are x: long=0, lat=0; y: long=90, lat=0; z: lat=90.
 *
 * @param xyz           [arb.u.] Absolute or relative u,v,w coordinates (double[3]).
 * @param ha            [h] Hourangle (LST - RA) i.e., the difference between the Local (apparent)
 *                      Sidereal Time and the apparent (true-of-date) Right Ascension of observed
 *                      source.
 * @param dec           [deg] Apparent (true-of-date) declination of source
 * @param[out] uvw      [arb.u.] Converted x,y,z coordinates (double[3]) in the same unit as uvw.
 *                      It may be the same vector as the input.
 *
 * @return              0 if successful, or else -1 if either vector argument is NULL
 *                      (errno will be set to EINVAL)
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_xyz_to_uvw()
 */
int novas_uvw_to_xyz(const double *uvw, double ha, double dec, double *xyz) {
  prop_error("novas_uvw_to_xyz", novas_los_to_xyz(uvw, -15.0 * ha, dec, xyz), 0);
  return 0;
}

/**
 * Converts an ITRF position vector to a local East-North-Up (ENU) vector at the
 * specified site.
 *
 * @param itrf        _xyz_ position vector in ITRF.
 * @param lon         [deg] ITRF longitude of site.
 * @param lat         [deg] ITRF latitude of site.
 * @param[out] enu    output East-North-Up (ENU) vector. It may be the same vector as the input.
 * @return            0 if successful, or else -1 if either pointer argument is NULL (errno will be
 *                    set to EINVAL).
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_enu_to_itrf()
 */
int novas_itrs_to_enu(const double *itrf, double lon, double lat, double *enu) {
  static const char *fn = "novas_itrs_to_enu";

  double x, y, z;
  double slon, clon, slat, clat;

  if(!itrf)
    return novas_error(-1, EINVAL, fn, "input ITRF vector is NULL");
  if(!enu)
    return novas_error(-1, EINVAL, fn, "output ENU vector is NULL");

  x = itrf[0];
  y = itrf[1];
  z = itrf[2];

  slon = sin(lon * DEGREE);
  clon = cos(lon * DEGREE);
  slat = sin(lat * DEGREE);
  clat = cos(lat * DEGREE);

  enu[0] = -slon * x + clon * y;
  enu[1] = -slat * (clon * x + slon * y) + clat * z;
  enu[2] =  clat * (clon * x + slon * y) + slat * z;

  return 0;
}

/**
 * Converts an  East-North-Up (ENU) vector at the specified site to an ITRF vector.
 *
 * @param enu         input East-North-Up (ENU) vector.
 * @param lon         [deg] ITRF longitude of site.
 * @param lat         [deg] ITRF latitude of site.
 * @param[out] itrf   output ITRF vector. It may be the same vector as the input.
 * @return            0 if successful, or else -1 if either pointer argument is NULL (errno will be
 *                    set to EINVAL).
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_itrf_to_enu()
 */
int novas_enu_to_itrs(const double *enu, double lon, double lat, double *itrf) {
  static const char *fn = "novas_itrs_to_enu";

  double E, N, U;
  double slon, clon, slat, clat;

  if(!enu)
    return novas_error(-1, EINVAL, fn, "input ENU vector is NULL");
  if(!itrf)
    return novas_error(-1, EINVAL, fn, "output ITRF vector is NULL");

  E = enu[0];
  N = enu[1];
  U = enu[2];

  slon = sin(lon * DEGREE);
  clon = cos(lon * DEGREE);
  slat = sin(lat * DEGREE);
  clat = cos(lat * DEGREE);

  itrf[0] = -slon * E - clon * (slat * N - clat * U);
  itrf[1] =  clon * E - slon * (slat * N - clat * U);
  itrf[2] =  clat * N + slat * U;

  return 0;
}


/**
 * Returns the Parallactic Angle (PA) calculated for a horizontal Az/El location of the
 * sky. The PA is the angle between the local horizontal coordinate directions and the local
 * true-of-date equatorial coordinate directions at the given location. The polar wobble is not
 * included in the calculation.
 *
 * The Parallactic Angle is sometimes referrred to as the Vertical Position Angle (VPA). Both
 * define the same quantity.
 *
 * @param az    [deg] Azimuth angle
 * @param el    [deg] Elevation angle
 * @param lat   [deg] Geodetic latitude of observer
 * @return      [deg] Parallactic Angle (PA). I.e., the clockwise position angle of the
 *              declination direction w.r.t. the elevation axis in the horizontal system. Same as
 *              the the clockwise position angle of the elevation direction w.r.t. the declination
 *              axis in the equatorial system.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_epa(), novas_h2e_offset()
 */
double novas_hpa(double az, double el, double lat) {
  double s, c;

  lat *= DEGREE;
  az *= DEGREE;
  el *= DEGREE;

  s = sin(lat);
  c = cos(lat);

  return atan2(-c * sin(az), s * cos(el) - c * sin(el) * cos(az)) / DEGREE;
}

/**
 * Returns the Parallactic Angle (PA) calculated for an RA/Dec location of the sky at
 * a given sidereal time. The PA is the angle between the local horizontal coordinate directions
 * and the local true-of-date equatorial coordinate directions, at the given location and time.
 * The polar wobble is not included in the calculation.
 *
 * The Parallactic Angle is sometimes referrred to as the Vertical Position Angle (VPA). Both
 * define the same quantity.
 *
 * @param ha      [h] Hour angle (LST - RA) i.e., the difference between the Local (apparent)
 *                Sidereal Time and the apparent (true-of-date) Right Ascension of observed
 *                source.
 * @param dec     [deg] Apparent (true-of-date) declination of observed source
 * @param lat     [deg] Geodetic latitude of observer
 * @return        [deg] Parallactic Angle (PA). I.e., the clockwise position angle of the
 *                elevation direction w.r.t. the declination axis in the equatorial system. Same
 *                as the clockwise position angle of the declination direction w.r.t. the
 *                elevation axis, in the horizontal system.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_hpa(), novas_e2h_offset()
 */
double novas_epa(double ha, double dec, double lat) {
  double coslat;

  ha *= HOURANGLE;
  lat *= DEGREE;
  dec *= DEGREE;

  coslat = cos(lat);
  return atan2(coslat * sin(ha), sin(lat) * cos(dec) - coslat * sin(dec) * cos(ha)) / DEGREE;
}

/**
 * Converts coordinate offsets, from the local horizontal system to local equatorial offsets.
 * Converting between local flat projections and spherical coordinates usually requires a WCS
 * projection.
 *
 * REFERENCES:
 * <ol>
 * <li>Calabretta, M.R., &amp; Greisen, E.W., (2002), Astronomy &amp; Astrophysics, 395, 1077-1122.</li>
 * </ol>
 *
 * @param daz         [arcsec] Projected offset position in the azimuth direction. The projected
 *                    offset between two azimuth positions at the same reference elevation is
 *                    &delta;Az = (Az2 - Az1) * cos(El<sub>0</sub>).
 * @param del         [arcsec] projected offset position in the elevation direction
 * @param pa          [deg] Parallactic Angle
 * @param[out] dra    [arcsec] Output offset position in the local true-of-date R.A. direction. It
 *                    can be a pointer to one of the input coordinates, or NULL if not required.
 * @param[out] ddec   [arcsec] Output offset position in the local true-of-date declination
 *                    direction. It can be a pointer to one of the input coordinates, or NULL if
 *                    not required.
 * @return            0
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_e2h_offset(), novas_hpa()
 */
int novas_h2e_offset(double daz, double del, double pa, double *restrict dra, double *restrict ddec) {
  double dx = daz, dy = del, c, s;

  pa *= DEGREE;
  c = cos(pa);
  s = sin(pa);

  if(dra)
    *dra =  s * dy - c * dx;
  if(ddec)
    *ddec = s * dx + c * dy;

  return 0;
}

/**
 * Converts coordinate offsets, from the local equatorial system to local horizontal offsets.
 * Converting between local flat projections and spherical coordinates usually requires a WCS
 * projection.
 *
 * REFERENCES:
 * <ol>
 * <li>Calabretta, M.R., &amp; Greisen, E.W., (2002), Astronomy &amp; Astrophysics, 395, 1077-1122.</li>
 * </ol>
 *
 * @param dra         [arcsec] Projected ffset position in the apparent true-of-date R.A.
 *                    direction. E.g. The projected offset between two RA coordinates at a same
 *                    reference declination, is &delta;RA = (RA2 - RA1) * cos(Dec<sub>0</sub>).
 * @param ddec        [arcsec] Projected offset position in the apparent true-of-date declination
 *                    direction.
 * @param pa          [deg] Parallactic Angle
 * @param[out] daz    [arcsec] Output offset position in the local azimuth direction. It can be a
 *                    pointer to one of the input coordinates, or NULL if not required.
 * @param[out] del    [arcsec] Output offset position in the local elevation direction. It can be a
 *                    pointer to one of the input coordinates, or NULL if not required.
 * @return            0
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_h2e_offset(), novas_epa()
 */
int novas_e2h_offset(double dra, double ddec, double pa, double *restrict daz, double *restrict del) {
  return novas_h2e_offset(dra, ddec, pa, daz, del);
}

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif
