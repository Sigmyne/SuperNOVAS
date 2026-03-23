/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "SuperNOVAS C API", "index.html", [
    [ "User's guide", "index.html", "index" ],
    [ "C99 Usage", "md__2github_2workspace_2doc_2USAGE-C99.html", [
      [ "Building your application with SuperNOVAS (C99)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md21", [
        [ "Using a GNU <span class=\"tt\">Makefile</span>", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md22", null ],
        [ "Using CMake", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md23", null ],
        [ "Deprecated API", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md24", null ],
        [ "Legacy linking <span class=\"tt\">solarsystem()</span> / <span class=\"tt\">solarsystem_hp()</span> and <span class=\"tt\">readeph()</span> modules", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md25", null ],
        [ "Legacy modules: a better way", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md26", null ]
      ] ],
      [ "Example C99 usage", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md28", [
        [ "Calculating positions for a sidereal source", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md29", [
          [ "Specify the object of interest", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md30", null ],
          [ "Specify the observer location", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md31", null ],
          [ "Specify the time of observation", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md32", null ],
          [ "Set up the observing frame", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md33", null ],
          [ "Calculate an apparent place on sky", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md34", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md35", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md36", [
          [ "Planets and/or ephemeris type objects", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md37", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md39", null ],
          [ "Approximate planet and Moon orbitals", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md42", null ]
        ] ],
        [ "Going in reverse...", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md44", null ],
        [ "Calculate rise, set, and transit times", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md48", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md50", null ]
      ] ],
      [ "Tips and tricks (C99)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md53", [
        [ "Multi-threaded calculations", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md54", null ],
        [ "Physical units", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md56", null ],
        [ "String times and angles", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md58", null ],
        [ "String dates", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md59", null ]
      ] ]
    ] ],
    [ "C++ Usage", "md__2github_2workspace_2doc_2USAGE-CPP.html", [
      [ "Building your application with SuperNOVAS (C++)", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md40", [
        [ "Using a GNU <span class=\"tt\">Makefile</span>", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md41", null ],
        [ "Using CMake", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md43", null ],
        [ "C++ Fundamentals", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md46", [
          [ "Namespace", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md47", null ],
          [ "Validation", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md49", null ],
          [ "Immutable instances", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md52", null ],
          [ "Classes don't reference external objects internally", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md55", null ],
          [ "Operator overloading", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md57", null ]
        ] ]
      ] ],
      [ "Example C++ usage", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md61", [
        [ "Calculating positions for a sidereal source", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md62", [
          [ "Specify the object of interest", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md64", null ],
          [ "Specify the observer location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md65", [
            [ "A. Earth-based observer location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md66", null ],
            [ "B. other observer locations", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md67", null ]
          ] ],
          [ "Specify the time of observation", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md68", null ],
          [ "Set up the observing frame", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md70", null ],
          [ "Calculate an apparent place on sky", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md71", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md75", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md76", [
          [ "Planets and/or ephemeris type objects", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md79", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md80", null ],
          [ "Approximate planet orbitals", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md81", null ]
        ] ],
        [ "Going in reverse...", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md83", null ],
        [ "Calculate rise, set, and transit times", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md87", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md89", null ]
      ] ]
    ] ],
    [ "SuperNOVAS C99 vs. astropy", "md_SuperNOVAS__vs__astropy.html", null ],
    [ "Deprecated List", "deprecated.html", null ],
    [ "Topics", "topics.html", "topics" ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"group__observer.html#ga9e3cbe310a40919ff94d2d530ffa74ee",
"group__time.html#gga9f2b43ae4b048783dc09c2a3d9b46ca4ab13cba930f91e5e907bfc72489ffbc61",
"place_8c.html#a0f307c2a37d594a68e99c5c8af6d50c9"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';