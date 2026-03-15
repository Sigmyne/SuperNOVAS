/**
 * @date Created  on Jan 30, 2024
 * @author Attila Kovacs
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#include "novas.h"

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

#define DEGREE  (M_PI / 180.0)
#define ARCSEC  (DEGREE / 3600.0)
#define HOURANGLE (M_PI / 12.0)
#define AU      1.495978707e+11
#define J2000   2451545.0

static observer obs;
static object source;
static double tdb = J2000;
static short accuracy;
static double ut12tt = 69.0;

// Initialized quantities.
static double lst, pos0[3], vel0[3], epos[3], evel[3], pobs[3], vobs[3], spos[3], svel[3];

static FILE *fp;
static int idx = -1;

static char *header;


static double vlen(double *pos) {
  return sqrt(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]);
}

static double vdist(const double *v1, const double *v2) {
  double d2 = 0.0;
  int i;
  for(i = 3; --i >= 0;) {
    const double d = v1[i] - v2[i];
    d2 += d * d;
  }
  return sqrt(d2);
}

static void newline() {
  fprintf(fp, "\n%8.1f %-10s S%d O%d A%d: ", (tdb - J2000), source.name, source.type, obs.where, accuracy);
}

static void openfile(const char *name) {
  char filename[100] = {'\0'};

  //if(idx >= 0) sprintf(filename, "data/%02d-%s.out", idx++, name);
  sprintf(filename, "data" PATH_SEP "%s.out", name);

  if(fp) {
    fprintf(fp, "\n");
    fclose(fp);
  }
  fp = fopen(filename, "a");
  if(!fp) {
    perror("opening output file");
    exit(errno);
  }

  if(idx >= 0) newline();
  else if(header) fprintf(fp, "%s", header);
}

static void printvector(double *v) {
  if(!v) fprintf(fp, "null ");
  if(accuracy == 0) fprintf(fp, "%17.11g %17.11g %17.11g ", v[0], v[1], v[2]);
  else fprintf(fp, "%14.7g %14.7g %14.7g ", v[0], v[1], v[2]);
}

static void printvel(double *v) {
  double norm = (AU / 86400.0) / 1000.0;

  // to mm/s or m/s accuracy...
  if(accuracy == 0) fprintf(fp, "%12.6f %12.6f %12.6f ", v[0] * norm, v[1] * norm, v[2] * norm);
  else fprintf(fp, "%9.3f %9.3f %9.3f ", v[0] * norm, v[1] * norm, v[2] * norm);
}

static void printunitvector(double *v) {
  if(!v) fprintf(fp, "null ");
  else {
    double l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if(accuracy == 0) fprintf(fp, "%14.11f %14.11f %14.11f ", v[0] / l, v[1] / l, v[2] / l);
    else fprintf(fp, "%11.7f %11.7f %11.7f ", v[0] / l, v[1] / l, v[2] / l);
  }
}

static int is_ok(int error) {
  if(error) fprintf(fp, "ERROR %d ", error);
  return !error;
}


// ======================================================================================
// Basic functions
// ======================================================================================


static void test_make_cat_entry() {
  cat_entry star;

  openfile("make_cat_entry");
  if(is_ok(make_cat_entry("Test", "TST", 1001, 1.1, -2.2, 3.3, -4.4, 5.5, -6.6, &star))) {
    fprintf(fp, "%s %s %ld %.3f %.3f %.3f %.3f %.3f %.3f ", star.starname, star.catalog, star.starnumber,
            star.ra, star.dec, star.promodec, star.promodec, star.parallax, star.radialvelocity);
  }
}


static void test_transform_cat() {
  cat_entry tr = { };
  int i;

  if(source.type != 2) return;

  for(i=1; i <= 5; i++) {
    // Use Julian dates
    openfile("transform_cat");
    transform_cat(i, J2000, &source.star, J2000 - 10000.0, "TR", &tr);
    fprintf(fp, "%d %s %s %ld %.3f %.3f %.3f %.3f %.3f %.3f ", i, tr.starname, tr.catalog, tr.starnumber,
            tr.ra, tr.dec, tr.promodec, tr.promodec, tr.parallax, tr.radialvelocity);

    // Use epoch years
    openfile("transform_cat");
    transform_cat(i, 2000.0, &source.star, 1950.0, "FK4", &tr);
    fprintf(fp, "%d %s %s %ld %.3f %.3f %.3f %.3f %.3f %.3f ", i, tr.starname, tr.catalog, tr.starnumber,
            tr.ra, tr.dec, tr.promodec, tr.promodec, tr.parallax, tr.radialvelocity);
  }
}

static void test_transform_hip() {
  cat_entry star, tr = { };

  if(!is_ok(make_cat_entry("Test", "TST", 1001, 1.1, -2.2, 3.3, -4.4, 5.5, -6.6, &star))) return;

  openfile("transform_hip");
  transform_hip(&star, &tr);
  fprintf(fp, "%s %s %ld %.3f %.3f %.3f %.3f %.3f %.3f ", tr.starname, tr.catalog, tr.starnumber,
          tr.ra, tr.dec, tr.promodec, tr.promodec, tr.parallax, tr.radialvelocity);
}


static void test_make_object() {
  object object;
  cat_entry star;

  openfile("make_object");

  if(!is_ok(make_object(0, 3, "Earth", NULL, &object))) return;
  fprintf(fp, "%-10s %d %ld ", object.name, object.type, object.number);

  openfile("make_object");
  if(!is_ok(make_object(1, 501, "Io", NULL, &object))) return;
  fprintf(fp, "%-10s %d %ld ", object.name, object.type, object.number);

  openfile("make_object");
  if(!is_ok(make_cat_entry("Test", "TST", 1001, 1.1, -2.2, 3.3, -4.4, 5.5, -6.6, &star))) return;
  if(!is_ok(make_object(2, 1234567890, star.starname, &star, &object))) return;
  fprintf(fp, "%-10s %d %ld ", object.name, object.type, object.number);
}


static void test_make_observer() {
  observer obs, earth, space;
  double pos[3] = {100.0, 200.0, 300.0};
  double vel[3] = {-10.0, -20.0, -30.0};
  on_surface *on = &earth.on_surf;
  in_space *in = &space.near_earth;

  openfile("make_observer");
  make_observer(0, NULL, NULL, &obs);
  fprintf(fp, "G1 %d ", obs.where);

  openfile("make_observer");
  make_observer_at_geocenter(&obs);
  fprintf(fp, "G2 %d ", obs.where);

  openfile("make_observer");
  make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 5.0, &earth);
  fprintf(fp, "E1 %d %8.3f %8.3f %8.3f %5.1f %5.1f ", earth.where, on->latitude, on->longitude, on->height, on->temperature, on->pressure);

  openfile("make_observer");
  make_observer(1, on, NULL, &obs);
  on = &obs.on_surf;
  fprintf(fp, "E2 %d %8.3f %8.3f %8.3f %5.1f %5.1f ", obs.where, on->latitude, on->longitude, on->height, on->temperature, on->pressure);

  openfile("make_observer");
  make_observer_in_space(pos, vel, &space);
  fprintf(fp, "S1 %d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f ", space.where, in->sc_pos[0], in->sc_pos[1], in->sc_pos[2], in->sc_vel[0], in->sc_vel[1], in->sc_vel[2]);

  openfile("make_observer");
  make_observer(2, NULL, in, &obs);
  in = &obs.near_earth;
  fprintf(fp, "S2 %d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f ", obs.where, in->sc_pos[0], in->sc_pos[1], in->sc_pos[2], in->sc_vel[0], in->sc_vel[1], in->sc_vel[2]);
}



static void test_refract() {
  on_surface surf;

  openfile("refract");

  make_on_surface(15.0, 20.0, 1200.0, -10.0, 1010.0, &surf);
  fprintf(fp, accuracy == 0 ? "%12.6f %12.6f " : "%9.3f    %9.3f    ", refract(&surf, 1, 89.0), refract(&surf, 2, 89.0));
}

static void test_mean_star() {
  double ra, dec;

  openfile("mean_star");
  if(is_ok(mean_star(2433282.42345905, 10.0, -40.0, 1, &ra, &dec)))
    fprintf(fp, accuracy == 0 ? "1 %12.9f %12.8f " : "1 %9.6f    %9.6f   ", ra, dec);

  openfile("mean_star");
  if(is_ok(mean_star(2433282.42345905, 19.0, 30.0, 1, &ra, &dec)))
    fprintf(fp, accuracy == 0 ? "1 %12.9f %12.8f " : "1 %9.6f    %9.6f   ", ra, dec);

  openfile("mean_star");
  if(is_ok(mean_star(2433282.42345905, 2.7, 68.3, 1, &ra, &dec)))
    fprintf(fp, accuracy == 0 ? "1 %12.9f %12.8f " : "1 %9.6f    %9.6f   ", ra, dec);
}


static void test_basics() {
  idx = -1;
  test_make_cat_entry();
  test_make_object();
  test_make_observer();
  test_transform_cat();
  test_transform_hip();
  test_refract();
  test_mean_star();
}






// ======================================================================================
// Time specific
// ======================================================================================

static void test_cal_date() {
  short y, m, d;
  double h;

  if(accuracy != 0) return;

  cal_date(tdb, &y, &m, &d, &h);
  openfile("cal_date");
  fprintf(fp, "%5d %02d %02d %10.6f ", y, m, d, h);
  cal_date(tdb + 0.5, &y, &m, &d, &h);
  fprintf(fp, "%10.6f ", h);
}

static void test_julian_date() {
  short y, m, d;
  double h;

  if(accuracy != 0) return;

  openfile("cal_date");
  fprintf(fp, "%12.6f %12.6f %12.6f ", julian_date(2024, 2, 9, 12.954), julian_date(1903, 5, 31, 23.021),
          julian_date(2111, 11, 11, 11.18642));

  cal_date(tdb, &y, &m, &d, &h);

  fprintf(fp, "%5d %02d %02d %10.6f ", y, m, d, h);
}


static void test_tdb2tt() {
  double tt, dt;
  openfile("tbd2tt");
  tdb2tt(tdb, &tt, &dt);
  fprintf(fp, accuracy == 0 ? "%12.6f " : "%9.3f    ", dt);
}

static void test_ephemeris() {
  double pos1[3], vel1[3], tdb2[2] = {tdb};
  object body[2];
  int i, j;

  if(!is_ok(make_object(0, 10, "Sun", NULL, &body[0]))) return;
  if(!is_ok(make_object(0, 3, "Earth", NULL, &body[1]))) return;

  for(i = 0; i < 2; i++) for(j = 0; j < 2; j++) {
    openfile("ephemeris");

    if(is_ok(ephemeris(tdb2, &body[i], j, accuracy, pos1, vel1))) {
      int k;
      for(k = 0; k < 3; k++) vel1[k] *= 1e-3 * (1.4959787069098932e+11 / 86400.0);

      fprintf(fp, "%-10s %d ", body[i].name, j);
      printvector(pos1);
      printvel(vel1);
      fprintf(fp, "\n");
    }
  }
}


static void test_era() {
  openfile("era");
  fprintf(fp, "%12.6f", era(tdb, 0.0));
}

static void test_mean_obliq() {
  openfile("mean_obliq");
  fprintf(fp, "%12.6f", mean_obliq(tdb));
}

static void test_ee_ct() {
  openfile("ee_ct");
  fprintf(fp, "A%d %12.6f", accuracy, ee_ct(tdb, 0.0, accuracy) / ARCSEC);
}

static void test_iau2000a() {
  double dpsi = 0.0, deps = 0.0;

  openfile("iau2000a");
  iau2000a(tdb, 0.0, &dpsi, &deps);
  fprintf(fp, "current %12.6f %12.6f ", dpsi / ARCSEC, deps / ARCSEC);

  openfile("iau2000a");
  iau2000a(tdb + 100000, 0.0, &dpsi, &deps);
  fprintf(fp, "future  %12.6f %12.6f ", dpsi / ARCSEC, deps / ARCSEC);
}

static void test_iau2000b() {
  double dpsi = 0.0, deps = 0.0;

  openfile("iau2000b");
  iau2000b(tdb, 0.0, &dpsi, &deps);
  fprintf(fp, "current %12.6f %12.6f ", dpsi / ARCSEC, deps / ARCSEC);

  openfile("iau2000b");
  iau2000b(tdb + 100000, 0.0, &dpsi, &deps);
  fprintf(fp, "future  %12.6f %12.6f ", dpsi / ARCSEC, deps / ARCSEC);
}

static void test_nu2000k() {
  double dpsi = 0.0, deps = 0.0;
  openfile("nu2000k");
  nu2000k(tdb, 0.0, &dpsi, &deps);
  fprintf(fp, "current %12.6f %12.6f ", dpsi / ARCSEC, deps / ARCSEC);

  openfile("nu2000k");
  nu2000k(tdb + 100000, 0.0, &dpsi, &deps);
  fprintf(fp, "future %12.6f %12.6f ", dpsi / ARCSEC, deps / ARCSEC);
}

static void test_nutation_angles() {
  double t = (tdb - J2000) / 36525.0;
  double dpsi = 0.0, deps = 0.0;

  openfile("nutation_angles");
  nutation_angles(t, accuracy, &dpsi, &deps);
  fprintf(fp, accuracy == 0 ? "current %12.6f %12.6f" : "current %9.3f    %9.3f", dpsi, deps);

  openfile("nutation_angles");
  nutation_angles(t + 1.0, accuracy, &dpsi, &deps);
  fprintf(fp, accuracy == 0 ? "future  %12.6f %12.6f" : "future  %9.3f    %9.3f", dpsi, deps);
}


static void test_e_tilt() {
  double r1=0.0, r2=0.0, r3=0.0, r4=0.0, r5=0.0;
  openfile("e_tilt");
  e_tilt(tdb, accuracy, &r1, &r2, &r3, &r4, &r5);
  fprintf(fp, accuracy == 0 ? "%12.6f %12.6f %12.6f %12.6f %12.6f" :  "%9.3f    %9.3f    %9.3f    %9.3f    %9.3f",
          r1, r2, r3, r4, r5);
}

static void test_cel_pole() {
  double a, b, c, dpsi = 0.0, deps = 0.0;

  openfile("cel_pole");
  cel_pole(tdb, 1, 0.0, 0.0);
  e_tilt(tdb, accuracy, &a, &b, &c, &dpsi, &deps);
  fprintf(fp, "PE %12.6f %12.6f ", dpsi, deps);

  cel_pole(tdb, 1, -2.0, 3.0);
  e_tilt(tdb, accuracy, &a, &b, &c, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f ", dpsi, deps);

  openfile("cel_pole");
  cel_pole(tdb, 2, 0.0, 0.0);
  e_tilt(tdb, accuracy, &a, &b, &c, &dpsi, &deps);
  fprintf(fp, "XY %12.6f %12.6f ", dpsi, deps);

  cel_pole(tdb, 2, -2.0, 3.0);
  e_tilt(tdb, accuracy, &a, &b, &c, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f ", dpsi, deps);

  cel_pole(tdb, 1, 0.0, 0.0);
}

static void test_nutation() {
  double pos1[3];
  openfile("nutation");
  nutation(tdb, 0, accuracy, pos0, pos1);
  printunitvector(pos1);
}

static void test_ira_equinox() {
  openfile("ira_equinox");
  fprintf(fp, accuracy == 0 ? "%12.9f %12.9f" : "%9.6f    %9.6f   ", ira_equinox(tdb, 0, accuracy), ira_equinox(tdb, 1, accuracy));
}

static void test_cio_location() {
  double h = 0.0;
  short sys = -1;
  openfile("cio_location");

  if(is_ok(cio_location(tdb, accuracy, &h, &sys)))
    fprintf(fp, accuracy == 0 ? "%d %12.9f " : "%d %9.6f    ", sys, h);

  // repeat.
  if(is_ok(cio_location(tdb, accuracy, &h, &sys)))
    fprintf(fp, accuracy == 0 ? "%d %12.9f " : "%d %9.6f    ", sys, h);

}

static void test_cio_basis() {
  double h = 0.0, x[3], y[3], z[3];
  short sys = -1;

  openfile("cio_basis");

  if(!is_ok(cio_location(tdb, accuracy, &h, &sys))) return;
  if(!is_ok(cio_basis(tdb, h, sys, accuracy, x, y, z))) return;

  fprintf(fp, "%d ", sys);
  printunitvector(x);
  printunitvector(y);
  printunitvector(z);
}

static void test_cio_ra() {
  double h = 0.0;
  short sys = -1;
  openfile("cio_ra");

  if(is_ok(cio_ra(tdb, accuracy, &h)))
    fprintf(fp, accuracy == 0 ? "%d %12.9f " : "%d %9.6f    ", sys, h);

}

static void test_sidereal_time() {
  double h = 0.0;
  openfile("sidereal_time");
  if(!is_ok(sidereal_time(tdb, 0.0, ut12tt, 0, 0, accuracy, &h))) return;
  fprintf(fp, accuracy == 0 ? "ERA %12.6f " : "ERA %9.3f    ", h);

  openfile("sidereal_time");
  if(!is_ok(sidereal_time(tdb, 0.0, ut12tt, 0, 1, accuracy, &h))) return;
  fprintf(fp, accuracy == 0 ? "GST %12.6f" : "GST %9.3f   ", h);
}

static void test_geo_posvel() {
  double pos1[3], vel1[3];
  openfile("geo_posvel");
  if(is_ok(geo_posvel(tdb, ut12tt, accuracy, &obs, pos1, vel1))) {
    printvector(pos1);
    printvel(vel1);
  }
}



static void test_time_specific() {
  static char th[40] = { };

  idx = -1;
  sprintf(th, "%12.3f A%d: ", (tdb - J2000), accuracy);
  header = th;

  test_cal_date();
  test_julian_date();
  test_tdb2tt();
  test_ephemeris();
  test_era();
  test_mean_obliq();
  test_ee_ct();
  test_iau2000a();
  test_iau2000b();
  test_nu2000k();
  test_nutation_angles();
  test_e_tilt();
  test_cel_pole();
  test_nutation();
  test_ira_equinox();
  test_cio_location();
  test_cio_basis();
  test_cio_ra();
  test_sidereal_time();
  test_geo_posvel();

  header = NULL;
}


// ======================================================================================
// Source/time specific
// ======================================================================================


static int init() {
  object earth, sun;
  double tdb2[2] = { tdb };

  memset(pos0, 0, sizeof(pos0));
  memset(vel0, 0, sizeof(vel0));
  memset(epos, 0, sizeof(epos));
  memset(evel, 0, sizeof(evel));
  memset(pobs, 0, sizeof(pobs));
  memset(vobs, 0, sizeof(vobs));

  lst = 0.0;

  if(source.type == 0) {
    int error = ephemeris(tdb2, &source, 0, 1, pos0, vel0);
    if(error) {
      fprintf(stderr, "init: Failed source ephemeris for %s: error %d\n", source.name, error);
      return -1;
    }
  }
  else if(source.type == 2) {
    starvectors(&source.star, pos0, vel0);
  }
  else {
    fprintf(stderr, "init: invalid source type %d\n", source.type);
    return -1;
  }

  newline();
  fprintf(fp, "SOU ");
  printvector(pos0);
  printvel(vel0);
  newline();

  if(make_object(0, 3, "Earth", NULL, &earth) != 0) {
    fprintf(stderr, "init: Failed make_object(Earth)\n");
    return -1;
  }
  if(ephemeris(tdb2, &earth, 0, 1, epos, evel) != 0) {
    fprintf(stderr, "init: Failed Earth ephemeris\n");
    return -1;
  }

  fprintf(fp, "EAR ");
  printvector(epos);
  printvel(evel);
  newline();

  if(make_object(0, 10, "Sun", NULL, &sun) != 0) {
    fprintf(stderr, "init: Failed make_object(Sun)\n");
    return -1;
  }
  if(ephemeris(tdb2, &sun, 0, 1, spos, svel) != 0) {
    fprintf(stderr, "init: Failed Earth ephemeris\n");
    return -1;
  }

  fprintf(fp, "SUN ");
  printvector(spos);
  printvel(svel);
  newline();

  if(sidereal_time(tdb, 0.0, ut12tt, 0, 1, accuracy, &lst) != 0) {
    fprintf(stderr, "init: Failed sidereal_time\n");
    return -1;
  }

  fprintf(fp, "LST ");
  fprintf(fp, "%12.6f", lst);
  newline();

  if(obs.where == 1) {
    int i;

    terra(&obs.on_surf, lst, pobs, vobs);

    for(i = 0; i < 3; i++) {
      pobs[i] += epos[i];
      vobs[i] += evel[i];
    }
  }
  else if (obs.where == 2) {
    int i;
    for(i = 0; i < 3; i++) {
      pobs[i] = epos[i] + obs.near_earth.sc_pos[i] * 1e-3 * AU;
      vobs[i] = evel[i] + obs.near_earth.sc_vel[i] / (1e-3 * AU / 86400.0);
    }
  }

  fprintf(fp, "OBS ");
  printvector(pobs);
  printvel(vobs);

  return 0;
}

static void test_frame_tie() {
  double pos1[3];

  if(source.type != 2) return;

  openfile("frame_tie");
  frame_tie(pos0, -1, pos1);
  fprintf(fp, "-1 ");
  printunitvector(pos1);

  openfile("frame_tie");
  frame_tie(pos0, 1, pos1);
  fprintf(fp, "+1 ");
  printunitvector(pos1);
}


static void test_wobble() {
  double pos1[3];

  if(source.type != 2) return;

  openfile("wobble");
  wobble(tdb, 0, 2.0, -3.0, pos0, pos1);
  fprintf(fp, "0 ");
  printunitvector(pos1);

  openfile("wobble");
  wobble(tdb, 1, 2.0, -3.0, pos0, pos1);
  fprintf(fp, "1 ");
  printunitvector(pos1);
}


static void test_precession() {
  double pos1[3];

  if(source.type != 2) return;

  openfile("precession");
  if(is_ok(precession(tdb, pos0, J2000, pos1)))
    printunitvector(pos1);
}


static void test_light_time() {
  double pos1[3], t = 0.0;

  openfile("light_time");

  if(is_ok(light_time(tdb, &source, pobs, 0.0, accuracy, pos1, &t)))
    fprintf(fp, "%12.6f", t);
}

static void test_grav_def() {
  double pos1[3] = {}, pos2[3] = {}, ps[3] = {}, vs[3] = {};
  double d, jd2[2] = { tdb };
  object sun = {};
  int k;

  make_object(0, 10, "Sun", NULL, &sun);

  openfile("grav_def");
  if(is_ok(grav_def(tdb, obs.where, accuracy, pos0, pobs, pos1))) {
    printunitvector(pos1);
  }

  // Anything but the Sun itself...
  if(source.type == 0 && source.number == 10) return;

  // Now test a position near the Sun in the direction of the source
  ephemeris(jd2, &sun, 0, accuracy, ps, vs);
  d = vlen(pos0);

  for(k = 3; --k >= 0;) pos1[k] = ps[k] + 0.01 * pos0[k] / d;

  if(is_ok(grav_def(tdb, obs.where, accuracy, pos1, pobs, pos2))) {
    printunitvector(pos2);
  }
}

static void test_aberration() {
  double vo[3] = {}, v0[3] = {}, pos1[3];
  int i;

  // Calculate for sidereal sources only.
  if (source.type != 2) return;

  for(i = 0; i < 3; i++) vo[i] = evel[i] + vobs[i];

  openfile("aberration");
  aberration(pos0, vo, 0.0, pos1);
  printunitvector(pos1);

  aberration(pos0, v0, 0.0, pos1);
  for(i = 0; i < 3; i++) fprintf(fp, "%d ", fabs(pos0[i] - pos1[i]) < 1e-9 * vlen(pos0));
}

static void test_place() {
  int i;

  for(i=0; i < 4; i++) {
    sky_pos out;
    openfile("place");
    if(is_ok(place(tdb, &source, &obs, ut12tt, i, accuracy, &out))) {
      // Velocities to 0.1 m/s accuracy
      if(accuracy == 0) fprintf(fp, "%d %12.8f %12.8f %12.8f %12.5f ", i, out.ra, out.dec, out.dis, out.rv);
      else fprintf(fp, "%d %8.4f     %8.4f     %10.6f   %11.4f  ", i, out.ra, out.dec, out.dis, out.rv);
    }
  }
}


static void test_astro_place() {
  double ra = 0.0, dec = 0.0, d = 0.0;

  openfile("astro_place");

  if(source.type == 2) {
    if(is_ok(astro_star(tdb, &source.star, accuracy, &ra, &dec))) {
      fprintf(fp, accuracy == 0 ? "%12.8f %12.8f " : "%8.4f     %8.4f    ", ra, dec);
    }
  }
  else if(is_ok(astro_planet(tdb, &source, accuracy, &ra, &dec, &d))) {
    fprintf(fp, accuracy == 0 ? "%12.8f %12.8f %12.8f " : "%8.4f     %8.4f     %10.6f   ", ra, dec, d);
  }
}

static void test_virtual_place() {
  double ra = 0.0, dec = 0.0, d = 0.0;

  openfile("virtual_place");

  if(source.type == 2) {
    if(is_ok(virtual_star(tdb, &source.star, accuracy, &ra, &dec))) {
      fprintf(fp, accuracy == 0 ? "%12.8f %12.8f " : "%8.4f     %8.4f     ", ra, dec);
    }
  }
  else if(is_ok(virtual_planet(tdb, &source, accuracy, &ra, &dec, &d))) {
    fprintf(fp, accuracy == 0 ? "%12.8f %12.8f %12.8f " : "%8.4f     %8.4f     %10.6f   ", ra, dec, d);
  }
}

static void test_app_place() {
  double ra = 0.0, dec = 0.0, d = 0.0;

  openfile("local_place");

  if(source.type == 2) {
    if(is_ok(app_star(tdb, &source.star, accuracy, &ra, &dec)))
      fprintf(fp, accuracy == 0 ? "%12.8f %12.8f " : "%8.4f     %8.4f     ", ra, dec);

  }
  else if(is_ok(app_planet(tdb, &source, accuracy, &ra, &dec, &d))) {
    fprintf(fp, accuracy == 0 ? "%12.8f %12.8f %12.8f " : "%8.4f     %8.4f     %10.6f   ", ra, dec, d);
  }

}


static void test_local_place() {
  double ra = 0.0, dec = 0.0, d = 0.0;

  openfile("local_place");

  if(source.type == 2) {
    if(is_ok(local_star(tdb, ut12tt, &source.star, &obs.on_surf, accuracy, &ra, &dec))) {
      fprintf(fp, accuracy == 0 ? "%12.8f %12.8f " : "8.4f     %8.4f     ", ra, dec);
    }
  }
  else if(is_ok(local_planet(tdb, &source, ut12tt, &obs.on_surf, accuracy, &ra, &dec, &d))) {
    fprintf(fp, accuracy == 0 ? "%12.8f %12.8f %12.8f " : "%8.4f     %8.4f     %10.6f   ", ra, dec, d);
  }
}

static void test_topo_place() {
  double ra = 0.0, dec = 0.0, d = 0.0;

  openfile("topo_place");

  if(source.type == 2) {
    if(is_ok(topo_star(tdb, ut12tt, &source.star, &obs.on_surf, accuracy, &ra, &dec))){
      fprintf(fp, accuracy == 0 ? "%12.8f %12.8f " : "%8.4f     %8.4f     ", ra, dec);
    }
  }
  else if(is_ok(topo_planet(tdb, &source, ut12tt, &obs.on_surf, accuracy, &ra, &dec, &d))) {
    fprintf(fp, accuracy == 0 ? "%12.8f %12.8f %12.8f " : "%8.4f     %8.4f     %10.6f  ", ra, dec, d);
  }
}


static void test_cel2ter() {
  double pos1[3];

  if(source.type != 2) return;

  openfile("cel2ter");
  if(is_ok(cel2ter(tdb, 0.0, ut12tt, 0, accuracy, 0, 0.0, 0.0, pos0, pos1))) {
    fprintf(fp, "ERA GCRS ");
    printunitvector(pos1);
  }

  openfile("cel2ter");
  if(is_ok(cel2ter(tdb, 0.0, ut12tt, 1, accuracy, 0, 0.0, 0.0, pos0, pos1))) {
    fprintf(fp, "GST GCRS ");
    printunitvector(pos1);
  }

  openfile("cel2ter");
  if(is_ok(cel2ter(tdb, 0.0, ut12tt, 1, accuracy, 1, 0.0, 0.0, pos0, pos1))) {
    fprintf(fp, "GST APP  ");
    printunitvector(pos1);
  }
}

static void test_ter2cel() {
  double pos1[3];

  if(source.type != 2) return;

  openfile("ter2cel");
  if(is_ok(ter2cel(tdb, 0.0, ut12tt, 0, accuracy, 0, 0.0, 0.0, pos0, pos1))) {
    fprintf(fp, "ERA GCRS ");
    printunitvector(pos1);
  }

  openfile("ter2cel");
  if(is_ok(ter2cel(tdb, 0.0, ut12tt, 1, accuracy, 0, 0.0, 0.0, pos0, pos1))) {
    fprintf(fp, "GST GCRS ");
    printunitvector(pos1);
  }

  openfile("ter2cel");
  if(is_ok(ter2cel(tdb, 0.0, ut12tt, 1, accuracy, 1, 0.0, 0.0, pos0, pos1))) {
    fprintf(fp, "GST TOD  ");
    printunitvector(pos1);
  }
}


static void test_equ2hor() {
  double zd = 0.0, az = 0.0, rar = 0.0, decr = 0.0;

  if(source.type != 2) return;

  openfile("equ2hor");
  equ2hor(tdb, ut12tt, accuracy, 0.1, -0.2, &obs.on_surf, source.star.ra, source.star.dec, 0, &zd, &az, &rar, &decr);
  fprintf(fp, accuracy == 0 ? "%12.6f %12.6f %12.6f %12.6f " : "%9.3f    %9.3f    %12.6f %12.6f ", zd, az, rar, decr);

  openfile("equ2hor-refract");
  equ2hor(tdb, ut12tt, accuracy, 0.1, -0.2, &obs.on_surf, source.star.ra, source.star.dec, 1, &zd, &az, &rar, &decr);
  fprintf(fp, accuracy == 0 ? "%12.6f %12.6f %12.6f %12.6f " : "%9.3f    %9.3f    %12.6f %12.6f ", zd, az, rar, decr);
}


static void test_equ2gal() {
  double glon, glat;

  if(source.type != 2) return;

  openfile("equ2gal");

  equ2gal(source.star.ra, source.star.dec, &glon, &glat);
  fprintf(fp, "%12.6f %12.6f ", glon, glat);
}



static void test_equ2ecl() {
  double elon, elat;

  if(source.type != 2) return;

  openfile("equ2ecl");
  if(is_ok(equ2ecl(tdb, 0, accuracy, source.star.ra, source.star.dec, &elon, &elat)))
    fprintf(fp, "mean %12.6f %12.6f ", elon, elat);

  openfile("equ2ecl");
  if(is_ok(equ2ecl(tdb, 1, accuracy, source.star.ra, source.star.dec, &elon, &elat)))
    fprintf(fp, "true %12.6f %12.6f ", elon, elat);

  openfile("equ2ecl");
  if(is_ok(equ2ecl(tdb, 2, accuracy, source.star.ra, source.star.dec, &elon, &elat)))
    fprintf(fp, "gcrs %12.6f %12.6f ", elon, elat);
}

static void test_ecl2equ_vec() {
  double pos1[3];

  if(source.type != 2) return;

  openfile("ecl2equ_vec");
  if(is_ok(ecl2equ_vec(tdb, 0, accuracy, pos0, pos1))) {
    fprintf(fp, "mean ");
    printunitvector(pos1);
  }

  openfile("ecl2equ_vec");
  if(is_ok(ecl2equ_vec(tdb, 1, accuracy, pos0, pos1))) {
    fprintf(fp, "true ");
    printunitvector(pos1);
  }

  openfile("ecl2equ_vec");
  if(is_ok(ecl2equ_vec(tdb, 2, accuracy, pos0, pos1))) {
    fprintf(fp, "gcrs ");
    printunitvector(pos1);
  }

}

static void test_gcrs2equ() {
  double ra, dec;

  if(source.type != 2 || obs.where != 0 || tdb != J2000) return;

  openfile("gcrs2equ");
  if(is_ok(gcrs2equ(tdb, 0, accuracy, source.star.ra, source.star.dec, &ra, &dec)))
    fprintf(fp, "mean %12.6f %12.6f ", ra, dec);

  openfile("gcrs2equ");
  if(is_ok(gcrs2equ(tdb, 1, accuracy, source.star.ra, source.star.dec, &ra, &dec)))
    fprintf(fp, "true %12.6f %12.6f ", ra, dec);

  openfile("gcrs2equ");
  if(is_ok(gcrs2equ(tdb, 2, accuracy, source.star.ra, source.star.dec, &ra, &dec)))
    fprintf(fp, "cirs %12.6f %12.6f ", ra, dec);
}

static void test_rad_vel() {
  double rv = 0.0;

  openfile("rad_vel");

  rad_vel(&source, pos0, vel0, vobs, 0.0, 0.0, 0.0, &rv);
  fprintf(fp, accuracy == 0 ? "%12.6f " : "%9.3f    ", rv);

  rad_vel(&source, pos0, vel0, vobs, vdist(pobs, epos), vdist(pobs, spos), vdist(pos0, spos), &rv);
  fprintf(fp, accuracy == 0 ? "%12.6f " : "%9.3f    ", rv);
}

static void test_limb_angle() {
  double limb = 0.0, nadir = 0.0;
  openfile("limb_angle");
  limb_angle(pos0, pobs, &limb, &nadir);
  fprintf(fp, "%12.6f %12.6e ", limb, nadir);
}

static int test_source() {
  openfile("init");

  idx = 0;

  if(init() != 0) return -1;

  test_frame_tie();
  test_wobble();
  test_precession();
  test_light_time();
  test_grav_def();
  test_place();
  test_aberration();
  test_rad_vel();
  test_limb_angle();

  if(obs.where == 0) {
    test_astro_place();
    test_virtual_place();
    test_app_place();
    test_ter2cel();
    test_cel2ter();
    test_equ2gal();
    test_equ2ecl();
    test_ecl2equ_vec();
    test_gcrs2equ();
  }

  if(obs.where == 1) {
    test_local_place();
    test_topo_place();
    test_equ2hor();
  }

  return 0;
}




static int test_observers() {
  double ps[3] = { 100.0, 30.0, 10.0 }, vs[3] = { 10.0 };

  test_transform_cat();

  make_observer_at_geocenter(&obs);
  if(test_source() != 0) return -1;

  make_observer_on_surface(20.0, -15.0, 0.0, 0.0, 1000.0, &obs);
  if(test_source() != 0) return -1;

  make_observer_in_space(ps, vs, &obs);
  if(test_source() != 0) return -1;

  return 0;
}






static int test_sources() {
  cat_entry star;

  printf(" Testing date %.3f\n", (tdb - J2000));

  make_cat_entry("22+20", "TST", 1001, 22.0, 20.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_object(2, star.starnumber, star.starname, &star, &source) != 0) return -1;
  if(test_observers() != 0) return -1;

  make_cat_entry("16-20", "TST", 1001, 16.0, -20.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_object(2, star.starnumber, star.starname, &star, &source) != 0) return -1;
  if(test_observers() != 0) return -1;

  if(make_object(0, 10, "Sun", NULL, &source) != 0) return -1;
  if(test_observers() != 0) return -1;

  if(make_object(0, 3, "Earth", NULL, &source) != 0) return -1;
  if(test_observers() != 0) return -1;

  return 0;
}



static int test_dates() {
  double offsets[] = {-10000.0, 0.0, 10000.0, 10000.0, 10000.01 };
  int i;

  for(i = 0; i < 5; i++) {
    tdb = J2000 + offsets[i];

    test_time_specific();

    if(test_sources() != 0) {
      fprintf(stderr, " -- FAILED\n");
      return -1;
    }
  }

  fprintf(stderr, " -- OK\n");
  return 0;
}

static int test_accuracy() {
  for(accuracy = 0; accuracy < 2; accuracy++) if(test_dates() < 0) return -1;
  return 0;
}


int main() {
  test_basics();
  test_accuracy();
  fprintf(fp, "\n");
  fclose(fp);
}
