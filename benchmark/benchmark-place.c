/**
 * @file
 *
 * @date Created  on Jan 24, 2025
 * @author Attila Kovacs
 */

#if __STDC_VERSION__ < 201112L
#  define _POSIX_C_SOURCE 199309      ///< struct timespec
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <novas.h>      ///< SuperNOVAS functions and definitions

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
using namespace novas;
#  endif
#endif

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.


static void timestamp(novas_timespec *t) {
  novas_set_current_time(LEAP_SECONDS, DUT1, t);
}

static void calc_pos(const cat_entry *star, const novas_frame *frame) {
  object source = NOVAS_OBJECT_INIT;
  sky_pos apparent = SKY_POS_INIT;

  make_cat_object(star, &source);

  if(novas_sky_pos(&source, frame, NOVAS_CIRS, &apparent) != 0) {
    fprintf(stderr, "ERROR! failed to calculate apparent position.\n");
    exit(1);
  }
}


static void calc_place(const cat_entry *star, const novas_frame *frame) {
  const novas_timespec *time = &frame->time;
  sky_pos apparent = SKY_POS_INIT;

  if(place_star(time->ijd_tt + time->fjd_tt, star, &frame->observer, time->ut1_to_tt, NOVAS_CIRS, frame->accuracy, &apparent) != 0) {
    fprintf(stderr, "ERROR! failed to calculate apparent position.\n");
    exit(1);
  }
}


int main(int argc, const char *argv[]) {
  // SuperNOVAS variables used for the calculations ------------------------->
  cat_entry *stars;                 // Array of sidereal source entries.
  observer obs;                     // observer location
  novas_timespec obs_time;          // astrometric time of observation
  novas_frame obs_frame;            // observing frame defined for observing time and location


  // Intermediate variables we'll use -------------------------------------->
  novas_timespec start, end;        // timestamps for execution time


  // Other variables we need ----------------------------------------------->
  int i, N = 300000, N2 = N  / 10, N3 = N / 30;

  novas_debug(1);

  if(argc > 1) N = (int) strtol(argv[1], NULL, 10);

  if(N < 1) {
    fprintf(stderr, "ERROR! invalid source count: %d\n", N);
    return 1;
  }

  stars = (cat_entry *) calloc(N, sizeof(cat_entry));
  if(!stars) {
    fprintf(stderr, "ERROR! alloc %d stars: %s\n", N, strerror(errno));
    return 1;
  }


  // -------------------------------------------------------------------------
  // Define observer somewhere on Earth (we can also define observers in Earth
  // or Sun orbit, at the geocenter or at the Solary-system barycenter...)

  // Specify the location we are observing from
  // 50.7374 deg N, 7.0982 deg E, 60m elevation
  // (We'll ignore the local weather parameters here, but you can set those too.)
  if(make_observer_on_surface(50.7374, 7.0982, 60.0, 0.0, 0.0, &obs) != 0) {
    fprintf(stderr, "ERROR! defining Earth-based observer location.\n");
    return 1;
  }


  // -------------------------------------------------------------------------
  // Set the astrometric time of observation...

  // Set the time of observation to the current UTC-based UNIX time
  if(novas_set_current_time(LEAP_SECONDS, DUT1, &obs_time) != 0) {
    fprintf(stderr, "ERROR! failed to set time of observation.\n");
    return 1;
  }


  // -------------------------------------------------------------------------
  // Initialize the observing frame with the given observing and Earth
  // orientation patameters.
  //
  if(novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &obs_time, POLAR_DX, POLAR_DY, &obs_frame) != 0) {
    fprintf(stderr, "ERROR! failed to define observing frame.\n");
    return 1;
  }

  // -------------------------------------------------------------------------
  // Allow faking high-accuracy calculations
  enable_earth_sun_hp(1);


  // -------------------------------------------------------------------------
  // Configure sources with random data.
  fprintf(stderr, "Configuring %d sources...\n", N);

  for(i = 0; i < N; i++) {
    cat_entry *star = &stars[i];

    sprintf(star->catalog, "TST");
    sprintf(star->starname, "test-%d", i);
    star->starnumber = i;
    star->ra = (23.0 * rand()) / RAND_MAX;
    star->dec = (180.0 * rand()) / RAND_MAX - 90.0;
    star->radialvelocity = (1000.0 * rand()) / RAND_MAX - 500.0;
    star->parallax = (20.0 * rand()) / RAND_MAX;
    star->promora = (200.0 * rand()) / RAND_MAX - 100.0;
    star->promodec = (200.0 * rand()) / RAND_MAX - 100.0;
  }


  // -------------------------------------------------------------------------
  // Start benchmarks...
  fprintf(stderr, "Starting single-thread benchmarks...\n");


  // -------------------------------------------------------------------------
  // Benchmark reduced accuracy, same frame
  timestamp(&start);
  for(i = 0; i < N; i++) calc_pos(&stars[i], &obs_frame);
  timestamp(&end);
  printf(" - novas_sky_pos(), same frame, red. acc.:        %12.1f positions/sec\n",
          N / novas_diff_time(&end, &start));

  // -------------------------------------------------------------------------
  // Benchmark full accuracy, same frame
  obs_frame.accuracy = NOVAS_FULL_ACCURACY;
  timestamp(&start);
  for(i = 0; i < N; i++) calc_pos(&stars[i], &obs_frame);
  timestamp(&end);
  printf(" - novas_sky_pos(), same frame, full acc.:        %12.1f positions/sec\n",
          N / novas_diff_time(&end, &start));



  // -------------------------------------------------------------------------
  // Benchmark place() reduced accuracy, same frame
  timestamp(&start);
  for(i = 0; i < N; i++) calc_place(&stars[i], &obs_frame);
  timestamp(&end);
  printf(" - place(), same frame, red. acc.:                %12.1f positions/sec\n",
          N / novas_diff_time(&end, &start));

  // -------------------------------------------------------------------------
  // Benchmark place() full accuracy, same frame
  obs_frame.accuracy = NOVAS_FULL_ACCURACY;
  timestamp(&start);
  for(i = 0; i < N; i++) calc_place(&stars[i], &obs_frame);
  timestamp(&end);
  printf(" - place(), same frame, full acc.:                %12.1f positions/sec\n",
          N / novas_diff_time(&end, &start));


  // individual frames are expected to be significantly slower, so
  // benchmark over fewer iterations
  N /= 10;

  // -------------------------------------------------------------------------
  // Benchmark reduced accuracy, individual fames
  timestamp(&start);
  for(i = 0; i < N2; i++) {
    novas_set_time(NOVAS_TT, novas_get_time(&start, NOVAS_TT) + i, LEAP_SECONDS, DUT1, &obs_time);
    novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &obs_time, POLAR_DX, POLAR_DY, &obs_frame);
    calc_pos(&stars[i], &obs_frame);
  }
  timestamp(&end);
  printf(" - novas_sky_pos, individual, red. acc.:          %12.1f positions/sec\n",
          N2 / novas_diff_time(&end, &start));

  // -------------------------------------------------------------------------
  // Benchmark full accuracy, individual frames
  timestamp(&start);
  for(i = 0; i < N3; i++) {
    novas_set_time(NOVAS_TT, novas_get_time(&start, NOVAS_TT) + i, LEAP_SECONDS, DUT1, &obs_time);
    novas_make_frame(NOVAS_FULL_ACCURACY, &obs, &obs_time, POLAR_DX, POLAR_DY, &obs_frame);
    calc_pos(&stars[i], &obs_frame);
  }
  timestamp(&end);
  printf(" - novas_sky_pos, individual, full acc.:          %12.1f positions/sec\n",
          N3 / novas_diff_time(&end, &start));


  // -------------------------------------------------------------------------
  // Benchmark place() reduced accuracy, individual frames
  obs_frame.accuracy = NOVAS_REDUCED_ACCURACY;
  timestamp(&start);
  for(i = 0; i < N2; i++) {
    novas_set_time(NOVAS_TT, novas_get_time(&start, NOVAS_TT) + ((i % 2) ? 1 : -1), LEAP_SECONDS, DUT1, &obs_frame.time);
    calc_place(&stars[i], &obs_frame);
  }
  timestamp(&end);
  printf(" - place(), individual, red. acc.:                %12.1f positions/sec\n",
          N2 / novas_diff_time(&end, &start));

  // -------------------------------------------------------------------------
  // Benchmark place full accuracy, individual frames
  obs_frame.accuracy = NOVAS_FULL_ACCURACY;
  timestamp(&start);
  for(i = 0; i < N3; i++) {
    novas_set_time(NOVAS_TT, novas_get_time(&start, NOVAS_TT) + ((i % 2) ? 1 : -1), LEAP_SECONDS, DUT1, &obs_frame.time);
    calc_place(&stars[i], &obs_frame);
  }
  timestamp(&end);
  printf(" - place(), individual, full acc.:                %12.1f positions/sec\n",
          N3 / novas_diff_time(&end, &start));


  return 0;
}

