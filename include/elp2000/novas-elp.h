/**
 * @file
 *
 * @date Created  on Apr 11, 2026
 * @author Attila Kovacs
 */

#ifndef NOVAS_ELP2000_H_
#define NOVAS_ELP2000_H_

#include <stdint.h>

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

#endif /* NOVAS_ELP2000_H_ */
