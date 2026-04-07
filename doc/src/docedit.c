/**
 * @date Created  on Sep 5, 2025
 * @author Attila Kovacs
 *
 *   Creates headless Doxyfile.local and README-orig.md variants
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define README_TITLE      "# User's guide\n\n"
#define HTML_CLEAR_ALL    "<br clear=\"all\">\n\n"

#if defined _WIN32 || defined __CYGWIN__
#  define PATH_SEP  "\\"
#else
#  define PATH_SEP  "/"
#endif

static char *inpath = "..";
static char *outpath = ".";

static FILE *openfile(const char *path, const char *name, const char *mode) {
  char filename[1024] = {'\0'};
  FILE *fp;

  sprintf(filename, "%s" PATH_SEP "%s", path, name);
  fp = fopen(filename, mode);
  if(!fp) fprintf(stderr, "ERROR! opening %s: %s", filename, strerror(errno));
  return fp;
}

static void replace(const char *str, const char *from, const char *to) {
  char *match = strstr(str, from);
  int lfrom, lto, lrem;

  if(!match) return;

  lfrom = strlen(from);
  lto = strlen(to);
  lrem = strlen(match + lfrom);

  memmove(match + lto, match + lfrom, lrem + 1);
  memcpy(match, to, lto);
  match[lrem] = '\0';
}

static int make_headless_readme() {
  FILE *in, *out;
  char line[16384] = {'\0'};
  int head = 1;

  in = openfile(inpath, "README.md", "r");
  if(!in) return -1;

  out = openfile(outpath, "README.md", "w");
  if(!out) {
    fclose(in);
    return -1;
  }

  while(fgets(line, sizeof(line) - 1, in) != NULL) {
    // In <img /> tags, replace resources/* with local reference.
    const char *img = strstr(line, "<img ");
    if(img) replace(img, "resources" PATH_SEP, "");

    if(head) {
      if(strncmp(line, "# ", 2) != 0) continue;
      fwrite(README_TITLE, sizeof(README_TITLE) - 1, 1, out);
      head = 0;
      continue;
    }

    // Remove 'doc/' prefix from internal links
    replace(line, "\"doc/", "\"");

    fwrite(line, strlen(line), 1, out);
  }

  fclose(out);
  fclose(in);

  return 0;
}

static int make_undecorated_readme() {
  FILE *in, *out;
  char line[16384] = {'\0'};

  in = openfile(outpath, "README.md", "r");
  if(!in) return -1;

  out = openfile(outpath, "README-undecorated.md", "w");
  if(!out) {
    fclose(in);
    return -1;
  }

  while(fgets(line, sizeof(line) - 1, in) != NULL) {
    char type[100] = {'\0'};

    if(sscanf(line, "> [!%99[^]]]", type) == 1) sprintf(line, "__%s__\n\n", type);
    if(strstr(line, "<details") || strstr(line, "</details>")) continue;

    fwrite(line, strlen(line), 1, out);
  }

  fclose(out);
  fclose(in);

  return 0;
}

// Syntax: docedit [docpath]
// Generates:
//   ../README.md --> README.md
//   README.md    --> README-undecorated.md
int main(int argc, const char *argv[]) {
  int nerr = 0;

  if(argc > 1) {
    outpath = (char *) argv[1];
    inpath = (char *) calloc(1, strlen(argv[1]) + 10);
    sprintf(inpath, "%s/..", argv[1]);
  }

  if(make_headless_readme() != 0) nerr++;
  if(make_undecorated_readme() != 0) nerr++;

  return nerr;
}
