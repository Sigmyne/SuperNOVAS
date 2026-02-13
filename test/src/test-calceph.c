/**
 * @date Created  on Feb 18, 2024
 * @author Attila Kovacs
 */

#define _DEFAULT_SOURCE             /// strdup()


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <calceph.h>

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
#include "novas-calceph.h"

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
using namespace novas;
#  endif
#endif

#define PLANET_EPH                  "de440s-j2000.bsp"
#define MARS_EPH                    "mar097-j2000.bsp"

#if defined _WIN32 || defined __CYGWIN__
#  define PATH_SEP  "\\"
#else
#  define PATH_SEP  "/"
#endif

static const char *prefix;

static int usage() {
  fprintf(stderr, " Syntax: test-calceph <ephem-path>\n\n");
  fprintf(stderr, "   <ephem-path>   Path to de440s.bsp and mar097.bsp containing J2000 data.\n\n");
  exit(1);
}

static int check_equal_pos(const double *posa, const double *posb, double tol) {
  int i;

  tol = fabs(tol);
  if(tol < 1e-30) tol = 1e-30;

  for(i = 0; i < 3; i++) {
    if(fabs(posa[i] - posb[i]) <= tol) continue;
    if(isnan(posa[i]) && isnan(posb[i])) continue;

    fprintf(stderr, "  A[%d] = %.9g vs B[%d] = %.9g\n", i, posa[i], i, posb[i]);
    return i + 1;
  }

  return 0;
}

static int is_ok(const char *func, int error) {
  if(error) fprintf(stderr, "ERROR %d! %s\n", error, func);
  return !error;
}

static int check(const char *func, int exp, int error) {
  if(error != exp) {
    fprintf(stderr, "ERROR! %s: expected %d, got %d\n", func, exp, error);
    return 1;
  }
  return 0;
}


static int test_calceph() {
  double pos[3], vel[3], pos0[3], vel0[3];
  double jd = NOVAS_JD_J2000;
  double jd2[2] = { jd, 0.0 };

  char filename[1024];
  object earth = NOVAS_EARTH_INIT, mars = NOVAS_MARS_INIT;
  t_calcephbin *eph;

  sprintf(filename, "%s" PATH_SEP PLANET_EPH, prefix);
  eph = calceph_open(filename);

  if(check("calceph:use", 0, novas_use_calceph(eph))) return 1;

  if(!is_ok("calceph:earth", ephemeris(jd2, &earth, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  earth_sun_calc(jd, NOVAS_EARTH, NOVAS_BARYCENTER, pos0, vel0);

  if(!is_ok("calceph:earth:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;
  if(!is_ok("calceph:earth:vel", check_equal_pos(vel, vel0, 1e-5))) return 1;

  if(!is_ok("calceph_planet:mars", ephemeris(jd2, &mars, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos0, vel0))) return 1;

  return 0;
}

static int test_calceph_planet() {
  double pos[3], vel[3], pos0[3] = {0.0}, vel0[3] = {0.0};
  double jd = NOVAS_JD_J2000;
  double jd2[2] = { jd, 0.0 };

  char filename[1024];
  object ssb, sun, earth, moon, mars, phobos;
  t_calcephbin *eph;

  make_planet(NOVAS_SSB, &ssb);
  make_planet(NOVAS_SUN, &sun);
  make_planet(NOVAS_EARTH, &earth);
  make_planet(NOVAS_MOON, &moon);
  make_planet(NOVAS_MARS, &mars);
  make_ephem_object("Phobos", 401, &phobos);


  sprintf(filename, "%s" PATH_SEP MARS_EPH, prefix);
  eph = calceph_open(filename);
  if(check("calceph_planet:use", 0, novas_use_calceph(eph))) return 1;

  sprintf(filename, "%s" PATH_SEP PLANET_EPH, prefix);
  eph = calceph_open(filename);
  if(check("calceph_planet:use_planets", 0, novas_use_calceph_planets(eph))) return 1;

  if(!is_ok("calceph_planet:ssb", ephemeris(jd2, &ssb, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  if(!is_ok("calceph_planet:ssb:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;

  if(!is_ok("calceph_planet:sun_vs_sun", ephemeris(jd2, &sun, NOVAS_HELIOCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  if(!is_ok("calceph_planet:sun_vs_sun:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;

  if(!is_ok("calceph_planet:sun", ephemeris(jd2, &sun, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  earth_sun_calc(jd, NOVAS_SUN, NOVAS_BARYCENTER, pos0, vel0);
  if(!is_ok("calceph_planet:sun:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;

  if(!is_ok("calceph_planet:earth", ephemeris(jd2, &earth, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  earth_sun_calc(jd, NOVAS_EARTH, NOVAS_BARYCENTER, pos0, vel0);

  if(!is_ok("calceph_planet:earth:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;
  if(!is_ok("calceph_planet:earth:vel", check_equal_pos(vel, vel0, 1e-5))) return 1;

  if(!is_ok("calceph_planet:moon", ephemeris(jd2, &moon, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  earth_sun_calc(jd, NOVAS_MOON, NOVAS_BARYCENTER, pos0, vel0);

  if(!is_ok("calceph_planet:moon:pos", check_equal_pos(pos, pos0, 1e-2))) return 1;
  if(!is_ok("calceph_planet:moon:vel", check_equal_pos(vel, vel0, 1e-3))) return 1;



  if(!is_ok("calceph_planet:mars", ephemeris(jd2, &mars, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos0, vel0))) return 1;
  if(!is_ok("calceph_planet:phobos", ephemeris(jd2, &phobos, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return -1;

  if(!is_ok("calceph_planet:mars-phobos:pos", check_equal_pos(pos, pos0, 1e-4))) return 1;

  phobos.number = -1;
  if(!is_ok("calceph_planet:phobos:byname", ephemeris(jd2, &phobos, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos0, vel0))) return -1;
  if(!is_ok("calceph_planet:phobos:match", check_equal_pos(pos, pos0, 1e-6))) return 1;


  return 0;
}

static int test_calceph_serialized() {
  extern int serialized_calceph_queries;

  object mars, phobos;
  double jd = NOVAS_JD_J2000;
  double jd2[2] = { jd, 0.0 };
  double pos[3], vel[3];

  serialized_calceph_queries = 1;

  make_planet(NOVAS_MARS, &mars);
  make_ephem_object("Phobos", 401, &phobos);

  if(!is_ok("calceph_serialized:mars", ephemeris(jd2, &mars, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  if(!is_ok("calceph_serialized:phobos", ephemeris(jd2, &phobos, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return -1;

  serialized_calceph_queries = 0;

  return 0;
}

static void dummy_error_handler(const char *msg) {
  // Do nothing...
  (void) msg;
}

static int test_errors() {
  double pos[3], vel[3];
  double jd = NOVAS_JD_J2000;
  double jd2[2] = { jd, 0.0 };
  int n = 0;

  object earth, phobos;
  novas_planet_provider_hp pl = get_planet_provider_hp();
  novas_ephem_provider eph = get_ephem_provider();
  enum novas_origin origin = NOVAS_BARYCENTER;

  make_planet(NOVAS_EARTH, &earth);
  make_ephem_object("Phobos", 401, &phobos);

  if(check("errors:tdb", -1, pl(NULL, NOVAS_MARS, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:planet:number:-1", 1, pl(jd2, -1, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:planet:number:hi", 1, pl(jd2, NOVAS_PLANETS, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:planet:origin", 2, pl(jd2, NOVAS_MARS, -1, pos, vel))) n++;

  calceph_seterrorhandler(3, dummy_error_handler);
  if(check("errors:body:name:NULL", -1, eph(NULL, -1, jd2[0], jd2[1], &origin, pos, vel))) n++;
  if(check("errors:body:name:empty", -1, eph("", -1, jd2[0], jd2[1], &origin, pos, vel))) n++;
  if(check("errors:body:name:nomatch", 1, eph("blah", -1, jd2[0], jd2[1], &origin, pos, vel))) n++;

  jd2[0] = -999999.0;
  if(check("errors:planet:time", 3, pl(jd2, NOVAS_MARS, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:body:time", 3, eph("phobos", 401, jd2[0], jd2[1], &origin, pos, vel))) n++;

  return n;
}


static int test_use_calceph() {
  if(check("use_calceph", -1, novas_use_calceph(NULL))) return 1;
  return 0;
}

static int test_use_calceph_planets() {
  if(check("use_calceph", -1, novas_use_calceph_planets(NULL))) return 1;
  return 0;
}



static int test_calceph_use_ids() {
  novas_ephem_provider calc = get_ephem_provider();
  double jd = NOVAS_JD_J2000;
  double jd2[2] = { jd, 0.0 };
  double pos[3], vel[3], pos0[3], vel0[3];
  int n = 0;

  char filename[1024];
  t_calcephbin *eph;
  enum novas_origin origin = NOVAS_BARYCENTER;

  sprintf(filename, "%s" PATH_SEP PLANET_EPH, prefix);
  eph = calceph_open(filename);
  if(novas_use_calceph(eph)) return 1;

  if(check("calceph_use_ids:-1", -1, novas_calceph_use_ids(-1))) n++;
  if(check("calceph_use_ids:hi", -1, novas_calceph_use_ids(NOVAS_ID_TYPES))) n++;

  if(!is_ok("calceph_use_ids:naif", novas_calceph_use_ids(NOVAS_ID_NAIF))) n++;
  if(!is_ok("calceph_use_ids:emb:naif", calc("EMB", 3, jd2[0], jd2[1], &origin, pos0, vel0))) n++;

  if(!is_ok("calceph_use_ids:calceph", novas_calceph_use_ids(NOVAS_ID_CALCEPH))) n++;
  if(!is_ok("calceph_use_ids:emb:calceph", calc("EMB", 13, jd2[0], jd2[1], &origin, pos, vel))) n++;

  novas_calceph_use_ids(NOVAS_ID_NAIF);

  if(!is_ok("calceph_use_ids:emb:pos", check_equal_pos(pos, pos0, 1e-6))) n++;
  if(!is_ok("calceph_use_ids:emb:vel", check_equal_pos(vel, vel0, 1e-6))) n++;

  return n;
}

int test_calceph_is_thread_safe() {
  if(!is_ok("calceph_is_thread_safe", novas_calceph_is_thread_safe() < 0)) return 1;
  return 0;
}

int main(int argc, char *argv[]) {
  int n = 0;

  if(argc < 2) usage();

  prefix = strdup(argv[1]);

  enable_earth_sun_hp(1);

  if(test_calceph()) n++;
  if(test_calceph_planet()) n++;
  if(test_use_calceph()) n++;
  if(test_use_calceph_planets()) n++;
  if(test_calceph_serialized()) n++;
  if(test_calceph_use_ids()) n++;
  if(test_calceph_is_thread_safe()) n++;


  novas_debug(NOVAS_DEBUG_OFF);
  if(test_errors()) n++;

  if(n) fprintf(stderr, " -- FAILED %d tests\n", n);
  else fprintf(stderr, " -- OK\n");


  return n;
}

