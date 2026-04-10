/**
 * @file
 *
 * @date Created  on Apr 10, 2026
 * @author Attila Kovacs
 */

#ifndef NOVAS_NUT_H_
#define NOVAS_NUT_H_

#include <stdint.h>

/**
 * Data structure to contain coefficients for a single periodic nutation term.
 *
 */
typedef struct {
  int32_t A;         ///< [10 nas] Sine term coefficient
  int32_t B;         ///< [10 nas] Cosine term coefficient
  int8_t n[14];      ///< multiples (5 fund args + 8 planets [Mercury -- Neptune] + accumulated precession)
  int8_t from;       ///< index of first non-zero multiple in n[].
  int8_t to;         ///< index after the last non-zero multiple in n[].
} nutation_terms;


/**
 * Calculates the nutation series.
 *
 * REFERENCES:
 *
 *  1. IERS Conventions (2003), Chapter 5.
 *  2. Simon et al. (1994) Astronomy and Astrophysics 282, 663-683, esp. Sections 3.4-3.5.
 *  3. Capitaine, N. et al. (2003), Astronomy And Astrophysics 412, pp. 567-586.
 *  4. https://hpiers.obspm.fr/eop-pc/models/nutations/nut.html
 *
 * @param jd_tt_high  [day] High-order part of the Terrestrial Time (TT) based Julian date.
 *                    Typically it may be the integer part of a split date for the highest
 *                    precision, or the full date for normal (reduced) precision.
 * @param jd_tt_low   [day] Low-order part of the Terrestrial Time (TT) based Julian date.
 *                    Typically, it may be the fractional part of a split date for the highest
 *                    precision, or 0.0 for normal (reduced) precision.
 * @param[out] dpsi   [rad] &delta;&psi; Nutation (luni-solar + planetary) in longitude. It may be
 *                    NULL if not required.
 * @param[out] deps   [rad] &delta;&epsilon; Nutation (luni-solar + planetary) in obliquity. It
 *                    may be NULL if not required.
 * @return            0
 *
 * @sa iau2000a(), iau2000b()
 */
typedef int (*nutation_function)(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps);

extern nutation_function user_iau2000a;
extern nutation_function user_iau2000b;


#endif /* NOVAS_NUT_H_ */
