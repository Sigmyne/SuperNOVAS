/**
 * @file
 *
 * @date Created  on Apr 12, 2026
 * @author Attila Kovacs
 */

#ifndef NOVAS_EOP_H_
#define NOVAS_EOP_H_

#include <stdint.h>

/**
 * Structure to contain terms for librational and tidal corrections to earth orientation.
 */
typedef struct {
  int8_t n[6];    ///< [rad] gamma (GMST + &pi;), followed by the Delaunay arguments
  float A1;       ///< [&mu;as / &mu;s] Sine coefficient for first parameter (xp or UT1)
  float B1;       ///< [&mu;as / &mu;s] Cosine coefficient for first parameter (xp or UT1)
  float A2;       ///< [&mu;as / &mu;s] Sine coefficient for second parameter (yp or LOD)
  float B2;       ///< [&mu;as / &mu;s] Cosine coefficient for second parameter (yp or LOD)
} novas_eop_terms;


#endif /* NOVAS_EOP_H_ */
