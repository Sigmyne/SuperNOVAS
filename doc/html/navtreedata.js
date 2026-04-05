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
    [ "User's guide", "index.html#autotoc_md34", [
      [ "Table of Contents", "index.html#autotoc_md40", null ],
      [ "Introduction", "index.html#autotoc_md43", [
        [ "Related links", "index.html#autotoc_md47", null ]
      ] ],
      [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md49", null ],
      [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md61", null ],
      [ "Building and installation", "index.html#autotoc_md64", [
        [ "Build SuperNOVAS using GNU make", "index.html#autotoc_md67", null ],
        [ "Build SuperNOVAS using CMake", "index.html#autotoc_md81", null ],
        [ "Install SuperNOVAS via <span class=\"tt\">vcpkg</span>", "index.html#autotoc_md85", null ],
        [ "Linux packages", "index.html#autotoc_md88", null ],
        [ "Homebrew package", "index.html#autotoc_md91", null ],
        [ "Nix package", "index.html#autotoc_md92", null ]
      ] ],
      [ "Celestial coordinate systems (old vs. new)", "index.html#autotoc_md96", null ],
      [ "Example use cases", "index.html#autotoc_md100", [
        [ "Recommendation: set up an ephemeris provider", "index.html#autotoc_md101", null ],
        [ "Pick your flavor (C or C++)", "index.html#autotoc_md102", null ]
      ] ],
      [ "Incorporating Solar-system ephemeris data or services", "index.html#autotoc_md104", [
        [ "CALCEPH integration", "index.html#autotoc_md105", null ],
        [ "NAIF CSPICE toolkit integration", "index.html#autotoc_md106", null ],
        [ "Universal ephemeris data / service integration", "index.html#autotoc_md107", null ]
      ] ],
      [ "Notes on precision", "index.html#autotoc_md108", [
        [ "Prerequisites to precise results", "index.html#autotoc_md109", null ],
        [ "Reduced accuracy shortcuts", "index.html#autotoc_md110", null ]
      ] ],
      [ "Runtime debug support", "index.html#autotoc_md112", null ],
      [ "Representative benchmarks", "index.html#autotoc_md114", null ],
      [ "SuperNOVAS added features", "index.html#autotoc_md116", [
        [ "New functionality highlights", "index.html#autotoc_md117", null ],
        [ "Refinements to the NOVAS C API", "index.html#autotoc_md118", null ]
      ] ],
      [ "Release schedule", "index.html#autotoc_md120", null ]
    ] ],
    [ "C99 Usage", "md__2github_2workspace_2doc_2USAGE-C99.html", [
      [ "Building your application with SuperNOVAS (C99)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md20", [
        [ "Using a GNU <span class=\"tt\">Makefile</span>", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md21", null ],
        [ "Using CMake", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md22", null ],
        [ "Deprecated API", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md23", null ],
        [ "Legacy linking <span class=\"tt\">solarsystem()</span> / <span class=\"tt\">solarsystem_hp()</span> and <span class=\"tt\">readeph()</span> modules", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md24", null ],
        [ "Legacy modules: a better way", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md25", null ]
      ] ],
      [ "Example C99 usage", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md27", [
        [ "Calculating positions for a sidereal source", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md28", [
          [ "Specify the object of interest", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md29", null ],
          [ "Specify the observer location", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md38", null ],
          [ "Specify the time of observation", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md41", null ],
          [ "Set up the observing frame", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md45", null ],
          [ "Calculate an apparent place on sky", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md50", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md53", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md56", [
          [ "Planets and/or ephemeris type objects", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md57", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md62", null ],
          [ "Approximate planet orbitals", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md65", null ],
          [ "Moon's position and phase", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md69", null ]
        ] ],
        [ "Going in reverse...", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md71", null ],
        [ "Calculate rise, set, and transit times", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md73", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md74", null ]
      ] ],
      [ "Tips and tricks (C99)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md78", [
        [ "Multi-threaded calculations", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md80", null ],
        [ "Physical units", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md82", null ],
        [ "String times and angles", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md84", null ],
        [ "String dates", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md89", null ]
      ] ]
    ] ],
    [ "C++ Usage", "md__2github_2workspace_2doc_2USAGE-CPP.html", [
      [ "Building your application with SuperNOVAS (C++)", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md31", [
        [ "Using a GNU <span class=\"tt\">Makefile</span>", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md32", null ],
        [ "Using CMake", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md33", null ]
      ] ],
      [ "C++ Fundamentals", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md36", [
        [ "Namespace", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md37", null ],
        [ "Validation", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md39", null ],
        [ "Thread safety", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md44", null ],
        [ "Operator overloading", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md46", null ]
      ] ],
      [ "Example C++ usage", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md52", [
        [ "Calculating positions for a sidereal source", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md54", [
          [ "Specify the object of interest", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md55", null ],
          [ "Specify the observer location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md58", [
            [ "A. Earth-based observer location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md59", null ],
            [ "B. other observer locations", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md66", null ]
          ] ],
          [ "Specify the time of observation", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md68", null ],
          [ "Set up the observing frame", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md70", null ],
          [ "Calculate an apparent place on sky", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md72", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md75", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md76", [
          [ "Planets and/or ephemeris type objects", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md79", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md83", null ],
          [ "Approximate planet orbitals", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md86", null ],
          [ "Moon's position and phase", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md87", null ]
        ] ],
        [ "Going in reverse...", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md90", null ],
        [ "Calculate rise, set, and transit times", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md94", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md97", null ]
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
"group__observer.html#ga5518e094e90ed1053aeb03c41d531452",
"group__time.html#gad2d5ef17b41a30ebca91e4c70e636acf",
"novas_8h.html#aee3f2f5b3807612025e2ce25d368ff2fae8c1b78646fefbd0ffdcae083b597241"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';