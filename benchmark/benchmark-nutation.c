/**
 * @file
 *
 * @date Created  on Jan 26, 2025
 * @author Attila Kovacs
 */


/**
 * @file
 *
 * @date Created  on Jan 24, 2025
 * @author Attila Kovacs
 */

#define _POSIX_C_SOURCE 199309L   ///< for clock_gettime()

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <novas.h>      ///< SuperNOVAS functions and definitions

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
using namespace novas;
#  endif
#endif

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A

static void timestamp(novas_timespec *t) {
  novas_set_current_time(LEAP_SECONDS, DUT1, t);
}


int main() {              // observer location
  // Other variables we need ----------------------------------------------->
  int i, N = 100000, N2 = N / 10;
  double tjd = 2460683.132905, dx, dy;
  novas_timespec start, end;

  // -------------------------------------------------------------------------
  // Start benchmarks...
  fprintf(stderr, "Starting single-thread benchmarks...\n");

  // -------------------------------------------------------------------------
  // Benchmark reduced accuracy, place(), same time
  timestamp(&start);
  for(i = 0; i < N2; i++) iau2000a(tjd + i * 0.01, 0.0, &dx, &dy);
  timestamp(&end);
  printf(" - iau2000a:   %12.1f nutations/sec\n", N2 / novas_diff_time(&end, &start));

  // -------------------------------------------------------------------------
  // Benchmark reduced accuracy, place(), different times()
  timestamp(&start);
  for(i = 0; i < N; i++)   for(i = 0; i < N; i++) iau2000b(tjd + i * 0.01, 0.0, &dx, &dy);
  timestamp(&end);
  printf(" - iau2000b:   %12.1f positions/sec\n", N / novas_diff_time(&end, &start));

  // -------------------------------------------------------------------------
  // Benchmark reduced accuracy, place(), different times()
  timestamp(&start);
  for(i = 0; i < N; i++)   for(i = 0; i < N; i++) nu2000k(tjd + i * 0.01, 0.0, &dx, &dy);
  timestamp(&end);
  printf(" - nu2000k:    %12.1f positions/sec\n", N / novas_diff_time(&end, &start));

  return 0;
}
