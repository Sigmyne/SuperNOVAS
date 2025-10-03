/**
 * @date Created  on Feb 19, 2024
 * @author Attila Kovacs
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
#include "solarsystem.h"

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
using namespace novas;
#  endif
#endif

#if defined _WIN32 || defined __CYGWIN__
#  define PATH_SEP  "\\"
#else
#  define PATH_SEP  "/"
#endif

static char *dataPath = "../data";

static int dummy_ephem(const char *name, long id, double jd_tdb_high, double jd_tdb_low, enum novas_origin *origin, double *pos, double *vel) {
  (void) name;
  (void) jd_tdb_high;
  (void) jd_tdb_low;
  *origin = NOVAS_BARYCENTER;

  if(pos) {
    memset(pos, 0, 3 * sizeof(double));
    pos[0] = id % 100;
  }
  if(vel) {
    memset(vel, 0, 3 * sizeof(double));
    vel[1] = 0.01 * (id % 100);
  }
  return 0;
}

static int check(const char *func, int exp, int error) {
  if(error != exp) {
    fprintf(stderr, "ERROR! %s: expected %d, got %d\n", func, exp, error);
    return 1;
  }
  return 0;
}

static int check_nan(const char *func, double value) {
  if(!isnan(value)) {
    fprintf(stderr, "ERROR! %s: expected NAN, got %f\n", func, value);
    return 1;
  }
  return 0;
}

static int test_make_on_surface() {
  int n = 0;

  on_surface loc = ON_SURFACE_INIT;

  if(check("make_on_surface", -1, make_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, NULL))) return 1;
  if(check("make_on_surface:temp:lo", -1, make_on_surface(0.0, 0.0, 0.0, -120.1, 0.0, &loc))) n++;
  if(check("make_on_surface:temp:hi", -1, make_on_surface(0.0, 0.0, 0.0, 70.1, 0.0, &loc))) n++;
  if(check("make_on_surface:pressure:lo", -1, make_on_surface(0.0, 0.0, 0.0, 0.0, -0.1, &loc))) n++;
  if(check("make_on_surface:pressure:hi", -1, make_on_surface(0.0, 0.0, 0.0, 0.0, 1200.1, &loc))) n++;
  return n;
}

static int test_make_in_space() {
  double p[3] = {1.0}, v[3] = {0.0};
  in_space sp;
  int n = 0;

  if(check("make_in_space", -1, make_in_space(p, v, NULL))) n++;
  if(check("make_in_space:p", 0, make_in_space(NULL, v, &sp))) n++;
  if(check("make_in_space:v", 0, make_in_space(p, NULL, &sp))) n++;

  return n;
}

static int test_make_observer() {
  in_space sp = IN_SPACE_INIT;
  on_surface on = ON_SURFACE_INIT;
  observer obs = OBSERVER_INIT;
  int n = 0;

  if(check("make_observer:where", 1, make_observer(-1, &on, &sp, &obs))) n++;
  if(check("make_observer", -1, make_observer(NOVAS_OBSERVER_AT_GEOCENTER, &on, &sp, NULL))) n++;
  if(check("make_observer:on", -1, make_observer(NOVAS_OBSERVER_ON_EARTH, NULL, &sp, &obs))) n++;
  if(check("make_observer:eorb", -1, make_observer(NOVAS_OBSERVER_IN_EARTH_ORBIT, &on, NULL, &obs))) n++;
  if(check("make_observer:air:surf", -1, make_observer(NOVAS_AIRBORNE_OBSERVER, NULL, &sp, &obs))) n++;
  if(check("make_observer:air:vel", -1, make_observer(NOVAS_AIRBORNE_OBSERVER, &on, NULL, &obs))) n++;

  return n;
}

static int test_make_airborne_observer() {
  on_surface on = ON_SURFACE_INIT;
  observer obs = OBSERVER_INIT;
  int n = 0;

  if(check("make_airborne_observer:vel", -1, make_airborne_observer(&on, NULL, &obs))) n++;

  return n;
}

static int test_make_ephem_object() {
  object o;
  int n = 0;

  char longname[SIZE_OF_OBJ_NAME + 1];
  memset(longname, 'A', SIZE_OF_OBJ_NAME);

  if(check("make_ephem_object", -1, make_ephem_object("dummy", 1, NULL))) n++;
  if(check("make_ephem_object:name", -1, make_ephem_object(longname, 1, &o))) n++;

  return n;
}

static int test_make_planet() {
  object o;
  int n = 0;

  if(check("make_ephem_object:lo", -1, make_planet(-1, &o))) n++;
  if(check("make_ephem_object:hi", -1, make_planet(NOVAS_PLANETS, &o))) n++;

  return n;
}

static int test_make_cat_entry() {
  cat_entry c;
  int n = 0;

  char longname[SIZE_OF_OBJ_NAME + 1];
  char longcat[SIZE_OF_CAT_NAME + 1];

  memset(longname, 'A', SIZE_OF_OBJ_NAME);
  memset(longcat, 'A', SIZE_OF_CAT_NAME);

  if(check("make_cat_entry", -1, make_cat_entry("dummy", "cat", 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, NULL))) n++;
  if(check("make_cat_entry:name", 1, make_cat_entry(longname, "cat", 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, &c))) n++;
  if(check("make_cat_entry:catname", 2, make_cat_entry("dummy", longcat, 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, &c))) n++;

  return n;
}

static int test_make_object() {
  cat_entry s = CAT_ENTRY_INIT;
  object o = NOVAS_OBJECT_INIT;
  int n = 0;

  char longname[SIZE_OF_OBJ_NAME + 1];
  memset(longname, 'A', SIZE_OF_OBJ_NAME);

  if(check("make_object", -1, make_object(NOVAS_PLANET, 1, "dummy", &s, NULL))) n++;
  if(check("make_object:star", -1, make_object(NOVAS_CATALOG_OBJECT, 1, "dummy", NULL, &o))) n++;
  if(check("make_object:type", 1, make_object(-1, 1, "dummy", &s, &o))) n++;
  if(check("make_object:pl:lo", 2, make_object(NOVAS_PLANET, -1, "dummy", NULL, &o))) n++;
  if(check("make_object:pl:lo", 2, make_object(NOVAS_PLANET, NOVAS_PLANETS, "dummy", NULL, &o))) n++;
  if(check("make_object:name", 5, make_object(NOVAS_PLANET, 1, longname, NULL, &o))) n++;

  return n;
}

static int test_make_cat_object() {
  cat_entry s;
  object source;
  int n = 0;

  make_cat_entry("test", "TST", 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, &s);

  if(check("make_cat_object", -1, make_cat_object(&s, NULL))) n++;
  if(check("make_cat_object:star", -1, make_cat_object(NULL, &source))) n++;

  return n;
}

static int test_make_redshifted_object() {
  object source;
  int n = 0;

  if(check("make_redshifted_object", -1, make_redshifted_object("TEST", 0.0, 0.0, 0.0, NULL))) n++;
  if(check("make_redshifted_object:z:lo", -1, make_redshifted_object("TEST", 0.0, 0.0, -1.0, &source))) n++;

  return n;
}

static int test_v2z() {
  int n = 0;

  if(check_nan("v2z:hi", novas_v2z(NOVAS_C / 1000.0 + 0.01))) n++;
  return n;
}

static int test_z2v() {
  int n = 0;

  if(check_nan("z2v:-1", novas_z2v(-1.0))) n++;
  return n;
}

static int test_refract() {
  on_surface loc = ON_SURFACE_INIT, loc0 = ON_SURFACE_INIT;
  int n = 0;

  novas_debug(NOVAS_DEBUG_ON);
  fprintf(stderr, ">>> Expecting error message...\n");

  errno = 0;
  double r = refract(NULL, NOVAS_STANDARD_ATMOSPHERE, 30.0);
  if(check("refract:loc", 1, r == 0.0 && errno == EINVAL)) n++;
  novas_debug(NOVAS_DEBUG_OFF);

  errno = 0;
  r = refract(&loc, -1, 30.0);
  if(check("refract:model:-1:ret", 1, r == 0.0)) n++;
  if(check("refract:model:-1:errno", EINVAL, errno)) n++;

  errno = 0;
  r = refract(&loc, NOVAS_REFRACTION_MODELS, 30.0);
  if(check("refract:model:hi:ret", 1, r == 0.0)) n++;
  if(check("refract:model:hi:errno", EINVAL, errno)) n++;

  errno = 0;
  r = refract(&loc, NOVAS_STANDARD_ATMOSPHERE, 91.01);
  if(check("refract:zd", 1, r == 0.0)) n++;

  loc = loc0;
  loc.temperature = -150.1;
  if(check_nan("refract:loc:temperature:lo", refract(&loc, NOVAS_WEATHER_AT_LOCATION, 30.0))) n++;

  loc.temperature = 100.1;
  if(check_nan("refract:loc:temperature:hi", refract(&loc, NOVAS_WEATHER_AT_LOCATION, 30.0))) n++;

  loc = loc0;
  loc.pressure = -0.1;
  if(check_nan("refract:loc:pressure:lo", refract(&loc, NOVAS_WEATHER_AT_LOCATION, 30.0))) n++;

  loc.pressure = 2000.1;
  if(check_nan("refract:loc:pressure:hi", refract(&loc, NOVAS_WEATHER_AT_LOCATION, 30.0))) n++;

  return n;
}

static int test_refract_astro() {
  on_surface surf = ON_SURFACE_INIT;
  int n = 0;

  if(check_nan("refract_astro:loc", refract_astro(NULL, NOVAS_STANDARD_ATMOSPHERE, 30.0))) n++;
  if(check_nan("refract_astro:model:-1", refract_astro(&surf, -1, 30.0))) n++;
  if(check_nan("refract_astro:model:hi", refract_astro(&surf, NOVAS_REFRACTION_MODELS, 30.0))) n++;

  novas_set_max_iter(0);
  if(check_nan("refract_astro:converge", refract_astro(&surf, NOVAS_STANDARD_ATMOSPHERE, 85.0))) n++;
  else if(check("refract_astro:converge:errno", ECANCELED, errno)) n++;

  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);

  return n;
}

static int test_inv_refract() {
  on_surface surf = ON_SURFACE_INIT;
  int n = 0;

  if(check_nan("inv_refract:loc", novas_inv_refract(novas_optical_refraction, NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 5.0))) n++;;

  novas_set_max_iter(0);
  if(check_nan("inv_refract:converge", novas_inv_refract(novas_optical_refraction, NOVAS_JD_J2000, &surf, NOVAS_REFRACT_OBSERVED, 5.0))) n++;
  else if(check("inv_refract:converge:errno", ECANCELED, errno)) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);

  return n;
}

static int test_radio_refraction() {
  int n = 0;

  on_surface loc = ON_SURFACE_INIT, loc0 = ON_SURFACE_INIT;

  if(check_nan("radio_refraction:loc:null", novas_radio_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;
  if(check_nan("radio_refraction:loc:type:1", novas_radio_refraction(NOVAS_JD_J2000, &loc, 1, 30.0))) n++;
  if(check_nan("radio_refraction:el:lo", novas_radio_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, -1.01))) n++;
  if(check_nan("radio_refraction:el:hi", novas_radio_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 90.01))) n++;

  loc.temperature = -150.1;
  if(check_nan("radio_refraction:temperature:lo", novas_radio_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc.temperature = 100.1;
  if(check_nan("radio_refraction:temperature:hi", novas_radio_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc = loc0;
  loc.pressure = -0.1;
  if(check_nan("radio_refraction:pressure:lo", novas_radio_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc.pressure = 2000.1;
  if(check_nan("radio_refraction:pressure:hi", novas_radio_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc = loc0;
  loc.humidity = -0.1;
  if(check_nan("radio_refraction:humidity:lo", novas_radio_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc.humidity = 100.1;
  if(check_nan("radio_refraction:humidity:hi", novas_radio_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  return n;
}

static int test_wave_refraction() {
  int n = 0;

  on_surface loc = ON_SURFACE_INIT, loc0 = ON_SURFACE_INIT;

  if(check_nan("wave_refraction:loc:null", novas_wave_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;
  if(check_nan("wave_refraction:loc:type:1", novas_wave_refraction(NOVAS_JD_J2000, &loc, 1, 30.0))) n++;
  if(check_nan("wave_refraction:el:lo", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_OBSERVED, 0.0))) n++;
  if(check_nan("wave_refraction:el:hi", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_OBSERVED, 90.01))) n++;

  loc.temperature = -150.1;
  if(check_nan("wave_refraction:temperature:lo", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc.temperature = 200.1;
  if(check_nan("wave_refraction:temperature:hi", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc = loc0;
  loc.pressure = -0.1;
  if(check_nan("wave_refraction:pressure:lo", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc.pressure = 10000.1;
  if(check_nan("wave_refraction:pressure:hi", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc = loc0;
  loc.humidity = -0.1;
  if(check_nan("wave_refraction:humidity:lo", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc.humidity = 100.1;
  if(check_nan("wave_refraction:humidity:hi", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  loc = loc0;
  novas_refract_wavelength(0.099);
  if(check_nan("wave_refraction:wl:lo", novas_wave_refraction(NOVAS_JD_J2000, &loc, NOVAS_REFRACT_ASTROMETRIC, 30.0))) n++;

  novas_refract_wavelength(NOVAS_DEFAULT_WAVELENGTH);

  return n;
}

static int test_refract_wavelength() {
  int n = 0;

  if(check("refract:wavelength:0", -1, novas_refract_wavelength(0.0))) n++;
  if(check("refract:wavelength:neg", -1, novas_refract_wavelength(-0.1))) n++;
  if(check("refract:wavelength:nan", -1, novas_refract_wavelength(NAN))) n++;

  return n++;
}


static int test_limb_angle() {
  double pos[3] = { 0.01 }, pn[3] = { -0.01 }, pz[3] = {0.0}, a, b;
  int n = 0;

  if(check("limb_angle:pos_obj", -1, limb_angle(NULL, pos, &a, &b))) n++;
  if(check("limb_angle:pos_obs", -1, limb_angle(pos, NULL, &a, &b))) n++;
  if(check("limb_angle:obj:0", -1, limb_angle(pz, pos, &a, &b))) n++;
  if(check("limb_angle:obj:0", -1, limb_angle(pos, pz, &a, &b))) n++;

  // Null outputs OK
  if(check("limb_angle:limb", 0, limb_angle(pos, pos, NULL, &b))) n++;
  if(check("limb_angle:nadir", 0, limb_angle(pos, pos, &a, NULL))) n++;

  // corner case (cosd = -1)
  if(check("limb_angle:corner", 0, limb_angle(pos, pn, &a, NULL))) n++;

  return n;
}


static int test_transform_cat() {
  cat_entry c = CAT_ENTRY_INIT, c1;
  int n = 0;

  char longname[SIZE_OF_CAT_NAME + 1] = {'\0'};
  memset(longname, 'A', SIZE_OF_CAT_NAME);

  if(check("transform_cat:in", -1, transform_cat(PRECESSION, NOVAS_JD_B1950, NULL, NOVAS_JD_J2000, "FK5", &c))) n++;
  if(check("transform_cat:out", -1, transform_cat(PRECESSION, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, "FK5", NULL))) n++;
  if(check("transform_cat:option", -1, transform_cat(-1, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, "FK5", &c1))) n++;
  if(check("transform_cat:option:same", -1, transform_cat(-1, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, "FK5", &c))) n++;
  if(check("transform_cat:name", 2, transform_cat(PRECESSION, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, longname, &c))) n++;

  return n;
}

static int test_transform_hip() {
  cat_entry c = CAT_ENTRY_INIT;
  int n = 0;

  if(check("transform_hip:in", -1, transform_hip(NULL, &c))) n++;
  if(check("transform_hip:in", -1, transform_hip(&c, NULL))) n++;

  return n;
}

static int test_ephemeris() {
  double p[3], v[3];
  double tdb[2] = { NOVAS_JD_J2000 };
  object ceres;
  int n = 0;

  make_ephem_object("Ceres", 2000001, &ceres);

  if(check("ephemeris:body", -1, ephemeris(tdb, NULL, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) n++;
  if(check("ephemeris:jd", -1, ephemeris(NULL, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) n++;
  if(check("ephemeris:pos", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, NULL, v))) n++;
  if(check("ephemeris:vel", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("ephemeris:pos+vel", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, NULL, NULL))) n++;
  if(check("ephemeris:origin", 1, ephemeris(tdb, &ceres, -1, NOVAS_FULL_ACCURACY, p, v))) n++;

#ifndef DEFAULT_READEPH
  if(check("ephemeris:noephem", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) n++;
#endif

  return n;
}

static int test_j2000_to_tod() {
  double p[3];
  int n = 0;

  if(check("j2000_to_tod:in", -1, j2000_to_tod(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("j2000_to_tod:out", -1, j2000_to_tod(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("j2000_to_tod:accuracy", -1, j2000_to_tod(0.0, -1, p, p))) n++;

  return n;
}

static int test_tod_to_j2000() {
  double p[3] = {1.0};
  int n = 0;

  if(check("tod_to_j2000:in", -1, tod_to_j2000(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("tod_to_j2000:out", -1, tod_to_j2000(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("tod_to_j2000:accuracy", -1, tod_to_j2000(0.0, -1, p, p))) n++;

  return n;
}

static int test_gcrs_to_tod() {
  double p[3];
  int n = 0;

  if(check("gcrs_to_tod:in", -1, gcrs_to_tod(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("gcrs_to_tod:out", -1, gcrs_to_tod(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("gcrs_to_tod:accuracy", -1, gcrs_to_tod(0.0, -1, p, p))) n++;

  return n;
}

static int test_tod_to_gcrs() {
  double p[3] = {1.0};
  int n = 0;

  if(check("tod_to_gcrs:in", -1, tod_to_gcrs(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("tod_to_gcrs:out", -1, tod_to_gcrs(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("tod_to_gcrs:accuracy", -1, tod_to_gcrs(0.0, -1, p, p))) n++;

  return n;
}

static int test_gcrs_to_mod() {
  double p[3];
  int n = 0;

  if(check("gcrs_to_mod:in", -1, gcrs_to_mod(0.0, NULL, p))) n++;
  if(check("gcrs_to_mod:out", -1, gcrs_to_mod(0.0, p, NULL))) n++;

  return n;
}

static int test_mod_to_gcrs() {
  double p[3] = {1.0};
  int n = 0;

  if(check("mod_to_gcrs:in", -1, mod_to_gcrs(0.0, NULL, p))) n++;
  if(check("mod_to_gcrs:out", -1, mod_to_gcrs(0.0, p, NULL))) n++;

  return n;
}

static int test_gcrs_to_cirs() {
  double p[3] = {1.0};
  int n = 0;

  if(check("gcrs_to_cirs:in", -1, gcrs_to_cirs(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("gcrs_to_cirs:out", -1, gcrs_to_cirs(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("gcrs_to_cirs:accuracy", -1, gcrs_to_cirs(0.0, -1, p, p))) n++;

  return n;
}

static int test_cirs_to_gcrs() {
  double p[3];
  int n = 0;

  if(check("cirs_to_gcrs:in", -1, cirs_to_gcrs(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("cirs_to_gcrs:out", -1, cirs_to_gcrs(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("cirs_to_gcrs:accuracy", -1, cirs_to_gcrs(0.0, -1, p, p))) n++;

  return n;
}

static int test_cirs_to_tod() {
  double p[3];
  int n = 0;

  if(check("cirs_to_tod:in", -1, cirs_to_tod(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("cirs_to_tod:out", -1, cirs_to_tod(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("cirs_to_tod:accuracy", -1, cirs_to_tod(0.0, -1, p, p))) n++;

  return n;
}

static int test_tod_to_cirs() {
  double p[3];
  int n = 0;

  if(check("tod_to_cirs:in", -1, tod_to_cirs(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("tod_to_cirs:out", -1, tod_to_cirs(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("tod_to_cirs:accuracy", -1, tod_to_cirs(0.0, -1, p, p))) n++;

  return n;
}

static int test_cirs_to_app_ra() {
  int n = 0;
  if(check_nan("cirs_to_app_ra:accuracy:-1", cirs_to_app_ra(NOVAS_JD_J2000, -1, 0.0))) n++;
  if(check_nan("cirs_to_app_ra:accuracy:2", cirs_to_app_ra(NOVAS_JD_J2000, 2, 0.0))) n++;
  return n;
}

static int test_app_to_cirs_ra() {
  int n = 0;
  if(check_nan("app_to_cirs_ra:accuracy:-1", app_to_cirs_ra(NOVAS_JD_J2000, -1, 0.0))) n++;
  if(check_nan("app_to_cirs_ra:accuracy:2", app_to_cirs_ra(NOVAS_JD_J2000, 2, 0.0))) n++;
  return n;
}

static int test_set_planet_provider() {
  if(check("set_planet_provider", -1, set_planet_provider(NULL))) return 1;
  return 0;
}

static int test_set_planet_provider_hp() {
  if(check("set_planet_provider_hp", -1, set_planet_provider_hp(NULL))) return 1;
  return 0;
}

static int test_place_star() {
  cat_entry c = CAT_ENTRY_INIT;
  observer loc = OBSERVER_INIT;
  sky_pos pos;
  int n = 0;

  if(check("place_star:in", -1, place_star(0.0, NULL, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place_star:out", -1, place_star(0.0, &c, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, NULL))) n++;

  return n;
}

static int test_place() {
  object o = NOVAS_OBJECT_INIT;
  observer loc = OBSERVER_INIT;
  sky_pos pos;
  int n = 0;

  if(check("place:object", -1, place(0.0, NULL, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:sys:-1", 1, place(0.0, &o, &loc, 0.0, -1, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:sys:itrs", 1, place(0.0, &o, &loc, 0.0, NOVAS_ITRS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:sys:hi", 1, place(0.0, &o, &loc, 0.0, NOVAS_REFERENCE_SYSTEMS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:accuracy", 2, place(0.0, &o, &loc, 0.0, NOVAS_GCRS, -1, &pos))) n++;


  return n;
}

static int test_radec_planet() {
  object o = NOVAS_OBJECT_INIT;
  observer loc = OBSERVER_INIT;
  double ra, dec, dis, rv;

  o.type = NOVAS_CATALOG_OBJECT;
  if(check("radec_planet:cat", -1, radec_planet(NOVAS_JD_J2000, &o, &loc, 0.0, NOVAS_GCRS, NOVAS_REDUCED_ACCURACY, &ra, &dec, &dis, &rv))) return 1;

  return 0;
}

static int test_mean_star() {
  double x, y;
  int n = 0;

  if(check("mean_star:ira", -1, mean_star(0.0, 0.0, 0.0, NOVAS_FULL_ACCURACY, NULL, &y))) n++;
  if(check("mean_star:idec", -1, mean_star(0.0, 0.0, 0.0, NOVAS_FULL_ACCURACY, &x, NULL))) n++;

  novas_set_max_iter(0);
  if(check("mean_star:converge", 1, mean_star(NOVAS_JD_J2000, 0.0, 0.0, NOVAS_REDUCED_ACCURACY, &x, &y))) n++;
  else if(check("mean_star:converge:errno", ECANCELED, errno)) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);

  return n;
}

static int test_equ2gal() {
  double x;
  int n = 0;

  if(check("equ2gal:lon", -1, equ2gal(0.0, 0.0, NULL, &x))) n++;
  if(check("equ2gal:lat", -1, equ2gal(0.0, 0.0, &x, NULL))) n++;

  return n;
}

static int test_gal2equ() {
  double x;
  int n = 0;

  if(check("gal2equ:ra", -1, gal2equ(0.0, 0.0, NULL, &x))) n++;
  if(check("gal2equ:dec", -1, gal2equ(0.0, 0.0, &x, NULL))) n++;

  return n;
}

static int test_equ2ecl() {
  double x;
  int n = 0;

  if(check("equ2ecl:lon", -1, equ2ecl(0.0, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, 0.0, 0.0, NULL, &x))) n++;
  if(check("equ2ecl:lat", -1, equ2ecl(0.0, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, 0.0, 0.0, &x, NULL))) n++;

  return n;
}


static int test_ecl2equ() {
  double x;
  int n = 0;

  if(check("ecl2equ:lon", -1, ecl2equ(0.0, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, 0.0, 0.0, NULL, &x))) n++;
  if(check("ecl2equ:lat", -1, ecl2equ(0.0, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, 0.0, 0.0, &x, NULL))) n++;

  return n;
}

static int test_equ2ecl_vec() {
  double p[3] = {1.0};
  int n = 0;

  if(check("equ2ecl_vec:in", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("equ2ecl_vec:out", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("equ2ecl_vec:accuracy", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, -1, p, p))) n++;
  if(check("equ2ecl_vec:equator", 1, equ2ecl_vec(0.0, -1, NOVAS_FULL_ACCURACY, p, p))) n++;

  return n;
}

static int test_ecl2equ_vec() {
  double p[3] = {1.0};
  int n = 0;

  if(check("ecl2equ_vec:in", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("ecl2equ_vec:out", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("ecl2equ_vec:accuracy", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, -1, p, p))) n++;
  if(check("ecl2equ_vec:equator", 1, ecl2equ_vec(0.0, -1, NOVAS_FULL_ACCURACY, p, p))) n++;

  return n;
}


static int test_itrs_to_hor() {
  on_surface loc = ON_SURFACE_INIT;
  double p[3] = {0.0}, az, za;
  int n = 0;

  if(check("itrs_to_hor:loc", -1, itrs_to_hor(NULL, p, &az, &za))) n++;
  if(check("itrs_to_hor:in", -1, itrs_to_hor(&loc, NULL, &az, &za))) n++;

  return n;
}

static int test_hor_to_itrs() {
  on_surface loc = ON_SURFACE_INIT;
  double p[3];
  int n = 0;

  if(check("hor_to_itrs:loc", -1, hor_to_itrs(NULL, 0.0, 0.0, p))) n++;
  if(check("hor_to_itrs:in", -1, hor_to_itrs(&loc, 0.0, 0.0, NULL))) n++;

  return n;
}

static int test_equ2hor() {
  on_surface loc = ON_SURFACE_INIT;
  double az, za, rar, decr;
  int n = 0;

  if(check("equ2hor:loc", -1, equ2hor(0.0, 0.0, NOVAS_FULL_ACCURACY, 0.0, 0.0, NULL, 0.0, 0.0, NOVAS_STANDARD_ATMOSPHERE, &az, &za, &rar, &decr))) n++;
  if(check("equ2hor:az", -1, equ2hor(0.0, 0.0, NOVAS_FULL_ACCURACY, 0.0, 0.0, &loc, 0.0, 0.0, NOVAS_STANDARD_ATMOSPHERE, NULL, &za, &rar, &decr))) n++;
  if(check("equ2hor:zd", -1, equ2hor(0.0, 0.0, NOVAS_FULL_ACCURACY, 0.0, 0.0, &loc, 0.0, 0.0, NOVAS_STANDARD_ATMOSPHERE, &az, NULL, &rar, &decr))) n++;

  return n;
}

static int test_gcrs2equ() {
  double ra, dec;
  int n = 0;

  if(check("gcrs2equ:ra", -1, gcrs2equ(0.0, NOVAS_DYNAMICAL_MOD, NOVAS_FULL_ACCURACY, 0.0, 0.0, NULL, &dec))) n++;
  if(check("gcrs2equ:dec", -1, gcrs2equ(0.0, NOVAS_DYNAMICAL_MOD, NOVAS_FULL_ACCURACY, 0.0, 0.0, &ra, NULL))) n++;
  if(check("gcrs2equ:sys", -1, gcrs2equ(0.0, -1, NOVAS_FULL_ACCURACY, 0.0, 0.0, &ra, &dec))) n++;

  return n;
}

static int test_sidereal_time() {
  double x;
  int n = 0;

  if(check("sidereal_time:out", -1, sidereal_time(0.0, 0.0, 0.0, NOVAS_MEAN_EQUINOX, EROT_GST, NOVAS_FULL_ACCURACY, NULL))) n++;
  if(check("sidereal_time:accuracy", 1, sidereal_time(0.0, 0.0, 0.0, NOVAS_MEAN_EQUINOX, EROT_GST, -1, &x))) n++;

  return n;
}

static int test_ter2cel() {
  double p[3] = {1.0};
  int n = 0;

  if(check("ter2cel:in", -1, ter2cel(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, NULL, p))) n++;
  if(check("ter2cel:out", -1, ter2cel(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, NULL))) n++;
  if(check("ter2cel:accuracy", 1, ter2cel(0.0, 0.0, 0.0, EROT_GST, -1, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;
  if(check("ter2cel:erot", 2, ter2cel(0.0, 0.0, 0.0, -1, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;

  return n;
}

static int test_cel2ter() {
  double p[3] = {1.0};
  int n = 0;

  if(check("cel2ter:in", -1, cel2ter(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, NULL, p))) n++;
  if(check("cel2ter:out", -1, cel2ter(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, NULL))) n++;
  if(check("cel2ter:accuracy", 1, cel2ter(0.0, 0.0, 0.0, EROT_GST, -1, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;
  if(check("cel2ter:erot", 2, cel2ter(0.0, 0.0, 0.0, -1, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;

  return n;
}


static int test_spin() {
  double p[3] = {1.0};
  int n = 0;

  if(check("spin:in", -1, spin(0.0, NULL, p))) n++;
  if(check("spin:out", -1, spin(0.0, p, NULL))) n++;

  return n;
}

static int test_wobble() {
  double p[3] = {1.0};
  int n = 0;

  if(check("wobble:dir:-1", 0, wobble(NOVAS_JD_J2000, -1, 0.0, 0.0, p, p))) n++;
  if(check("wobble:dir:hi", -1, wobble(NOVAS_JD_J2000, NOVAS_WOBBLE_DIRECTIONS, 0.0, 0.0, p, p))) n++;
  if(check("wobble:in", -1, wobble(0.0, WOBBLE_ITRS_TO_PEF, 0.0, 0.0, NULL, p))) n++;
  if(check("wobble:out", -1, wobble(0.0, WOBBLE_ITRS_TO_PEF, 0.0, 0.0, p, NULL))) n++;

  return n;
}

static int test_terra() {
  on_surface loc = ON_SURFACE_INIT;
  double p[3], v[3];
  int n = 0;

  if(check("terra:loc", -1, terra(NULL, 0.0, p, v))) n++;
  if(check("terra:pos+vel", -1, terra(&loc, 0.0, NULL, NULL))) n++;

  return n;
}

static int test_e_tilt() {
  int n = 0;

  if(check("e_tilt:accuracy", -1, e_tilt(0.0, -1, NULL, NULL, NULL, NULL, NULL))) n++;

  return n;
}



static int test_cel_pole() {
  int n = 0;

  if(check("cel_pole:type", 1, cel_pole(0.0, -1, 0.0, 0.0))) n++;

  return n;
}

static int test_frame_tie() {
  double p[3] = {1.0};
  int n = 0;

  if(check("frame_tie:in", -1, frame_tie(NULL, 0, p))) n++;
  if(check("frame_tie:out", -1, frame_tie(p, 0, NULL))) n++;

  return n;
}


static int test_proper_motion() {
  double p[3] = {1.0}, v[3] = {1.0};
  int n = 0;

  if(check("frame_tie:p", -1, proper_motion(0.0, NULL, v, 1.0, p))) n++;
  if(check("frame_tie:v", -1, proper_motion(0.0, p, NULL, 1.0, p))) n++;
  if(check("frame_tie:out", -1, proper_motion(0.0, p, v, 1.0, NULL))) n++;

  return n;
}

static int test_bary2obs() {
  double p[3] = {0.0}, po[3] = {0.0}, out[3], lt;
  int n = 0;

  if(check("bary2obs:pos", -1, bary2obs(NULL, po, out, &lt))) n++;
  if(check("bary2obs:obs", -1, bary2obs(p, NULL, out, &lt))) n++;
  if(check("bary2obs:out", -1, bary2obs(p, po, NULL, &lt))) n++;

  return n;
}

static int test_geo_posvel() {
  observer o;
  double p[3] = {0.0}, v[3] = {0.0};
  int n = 0;

  o.where = NOVAS_OBSERVER_ON_EARTH;
  if(check("geo_posvel:loc", -1, geo_posvel(0.0, 0.0, NOVAS_FULL_ACCURACY, NULL, p, v))) n++;
  if(check("geo_posvel:pos+vel", -1, geo_posvel(0.0, 0.0, NOVAS_FULL_ACCURACY, &o, NULL, NULL))) n++;
  if(check("geo_posvel:accuracy", 1, geo_posvel(0.0, 0.0, -1, &o, p, v))) n++;

  o.where = -1;
  if(check("geo_posvel:where", 2, geo_posvel(0.0, 0.0, NOVAS_FULL_ACCURACY, &o, p, v))) n++;

  return n;
}

static int test_light_time2() {
  object o;
  double pos[3] = {1.0}, p[3], v[3], t;
  int n = 0;

  make_planet(NOVAS_SUN, &o);

  if(check("light_time2:tout", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, v, NULL))) n++;
  if(check("light_time2:object", -1, light_time2(0.0, NULL, pos, 0.0, NOVAS_FULL_ACCURACY, p, v, &t))) n++;
  if(check("light_time2:pos", -1, light_time2(0.0, &o, NULL, 0.0, NOVAS_FULL_ACCURACY, p, v, &t))) n++;

  novas_set_max_iter(0);
  if(check("light_time2:converge", 1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, v, &t))) n++;
  else if(check("light_time2:converge:errno", ECANCELED, errno)) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);

  return n;
}

static int test_d_light() {
  double p[3] = {1.0};
  int n = 0;

  if(check_nan("d_light:1", d_light(NULL, p))) n++;
  if(check_nan("d_light:2", d_light(p, NULL))) n++;

  return n;
}

static int test_cio_array() {
  ra_of_cio x[5];
  int n = 0;

  if(check("cio_array:out", -1, cio_array(0.0, 5, NULL))) n++;
  if(check("cio_array:n_pts:lo", 3, cio_array(0.0, 1, x))) n++;

  return n;
}

static int test_cio_basis() {
  double x[3], y[3], z[3];
  int n = 0;

  if(check("cio_basis:x", -1, cio_basis(0.0, 0.0, CIO_VS_GCRS, NOVAS_FULL_ACCURACY, NULL, y, z))) n++;
  if(check("cio_basis:y", -1, cio_basis(0.0, 0.0, CIO_VS_GCRS, NOVAS_FULL_ACCURACY, x, NULL, z))) n++;
  if(check("cio_basis:z", -1, cio_basis(0.0, 0.0, CIO_VS_GCRS, NOVAS_FULL_ACCURACY, x, y, NULL))) n++;
  if(check("cio_basis:accuracy", -1, cio_basis(0.0, 0.0, CIO_VS_GCRS, -1, x, y, z))) n++;
  if(check("cio_basis:ref", 1, cio_basis(0.0, 0.0, -1, NOVAS_FULL_ACCURACY, x, y, z))) n++;

  return n;
}

static int test_cio_location() {
  double x;
  short type;
  int n = 0;

  if(check("cio_location:ra", -1, cio_location(0.0, NOVAS_FULL_ACCURACY, NULL, &type))) n++;
  if(check("cio_location:type", -1, cio_location(0.0, NOVAS_FULL_ACCURACY, &x, NULL))) n++;
  if(check("cio_location:accuracy", -1, cio_location(0.0, -1, &x, &type))) n++;

  return n;
}

static int test_cio_ra() {
  double x;
  int n = 0;

  if(check("cio_location:ra", -1, cio_ra(0.0, NOVAS_FULL_ACCURACY, NULL))) n++;
  if(check("cio_location:accuracy", 1, cio_ra(0.0, -1, &x))) n++;

  return n;
}

static int test_starvectors() {
  cat_entry star = CAT_ENTRY_INIT;
  double p[3], v[3];
  int n = 0;

  if(check("starvectors:star", -1, starvectors(NULL, p, v))) n++;
  if(check("starvectors:pos+vel", -1, starvectors(&star, NULL, NULL))) n++;

  return n;
}

static int test_radec2vector() {
  int n = 0;

  if(check("radec2vector", -1, radec2vector(0.0, 0.0, 1.0, NULL))) n++;

  return n;
}

static int test_vector2radec() {
  double p[3] = {0.0}, ra, dec;
  int n = 0;

  if(check("vector2radec:vec", -1, vector2radec(NULL, &ra, &dec))) n++;

  if(check("vector2radec:zero", 1, vector2radec(p, &ra, &dec))) n++;

  p[2] = 1.0;
  if(check("vector2radec:pole", 2, vector2radec(p, &ra, &dec))) n++;
  if(check("vector2radec:pole:ra:null", 2, vector2radec(p, NULL, &dec))) n++;
  if(check("vector2radec:pole:dec:null", 2, vector2radec(p, &ra, NULL))) n++;

  return n;
}

static int test_planet_lon() {
  int n = 0;

  if(check_nan("planet_lon:-1", planet_lon(0.0, -1))) n++;
  if(check_nan("planet_lon:pluto", planet_lon(0.0, NOVAS_PLUTO))) n++;

  return n;
}

static int test_fund_args() {
  int n = 0;

  if(check("find_args", -1, fund_args(0.0, NULL))) n++;

  return n;
}

static int test_nutation_angles() {
  double x;
  int n = 0;

  if(check("nutation_angles:dpsi", -1, nutation_angles(0.0, NOVAS_FULL_ACCURACY, NULL, &x))) n++;
  if(check("nutation_angles:deps", -1, nutation_angles(0.0, NOVAS_FULL_ACCURACY, &x, NULL))) n++;

  return n;
}

static int test_set_nutation_lp_provider() {
  int n = 0;

  if(check("set_nutation_lp_provider", -1, set_nutation_lp_provider(NULL))) n++;

  return n;
}

static int test_nutation() {
  double p[3] = {1.0};
  int n = 0;

  if(check("nutation:in", -1, nutation(0.0, NUTATE_MEAN_TO_TRUE, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("nutation:out", -1, nutation(0.0, NUTATE_MEAN_TO_TRUE, NOVAS_FULL_ACCURACY, p, NULL))) n++;

  return n;
}

static int test_precession() {
  double p[3] = {1.0};
  int n = 0;

  if(check("precesion:in", -1, precession(0.0, NULL, 1.0, p))) n++;
  if(check("precesion:out", -1, precession(0.0, p, 1.0, NULL))) n++;

  return n;
}

static int test_rad_vel() {
  object o;
  double p[3] = {0.0}, v[3] = {0.0}, vo[3] = {0.0}, rv;
  int n = 0;

  o.type = NOVAS_PLANET;

  if(check("rad_vel:object", -1, rad_vel(NULL, p, v, vo, 1.0, 1.0, 1.0, &rv))) n++;
  if(check("rad_vel:pos", -1, rad_vel(&o, NULL, v, vo, 1.0, 1.0, 1.0, &rv))) n++;
  if(check("rad_vel:vel", -1, rad_vel(&o, p, NULL, vo, 1.0, 1.0, 1.0, &rv))) n++;
  if(check("rad_vel:vobs", -1, rad_vel(&o, p, v, NULL, 1.0, 1.0, 1.0, &rv))) n++;
  if(check("rad_vel:out", -1, rad_vel(&o, p, v, vo, 1.0, 1.0, 1.0, NULL))) n++;

  if(check_nan("rad_vel2:emit", rad_vel2(&o, NULL, v, p, vo, 1.0, 1.0, 1.0))) n++;
  if(check_nan("rad_vel2:det", rad_vel2(&o, p, v, NULL, vo, 1.0, 1.0, 1.0))) n++;

  o.type = -1;
  if(check("rad_vel", -1, rad_vel(&o, p, v, vo, 1.0, 1.0, 1.0, &rv))) n++;

  return n;
}



static int test_aberration() {
  double p[3] = {1.0}, v[3] = {0.0};
  int n = 0;

  if(check("aberration:pos", -1, aberration(NULL, v, 0.0, p))) n++;
  if(check("aberration:vel", -1, aberration(p, NULL, 0.0, p))) n++;
  if(check("aberration:out", -1, aberration(p, v, 0.0, NULL))) n++;

  return n;
}

static int test_grav_vec() {
  double p[3] = {2.0}, po[3] = {0.0, 1.0}, pb[3] = {0.0};
  int n = 0;

  if(check("grav_vec:pos", -1, grav_vec(NULL, po, pb, 1.0, p))) n++;
  if(check("grav_vec:po", -1, grav_vec(p, NULL, pb, 1.0, p))) n++;
  if(check("grav_vec:pb", -1, grav_vec(p, po, NULL, 1.0, p))) n++;
  if(check("grav_vec:out", -1, grav_vec(p, po, pb, 1.0, NULL))) n++;

  return n;
}

static int test_grav_def() {
  double p[3] = {2.0}, po[3] = {0.0, 1.0};
  int n = 0;

  if(check("grav_def:pos", -1, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, NULL, po, p))) n++;
  if(check("grav_def:po", -1, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, NULL, p))) n++;
  if(check("grav_def:out", -1, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, po, NULL))) n++;

  if(check("grav_def:sun", 13, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, po, p))) n++;

  fprintf(stderr, ">>> Expecting an error and trace...\n");
  novas_debug(NOVAS_DEBUG_EXTRA);
  enable_earth_sun_hp(1);
  if(check("grav_def:planets", 12, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, po, p))) n++;
  enable_earth_sun_hp(0);
  novas_debug(NOVAS_DEBUG_OFF);

  return n;
}

static int test_grav_undef() {
  double p[3] = {2.0}, po[3] = {0.0, 1.0};
  int n = 0;

  if(check("grav_def:pos", -1, grav_undef(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, NULL, po, p))) n++;
  if(check("grav_def:po", -1, grav_undef(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, p, NULL, p))) n++;
  if(check("grav_def:out", -1, grav_undef(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, p, po, NULL))) n++;

  return n;
}

static int test_grav_init_planets() {
  novas_planet_bundle planets = NOVAS_PLANET_BUNDLE_INIT;
  double p[3] = {2.0};
  int n = 0;

  if(check("grav_init_planets:pos_obs", -1, obs_planets(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, NULL, 0, &planets))) n++;
  if(check("grav_init_planets:planets", -1, obs_planets(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, p, 0, NULL))) n++;

  return n;
}

static int test_grav_planets() {
  novas_planet_bundle planets = NOVAS_PLANET_BUNDLE_INIT;
  double p[3] = {2.0}, po[3] = {0.0, 1.0}, out[3] = {0.0};
  int n = 0;

  if(check("grav_planets:pos_src", -1, grav_planets(NULL, po, &planets, out))) n++;
  if(check("grav_planets:pos_obs", -1, grav_planets(p, NULL, &planets, out))) n++;
  if(check("grav_planets:planets", -1, grav_planets(p, po, NULL, out))) n++;
  if(check("grav_planets:pos_src", -1, grav_planets(p, po, &planets, NULL))) n++;

  return n;
}

static int test_grav_undo_planets() {
  novas_planet_bundle planets = NOVAS_PLANET_BUNDLE_INIT;
  double p[3] = {2.0}, po[3] = {0.0, 1.0}, out[3] = {0.0};
  int n = 0;

  if(check("grav_undo_planets:pos_app", -1, grav_undo_planets(NULL, po, &planets, out))) n++;
  if(check("grav_undo_planets:pos_obs", -1, grav_undo_planets(p, NULL, &planets, out))) n++;
  if(check("grav_undo_planets:planets", -1, grav_undo_planets(p, po, NULL, out))) n++;
  if(check("grav_undo_planets:pos_src", -1, grav_undo_planets(p, po, &planets, NULL))) n++;

  planets.mask = 1 << NOVAS_SUN;
  novas_set_max_iter(0);
  if(check("grav_undo_planets:converge", -1, grav_undo_planets(p, po, &planets, out))) n++;
  else if(check("grav_undo_planets:converge:errno", ECANCELED, errno)) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);

  return n;
}

static int test_earth_sun_calc() {
  double p[3], v[3];
  int n = 0;

  if(check("earth_sun_calc:pos", -1, earth_sun_calc(NOVAS_JD_J2000, NOVAS_SUN, NOVAS_BARYCENTER, NULL, v))) n++;
  if(check("earth_sun_calc:vel", -1, earth_sun_calc(NOVAS_JD_J2000, NOVAS_SUN, NOVAS_BARYCENTER, p, NULL))) n++;
  if(check("earth_sun_calc:tdb:lo", 1, earth_sun_calc(2340000.0, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;
  if(check("earth_sun_calc:tdb:hi", 1, earth_sun_calc(2560001.0, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;
  if(check("earth_sun_calc:number", 2, earth_sun_calc(NOVAS_JD_J2000, NOVAS_JUPITER, NOVAS_BARYCENTER, p, v))) n++;

  if(check("earth_sun_calc:solarsystem:pos", -1, solarsystem(NOVAS_JD_J2000, NOVAS_SUN, NOVAS_BARYCENTER, NULL, v))) n++;
  if(check("earth_sun_calc:solarsystem:OK", 0, solarsystem(NOVAS_JD_J2000, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;

  return n;
}

static int test_earth_sun_calc_hp() {
  double p[3], v[3], tdb2[2] = { NOVAS_JD_J2000 };
  int n = 0;

  enable_earth_sun_hp(1);

  if(check("earth_sun_calc_hp:tdb", -1, earth_sun_calc_hp(NULL, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;
  if(check("earth_sun_calc_hp:pos", -1, earth_sun_calc_hp(tdb2, NOVAS_SUN, NOVAS_BARYCENTER, NULL, v))) n++;
  if(check("earth_sun_calc_hp:vel", -1, earth_sun_calc_hp(tdb2, NOVAS_SUN, NOVAS_BARYCENTER, p, NULL))) n++;
  if(check("earth_sun_calc_hp:number", 2, earth_sun_calc_hp(tdb2, NOVAS_JUPITER, NOVAS_BARYCENTER, p, v))) n++;

  if(check("earth_sun_calc_hp:solarsystem_hp:tdb", -1, solarsystem_hp(NULL, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;
  if(check("earth_sun_calc_hp:solarsystem_hp:OK", 0, solarsystem_hp(tdb2, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;

  enable_earth_sun_hp(0);

  return n;
}

static int test_sun_eph() {
  extern int sun_eph(double jd, double *ra, double *dec, double *dis);

  double ra, dec, dis;
  int n = 0;

  if(check("sun_eph:ra", -1, sun_eph(NOVAS_JD_J2000, NULL, &dec, &dis))) n++;
  if(check("sun_eph:dec", -1, sun_eph(NOVAS_JD_J2000, &ra, NULL, &dis))) n++;
  if(check("sun_eph:dis", -1, sun_eph(NOVAS_JD_J2000, &ra, &dec, NULL))) n++;

  return n;
}

static int test_obs_posvel() {
  observer obs;
  double x;
  int n = 0;

  make_observer_at_geocenter(&obs);

  if(check("obs_posvel:obs", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, NULL, NULL, NULL, &x, NULL))) n++;
  if(check("obs_posvel:obs:pos+vel", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, NULL, NULL))) n++;
  obs.where = -1;
  if(check("obs_posvel:obs:where:-1", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, &x, NULL))) n++;

  obs.where = NOVAS_OBSERVER_PLACES;
  if(check("obs_posvel:obs:where:hi", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, &x, NULL))) n++;

  return n;
}

static int test_time() {
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  int n = 0;
  long ijd = 0;

  if(check("time:set:time", -1, novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 37, 0.11, NULL))) n++;
  if(check("time:set:scale:-1", -1, novas_set_time(-1, NOVAS_JD_J2000, 37, 0.11, &time))) n++;
  if(check("time:set:scale:hi", -1, novas_set_time(NOVAS_TIMESCALES, NOVAS_JD_J2000, 37, 0.11, &time))) n++;

  if(check_nan("time:get:time", novas_get_time(NULL, NOVAS_TT))) n++;
  if(check_nan("time:get:scale:-1", novas_get_time(&time, -1))) n++;
  if(check_nan("time:get:scale:hi", novas_get_time(&time, NOVAS_TIMESCALES))) n++;

  if(check("time:get_unix_time:time", -1, novas_get_unix_time(NULL, &ijd))) n++;
  if(check("time:get_unix_time:time+ijd", -1, novas_get_unix_time(NULL, NULL))) n++;

  if(check("time:offset:time", -1, novas_offset_time(NULL, 0.1, &time))) n++;
  if(check("time:offset:out", -1, novas_offset_time(&time, 0.1, NULL))) n++;
  if(check("time:offset:both", -1, novas_offset_time(NULL, 0.1, NULL))) n++;

  if(check_nan("time:diff:t1", novas_diff_time(NULL, &time))) n++;
  if(check_nan("time:diff:t2", novas_diff_time(&time, NULL))) n++;
  if(check_nan("time:diff:both", novas_diff_time(NULL, NULL))) n++;

  if(check_nan("time:diff_tcg:t1", novas_diff_tcg(NULL, &time))) n++;
  if(check_nan("time:diff_tcg:t2", novas_diff_tcg(&time, NULL))) n++;
  if(check_nan("time:diff_tcg:both", novas_diff_tcg(NULL, NULL))) n++;

  if(check_nan("time:diff_tcb:t1", novas_diff_tcb(NULL, &time))) n++;
  if(check_nan("time:diff_tcb:t2", novas_diff_tcb(&time, NULL))) n++;
  if(check_nan("time:diff_tcb:both", novas_diff_tcb(NULL, NULL))) n++;

  return n;
}

static int test_set_str_time() {
  int n = 0;

  if(check("set_str_time:NULL", -1, novas_set_str_time(NOVAS_TT, NULL, 37, 0.11, NULL))) n++;
  if(check("set_str_time:empty", -1, novas_set_str_time(NOVAS_TT, "", 37, 0.11, NULL))) n++;
  if(check("set_str_time:invalid", -1, novas_set_str_time(NOVAS_TT, "blah", 37, 0.11, NULL))) n++;

  return n;
}



static double switching_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el) {
  static int count;
  (void) jd_tt;
  (void) loc;
  (void) type;
  (void) el;
  return (count++) % 2 ? -0.1 : 0.1;
}


static int test_refraction() {
  int n = 0;
  on_surface obs = ON_SURFACE_INIT;

  if(check_nan("stardard_refraction:loc", novas_standard_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0))) n++;
  if(check_nan("stardard_refraction:type:-2", novas_standard_refraction(NOVAS_JD_J2000, &obs, -2, 10.0))) n++;
  if(check_nan("stardard_refraction:type:1", novas_standard_refraction(NOVAS_JD_J2000, &obs, 1, 10.0))) n++;
  if(check_nan("stardard_refraction:el:neg", novas_standard_refraction(NOVAS_JD_J2000, &obs, 1, -10.0))) n++;

  if(check_nan("optical_refraction:loc", novas_optical_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0))) n++;
  if(check_nan("optical_refraction:type:-2", novas_optical_refraction(NOVAS_JD_J2000, &obs, -2, 10.0))) n++;
  if(check_nan("optical_refraction:type:1", novas_optical_refraction(NOVAS_JD_J2000, &obs, 1, 10.0))) n++;
  if(check_nan("optical_refraction:el:neg", novas_optical_refraction(NOVAS_JD_J2000, &obs, 1, -10.0))) n++;

  if(check_nan("radio_refraction:loc", novas_radio_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0))) n++;
  if(check_nan("radio_refraction:type:-2", novas_radio_refraction(NOVAS_JD_J2000, &obs, -2, 10.0))) n++;
  if(check_nan("radio_refraction:type:1", novas_radio_refraction(NOVAS_JD_J2000, &obs, 1, 10.0))) n++;
  if(check_nan("radio_refraction:el:neg", novas_radio_refraction(NOVAS_JD_J2000, &obs, 1, -10.0))) n++;

  if(check_nan("inv_refract:conv", novas_inv_refract(switching_refraction, NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0))) n++;
  else if(check("inv_refract:conv:errno", ECANCELED, errno)) n++;

  fprintf(stderr, ">>> Expecting an error and trace...\n");
  novas_debug(1);
  novas_optical_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0);
  novas_debug(0);

  obs.humidity = -1.01;
  if(check_nan("radio_refraction:humidity:lo", novas_radio_refraction(NOVAS_JD_J2000, &obs, NOVAS_REFRACT_OBSERVED, 10.0))) n++;

  obs.humidity = 101.01;
  if(check_nan("radio_refraction:humidity:hi", novas_radio_refraction(NOVAS_JD_J2000, &obs, NOVAS_REFRACT_OBSERVED, 10.0))) n++;

  return n;
}

static int test_make_frame() {
  int n = 0;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;

  if(check("make_frame:obs", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, NULL, &ts, 0.0, 0.0, &frame))) n++;
  if(check("make_frame:time", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, NULL, 0.0, 0.0, &frame))) n++;
  if(check("make_frame:frame", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, NULL))) n++;
  if(check("make_frame:accuracy:-1", -1, novas_make_frame(-1, &obs, &ts, 0.0, 0.0, &frame))) n++;
  if(check("make_frame:accuracy:2", -1, novas_make_frame(2, &obs, &ts, 0.0, 0.0, &frame))) n++;

  obs.where = -1;
  if(check("make_frame:obs:where:-1", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) n++;

  obs.where = NOVAS_OBSERVER_PLACES;
  if(check("make_frame:obs:where:hi", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) n++;

  return n;
}

static int test_change_observer() {
  int n = 0;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT, out = NOVAS_FRAME_INIT;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("change_observer:orig", -1, novas_change_observer(NULL, &obs, &out))) n++;
  if(check("change_observer:orig:init", -1, novas_change_observer(&frame, &obs, &out))) n++;

  if(check("change_observer:make_frame", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;
  if(check("change_observer:orig:ok", 0, novas_change_observer(&frame, &obs, &out))) n++;

  if(check("change_observer:obs", -1, novas_change_observer(&frame, NULL, &out))) n++;
  if(check("change_observer:out", -1, novas_change_observer(&frame, &obs, NULL))) n++;

  return n;
}

static int test_make_transform() {
  int n = 0;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  novas_transform T = NOVAS_TRANSFORM_INIT;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("make_transform:frame", -1, novas_make_transform(NULL, NOVAS_ICRS, NOVAS_J2000, &T))) n++;
  if(check("make_transform:frame:init", -1, novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, &T))) n++;

  if(check("make_transform:frame", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) n++;
  if(check("make_transform:out", -1, novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, NULL))) n++;

  if(check("make_transform:from:-1", -1, novas_make_transform(&frame, -1, NOVAS_J2000, &T))) n++;
  if(check("make_transform:from:hi", -1, novas_make_transform(&frame, NOVAS_REFERENCE_SYSTEMS, NOVAS_J2000, &T))) n++;

  if(check("make_transform:to:-1", -1, novas_make_transform(&frame, NOVAS_ICRS, -1, &T))) n++;
  if(check("make_transform:to:hi", -1, novas_make_transform(&frame, NOVAS_ICRS, NOVAS_REFERENCE_SYSTEMS, &T))) n++;

  return n;
}

static int test_geom_posvel() {
  int n = 0;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  object o = NOVAS_SUN_INIT;
  double pos[3] = {0.0}, vel[3] = {0.0};

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("geom_posvel:frame", -1, novas_geom_posvel(&o, NULL, NOVAS_ICRS, pos, vel))) n++;
  if(check("geom_posvel:frame:init", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("geom_posvel:frame:ok", 0, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;

  if(check("geom_posvel:object", -1, novas_geom_posvel(NULL, &frame, NOVAS_ICRS, pos, vel))) n++;
  if(check("geom_posvel:pos+vel", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, NULL, NULL))) n++;
  if(check("geom_posvel:sys:-1", -1, novas_geom_posvel(&o, &frame, -1, pos, vel))) n++;
  if(check("geom_posvel:sys:hi", -1, novas_geom_posvel(&o, &frame, NOVAS_REFERENCE_SYSTEMS, pos, vel))) n++;

  frame.accuracy = -1;
  if(check("geom_posvel:frame:accuracy:-1", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;

  frame.accuracy = 2;
  if(check("geom_posvel:frame:accuracy:2", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;

#ifndef DEFAULT_READEPH
  frame.accuracy = NOVAS_REDUCED_ACCURACY;
  make_ephem_object("blah", 111111, &o);
  if(check("geom_posvel:ephem_object", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;
#endif

  return n;
}

static int test_sky_pos() {
  int n = 0;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  object o = NOVAS_SSB_INIT;
  sky_pos out = SKY_POS_INIT;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("sky_pos:frame", -1, novas_sky_pos(&o, NULL, NOVAS_ICRS, &out))) n++;
  if(check("sky_pos:frame:init", -1, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("sky_pos:frame:ok", 0, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  if(check("sky_pos:object", -1, novas_sky_pos(NULL, &frame, NOVAS_ICRS, &out))) n++;
  if(check("sky_pos:out", -1, novas_sky_pos(&o, &frame, NOVAS_ICRS, NULL))) n++;
  if(check("sky_pos:sys:-1", -1, novas_sky_pos(&o, &frame, -1, &out))) n++;
  if(check("sky_pos:sys:hi", -1, novas_sky_pos(&o, &frame, NOVAS_REFERENCE_SYSTEMS, &out))) n++;

  frame.accuracy = -1;
  if(check("sky_pos:frame:accuracy:-1", -1, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  frame.accuracy = 2;
  if(check("sky_pos:frame:accuracy:2", -1, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  frame.accuracy = NOVAS_FULL_ACCURACY;
  if(check("sky_pos:frame:accuracy:full", 73, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  return n;
}

static int test_app_to_geom() {
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double pos[3] = {0.0};
  int n = 0;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("app_to_geom:frame", -1, novas_app_to_geom(NULL, NOVAS_ICRS, 1.0, 2.0, 10.0, pos))) n++;
  if(check("app_to_geom:frame:init", -1, novas_app_to_geom(&frame, NOVAS_ICRS, 1.0, 2.0, 10.0, pos))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  novas_set_max_iter(0);
  if(check("app_to_geom:frame:converge", -1, novas_app_to_geom(&frame, NOVAS_ICRS, 1.0, 2.0, 10.0, pos))) n++;
  else if(check("app_to_geom:frame:converge:errno", ECANCELED, errno)) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);

  if(check("app_to_geom:pos", -1, novas_app_to_geom(&frame, NOVAS_ICRS, 1.0, 2.0, 10.0, NULL))) n++;
  if(check("app_to_geom:sys:-1", -1, novas_app_to_geom(&frame, -1, 1.0, 2.0, 10.0, pos))) n++;
  if(check("app_to_geom:sys:hi", -1, novas_app_to_geom(&frame, NOVAS_REFERENCE_SYSTEMS, 1.0, 2.0, 10.0, pos))) n++;

  return n;
}

static int test_geom_to_app() {
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  sky_pos out = SKY_POS_INIT;
  double pos[3] = {0.0};
  int n = 0;

  make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("geom_to_app:frame", -1, novas_geom_to_app(NULL, pos, NOVAS_ICRS, &out))) n++;
  if(check("geom_to_app:frame:init", -1, novas_geom_to_app(&frame, pos, NOVAS_ICRS, &out))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("geom_to_app:frame:ok", 0, novas_geom_to_app(&frame, pos, NOVAS_ICRS, &out))) n++;

  if(check("geom_to_app:pos", -1, novas_geom_to_app(&frame, NULL, NOVAS_ICRS, &out))) n++;
  if(check("geom_to_app:out", -1, novas_geom_to_app(&frame, pos, NOVAS_ICRS, NULL))) n++;
  if(check("geom_to_app:sys:-1", -1, novas_geom_to_app(&frame, pos, -1, &out))) n++;
  if(check("geom_to_app:sys:hi", -1, novas_geom_to_app(&frame, pos, NOVAS_REFERENCE_SYSTEMS, &out))) n++;

  frame.accuracy = -1;
  if(check("geom_to_app:frame:accuracy:-1", -1, novas_geom_to_app(&frame, pos, NOVAS_ICRS, &out))) n++;

  frame.accuracy = 2;
  if(check("geom_to_app:frame:accuracy:2", -1, novas_geom_to_app(&frame, pos, NOVAS_ICRS, &out))) n++;

  return n;
}

static int test_app_to_hor() {
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double az, el;
  int n = 0;

  make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("app_to_hor:frame", -1, novas_app_to_hor(NULL, NOVAS_ICRS, 1.0, 2.0, NULL, &az, &el))) n++;
  if(check("app_to_hor:frame:init", -1, novas_app_to_hor(&frame, NOVAS_ICRS, 1.0, 2.0, NULL, &az, &el))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("app_to_hor:frame:ok", 0, novas_app_to_hor(&frame, NOVAS_ICRS, 1.0, 2.0, NULL, &az, &el))) n++;

  if(check("app_to_hor:az+el", -1, novas_app_to_hor(&frame, NOVAS_ICRS, 1.0, 2.0, NULL, NULL, NULL))) n++;
  if(check("app_to_hor:sys:-1", -1, novas_app_to_hor(&frame, -1, 1.0, 2.0, NULL, &az, &el))) n++;
  if(check("app_to_hor:sys:hi", -1, novas_app_to_hor(&frame, NOVAS_REFERENCE_SYSTEMS, 1.0, 2.0, NULL, &az, &el))) n++;

  frame.observer.where = NOVAS_OBSERVER_AT_GEOCENTER;
  if(check("app_to_hor:frame:obs:where", -1, novas_app_to_hor(&frame, NOVAS_ICRS, 1.0, 2.0, NULL, &az, &el))) n++;

  return n;
}

static int test_hor_to_app() {
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double ra, dec;
  int n = 0;

  make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("hor_to_app:frame", -1, novas_hor_to_app(NULL, 1.0, 2.0, NULL, NOVAS_ICRS, &ra, &dec))) n++;
  if(check("hor_to_app:frame:init", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_ICRS, &ra, &dec))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("hor_to_app:frame:ok", 0, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_ICRS, &ra, &dec))) n++;

  if(check("hor_to_app:ra+dec", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_ICRS, NULL, NULL))) n++;
  if(check("hor_to_app:sys:-1", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, -1, &ra, &dec))) n++;
  if(check("hor_to_app:sys:hi", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_REFERENCE_SYSTEMS, &ra, &dec))) n++;

  frame.observer.where = NOVAS_OBSERVER_AT_GEOCENTER;
  if(check("hor_to_app:frame:obs:where", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_ICRS, &ra, &dec))) n++;

  return n;
}


static int test_transform_vector() {
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  novas_transform T = NOVAS_TRANSFORM_INIT;
  double pos[3] = {0.0};
  int n = 0;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);
  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);

  novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, &T);

  if(check("transform_vector:in", -1, novas_transform_vector(NULL, &T, pos))) n++;
  if(check("transform_vector:out", -1, novas_transform_vector(pos, &T, NULL))) n++;
  if(check("transform_vector:in+out", -1, novas_transform_vector(NULL, &T, NULL))) n++;
  if(check("transform_vector:in", -1, novas_transform_vector(pos, NULL, pos))) n++;

  return n;
}

static int test_transform_sky_pos() {
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  novas_transform T = NOVAS_TRANSFORM_INIT;
  sky_pos pos = SKY_POS_INIT;
  int n = 0;


  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);
  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);

  novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, &T);

  if(check("transform_sky_pos:in", -1, novas_transform_sky_pos(NULL, &T, &pos))) n++;
  if(check("transform_sky_pos:out", -1, novas_transform_sky_pos(&pos, &T, NULL))) n++;
  if(check("transform_sky_pos:in+out", -1, novas_transform_sky_pos(NULL, &T, NULL))) n++;
  if(check("transform_sky_pos:in", -1, novas_transform_sky_pos(&pos, NULL, &pos))) n++;

  return n;
}

static int test_inv_transform() {
  novas_transform T = NOVAS_TRANSFORM_INIT;
  int n = 0;

  if(check("invert_transform:in", -1, novas_invert_transform(NULL, &T))) n++;
  if(check("invert_transform:out", -1, novas_invert_transform(&T, NULL))) n++;
  if(check("invert_transform:in+out", -1, novas_invert_transform(NULL, NULL))) n++;

  return n;
}

static int test_redshift_vrad() {
  int n = 0;

  if(check_nan("redshift_vrad", redshift_vrad(0.0, -1.0))) n++;
  return n;
}

static int test_unredshift_vrad() {
  int n = 0;

  if(check_nan("unredshift_vrad", unredshift_vrad(0.0, -1.0))) n++;
  return n;
}

static int test_z_add() {
  int n = 0;

  if(check_nan("z_add:z1", novas_z_add(-1.0, 0.0))) n++;
  if(check_nan("z_add:z2", novas_z_add(0.0, -1.0))) n++;
  if(check_nan("z_add:z1+z2", novas_z_add(-1.0, -1.0))) n++;
  return n;
}

static int test_z_inv() {
  int n = 0;

  if(check_nan("z_inv", novas_z_inv(-1.0))) n++;
  return n;
}

static int test_novas_to_naif_planet() {
  int n = 0;

  if(check("novas_to_naif_planet:-2", -1, novas_to_naif_planet(-2))) n++;
  if(check("novas_to_naif_planet:-1", -1, novas_to_naif_planet(-1))) n++;
  if(check("novas_to_naif_planet:hi", -1, novas_to_naif_planet(NOVAS_PLANETS))) n++;

  return n;
}

static int test_novas_to_dexxx_planet() {
  int n = 0;

  if(check("novas_to_dexxx_planet:-2", -1, novas_to_dexxx_planet(-2))) n++;
  if(check("novas_to_dexxx_planet:-1", -1, novas_to_dexxx_planet(-1))) n++;
  if(check("novas_to_dexxx_planet:hi", -1, novas_to_dexxx_planet(NOVAS_PLANETS))) n++;

  return n;
}

static int test_naif_to_novas_planet() {
  int n = 0;

  if(check("naif_to_novas_planet:-2", -1, naif_to_novas_planet(-1))) n++;
  if(check("naif_to_novas_planet:-1", -1, naif_to_novas_planet(-1))) n++;
  if(check("naif_to_novas_planet:", -1, naif_to_novas_planet(-1))) n++;
  if(check("naif_to_novas_planet:500", -1, naif_to_novas_planet(500))) n++;
  if(check("naif_to_novas_planet:501", -1, naif_to_novas_planet(501))) n++;
  if(check("naif_to_novas_planet:598", -1, naif_to_novas_planet(598))) n++;
  if(check("naif_to_novas_planet:1000", -1, naif_to_novas_planet(1000))) n++;

  return n;
}

static int test_planet_for_name() {
  int n = 0;

  if(check("planet_for_name:NULL", -1, novas_planet_for_name(NULL))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name(""))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("blah"))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("solar"))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("Solar flare"))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("Solar system"))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("Solar system size"))) n++;

  return n;
}

static int test_make_orbital_object() {
  int n = 0;
  novas_orbital orbit = NOVAS_ORBIT_INIT;
  object body = NOVAS_OBJECT_INIT;

  if(check("make_orbital_object:orbit", -1, make_orbital_object("blah", -1, NULL, &body))) n++;
  if(check("make_orbital_object:body", -1, make_orbital_object("blah", -1, &orbit, NULL))) n++;
  if(check("make_orbital_object:orbit+body", -1, make_orbital_object("blah", -1, NULL, NULL))) n++;

  return n;
}

static int test_orbit_posvel() {
  int n = 0;
  double pos[3] = {0.0}, vel[3] = {0.0};

  novas_orbital orbit = NOVAS_ORBIT_INIT;

  orbit.a = 1.0;

  if(check("set_obsys_pole:orbit", -1, novas_orbit_posvel(0.0, NULL, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;
  if(check("set_obsys_pole:pos=vel:NULL", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, NULL, NULL))) n++;
  if(check("set_obsys_pole:accuracy:-1", -1, novas_orbit_posvel(0.0, &orbit, -1, pos, vel))) n++;
  if(check("set_obsys_pole:accuracy:2", -1, novas_orbit_posvel(0.0, &orbit, 2, pos, vel))) n++;

  if(check("set_obsys_pole:orbit:converge", 0, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  novas_set_max_iter(0);
  if(check("set_obsys_pole:orbit:converge", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;
  else if(check("set_obsys_pole:orbit:converge:errno", ECANCELED, errno)) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);

  orbit.system.type = -1;
  if(check("set_obsys_pole:orbit:type:-1", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  orbit.system.type = NOVAS_TIRS;
  if(check("set_obsys_pole:orbit:type:tirs", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  orbit.system.type = NOVAS_ITRS;
  if(check("set_obsys_pole:orbit:type:itrs", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  orbit.system.type = NOVAS_REFERENCE_SYSTEMS;
  if(check("set_obsys_pole:orbit:type:hi", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  orbit.system.plane = NOVAS_EQUATORIAL_PLANE;
  orbit.system.type = -1;
  if(check("set_obsys_pole:orbit:type:-1:eq", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  orbit.system.type = NOVAS_GCRS;
  orbit.system.plane = -1;
  if(check("set_obsys_pole:orbit:plane:-1", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  return n;
}

static int test_set_obsys_pole() {
  int n = 0;

  if(check("set_obsys_pole:orbit:null", -1, novas_set_orbsys_pole(NOVAS_GCRS, 0.0, 0.0, NULL))) n++;

  if(check("set_obsys_pole:orbit:-1", -1, novas_set_orbsys_pole(-1, 0.0, 0.0, NULL))) n++;
  if(check("set_obsys_pole:orbit:tirs", -1, novas_set_orbsys_pole(NOVAS_TIRS, 0.0, 0.0, NULL))) n++;
  if(check("set_obsys_pole:orbit:itrs", -1, novas_set_orbsys_pole(NOVAS_ITRS, 0.0, 0.0, NULL))) n++;
  if(check("set_obsys_pole:orbit:hi", -1, novas_set_orbsys_pole(NOVAS_REFERENCE_SYSTEMS, 0.0, 0.0, NULL))) n++;

  return n;
}

static int test_hms_hours() {
  int n = 0;

  if(check_nan("hms_hours:null", novas_hms_hours(NULL))) n++;
  if(check_nan("hms_hours:empty", novas_hms_hours(""))) n++;
  if(check_nan("hms_hours:empty", novas_hms_hours(""))) n++;
  if(check_nan("hms_hours:few", novas_hms_hours("12"))) n++;
  if(check_nan("hms_hours:dms", novas_hms_hours("12d 39m 33.0"))) n++;
  if(check_nan("hms_hours:sep", novas_hms_hours("12,39,33.0"))) n++;
  if(check_nan("hms_hours:min:neg", novas_hms_hours("12 -1 33.0"))) n++;
  if(check_nan("hms_hours:min:60", novas_hms_hours("12 60 33.0"))) n++;
  if(check_nan("hms_hours:sec:neg", novas_hms_hours("12 39 -0.1"))) n++;
  if(check_nan("hms_hours:min:60", novas_hms_hours("12 39 60.0"))) n++;

  return n;
}

static int test_dms_degrees() {
  int n = 0;

  if(check_nan("dms_degrees:null", novas_dms_degrees(NULL))) n++;
  if(check_nan("dms_degrees:empty", novas_dms_degrees(""))) n++;
  if(check_nan("dms_degrees:empty", novas_dms_degrees(""))) n++;
  if(check_nan("dms_degrees:few", novas_dms_degrees("122"))) n++;
  if(check_nan("dms_degrees:hms", novas_dms_degrees("122h 39m 33.0"))) n++;
  if(check_nan("dms_degrees:sep", novas_dms_degrees("122,39,33.0"))) n++;
  if(check_nan("dms_degrees:min:neg", novas_dms_degrees("122 -1 33.0"))) n++;
  if(check_nan("dms_degrees:min:60", novas_dms_degrees("122 60 33.0"))) n++;
  if(check_nan("dms_degrees:sec:neg", novas_dms_degrees("122 39 -0.1"))) n++;
  if(check_nan("dms_degrees:min:60", novas_dms_degrees("122 39 60.0"))) n++;

  return n;
}

static int test_parse_hours() {
  int n = 0;
  char *tail;

  if(check_nan("parse_hours:null", novas_parse_hours(NULL, &tail))) n++;
  if(check_nan("parse_hours:blah", novas_parse_hours("blah", &tail))) n++;

  return n;
}

static int test_parse_degrees() {
  int n = 0;
  char *tail;

  if(check_nan("parse_degrees:null", novas_parse_degrees(NULL, &tail))) n++;
  if(check_nan("parse_degrees:blah", novas_parse_degrees("blah", &tail))) n++;
  if(check_nan("parse_degrees:East+space", novas_parse_degrees("East ", &tail))) n++;
  if(check_nan("parse_degrees:East+blah", novas_parse_degrees("East blah", &tail))) n++;
  if(check_nan("parse_degrees:East..0", novas_parse_degrees("East ..0", &tail))) n++;

  return n;
}

static int test_str_hours() {
  int n = 0;

  if(check_nan("str_hours:null", novas_str_hours(NULL))) n++;
  if(check_nan("str_hours:blah", novas_str_hours("blah"))) n++;

  return n;
}

static int test_str_degrees() {
  int n = 0;

  if(check_nan("str_degrees:null", novas_str_degrees(NULL))) n++;
  if(check_nan("str_degrees:blah", novas_str_degrees("blah"))) n++;

  return n;
}


static int test_helio_dist() {
  int n = 0;
  double rate = 0.0;
  object star = NOVAS_OBJECT_INIT;
  object jupiter = NOVAS_JUPITER_INIT;

  star.type = NOVAS_CATALOG_OBJECT;

  if(check_nan("helio_dist:null", novas_helio_dist(JD_J2000, NULL, NULL))) n++;
  if(check_nan("helio_dist:cat_object", novas_helio_dist(JD_J2000, &star, NULL))) n++;
  if(check_nan("helio_dist:null:rate", novas_helio_dist(JD_J2000, NULL, &rate))) n++;
  if(check_nan("helio_dist:null:rate:nan", rate)) n++;
  if(check_nan("helio_dist:cat_object:rate", novas_helio_dist(JD_J2000, &star, &rate))) n++;
  if(check_nan("helio_dist:cat_object:rate:nan", rate)) n++;
  if(check_nan("helio_dist:source:invalid", novas_helio_dist(JD_J2000, &jupiter, &rate))) n++;

  return n;
}

static int test_frame_lst() {
  int n = 0;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;

  if(check_nan("frame_lst:frame:null", novas_frame_lst(NULL))) n++;
  if(check_nan("frame_lst:frame:init", novas_frame_lst(&frame))) n++;

  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_at_geocenter(&obs);
  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame);

  if(check_nan("frame_lst:obs:invalid", novas_frame_lst(&frame))) n++;

  return n;
}

static int test_rise_set() {
  int n = 0;
  object sun = NOVAS_SUN_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;

  if(check_nan("rise_set:rises_above:frame:null", novas_rises_above(0.0, &sun, NULL, NULL))) n++;
  if(check_nan("rise_set:rises_above:frame:init", novas_rises_above(0.0, &sun, &frame, NULL))) n++;
  if(check_nan("rise_set:sets_below:frame:null", novas_sets_below(0.0, &sun, NULL, NULL))) n++;
  if(check_nan("rise_set:sets_below:frame:init", novas_sets_below(0.0, &sun, &frame, NULL))) n++;
  if(check_nan("rise_set:transit_time:frame:null", novas_transit_time(&sun, NULL))) n++;
  if(check_nan("rise_set:transit_time:frame:init", novas_transit_time(&sun, &frame))) n++;

  // noon (near transit)
  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, &obs);
  if(check("rise_set:make_frame", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  if(check_nan("rise_set:rises_above:source:null", novas_rises_above(0.0, NULL, &frame, NULL))) n++;
  if(check_nan("rise_set:sets_below:source:null", novas_sets_below(0.0, NULL, &frame, NULL))) n++;
  if(check_nan("rise_set:transit_time:source:null", novas_transit_time(NULL, &frame))) n++;

  make_observer_on_surface(60.0, 0.0, 0.0, 0.0, 0.0, &obs);
  novas_change_observer(&frame, &obs, &frame);
  if(check_nan("rise_set:rises_above:source:null", novas_rises_above(31.0, &sun, &frame, NULL))) n++;
  if(check_nan("rise_set:sets_below:source:null", novas_sets_below(31.0, &sun, &frame, NULL))) n++;

  novas_set_max_iter(0);
  if(check_nan("rise_set:rises_above:noconv", novas_rises_above(0.0, &sun, &frame, NULL))) n++;
  if(check_nan("rise_set:sets_below:noconv", novas_rises_above(0.0, &sun, &frame, NULL))) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);

  return n;
}

static int test_tracks() {
  int n = 0;
  object sun = NOVAS_SUN_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  novas_track track = NOVAS_TRACK_INIT;
  double vel[3] = {0.0}, x;

  if(check("equ_track:frame:null", -1, novas_equ_track(&sun, NULL, 1000.0, &track))) n++;
  if(check("equ_track:frame:init", -1, novas_equ_track(&sun, &frame, 1000.0, &track))) n++;
  if(check("hor_track:frame:null", -1, novas_hor_track(&sun, NULL, NULL, &track))) n++;
  if(check("hor_track:frame:init", -1, novas_hor_track(&sun, &frame, NULL, &track))) n++;

  // noon (near transit)
  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, &obs);
  if(check("rise_set:make_frame", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  if(check("equ_track:source:null", -1, novas_equ_track(NULL, &frame, 1000.0, &track))) n++;
  if(check("equ_track:track:null", -1, novas_equ_track(&sun, &frame, 1000.0, NULL))) n++;
  if(check("hor_track:source:null", -1, novas_hor_track(NULL, &frame, NULL, &track))) n++;
  if(check("hor_track:track:null", -1, novas_hor_track(&sun, &frame, NULL, NULL))) n++;

  if(check("hor_track:make_airborne_observer", 0, make_airborne_observer(&obs.on_surf, vel, &obs))) n++;
  if(check("rise_set:make_frame:airborne", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(check("hor_track:track:null", 0, novas_hor_track(&sun, &frame, NULL, &track))) n++;

  make_observer_at_geocenter(&obs);
  if(check("rise_set:make_frame:geocenter", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(check("hor_track:track:null", -1, novas_hor_track(&sun, &frame, NULL, &track))) n++;

  if(check("track_pos:track:null", -1, novas_track_pos(NULL, &time, &x, NULL, NULL, NULL))) n++;
  if(check("track_pos:time:null", -1, novas_track_pos(&track, NULL, &x, NULL, NULL, NULL))) n++;

  return n;
}

static int test_solar_illum() {
  int n = 0;
  object earth = NOVAS_EARTH_INIT;
  object jupiter = NOVAS_JUPITER_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double pos[3] = {1.0, 1.0, 0.0}, vel[3] = {0.0};

  make_solar_system_observer(pos, vel, &obs);
  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);

  if(check_nan("solar_illum:frame:null", novas_solar_illum(&earth, NULL))) n++;
  if(check_nan("solar_illum:frame:init", novas_solar_illum(&earth, &frame))) n++;

  if(check("solar_illum:make_frame", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(check_nan("solar_illum:source:null", novas_solar_illum(NULL, &frame))) n++;
  if(check_nan("solar_illum:source:invalid", novas_solar_illum(&jupiter, &frame))) n++;

  return n;
}

static int test_object_sep() {
  int n = 0;
  object sun = NOVAS_SUN_INIT;
  object earth = NOVAS_EARTH_INIT;
  object jupiter = NOVAS_JUPITER_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);

  if(check_nan("object_sep:frame:null", novas_object_sep(&sun, &sun, NULL))) n++;
  if(check_nan("object_sep:frame:init", novas_object_sep(&sun, &sun, &frame))) n++;

  if(check("object_sep:make_frame", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  if(check_nan("object_sep:a:null", novas_object_sep(NULL, &sun, &frame))) n++;
  if(check_nan("object_sep:b:null", novas_object_sep(&sun, NULL, &frame))) n++;

  if(check_nan("object_sep:a:invalid", novas_object_sep(&jupiter, &sun, &frame))) n++;
  if(check_nan("object_sep:b:invalid", novas_object_sep(&sun, &jupiter, &frame))) n++;

  if(check_nan("object_sep:a=obs", novas_object_sep(&earth, &sun, &frame))) n++;
  if(check_nan("object_sep:b=obs", novas_object_sep(&sun, &earth, &frame))) n++;
  if(check_nan("object_sep:a=b=obs", novas_object_sep(&earth, &earth, &frame))) n++;

  if(check_nan("object_sep:sun_angle", novas_sun_angle(&earth, &frame))) n++;
  if(check_nan("object_sep:sun_angle", novas_moon_angle(&earth, &frame))) n++;

  return n;
}

static int test_uvw_to_xyz() {
  int n = 0;
  double v[3] = {0.0};

  if(check("uvw_to_xyz:uvw:null", -1, novas_uvw_to_xyz(NULL, 0.0, 0.0, v))) n++;
  if(check("xyz_to_uvw:xyz:null", -1, novas_uvw_to_xyz(v, 0.0, 0.0, NULL))) n++;

  return n;
}

static int test_xyz_to_uvw() {
  int n = 0;
  double v[3] = {0.0};

  if(check("xyz_to_uvw:xyz:null", -1, novas_xyz_to_uvw(NULL, 0.0, 0.0, v))) n++;
  if(check("xyz_to_uvw:uvw:null", -1, novas_xyz_to_uvw(v, 0.0, 0.0, NULL))) n++;

  return n;
}

static int test_julian_date() {
  int n = 0;

  if(check_nan("julian_date:m:0", julian_date(2000.0, 0, 1, 0.0))) n++;
  if(check_nan("julian_date:m:13", julian_date(2000.0, 13, 1, 0.0))) n++;
  if(check_nan("julian_date:d:0", julian_date(2000.0, 2, 0, 0.0))) n++;
  if(check_nan("julian_date:feb:d:30", julian_date(2000.0, 2, 30, 0.0))) n++;

  return n;
}

static int test_jd_from_date() {
  int n = 0;

  if(check("jd_from_date:calendar:-2", -1, novas_jd_from_date(-2, 2000, 1, 1, 0.0))) n++;
  if(check("jd_from_date:calendar:2", -1, novas_jd_from_date(2, 2000, 1, 1, 0.0))) n++;

  return n;
}

static int test_jd_to_date() {
  int n = 0;
  int y, m, d;
  double h;

  if(check("jd_to_date:calendar:-2", -1, novas_jd_to_date(NOVAS_JD_J2000, -2, &y, &m, &d, &h))) n++;
  if(check("jd_to_date:calendar:2", -1, novas_jd_to_date(NOVAS_JD_J2000, 2, &y, &m, &d, &h))) n++;

  return n;
}

static int test_parse_date() {
  int n = 0;
  char *tail = NULL;

  if(check_nan("parse_date:null", novas_parse_date(NULL, &tail))) n++;
  if(check_nan("parse_date:empty", novas_parse_date("", &tail))) n++;

  if(check_nan("parse_date_time:format:-1", novas_parse_date_format(0, -1, "2025-01-28", &tail))) n++;
  if(check_nan("parse_date_time:format:3", novas_parse_date_format(0, 3, "2025-01-28", &tail))) n++;

  if(check_nan("parse_date_time:few", novas_parse_date_format(0, NOVAS_YMD, "2025-01", &tail))) n++;
  if(check_nan("parse_date_time:sep:invalid", novas_parse_date_format(0, 3, "2025$01$28", &tail))) n++;

  if(check_nan("parse_date_time:m:0", novas_parse_date_format(0, NOVAS_YMD, "2025-00-28", &tail))) n++;
  if(check_nan("parse_date_time:m:13", novas_parse_date_format(0, NOVAS_YMD, "2025-13-28", &tail))) n++;
  if(check_nan("parse_date_time:d:0", novas_parse_date_format(0, NOVAS_YMD, "2025-2-0", &tail))) n++;
  if(check_nan("parse_date_time:feb:d:30", novas_parse_date_format(0, NOVAS_YMD, "2025-2-30", &tail))) n++;

  if(check_nan("parse_date_time:m:invalid", novas_parse_date_format(0, NOVAS_YMD, "2025-blah-28", &tail))) n++;

  if(check_nan("parse_date_time:zone:missing", novas_parse_date_format(0, NOVAS_YMD, "2025-01-28 12:50:00+", &tail))) n++;
  if(check_nan("parse_date_time:zone:bad", novas_parse_date_format(0, NOVAS_YMD, "2025-01-28 12:50:00+:", &tail))) n++;
  if(check_nan("parse_date_time:zone:bad2", novas_parse_date_format(0, NOVAS_YMD, "2025-01-28 12:50:00+0:", &tail))) n++;
  if(check_nan("parse_date_time:zone:hours:24", novas_parse_date_format(0, NOVAS_YMD, "2025-01-28 12:50:00+2400", &tail))) n++;
  if(check_nan("parse_date_time:zone:mins:60", novas_parse_date_format(0, NOVAS_YMD, "2025-01-28 12:50:00+0260", &tail))) n++;
  if(check_nan("parse_date_time:zone:mins:incomplete", novas_parse_date_format(0, NOVAS_YMD, "2025-01-28 12:50:00+020", &tail))) n++;

  return n;
}

static int test_parse_iso_date() {
  int n = 0;
  char *tail = NULL;

  if(check_nan("parse_iso_date:null", novas_parse_iso_date(NULL, &tail))) n++;
  if(check_nan("parse_iso_date:empty", novas_parse_iso_date("", &tail))) n++;

  return n;
}

static int test_date() {
  int n = 0;

  if(check_nan("date:null", novas_date(NULL))) n++;

  return n;
}

static int test_date_scale() {
  int n = 0;
  enum novas_timescale scale;

  if(check_nan("date_scale:date:null", novas_date_scale(NULL, &scale))) n++;
  if(check("date_scale:date:null:scale", -1, scale)) n++;
  if(check_nan("date_scale:scale:null", novas_date_scale("2025-03-01T11:08:38.338+0200", NULL))) n++;

  return n;
}

static int test_iso_timestamp() {
  int n = 0;
  char buf[30] = {'\0'};
  novas_timespec time = NOVAS_TIMESPEC_INIT;

  if(check("iso_timestamp:time:null", -1, novas_iso_timestamp(NULL, buf, sizeof(buf)))) n++;
  if(check("iso_timestamp:buf:null", -1, novas_iso_timestamp(&time, NULL, sizeof(buf)))) n++;
  if(check("iso_timestamp:len:0", -1, novas_iso_timestamp(&time, buf, 0))) n++;

  return n;
}

static int test_timestamp() {
  int n = 0;
  char buf[30] = {'\0'};
  novas_timespec time = NOVAS_TIMESPEC_INIT;

  if(check("timestamp:time:null", -1, novas_timestamp(NULL, NOVAS_UTC, buf, sizeof(buf)))) n++;
  if(check("timestamp:time:scale:-1", -1, novas_timestamp(NULL, -1, buf, sizeof(buf)))) n++;
  if(check("timestamp:time:scale:hi", -1, novas_timestamp(NULL, NOVAS_TIMESCALES, buf, sizeof(buf)))) n++;
  if(check("timestamp:buf:null", -1, novas_timestamp(&time, NOVAS_UTC, NULL, sizeof(buf)))) n++;
  if(check("timestamp:len:0", -1, novas_timestamp(&time, NOVAS_UTC, buf, 0))) n++;

  return n;
}

static int test_timescale_for_string() {
  int n = 0;

  if(check("timescale_for_string:null", -1, novas_timescale_for_string(NULL))) n++;
  if(check("timescale_for_string:empty", -1, novas_timescale_for_string(""))) n++;
  if(check("timescale_for_string:invalid", -1, novas_timescale_for_string("blah"))) n++;

  return n;
}

static int test_parse_timescale() {
  int n = 0;
  char *tail = NULL;

  if(check("parse_timescale:null", -1, novas_parse_timescale(NULL, &tail))) n++;
  if(check("parse_timescale:invalid", -1, novas_parse_timescale("blah", &tail))) n++;

  return n;
}

static int test_print_timescale() {
  int n = 0;
  char buf[4] = {'\0'};

  if(check("print_timescale:buf:null", -1, novas_print_timescale(NOVAS_UTC, NULL))) n++;
  if(check("print_timescale:buf:-1", -1, novas_print_timescale(-1, buf))) n++;
  if(check("print_timescale:buf:hi", -1, novas_print_timescale(NOVAS_TIMESCALES, buf))) n++;

  return n;
}

static int test_epoch() {
  int n = 0;

  if(check_nan("epoch:null", novas_epoch(NULL))) n++;
  if(check_nan("epoch:empty", novas_epoch(""))) n++;
  if(check_nan("epoch:blah", novas_epoch("blah"))) n++;

  return n;
}

static int test_make_cat_object_sys() {
  int n = 0;
  cat_entry star = CAT_ENTRY_INIT;
  object source = NOVAS_OBJECT_INIT;

  if(check("make_cat_object_sys:star:null", -1, make_cat_object_sys(NULL, "ICRS", &source))) n++;
  if(check("make_cat_object_sys:sys:null", -1, make_cat_object_sys(&star, NULL, &source))) n++;
  if(check("make_cat_object_sys:source:null", -1, make_cat_object_sys(&star, "ICRS", NULL))) n++;
  if(check("make_cat_object_sys:sys:bad", -1, make_cat_object_sys(&star, "blah", &source))) n++;

  return n;
}

static int test_make_redshifted_object_sys() {
  int n = 0;
  object source = NOVAS_OBJECT_INIT;

  if(check("make_redshifted_object_sys:sys:null", -1, make_redshifted_object_sys("test", 0.0, 0.0, NULL, 0.0, &source))) n++;
  if(check("make_redshifted_object_sys:source:null", -1, make_redshifted_object_sys("test", 0.0, 0.0, "ICRS", 0.0, NULL))) n++;
  if(check("make_redshifted_object_sys:sys:bad", -1, make_redshifted_object_sys("test", 0.0, 0.0, "blah", 0.0, &source))) n++;

  return n;
}

static int test_planet_ephem_provider() {
  int n = 0;
  double pos[3] = {0.0}, vel[3] = {0.0};

  if(check("planet_ephem_provider:none", 1, planet_ephem_provider(NOVAS_JD_J2000, NOVAS_SUN, NOVAS_BARYCENTER, pos, vel))) n++;
  set_ephem_provider(dummy_ephem);

  if(check("planet_ephem_provider:set", 0, planet_ephem_provider(NOVAS_JD_J2000, NOVAS_SUN, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("planet_ephem_provider:planet:-1", -1, planet_ephem_provider(NOVAS_JD_J2000, -1, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("planet_ephem_provider:planet:hi", -1, planet_ephem_provider(NOVAS_JD_J2000, NOVAS_PLANETS, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("planet_ephem_provider:origin:-1", 1, planet_ephem_provider(NOVAS_JD_J2000, NOVAS_SUN, -1, pos, vel))) n++;
  if(check("planet_ephem_provider:origin:2", 1, planet_ephem_provider(NOVAS_JD_J2000, NOVAS_SUN, 2, pos, vel))) n++;

  return n;
}

static int test_print_hms() {
  int n = 0;
  char buf[20] = {'\0'};

  if(check("print_hms:buf:null", -1, novas_print_hms(12.0, NOVAS_SEP_COLONS, 3, NULL, sizeof(buf)))) n++;
  if(check("print_hms:len:0", -1, novas_print_hms(12.0, NOVAS_SEP_COLONS, 3, buf, 0))) n++;

  return n;
}

static int test_print_dms() {
  int n = 0;
  char buf[20] = {'\0'};

  if(check("print_dms:buf:null", -1, novas_print_dms(90.0, NOVAS_SEP_COLONS, 3, NULL, sizeof(buf)))) n++;
  if(check("print_dms:len:0", -1, novas_print_dms(90.0, NOVAS_SEP_COLONS, 3, buf, 0))) n++;

  return n;
}

static int test_time_lst() {
  int n = 0;
  novas_timespec t = NOVAS_TIMESPEC_INIT;

  if(check_nan("time_lst:time:null", novas_time_lst(NULL, 0.0, NOVAS_FULL_ACCURACY))) n++;
  if(check_nan("time_lst:acc:-1", novas_time_lst(&t, 0.0, -1))) n++;

  return n;
}

static int test_novas_make_planet_orbit() {
  int n = 0;

  novas_orbital orbit = NOVAS_ORBIT_INIT;

  if(check("novas_make_planet_orbit:sun", -1, novas_make_planet_orbit(NOVAS_SUN, NOVAS_JD_J2000, &orbit))) n++;
  if(check("novas_make_planet_orbit:ssb", -1, novas_make_planet_orbit(NOVAS_SSB, NOVAS_JD_J2000, &orbit))) n++;
  if(check("novas_make_planet_orbit:jd:lo", -1, novas_make_planet_orbit(NOVAS_EARTH, 0.0, &orbit))) n++;
  if(check("novas_make_planet_orbit:jd:hi", -1, novas_make_planet_orbit(NOVAS_EARTH, 2 * NOVAS_JD_J2000, &orbit))) n++;
  if(check("novas_make_planet_orbit:orbit:null", -1, novas_make_planet_orbit(NOVAS_EARTH, NOVAS_JD_J2000, NULL))) n++;

  return n;
}

static int test_novas_make_moon_orbit() {
  if(check("novas_make_moon_orbit:orbit:null", -1, novas_make_moon_orbit(NOVAS_JD_J2000, NULL))) return 1;
  return 0;
}

static int test_approx_heliocentric() {
  double p[3] = {0.0}, v[3] = {0.0};

  if(check("approx_heliocentric:pos=vel", -1, novas_approx_heliocentric(NOVAS_EMB, NOVAS_JD_J2000, NULL, NULL))) return 1;
  if(check("approx_heliocentric:time:lo", -1, novas_approx_heliocentric(NOVAS_EMB, NOVAS_JD_J2000 - 31.0 * JULIAN_CENTURY_DAYS, p, v))) return 1;
  if(check("approx_heliocentric:time:hi", -1, novas_approx_heliocentric(NOVAS_EMB, NOVAS_JD_J2000 + 31.0 * JULIAN_CENTURY_DAYS, p, v))) return 1;

  return 0;
}

static int test_approx_sky_pos() {
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  sky_pos out = SKY_POS_INIT;
  int n = 0;

  if(check("approx_sky_pos:frame:null", -1, novas_approx_sky_pos(NOVAS_EMB, NULL, NOVAS_ICRS, &out))) n++;
  if(check("approx_sky_pos:frame:init", -1, novas_approx_sky_pos(NOVAS_EMB, &frame, NOVAS_ICRS, &out))) n++;

  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);
  make_observer_at_geocenter(&obs);
  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);

  if(check("approx_sky_pos:out:null", -1, novas_approx_sky_pos(NOVAS_EMB, &frame, NOVAS_ICRS, NULL))) n++;

  return n;
}

static int test_moon_phase() {
  int n = 0;

  novas_set_max_iter(0);
  if(check_nan("moon_phase:conv", novas_moon_phase(NOVAS_JD_J2000))) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);
  return n;
}

static int test_next_moon_phase() {
  int n = 0;

  if(check_nan("next_moon_phase:time:lo", novas_next_moon_phase(0.0, NOVAS_JD_J2000 - 31.0 * JULIAN_CENTURY_DAYS))) n++;
  if(check_nan("next_moon_phase:time:hi", novas_next_moon_phase(0.0, NOVAS_JD_J2000 + 31.0 * JULIAN_CENTURY_DAYS))) n++;

  novas_set_max_iter(0);
  if(check_nan("next_moon_phase:conv", novas_next_moon_phase(0.0, NOVAS_JD_J2000))) n++;
  novas_set_max_iter(NOVAS_DEFAULT_MAX_ITER);
  return n;
}

static int test_libration() {
  int n = 0;
  double xp, yp, dut;

  if(check("libration:args", -1, novas_diurnal_libration(0.0, NULL, &xp, &yp, &dut))) n++;

  return n;
}

static int test_ocean_tides() {
  int n = 0;
  double xp, yp, dut;

  if(check("ocean_tides:args", -1, novas_diurnal_ocean_tides(0.0, NULL, &xp, &yp, &dut))) n++;

  return n;
}

static int test_diurnal_eop() {
  int n = 0;
  double xp, yp, dut;

  if(check("diurnal_eop:args", -1, novas_diurnal_eop(0.0, NULL, &xp, &yp, &dut))) n++;

  return n;
}

static int test_diurnal_eop_at_time() {
  int n = 0;
  double xp, yp, dut;

  if(check("diurnal_eop_at_time:time", -1, novas_diurnal_eop_at_time(NULL, &xp, &yp, &dut))) n++;

  return n;
}

static int test_cartesian_to_geodetic() {
  int n = 0;
  double x[3] = {0.0};
  double lon, lat, alt;

  if(check("cartesian_to_geodetic:x", -1, novas_cartesian_to_geodetic(NULL, NOVAS_GRS80_ELLIPSOID, &lon, &lat, &alt))) n++;
  if(check("cartesian_to_geodetic:ellipsoid", -1, novas_cartesian_to_geodetic(x, -1, &lon, &lat, &alt))) n++;

  return n;
}

static int test_geodetic_to_cartesian() {
  int n = 0;
  double x[3] = {0.0};

  if(check("geodetic_to_cartesian:x", -1, novas_geodetic_to_cartesian(0.0, 0.0, 0.0, NOVAS_GRS80_ELLIPSOID, NULL))) n++;
  if(check("geodetic_to_cartesian:ellipsoid", -1, novas_geodetic_to_cartesian(0.0, 0.0, 0.0, -1, x))) n++;

  return n;
}

static int test_itrf_transform() {
  int n = 0;
  double from_coords[3] = {0.0}, from_rates[3] = {0.0}, to_coords[3] = {0.0}, to_rates[3] = {0.0};

  if(check("itrf_transform:from_coords", -1, novas_itrf_transform(2000, NULL, from_rates, 2014, to_coords, to_rates))) n++;
  if(check("itrf_transform:rates", -1, novas_itrf_transform(2000, from_coords, NULL, 2014, to_coords, to_rates))) n++;

  return n;
}

static int test_clock_skew() {
  int n = 0;
  observer obs = {};
  novas_timespec time = {};
  novas_frame frame = {};

  if(check_nan("clock_skew:frame", novas_mean_clock_skew(NULL, NOVAS_TCG))) n++;
  if(check_nan("clock_skew:frame:init", novas_mean_clock_skew(&frame, NOVAS_TCG))) n++;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32.0, 0.0, &time);

  enable_earth_sun_hp(1);
  novas_make_frame(NOVAS_FULL_ACCURACY, &obs, &time, 0.0, 0.0, &frame);

  if(check_nan("clock_skew:timescale:-1", novas_mean_clock_skew(&frame, -1))) n++;
  if(check_nan("clock_skew:timescale:UT1", novas_mean_clock_skew(&frame, NOVAS_UT1))) n++;

  if(check_nan("clock_skew:hp:no_planets", novas_mean_clock_skew(&frame, NOVAS_TCG))) n++;
  enable_earth_sun_hp(0);

  return n;
}

static int test_cat_entry() {
  int n = 0;
  char *name = "blah", longname[SIZE_OF_OBJ_NAME + 2] = {};
  cat_entry star = {};

  if(check("cat_entry:init", -1, novas_init_cat_entry(NULL, name, 0.0, 0.0))) n++;

  memset(longname, 'x', SIZE_OF_OBJ_NAME);
  if(check("cat_entry:init:long", -1, novas_init_cat_entry(NULL, longname, 0.0, 0.0))) n++;

  longname[SIZE_OF_OBJ_NAME + 1] = 'x';
  if(check("cat_entry:init:verylong", -1, novas_init_cat_entry(NULL, longname, 0.0, 0.0))) n++;

  if(check("cat_entry:set_catalog", -1, novas_set_catalog(NULL, name, 0))) n++;
  if(check("cat_entry:set_proper_motion", -1, novas_set_proper_motion(NULL, 0.0, 0.0))) n++;
  if(check("cat_entry:set_parallax", -1, novas_set_parallax(NULL, 0.0))) n++;
  if(check("cat_entry:set_distance", -1, novas_set_distance(NULL, 0.0))) n++;
  if(check("cat_entry:set_ssb_vel", -1, novas_set_ssb_vel(NULL, 0.0))) n++;
  if(check("cat_entry:set_ssb_vel:hi", -1, novas_set_ssb_vel(&star, NOVAS_C + 1.0))) n++;
  if(check("cat_entry:set_ssb_vel:lo", -1, novas_set_ssb_vel(&star, -(NOVAS_C + 1.0)))) n++;
  if(check("cat_entry:set_lsr_vel", -1, novas_set_lsr_vel(NULL, 2000.0, 0.0))) n++;
  if(check("cat_entry:set_redshift", -1, novas_set_redshift(NULL, 0.0))) n++;

  return n;
}

static int test_make_itrf_site() {
  int n = 0;
  on_surface site;

  if(check("make_itrf_site:site", -1, make_itrf_site(0.0, 0.0, 0.0, NULL))) n++;
  if(check("make_itrf_site:lat:-91", -1, make_itrf_site(-91.0, 0.0, 0.0, &site))) n++;
  if(check("make_itrf_site:lat:+91", -1, make_itrf_site(91.0, 0.0, 0.0, &site))) n++;

  return n;
}

static int test_make_xyz_site() {
  int n = 0;
  on_surface site;
  double xyz[3] = {0.0};

  if(check("make_xyz_site:xyz", -1, make_xyz_site(NULL, &site))) n++;
  if(check("make_xyz_site:site", -1, make_xyz_site(xyz, NULL))) n++;

  return n;
}

static int test_make_observer_at_site() {
  int n = 0;
  observer obs;
  on_surface site;

  if(check("make_observer_at_site:site", -1, make_observer_at_site(NULL, &obs))) n++;
  if(check("make_observer_at_site:obs", -1, make_observer_at_site(&site, NULL))) n++;

  return n;
}

static int test_set_default_weather() {
  if(check("set_default_weather", -1, novas_set_default_weather(NULL))) return 1;
  return 0;
}

static int test_itrf_transform_site() {
  int n = 0;
  on_surface site = {};

  if(check("itrf_transform_site:in", -1, novas_itrf_transform_site(2003, NULL, 2014, &site))) n++;
  if(check("itrf_transform_site:out", -1, novas_itrf_transform_site(2003, &site, 2014, NULL))) n++;

  return n;
}

static int test_transform_geodetic() {
  int n = 0;
  on_surface site = {};

  if(check("transform_geodetic:in", -1, novas_geodetic_transform_site(NOVAS_WGS84_ELLIPSOID, NULL, NOVAS_GRS80_ELLIPSOID, &site))) n++;
  if(check("transform_geodetic:out", -1, novas_geodetic_transform_site(NOVAS_WGS84_ELLIPSOID, &site, NOVAS_GRS80_ELLIPSOID, NULL))) n++;

  return n;
}

static int test_rot() {
  int n = 0;

  if(check("rot:Rx", -1, novas_Rx(0.0, NULL))) n++;
  if(check("rot:Ry", -1, novas_Ry(0.0, NULL))) n++;
  if(check("rot:Rz", -1, novas_Rz(0.0, NULL))) n++;

  return n;
}

static int test_enu_to_itrs() {
  int n = 0;

  double v[3] = {0.0};

  if(check("enu_to_itrs:enu", -1, novas_enu_to_itrs(NULL, 0.0, 0.0, v))) n++;
  if(check("enu_to_itrs:itrf", -1, novas_enu_to_itrs(v, 0.0, 0.0, NULL))) n++;

  return n;
}

static int test_itrs_to_enu() {
  int n = 0;

  double v[3] = {0.0};

  if(check("itrs_to_enu:itrs", -1, novas_itrs_to_enu(NULL, 0.0, 0.0, v))) n++;
  if(check("itrs_to_enu:enu", -1, novas_itrs_to_enu(v, 0.0, 0.0, NULL))) n++;

  return n;
}

int main(int argc, const char *argv[]) {
  int n = 0;

  if(argc > 1)
    dataPath = (char *) argv[1];

  if(test_v2z()) n++;
  if(test_z2v()) n++;

  if(test_make_on_surface()) n++;
  if(test_make_in_space()) n++;
  if(test_make_observer()) n++;
  if(test_make_airborne_observer()) n++;

  if(test_make_object()) n++;
  if(test_make_cat_object()) n++;
  if(test_make_redshifted_object()) n++;
  if(test_make_ephem_object()) n++;
  if(test_make_planet()) n++;
  if(test_make_cat_entry()) n++;
  if(test_transform_cat()) n++;
  if(test_transform_hip()) n++;

  if(test_refract()) n++;
  if(test_refract_astro()) n++;
  if(test_inv_refract()) n++;
  if(test_radio_refraction()) n++;
  if(test_wave_refraction()) n++;
  if(test_refract_wavelength()) n++;
  if(test_limb_angle()) n++;

  if(test_ephemeris()) n++;

  if(test_j2000_to_tod()) n++;
  if(test_tod_to_j2000()) n++;
  if(test_gcrs_to_cirs()) n++;
  if(test_cirs_to_gcrs()) n++;
  if(test_cirs_to_tod()) n++;
  if(test_tod_to_cirs()) n++;
  if(test_cirs_to_app_ra()) n++;
  if(test_app_to_cirs_ra()) n++;

  if(test_set_planet_provider()) n++;
  if(test_set_planet_provider_hp()) n++;

  if(test_place()) n++;
  if(test_place_star()) n++;
  if(test_radec_planet()) n++;
  if(test_mean_star()) n++;

  if(test_equ2gal()) n++;
  if(test_gal2equ()) n++;

  if(test_equ2ecl_vec()) n++;
  if(test_ecl2equ_vec()) n++;
  if(test_equ2ecl()) n++;
  if(test_ecl2equ()) n++;

  if(test_itrs_to_hor()) n++;
  if(test_hor_to_itrs()) n++;

  if(test_equ2hor()) n++;
  if(test_gcrs2equ()) n++;

  if(test_sidereal_time()) n++;
  if(test_ter2cel()) n++;
  if(test_cel2ter()) n++;

  if(test_spin()) n++;
  if(test_wobble()) n++;
  if(test_terra()) n++;
  if(test_e_tilt()) n++;
  if(test_cel_pole()) n++;
  if(test_frame_tie()) n++;

  if(test_proper_motion()) n++;
  if(test_bary2obs()) n++;
  if(test_geo_posvel()) n++;

  if(test_light_time2()) n++;
  if(test_d_light()) n++;

  if(test_cio_array()) n++;
  if(test_cio_basis()) n++;
  if(test_cio_location()) n++;
  if(test_cio_ra()) n++;

  if(test_starvectors()) n++;
  if(test_radec2vector()) n++;
  if(test_vector2radec()) n++;

  if(test_planet_lon()) n++;
  if(test_fund_args()) n++;
  if(test_nutation_angles()) n++;
  if(test_set_nutation_lp_provider()) n++;
  if(test_nutation()) n++;
  if(test_precession()) n++;
  if(test_rad_vel()) n++;
  if(test_aberration()) n++;
  if(test_grav_vec()) n++;
  if(test_grav_def()) n++;
  if(test_grav_undef()) n++;
  if(test_grav_init_planets()) n++;
  if(test_grav_planets()) n++;
  if(test_grav_undo_planets()) n++;

  if(test_earth_sun_calc()) n++;
  if(test_earth_sun_calc_hp()) n++;
  if(test_sun_eph()) n++;

  if(test_obs_posvel()) n++;
  if(test_time()) n++;
  if(test_refraction()) n++;

  if(test_make_frame()) n++;
  if(test_change_observer()) n++;
  if(test_make_transform()) n++;
  if(test_geom_posvel()) n++;
  if(test_geom_to_app()) n++;
  if(test_app_to_geom()) n++;
  if(test_app_to_hor()) n++;
  if(test_hor_to_app()) n++;
  if(test_sky_pos()) n++;
  if(test_transform_vector()) n++;
  if(test_transform_sky_pos()) n++;
  if(test_inv_transform()) n++;

  if(test_redshift_vrad()) n++;
  if(test_unredshift_vrad()) n++;
  if(test_z_add()) n++;
  if(test_z_inv()) n++;

  if(test_novas_to_naif_planet()) n++;
  if(test_novas_to_dexxx_planet()) n++;
  if(test_naif_to_novas_planet()) n++;

  if(test_planet_for_name()) n++;
  if(test_make_orbital_object()) n++;
  if(test_set_obsys_pole()) n++;
  if(test_orbit_posvel()) n++;

  if(test_gcrs_to_tod()) n++;
  if(test_tod_to_gcrs()) n++;
  if(test_gcrs_to_mod()) n++;
  if(test_mod_to_gcrs()) n++;

  if(test_hms_hours()) n++;
  if(test_dms_degrees()) n++;
  if(test_parse_hours()) n++;
  if(test_parse_degrees()) n++;
  if(test_str_hours()) n++;
  if(test_str_degrees()) n++;

  if(test_helio_dist()) n++;

  if(test_frame_lst()) n++;
  if(test_rise_set()) n++;
  if(test_tracks()) n++;
  if(test_solar_illum()) n++;
  if(test_object_sep()) n++;
  if(test_uvw_to_xyz()) n++;
  if(test_xyz_to_uvw()) n++;
  if(test_julian_date()) n++;
  if(test_jd_from_date()) n++;
  if(test_jd_to_date()) n++;
  if(test_parse_date()) n++;
  if(test_parse_iso_date()) n++;
  if(test_date()) n++;
  if(test_date_scale()) n++;
  if(test_iso_timestamp()) n++;
  if(test_timestamp()) n++;
  if(test_timescale_for_string()) n++;
  if(test_parse_timescale()) n++;
  if(test_print_timescale()) n++;

  if(test_epoch()) n++;
  if(test_make_cat_object_sys()) n++;
  if(test_make_redshifted_object_sys()) n++;

  if(test_planet_ephem_provider()) n++;

  if(test_print_hms()) n++;
  if(test_print_dms()) n++;

  if(test_time_lst()) n++;

  if(test_novas_make_planet_orbit()) n++;
  if(test_novas_make_moon_orbit()) n++;
  if(test_approx_heliocentric()) n++;
  if(test_approx_sky_pos()) n++;
  if(test_moon_phase()) n++;
  if(test_next_moon_phase()) n++;

  if(test_libration()) n++;
  if(test_ocean_tides()) n++;
  if(test_diurnal_eop()) n++;
  if(test_diurnal_eop_at_time()) n++;

  if(test_cartesian_to_geodetic()) n++;
  if(test_geodetic_to_cartesian()) n++;
  if(test_itrf_transform()) n++;

  if(test_clock_skew()) n++;
  if(test_cat_entry()) n++;
  if(test_set_str_time()) n++;
  if(test_make_itrf_site()) n++;
  if(test_make_xyz_site()) n++;
  if(test_make_observer_at_site()) n++;
  if(test_set_default_weather()) n++;
  if(test_itrf_transform_site()) n++;
  if(test_transform_geodetic()) n++;

  if(test_rot()) n++;

  if(test_enu_to_itrs()) n++;
  if(test_itrs_to_enu()) n++;

  if(n) fprintf(stderr, " -- FAILED %d tests\n", n);
  else fprintf(stderr, " -- OK\n");

  return n;
}
