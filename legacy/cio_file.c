/**
 * @file
 *
 *  SuperNOVAS tool to Produces binary data file of RA values for CIO. The resulting binary file
 *  is platform-dependent. As of SuperNOVAS version 1.1, one may use the ASCII file directly with
 *  SuperNOVAS.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  http://www.usno.navy.mil/USNO/astronomical-applications
 *
 * @author G. Kaplan and Attila Kovacs
 *
 * @sa set_cio_locator_file()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int usage() {
  printf(" cio_file -- Legacy conversion tool for NOVAS C CIO vs GCRS locator data.\n\n");
  printf(" Syntax: cio_file [ascii-file [binary-file]]\n\n");
  printf(" Parameters:\n\n");
  printf("   ascii-file      File name/path of the input ASCII data ('CIO_RA.TXT').\n");
  printf("   binary-file     File name/path of the output binary data ('cio_ra.bin').\n");
  printf("\n");
  return 0;
}

int main(int argc, const char *argv[]) {

  /*
   Program to produce a binary direct access file of right ascension
   values of the celestial intermediate origin (CIO), given a formatted
   text file of the same data.  Each input and output data record
   contains a TDB Julian date and a right ascension value in
   arcseconds.

   The input formatted text file is 'CIO_RA.TXT'. It is included in the
   NOVAS-C package.
   */

  const char *filename = "CIO_RA.TXT";
  const char *outname = "cio_ra.bin";
  char identifier[25];

  long header_size, record_size, i, n_recs;
  int version;

  double jd_tdb, ra_cio, jd_first = 0.0, jd_last = 0.0, interval = 0.0, jd_beg, jd_end, t_int, jd_1, ra_1, jd_n, ra_n;

  size_t double_size, long_size;

  FILE *in_file, *out_file;

  double_size = sizeof(double);
  long_size = sizeof(long int);
  header_size = (long) ((size_t) 3 * double_size + long_size);
  record_size = (long) ((size_t) 2 * double_size);

  if(argc > 1) {
    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) return usage();
    if(strcmp(argv[1], "/help") == 0 || strcmp(argv[1], "/h") == 0) return usage();
    if(strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "/?") == 0) return usage();

    filename = argv[1];
  }
  if(argc > 2)
    outname = argv[2];

  /*
   Open the input formatted text file.
   */

  if((in_file = fopen(filename, "r")) == NULL) {
    printf("Error opening input file.\n");
    return (1);
  }

  /*
   Open the output binary, random-access file.
   */

  if((out_file = fopen(outname, "wb+")) == NULL) {
    printf("Error opening output file.\n");
    fclose(in_file);
    return (1);
  }

  /*
   Read input file identifier.
   */

  if(fgets(identifier, sizeof(identifier) - 1, in_file) == NULL) {
    printf("Empty input file.\n");
    fclose(in_file);
    fclose(out_file);
    return (1);
  }

  if(sscanf(identifier, "CIO RA P%d @ %lfd", &version, &interval) != 2) {
    printf("Invalid header: %s.\n", identifier);
    fclose(in_file);
    fclose(out_file);
    return (1);
  }

  /*
   Read the input file and write the output file.
   */

  i = 0L;
  while(!feof(in_file)) {

    /*
     Read a record from the input file.
     */

    if(fscanf(in_file, " %lf %lf ", &jd_tdb, &ra_cio) != 2) {
      printf("Error invalid input record %ld.\n", i);
      fclose(in_file);
      fclose(out_file);
      return (1);
    }
    i++;

    /*
     If this is the first record, capture the Julian date and position
     the output file pointer so as reserve space for the output file
     header.
     */

    if(i == 1L) {
      jd_first = jd_tdb;
      fseek(out_file, header_size, SEEK_SET);
    }

    /*
     Capture the value of the Julian date of the last data point.
     */

    jd_last = jd_tdb;

    /*
     Write a regular data record to the output file.
     */

    fwrite(&jd_tdb, double_size, (size_t) 1, out_file);
    fwrite(&ra_cio, double_size, (size_t) 1, out_file);
    if(ferror(out_file)) {
      printf("Error on output file while writing record %ld.\n", i);
      fclose(in_file);
      fclose(out_file);
      return (1);
    }
  }

  if(interval <= 0) {
    printf("Error no data.\n");
    fclose(in_file);
    fclose(out_file);
    return (1);
  }

  /*
   Now write the file header.
   */

  fseek(out_file, 0L, SEEK_SET);
  fwrite(&jd_first, double_size, (size_t) 1, out_file);
  fwrite(&jd_last, double_size, (size_t) 1, out_file);
  fwrite(&interval, double_size, (size_t) 1, out_file);
  fwrite(&i, long_size, (size_t) 1, out_file);
  if(ferror(out_file)) {
    printf("Error on output file while writing file header.\n");
    fclose(in_file);
    fclose(out_file);
    return (1);
  }

  /*
   Do a trial read of the file header, the first data record, and the
   last data record, then write summary to standard output.
   */

  rewind(out_file);
  if(fread(&jd_beg, double_size, (size_t) 1, out_file) != 1) goto read_error; // @suppress("Goto statement used")
  if(fread(&jd_end, double_size, (size_t) 1, out_file) != 1) goto read_error; // @suppress("Goto statement used")
  if(fread(&t_int, double_size, (size_t) 1, out_file) != 1) goto read_error; // @suppress("Goto statement used")
  if(fread(&n_recs, long_size, (size_t) 1, out_file) != 1) goto read_error; // @suppress("Goto statement used")

  if(fread(&jd_1, double_size, (size_t) 1, out_file) != 1) goto read_error; // @suppress("Goto statement used")
  if(fread(&ra_1, double_size, (size_t) 1, out_file) != 1) goto read_error; // @suppress("Goto statement used")

  fseek(out_file, -(record_size), SEEK_END);

  if(fread(&jd_n, double_size, (size_t) 1, out_file) != 1) goto read_error; // @suppress("Goto statement used")
  if(fread(&ra_n, double_size, (size_t) 1, out_file) != 1) goto read_error; // @suppress("Goto statement used")

  printf("Results from program cio_file:\n\n");
  printf("Input file identifier: %s\n", identifier);
  printf("%ld records read from the input file:\n", n_recs);
  printf("   First Julian date: %f\n", jd_beg);
  printf("   Last Julian date:  %f\n", jd_end);
  printf("   Data interval: %f days\n\n", t_int);
  printf("First data point: %f  %f\n", jd_1, ra_1);
  printf("Last data point:  %f  %f\n\n", jd_n, ra_n);
  printf("Binary file %s created.\n", outname);

  /*
   Close files.
   */

  fclose(in_file);
  fclose(out_file);

  return (0);

  read_error:

  fclose(in_file);
  fclose(out_file);
  perror("read error");
  return (1);
}
