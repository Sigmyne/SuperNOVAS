/**
 * @file
 *
 *  Various commonly used routines used throughout the SuperNOVAS library.
 *
 * @date Created  on Mar 6, 2025
 * @author G. Kaplan, Attila Kovacs, and Kiran Shila
 */

#include <stdarg.h>               // before stdio for vfprintf on LynxOS 3.1
#include <stdio.h>                // vfprintf()
#include <string.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"

/// \cond PRIVATE
#define MAX_SECONDS_DECIMALS      9                 ///< Maximum decimal places for seconds in HMS/DMS formats
/// \endcond


#ifdef WITHOUT_LIBC
#  define DEFAULT_ERROR_HANDLER   NULL                    ///< No default error handler without LIBC
#else
#  define DEFAULT_ERROR_HANDLER   default_error_handler   ///< Prints debug error messages to `stderr`.

/// Default error handler: writes the formatted message to stderr.
static void default_error_handler(const char *fmt, va_list args) {
  vfprintf(stderr, fmt, args);
}
#endif // LIBC


/// Current debugging state for reporting errors and traces to stderr.
static enum novas_debug_mode novas_debug_state = NOVAS_DEBUG_OFF;

/// Active error handler; restored to default_error_handler when NULL is passed to novas_set_error_handler().
static novas_error_handler error_handler_fn = DEFAULT_ERROR_HANDLER;

/**
 * Replaces the trace / error message handler. Pass NULL to restore the default error reporting to
 * `stderr`. Or, to silence error output entirely, use `novas_debug(NOVAS_DEBUG_OFF)`.
 *
 * When built with `WITHOUT_LIBC` there is no default stderr handler, so passing NULL silences
 * output entirely (same effect as `novas_debug(NOVAS_DEBUG_OFF)`).
 *
 * NOTES:
 *
 *  - This call is not thread-safe. You should avoid calling it from concurrent threads if you
 *    want it to have predictable behavior.
 *
 * @param handler  new handler, or NULL to restore the default error messaging to `stderr`
 *                 (or to silence output when built with `WITHOUT_LIBC`).
 * @return         the previous handler (so it can be restored or chained)
 *
 * @since 1.7
 * @author Kiran Shila, Attila Kovacs
 *
 * @sa novas_debug()
 */
novas_error_handler novas_set_error_handler(novas_error_handler handler) {
  novas_error_handler prev = error_handler_fn;
  error_handler_fn = handler ? handler : DEFAULT_ERROR_HANDLER;
  return prev;
}

static void novas_emit(const char *fmt, ...) {
  if(!error_handler_fn)
    return;
  {
    va_list args;
    va_start(args, fmt);
    error_handler_fn(fmt, args);
    va_end(args);
  }
}

/**
 * Maximum number of iterations for convergent inverse calculations. Most iterative inverse
 * functions should normally converge in a handful of iterations. In some pathological cases more
 * iterations may be required. This variable sets an absolute maximum for the number of iterations
 * in order to avoid runaway (zombie) behaviour. If inverse functions fail to converge, they will
 * return a value indicating an error, and errno should be set to ECANCELED.
 *
 * @since 1.1
 */
int novas_inv_max_iter = 100;

/// \cond PROTECTED

#ifdef WITHOUT_SNPRINTF
/**
 * A dummy local `snprintf()` implementation when it is not provided by libc, such as on some very
 * old platforms. It simply behaves like `sprintf()`, ignoring the `len` argument altogether.
 *
 * @param buf     buffer to print to
 * @param len     maximum number of characters that can be printed in the buffer (ignored)
 * @param fmt     format string, as for `printf()`.
 * @return        the number of character printed (may be larger than `len`!).
 *
 * @author Attila Kovacs
 */
int novas_snprintf(char *buf, size_t len, const char *fmt, ...) {
  va_list varg;
  int n;

  (void) len; // unused

  va_start(varg, fmt);
  n = vsprintf(buf, fmt, varg);
  va_end(varg);
  return n;
}
#endif  // WITHOUT_SNPRINTF

/**
 * (_for internal use_) Propagates an error (if any) with an offset. If the error is
 * non-zero, it returns with the offset error value. Otherwise it keeps going as if it weren't
 * even there...
 *
 * @param loc     Function [:location] where error was produced.
 * @param n       error code that was received.
 * @param offset  Offset to add to error code (if &lt;0) to return.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_error()
 */
int novas_trace(const char *restrict loc, int n, int offset) {
  if(n != 0) {
    n = n < 0 ? -1 : n + offset;
    if(novas_get_debug_mode() != NOVAS_DEBUG_OFF)
      novas_emit("       @ %s [=> %d]\n", loc, n);
  }
  return n;
}

/**
 * (_for internal use_) Traces an error before returning NAN.
 *
 * @param loc    function [:location] where error was produced.
 * @return       NAN
 *
 * @since 1.1.1
 * @author Attila Kovacs
 */
double novas_trace_nan(const char *restrict loc) {
  if(novas_get_debug_mode() != NOVAS_DEBUG_OFF)
    novas_emit("       @ %s [=> NAN]\n", loc);
  return NAN;
}

/**
 * (_for internal use_) Traces invalid data.
 *
 * @param loc    function [:location] where error was produced.
 *
 * @since 1.1.1
 * @author Attila Kovacs
 */
void novas_trace_invalid(const char *restrict loc) {
  if(novas_get_debug_mode() != NOVAS_DEBUG_OFF)
    novas_emit("       @ %s [=> invalid]\n", loc);
}



/**
 * (_for internal use_) Checks a floating point value for NAN, before returning it.
 * If it is NAN the error is traced.
 *
 * @param loc   function [:location] where error was produced.
 * @param x     value to check
 * @return      x
 *
 * @since 1.6
 * @author Attila Kovacs
 */
double novas_check_nan(const char *loc, double x) {
  if(isnan(x))
    novas_trace_nan(loc);
  return x;
}

/**
 * (_for internal use_) Sets an errno and report errors to the standard error, depending
 * on the current debug mode.
 *
 * @param en    {int} UNIX error number (see errno.h)
 * @param from  {string} Function (:location) where error originated
 * @param desc  {string} Description of error, with information to convey to user.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_error(), novas_debug()
 */
void novas_set_errno(int en, const char *restrict from, const char *restrict desc, ...) {
  va_list varg;

  va_start(varg, desc);
  if(novas_get_debug_mode() != NOVAS_DEBUG_OFF && error_handler_fn) {
    novas_emit("\n  ERROR! %s: ", from);
    error_handler_fn(desc, varg);
    novas_emit("\n");
  }
  va_end(varg);

  errno = en;
}

/**
 * (_for internal use_) Sets errno and reports errors to the standard error, depending
 * on the current debug mode, before returning the supplied return code.
 *
 * @param ret   return value
 * @param en    UNIX error code (see errno.h)
 * @param from  function (:location) where error originated
 * @param desc  description of error, with information to convey to user.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_set_errno(), novas_trace()
 */
int novas_error(int ret, int en, const char *restrict from, const char *restrict desc, ...) {
  va_list varg;

  va_start(varg, desc);
  if(novas_get_debug_mode() != NOVAS_DEBUG_OFF && error_handler_fn) {
    novas_emit("\n  ERROR! %s: ", from);
    error_handler_fn(desc, varg);
    novas_emit(" [=> %d]\n", ret);
  }
  va_end(varg);

  errno = en;
  return ret;
}

/**
 * (_for internal use only_) Calculates the distance-squared between two 3-vectors.
 *
 * @param v1    Pointer to a 3-component (x, y, z) vector. The argument cannot be NULL
 * @param v2    Pointer to another 3-component (x, y, z) vector. The argument cannot
 *              be NULL
 * @return      The distance squared between the two vectors
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_vlen(), novas_vdot()
 */
double novas_vdist2(const double *v1, const double *v2) {
  double d2 = 0.0;
  int i;
  for(i = 3; --i >= 0;) {
    const double d = v1[i] - v2[i];
    d2 += d * d;
  }
  return d2;
}

/**
 * (_for internal use only_) Calculates the distance between two 3-vectors.
 *
 * @param v1    Pointer to a 3-component (x, y, z) vector. The argument cannot be NULL
 * @param v2    Pointer to another 3-component (x, y, z) vector. The argument cannot
 *              be NULL
 * @return      The distance between the two vectors
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_vlen(), novas_vdot()
 */
double novas_vdist(const double *v1, const double *v2) {
  return sqrt(novas_vdist2(v1, v2));
}

/**
 * (_for internal use only_) Calculates the dot product between two 3-vectors.
 *
 * @param v1    Pointer to a 3-component (x, y, z) vector. The argument cannot be NULL
 * @param v2    Pointer to another 3-component (x, y, z) vector. The argument cannot
 *              be NULL
 * @return      The dot product between the two vectors.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_vlen(), novas_vdist()
 */
double novas_vdot(const double *v1, const double *v2) {
  return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
}

/**
 * (_for internal use only_) Checks if two Julian dates are equal with regard to the dynamic
 * equator. The two dates are considered equal if they agree within 10<sup>-7</sup> days (or about
 * 10 ms).
 *
 * For reference, with a precession rate of ~50"/year, the precession in in 10 ms is on the order
 * of 0.015 &mu;as, which is well below the promised sub-&mu;as precision even for the highest
 * accuracy calculations. As such it is safe to use the reduced accuracy time check for cached
 * precession-related quantities. However, coordinates in an non-inertial Earth-rotating frame
 * will be accurate to ~150 mas only, so not so safe for precise Earth rotation measures.
 *
 * @param jd1       [day] a Julian date (in any time measure)
 * @param jd2       [day] a Julian date in the same time measure as the first argument
 * @return          TRUE (1) if the two dates are effectively the same at the precision of
 *                  comparison, or else FALSE (0) if they differ by more than the allowed
 *                  tolerance.
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int novas_time_equals(double jd1, double jd2) {
  return fabs(jd1 - jd2) < 1e-7;
}

/**
 * (_for internal use only_) Checks if two Julian dates are precisely equal with regard to the
 * dynamic equator. The two dates are considered equal if they agree within 10<sup>-9</sup> days
 * (or about 100 &mu;s) of each other.
 *
 * For reference, with a precession rate of ~50"/year, the precession in 100 &mu;s is on the
 * order of 0.00015 &mu;as, which is way below the promised sub-&mu;as precision even in for the
 * highest accuracy calculations. However, coordinates in a non-inertial Earth-rotating frame will
 * be accurate to ~1.5 mas only, which is still not very precise Earth rotation measures.
 *
 * @param jd1       [day] a Julian date (in any time measure)
 * @param jd2       [day] a Julian date in the same time measure as the first argument
 * @return          TRUE (1) if the two dates are effectively the same at the precision of
 *                  comparison, or else FALSE (0) if they differ by more than the allowed
 *                  tolerance.
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int novas_time_equals_hp(double jd1, double jd2) {
  return fabs(jd1 - jd2) < 1e-9;
}

/**
 * Computationally efficient implementation of 3D rotation with small angles.
 *
 * @param in            3D vector to rotate
 * @param ax            (rad) rotation angle around x
 * @param ay            (rad) rotation angle around y
 * @param az            (rad) rotation angle around z
 * @param[out] out      Rotated vector. It can be the same as the input.
 *
 * @since 1.0
 * @author Attila Kovacs
 */
void novas_tiny_rotate(const double *in, double ax, double ay, double az, double *out) {
  const double x = in[0], y = in[1], z = in[2];
  const double A[3] = { ax * ax, ay * ay, az * az };

  out[0] = x - 0.5 * (A[1] + A[2]) * x - az * y + ay * z;
  out[1] = y - 0.5 * (A[0] + A[2]) * y + az * x - ax * z;
  out[2] = z - 0.5 * (A[0] + A[1]) * z - ay * x + ax * y;
}

/**
 * Sets the maximum number of iterations allowed for convergent inverse calculations.
 *
 * NOTES:
 *
 *  - This call is not thread-safe. You should avoid calling it from concurrent threads if you
 *    want it to have predictable behavior.
 *
 * @param n   Maximum number of iterations allowed.
 *
 * @since 1.5
 * @author Attila Kovacs
 */
void novas_set_max_iter(int n) {
  novas_inv_max_iter = n;
}

static int novas_rot(double angle, double *x, double *y) {
  const double s = sin(angle);
  const double c = cos(angle);
  const double x0 = *x;

  *x =  c * x0 + s * (*y);
  *y = -s * x0 + c * (*y);

  return 0;
}

/**
 * Rotates a vector around the _x_-axis, counter clockwise when looking out from
 * the origin along _x_.
 *
 * @param angle       [rad] rotation angle
 * @param[in,out] v   Vector to rotate
 * @return            0 if successful, or else -1 if the vector argument is NULL
 *                    (errno set to EINVAL).
 *
 *  @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_Ry(), novas_Rz()
 */
int novas_Rx(double angle, double *v) {
  if(!v)
    return novas_error(-1, EINVAL, "novas_Rx", "input/output vector is NULL");
  return novas_rot(angle, &v[1], &v[2]);
}

/**
 * Rotates a vector around the _y_-axis, counter clockwise when looking out from
 * the origin along _y_.
 *
 * @param angle       [rad] rotation angle
 * @param[in,out] v   Vector to rotate
 * @return            0 if successful, or else -1 if the vector argument is NULL
 *                    (errno set to EINVAL).
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_Rx(), novas_Rz()
 */
int novas_Ry(double angle, double *v) {
  if(!v)
    return novas_error(-1, EINVAL, "novas_Ry", "input/output vector is NULL");
  return novas_rot(angle, &v[2], &v[0]);
}

/**
 * Rotates a vector around the _z_-axis, counter clockwise when looking out from
 * the origin along _z_.
 *
 * @param angle       [rad] rotation angle
 * @param[in,out] v   Vector to rotate
 * @return            0 if successful, or else -1 if the vector argument is NULL
 *                    (errno set to EINVAL).
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_Rx(), novas_Ry()
 */
int novas_Rz(double angle, double *v) {
  if(!v)
    return novas_error(-1, EINVAL, "novas_Rz", "input/output vector is NULL");
  return novas_rot(angle, &v[0], &v[1]);
}

/**
 * Returns a string representation of a decimal value with the specified precision. It is
 * effectively the same as `%.<n+1>g` where `<n>` is the number of decimal places to print.
 *
 * @param value       The value to print
 * @param decimals    Number of decimal places to show
 * @param[out] str    output string, with at least `len` bytes of space allocated.
 * @param len         maximum number of bytes to print into output buffer, including termination.
 * @return            the number of characters printed into the output string, excluding the
 *                    termination, or -1 if there was an error (errno will indicate the type of
 *                    error).
 *
 * @since 1.6
 * @author Attila Kovacs
 */
int novas_print_decimal(double value, int decimals, char *str, int len) {
  static const char *fn = "novas_print_decimal";

#ifdef WITHOUT_SNPRINTF
  char s[40] = {'\0'};
  int n;
#endif

  if(!str)
    return novas_error(-1, EINVAL, fn, "output string is NULL");

  if(len < 1)
    return novas_error(-1, EINVAL, fn, "invalid output string length: %d", len);

  if(decimals < 0)
    decimals = 0;
  else if(decimals > 15)
    decimals = 15;

#ifdef WITHOUT_SNPRINTF
  // We are using our dummy snprintf(), without length checking, so make sure
  // otherwise that we don't write more then len bytes into the output buffer.

  // We first print into a large enough local buffer...
  n = novas_snprintf(s, sizeof(s), "%.*g", (decimals + 1), value);

  // Use at most len-1 bytes from local buffer
  if(n >= len)
    n = len - 1; // count excluding termination

  memcpy(str, s, n); // Copy string from local buffer into output
  str[n] = '\0';     // Terminate the output buffer

  return n;
#else
  return novas_snprintf(str, len, "%.*g", (decimals + 1), value);
#endif
}

/// \endcond PROTECTED

// ===========================================================================

/**
 * Enables or disables reporting errors and traces to the standard error stream.
 *
 * @param mode    NOVAS_DEBUG_OFF (0; or &lt;0), NOVAS_DEBUG_ON (1), or NOVAS_DEBUG_EXTRA (2; or
 *                &gt;2).
 *
 * NOTES:
 *
 *  - This call is not thread-safe. You should avoid calling it from concurrent threads if you
 *    want it to have predictable behavior.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_get_debug_mode()
 */
void novas_debug(enum novas_debug_mode mode) {
  if(mode < 0)
    mode = NOVAS_DEBUG_OFF;
  else if(mode > NOVAS_DEBUG_EXTRA)
    mode = NOVAS_DEBUG_EXTRA;

  novas_debug_state = mode;
}

/**
 * Returns the current, thread-local, mode for reporting errors encountered (and traces).
 *
 * @return    The current debug mode in the calling thread.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_debug()
 */
enum novas_debug_mode novas_get_debug_mode() {
  return novas_debug_state;
}

/**
 * Returns the normalized angle in the [0:2&pi;) range.
 *
 * @param angle   [rad] an angle in radians.
 * @return        [rad] the normalized angle in the [0:2&pi;) range.
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double novas_norm_ang(double angle) {
  double a;

  a = remainder(angle, TWOPI);
  if(a < 0.0)
    a += TWOPI;

  return a;
}

/**
 * (_for internal use only_) Calculates the length of a 3-vector
 *
 * @param v     Pointer to a 3-component (x, y, z) vector. The argument cannot be NULL
 * @return      the length of the vector
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_vdot(), novas_vdist()
 */
double novas_vlen(const double *restrict v) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

/**
 * Transforms a vector from one coordinate system to another with same origin and axes rotated
 * about the z-axis.
 *
 * REFERENCES:
 *
 *  1. Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.
 *
 * @param angle   [deg] Angle of coordinate system rotation, positive counterclockwise when
 *                viewed from +z, in degrees.
 * @param in      Input position vector.
 * @param[out] out    Position vector expressed in new coordinate system rotated about z by
 *                    'angle'. It can be the same vector as the input.
 *
 * @return        0 if successful, or -1 if the output vector is NULL.
 *
 */
int spin(double angle, const double *in, double *out) {
  static THREAD_LOCAL double ang_last = -999.0;
  static THREAD_LOCAL double xx, yx, xy, yy, zz = 1.0;

  double x, y, z;

  if(!in || !out)
    return novas_error(-1, EINVAL, "spin", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  angle = remainder(angle * DEGREE, TWOPI);

  if(fabs(angle - ang_last) >= 1.0e-12) {
    const double cosang = cos(angle);
    const double sinang = sin(angle);

    // Rotation matrix (non-zero elements only).
    xx = cosang;
    yx = sinang;
    xy = -sinang;
    yy = cosang;

    ang_last = angle;
  }

  x = in[0];
  y = in[1];
  z = in[2];

  // Perform rotation.
  out[0] = xx * x + yx * y;
  out[1] = xy * x + yy * y;
  out[2] = zz * z;

  return 0;
}

/**
 * Converts a vector in equatorial rectangular coordinates to equatorial spherical
 * coordinates.
 *
 * REFERENCES:
 *
 *  1. Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.
 *
 * @param pos       Position 3-vector, equatorial rectangular coordinates.
 * @param[out] ra   [h] Right ascension in hours [0:24] or NAN if the position vector is NULL or a
 *                  null-vector. It may be NULL if not required.
 * @param[out] dec  [deg] Declination in degrees [-90:90] or NAN if the position vector is NULL or
 *                  a null-vector. It may be NULL if not required.
 * @return          0 if successful, -1 of any of the arguments are NULL, or
 *                  1 if all input components are 0 so 'ra' and 'dec' are indeterminate, or else 2
 *                  if both x and y are zero, but z is nonzero, and so 'ra' is indeterminate.
 *
 * @sa radec2vector()
 */
short vector2radec(const double *restrict pos, double *restrict ra, double *restrict dec) {
  static const char *fn = "vector2radec";
  double xyproj;

  // Default return values.
  if(ra)
    *ra = NAN;
  if(dec)
    *dec = NAN;

  if(!pos)
    return novas_error(-1, EINVAL, fn, "NULL input or output pointer: pos=%p, ra=%p, dec=%p", pos, ra, dec);

  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
  if(xyproj == 0.0) {
    if(pos[2] == 0)
      return novas_error(1, EINVAL, fn, "all input components are zero");

    if(ra)
      *ra = 0.0;
    if(dec)
      *dec = (pos[2] < 0.0) ? -90.0 : 90.0;

    return novas_error(2, EDOM, fn, "indeterminate RA for equatorial pole input");
  }

  if(ra) {
    *ra = atan2(pos[1], pos[0]) / HOURANGLE;
    if(*ra < 0.0)
      *ra += DAY_HOURS;
  }
  if(dec)
    *dec = atan2(pos[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Converts equatorial spherical coordinates to a vector (equatorial rectangular coordinates).
 *
 * @param ra          [h] Right ascension (hours).
 * @param dec         [deg] Declination (degrees).
 * @param dist        [AU] Distance (AU)
 * @param[out] pos    [AU] Position 3-vector, equatorial rectangular coordinates (AU).
 *
 * @return            0 if successful, or -1 if the vector argument is NULL.
 *
 * @sa vector2radec(), starvectors()
 */
int radec2vector(double ra, double dec, double dist, double *restrict pos) {
  double cosdec;

  if(!pos)
    return novas_error(-1, EINVAL, "radec2vector", "NULL output 3-vector");

  dec *= DEGREE;
  ra *= HOURANGLE;

  cosdec = cos(dec);

  pos[0] = dist * cosdec * cos(ra);
  pos[1] = dist * cosdec * sin(ra);
  pos[2] = dist * sin(dec);

  return 0;
}

/**
 * Returns the difference in light-time, for a star, between the barycenter of the solar system
 * and the observer (or the geocenter) (_Usage A_).
 *
 * Alternatively (_Usage B_), this function returns the light-time from the observer (or the
 * geocenter) to a point on a light ray that is closest to a specific solar system body.  For this
 * purpose, 'pos_src' is the position vector toward observed object, with respect to origin at
 * observer (or the geocenter); 'pos_body' is the position vector of solar system body, with
 * respect to origin at observer (or the geocenter), components in AU; and the returned value is
 * the light time to point on line defined by 'pos' that is closest to solar system body (positive
 * if light passes body before hitting observer, i.e., if 'pos_body' is within 90 degrees of
 * 'pos_src').
 *
 * NOTES:
 *
 *  1. This function is called by `novas_geom_posvel()`, novas_sky_pos(), or `place()`
 *
 * @param pos_src   Position vector towards observed object, with respect to the SSB
 *                  (_Usage A_), or relative to the observer / geocenter (_Usage B_).
 * @param pos_body  [AU] Position of observer relative to SSB (_Usage A_), or position of
 *                  intermediate solar-system body with respect to the observer / geocenter
 *                  (_Usage B_).
 * @return          [day] Difference in light time to observer, either relative to SSB
 *                  (_Usage A_) or relative intermediate solar-system body
 *                  (_Usage B_); or else NAN if either of the input arguments is NULL.
 *
 * @sa novas_sky_pos(), novas_geom_posvel()
 */
double d_light(const double *pos_src, const double *pos_body) {
  double d_src;

  if(!pos_src || !pos_body) {
    novas_set_errno(EINVAL, "d_light", "NULL input 3-vector: pos_src=%p, pos_body=%p [=> NAN]", pos_src, pos_body);
    return NAN;
  }

  d_src = novas_vlen(pos_src);

  // Light-time returned is the projection of vector 'pos_obs' onto the
  // unit vector 'u1' (formed from 'pos_body'), divided by the speed of light.
  return d_src > 1e-30 ? novas_vdot(pos_body, pos_src) / d_src / C_AUDAY : 0.0;
}

/**
 * Returns the angular separation of two locations on a sphere. It uses the Vincenty formula for
 * accurate results across all locations on the sphere.
 *
 * NOTES:
 *
 *  1. Prior to version 1.5, this function used the law of cosines, which is imprecise for
 *     nearby locations
 *  2. In version 1.5, this function used the haversine formula, albeit with an error in the
 *     implementation. The haversine formula is accurate for nearby locations, but has rounding
 *     errors for antipodal locations.
 *  3. From version 1.5.1, this function uses the Vincenty formula, which is accurate for all
 *     locations on the sphere.
 *
 * REFERENCES:
 *
 *  1. Vincenty, Thaddeus. "Direct and Inverse Solutions of Geodesics on the Ellipsoid with
 *     Application of Nested Equations", Survey Review. 23 (176), Directorate of Overseas Surveys,
 *     doi:10.1179/sre.1975.23.176.88.
 *
 * @param lon1    [deg] longitude of first location
 * @param lat1    [deg] latitude of first location
 * @param lon2    [deg] longitude of second location
 * @param lat2    [deg] latitude of second location
 * @return        [deg] the angular separation of the two locations.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_equ_sep(), novas_sun_angle(), novas_moon_angle()
 * @sa novas_offset_by()
 */
double novas_sep(double lon1, double lat1, double lon2, double lat2) {
  const double cp1 = cos(lat1 * DEGREE);
  const double cp2 = cos(lat2 * DEGREE);
  const double sp1 = sin(lat1 * DEGREE);
  const double sp2 = sin(lat2 * DEGREE);
  const double cdl = cos((lon1 - lon2) * DEGREE);

  const double a = cp2 * sin((lon1 - lon2) * DEGREE);
  const double b = cp1 * sp2 - sp1 * cp2 * cdl;

  return atan2(sqrt(a*a + b * b), sp1 * sp2 + cp1 * cp2 * cdl) / DEGREE;
}

/**
 * Returns the angular separation of two equatorial locations on a sphere.
 *
 * @param ra1     [h] right ascension of first location
 * @param dec1    [deg] declination of first location
 * @param ra2     [h] right ascension of second location
 * @param dec2    [deg] declination of second location
 * @return        [deg] the angular separation of the two locations.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_sep(), novas_sun_angle(), novas_moon_angle()
 * @sa novas_equ_offset_by()
 */
double novas_equ_sep(double ra1, double dec1, double ra2, double dec2) {
  return novas_sep(15.0 * ra1, dec1, 15.0 * ra2, dec2);
}

/**
 * Breaks down a value into hours/degrees, minutes, seconds, and a subsecond part given the
 * number of decimals requested. The last digit is rounded as appropriate.
 *
 * @param value     The input hours or degrees
 * @param decimals  Number of requested decimals for the sub-second component
 * @param[out] h    The hours or degrees part
 * @param[out] m    the minutes components
 * @param[out] s    the seconds component
 * @param[out] ss   the subseconds component for the given number of decimals.
 */
static void breakdown(double value, int decimals, int *h, int *m, int *s, long long *ss) {
  long long mult = (long long) pow(10.0, decimals > 0 ? decimals : 0);
  long long u;

  *ss = (long long) floor(value * 3600L * mult + 0.5);

  u = 3600L * mult;
  *h = (int) (*ss / u);
  *ss -= (*h) * u;

  u = 60L * mult;
  *m = (int) (*ss / u);
  *ss -= (*m) * u;

  *s = (int) (*ss / mult);
  *ss -= (*s) * mult;
}

/**
 * Prints a time in hours as hh:mm:ss[.S...] into the specified buffer, with up to nanosecond
 * precision.
 *
 * NaN and infinite values, are printed as their standard floating-point representations.
 *
 * @param hours       [h] time value
 * @param sep         Type of separators to use between or after components. If the separator
 *                    value is outside of the enum range, it will default to using colons.
 * @param decimals    Requested number of decimal places to print for the seconds [0:9].
 * @param[out] buf    String buffer in which to print HMS string, represented in the [0:24)
 *                    hour range.
 * @param len         Maximum number of bytes that may be written into the output buffer,
 *                    including termination.
 * @return            The number of characters actually printed in the buffer, excluding
 *                    termination, or else -1 if the input buffer is NULL or the length
 *                    is less than 1 (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_hms(), novas_print_dms(), novas_timestamp()
 */
int novas_print_hms(double hours, enum novas_separator_type sep, int decimals, char *restrict buf, int len) {
  static const char *fn = "novas_print_hms";

  char tmp[40] = {'\0'}; // 6 integer digits + up to 6 separator chars + decimal point + up to 9 decimals + termination = 23 max.

  if(!buf)
    return novas_error(-1, EINVAL, fn, "output buffer is NULL");

  if(len < 1)
    return novas_error(-1, EINVAL, fn, "invalid output buffer len: %d", len);

  if(hours != hours)
    novas_snprintf(tmp, sizeof(tmp), "%f", hours);      // nan, inf
  else {
    int h, m, s;
    long long ss;
    const char *seph, *sepm, *seps;

    if(decimals > MAX_SECONDS_DECIMALS)
      decimals = MAX_SECONDS_DECIMALS;

    switch(sep) {
      case NOVAS_SEP_UNITS:
        seph = "h";
        sepm = "m";
        seps = "s";
        break;

      case NOVAS_SEP_UNITS_AND_SPACES:
        seph = "h ";
        sepm = "m ";
        seps = "s";
        break;

      case NOVAS_SEP_SPACES:
        seph = " ";
        sepm = " ";
        seps = "";
        break;

      case NOVAS_SEP_COLONS:
      default:
        seph = ":";
        sepm = ":";
        seps = "";
    }

    // in [0:24h] range
    hours -= 24.0 * floor(hours / 24.0);
    breakdown(hours, decimals, &h, &m, &s, &ss);

    if(decimals > 0)
      novas_snprintf(tmp, sizeof(tmp), "%02d%s%02d%s%02d.%0*lld%s", h, seph, m, sepm, s, decimals, ss, seps);
    else
      novas_snprintf(tmp, sizeof(tmp), "%02d%s%02d%s%02d%s", h, seph, m, sepm, s, seps);
  }

  strncpy(buf, tmp, len - 1);
  buf[len-1] = '\0';

  return strlen(buf);
}

/**
 * Prints an angle in degrees as [-]ddd:mm:ss[.S...] into the specified buffer, with up to
 * nanosecond precision.
 *
 * The degrees component is always printed as 4 characters (up to 3 digits
 * plus optional negative sign), so the output is always aligned. If positive values are
 * expected to be explicitly signed also, the caller may simply put the '+' sign into the
 * leading byte.
 *
 * NaN and infinite values, are printed as their standard floating-point representations.
 *
 * @param degrees     [deg] angle value
 * @param sep         Type of separators to use between or after components. If the separator
 *                    value is outside of the enum range, it will default to using colons.
 * @param decimals    Requested number of decimal places to print for the seconds [0:9].
 * @param[out] buf    String buffer in which to print DMS string, represented in the [-180:180)
 *                    degree range.
 * @param len         Maximum number of bytes that may be written into the output buffer,
 *                    including termination.
 * @return            The number of characters actually printed in the buffer, excluding
 *                    termination, or else -1 if the input buffer is NULL or the length
 *                    is less than 1 (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_dms(), novas_print_hms()
 */
int novas_print_dms(double degrees, enum novas_separator_type sep, int decimals, char *restrict buf, int len) {
  static const char *fn = "novas_print_dms";

  char tmp[40] = {'\0'}; // sign + 7 integer digits + up to 6 separator chars + decimal point + up to 9 decimals + termination = 25 max.

  if(!buf)
    return novas_error(-1, EINVAL, fn, "output buffer is NULL");

  if(len < 1)
    return novas_error(-1, EINVAL, fn, "invalid output buffer len: %d", len);

  if(degrees != degrees)
    novas_snprintf(tmp, sizeof(tmp), "%f", degrees);      // nan, inf
  else {
    int d, m, s;
    long long ss;
    const char *sepd, *sepm, *seps;

    if(decimals > MAX_SECONDS_DECIMALS)
      decimals = MAX_SECONDS_DECIMALS;

    degrees = remainder(degrees, DEG360);
    if(degrees < 0.0) {
      breakdown(-degrees, decimals, &d, &m, &s, &ss);
      d = -d;
    }
    else {
      breakdown(degrees, decimals, &d, &m, &s, &ss);
    }

    switch(sep) {
      case NOVAS_SEP_UNITS:
        sepd = "d";
        sepm = "m";
        seps = "s";
        break;

      case NOVAS_SEP_UNITS_AND_SPACES:
        sepd = "d ";
        sepm = "m ";
        seps = "s";
        break;

      case NOVAS_SEP_SPACES:
        sepd = " ";
        sepm = " ";
        seps = "";
        break;

      case NOVAS_SEP_COLONS:
      default:
        sepd = ":";
        sepm = ":";
        seps = "";
    }

    if(decimals > 0)
      novas_snprintf(tmp, sizeof(tmp), "%4d%s%02d%s%02d.%0*lld%s", d, sepd, m, sepm, s, decimals, ss, seps);
    else
      novas_snprintf(tmp, sizeof(tmp), "%4d%s%02d%s%02d%s", d, sepd, m, sepm, s, seps);
  }

  strncpy(buf, tmp, len - 1);
  buf[len-1] = '\0';

  return strlen(buf);
}

/**
 * Calculates offset spherical coordinates at some distance away from the input coordinates along
 * a great circle which crosses the input position at the specified direction. Based on the
 * __astropy__ `offset_by()` implementation.
 *
 * @param lon             [deg] reference point longitude
 * @param lat             [deg] reference point latitude
 * @param direction       [deg] direction (measured East of North).
 * @param distance        [deg] offset distance on great circle
 * @param[out] out_lon    [deg] longitude at offset position in [-&pi;:&pi;) range. It may be NULL
 *                        if not needed.
 * @param[out] out_lat    [deg] latitude at offset position in [-&pi/2:&pi;/2] range. It may be
 *                        NULL if not needed.
 * @return                0 if successful, or else -1 if very close to the pole, and longitude at
 *                        the offset position is requested, or if the input latitude is outside of
 *                        the [-90:90] range.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_equ_offset_by(), novas_sep()
 * @sa https://docs.astropy.org/en/stable/_modules/astropy/coordinates/angles/utils.html#offset_at
 */
int novas_offset_by(double lon, double lat, double direction, double distance, double *restrict out_lon, double *restrict out_lat) {
  static const char *fn = "novas_offset_by";

  const double eps = 1e-12;  // precision tolerance around poles
  double clat, slat, cD, sD_clat, cb;

  if(fabs(lat) > 90.0 + eps)
    return novas_error(-1, EINVAL, fn, "latitude is outside of the [-90:90] range.");

  if(distance == 0.0) {
    // not offset at all...
    if(out_lon)
      *out_lon = lon;
    if(out_lat)
      *out_lat = lat;
    return 0;
  }

  clat = cos(lat * DEGREE);

  if(fabs(clat) < eps) {
    // Very close to the pole, longitude is undefined, but latitude can be determined
    // assuming that we start exactly from the pole.
    if(out_lat) {
      *out_lat = remainder(90.0 - distance, DEG360);

      // wrap as needed...
      if(*out_lat > 90.0)
        *out_lat = 180.0 - *out_lat;
      else if(*out_lat < -90.0)
        *out_lat = -180.0 - *out_lat;
    }

    if(out_lon) {
      *out_lon = NAN;
      return novas_error(-1, EDOM, fn, "longitude is undefined at the poles");
    }

    return 0;
  }

  slat = sin(lat * DEGREE);
  cD = cos(distance * DEGREE);
  sD_clat = sin(distance * DEGREE) * clat;
  cb = slat * cD + sD_clat * cos(direction * DEGREE);

  if(out_lon) {
    double dlon = atan2(sD_clat * sin(direction * DEGREE), cD - cb * slat) / DEGREE;
    *out_lon = remainder(lon + dlon, DEG360);
  }

  if(out_lat)
    *out_lat = asin(cb) / DEGREE;

  return 0;
}

/**
 * Calculates offset equatorial coordinates at some distance away from the input coordinates along
 * a great circle which crosses the input position at the specified position angle.
 *
 * @param ra              [h] reference point right ascension (R.A.)
 * @param dec             [deg] reference point declination
 * @param direction       [deg] direction (measured East of North).
 * @param distance        [deg] offset distance on great circle
 * @param[out] out_ra     [h] right ascension (R.A.) at offset position in [0:24) range. It may
 *                        be NULL if not needed.
 * @param[out] out_dec    [deg] declination at offset position in [-&pi/2:&pi;/2] range. It may be
 *                        NULL if not needed.
 * @return                0 if successful, or else -1 if very close to the pole, and right
 *                        ascension (R.A.) at the offset position is requested, or if the
 *                        input declination is outside of the [-90:90] range.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_offset_by(), novas_equ_sep()
 */
int novas_equ_offset_by(double ra, double dec, double direction, double distance, double *restrict out_ra, double *restrict out_dec) {
  double out_lon = NAN;

  int status = novas_offset_by(15.0 * ra, dec, direction, distance, &out_lon, out_dec);

  if(out_lon < 0.0)
    out_lon += DEG360;

  if(out_ra)
    *out_ra = out_lon / 15.0;

  prop_error("novas_equ_offset_by", status, 0);

  return 0;
}
