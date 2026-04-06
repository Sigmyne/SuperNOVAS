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
  [ "SuperNOVAS C++ API", "index.html", [
    [ "User's guide", "index.html#autotoc_md0", [
      [ "Table of Contents", "index.html#autotoc_md1", null ],
      [ "Introduction", "index.html#autotoc_md3", [
        [ "Related links", "index.html#autotoc_md4", null ]
      ] ],
      [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md6", null ],
      [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md8", null ],
      [ "Building and installation", "index.html#autotoc_md10", [
        [ "Build SuperNOVAS using GNU make", "index.html#autotoc_md11", null ],
        [ "Build SuperNOVAS using CMake", "index.html#autotoc_md12", null ],
        [ "Install SuperNOVAS via <span class=\"tt\">vcpkg</span>", "index.html#autotoc_md13", null ],
        [ "Linux packages", "index.html#autotoc_md14", null ],
        [ "Homebrew package", "index.html#autotoc_md15", null ],
        [ "Nix package", "index.html#autotoc_md16", null ]
      ] ],
      [ "Celestial coordinate systems (old vs. new)", "index.html#autotoc_md18", null ],
      [ "Example use cases", "index.html#autotoc_md20", [
        [ "Recommendation: set up an ephemeris provider", "index.html#autotoc_md21", null ],
        [ "Pick your flavor (C or C++)", "index.html#autotoc_md22", null ]
      ] ],
      [ "Incorporating Solar-system ephemeris data or services", "index.html#autotoc_md24", [
        [ "CALCEPH integration", "index.html#autotoc_md25", null ],
        [ "NAIF CSPICE toolkit integration", "index.html#autotoc_md26", null ],
        [ "Universal ephemeris data / service integration", "index.html#autotoc_md27", null ]
      ] ],
      [ "Notes on precision", "index.html#autotoc_md28", [
        [ "Prerequisites to precise results", "index.html#autotoc_md29", null ],
        [ "Reduced accuracy shortcuts", "index.html#autotoc_md30", null ]
      ] ],
      [ "Runtime debug support", "index.html#autotoc_md32", null ],
      [ "Representative benchmarks", "index.html#autotoc_md34", null ],
      [ "SuperNOVAS added features", "index.html#autotoc_md36", [
        [ "New functionality highlights", "index.html#autotoc_md37", null ],
        [ "Refinements to the NOVAS C API", "index.html#autotoc_md38", null ]
      ] ],
      [ "Release schedule", "index.html#autotoc_md40", null ]
    ] ],
    [ "C99 Usage", "md__2github_2workspace_2doc_2USAGE-C99.html", [
      [ "Building your application with SuperNOVAS (C99)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md44", [
        [ "Using a GNU <span class=\"tt\">Makefile</span>", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md45", null ],
        [ "Using CMake", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md46", null ],
        [ "Deprecated API", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md47", null ],
        [ "Legacy linking <span class=\"tt\">solarsystem()</span> / <span class=\"tt\">solarsystem_hp()</span> and <span class=\"tt\">readeph()</span> modules", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md48", null ],
        [ "Legacy modules: a better way", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md49", null ]
      ] ],
      [ "Example C99 usage", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md51", [
        [ "Calculating positions for a sidereal source", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md52", [
          [ "Specify the object of interest", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md53", null ],
          [ "Specify the observer location", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md54", null ],
          [ "Specify the time of observation", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md55", null ],
          [ "Set up the observing frame", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md56", null ],
          [ "Calculate an apparent place on sky", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md57", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md58", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md59", [
          [ "Planets and/or ephemeris type objects", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md60", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md61", null ],
          [ "Approximate planet orbitals", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md62", null ],
          [ "Moon's position and phase", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md63", null ]
        ] ],
        [ "Going in reverse...", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md64", null ],
        [ "Calculate rise, set, and transit times", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md65", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md66", null ]
      ] ],
      [ "Tips and tricks (C99)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md68", [
        [ "Multi-threaded calculations", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md69", null ],
        [ "Physical units", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md70", null ],
        [ "String times and angles", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md71", null ],
        [ "String dates", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md72", null ]
      ] ]
    ] ],
    [ "C++ Usage", "md__2github_2workspace_2doc_2USAGE-CPP.html", [
      [ "Building your application with SuperNOVAS (C++)", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md80", [
        [ "Using a GNU <span class=\"tt\">Makefile</span>", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md81", null ],
        [ "Using CMake", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md82", null ]
      ] ],
      [ "C++ Fundamentals", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md84", [
        [ "Namespace", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md85", null ],
        [ "Validation", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md86", null ],
        [ "Thread safety", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md87", null ],
        [ "Operator overloading", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md88", null ]
      ] ],
      [ "Example C++ usage", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md90", [
        [ "Calculating positions for a sidereal source", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md91", [
          [ "Specify the object of interest", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md92", null ],
          [ "Specify the observer location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md93", [
            [ "A. Earth-based observer location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md94", null ],
            [ "B. other observer locations", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md95", null ]
          ] ],
          [ "Specify the time of observation", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md96", null ],
          [ "Set up the observing frame", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md97", null ],
          [ "Calculate an apparent place on sky", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md98", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md99", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md100", [
          [ "Planets and/or ephemeris type objects", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md101", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md102", null ],
          [ "Approximate planet orbitals", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md103", null ],
          [ "Moon's position and phase", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md104", null ]
        ] ],
        [ "Going in reverse...", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md105", null ],
        [ "Calculate rise, set, and transit times", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md106", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md107", null ]
      ] ]
    ] ],
    [ "SuperNOVAS C++ vs. astropy", "md_SuperNOVAS_09_09__vs__astropy.html", null ],
    [ "SuperNOVAS C++ vs. C99", "md_SuperNOVAS_09_09__vs__C99.html", null ],
    [ "Deprecated List", "deprecated.html", null ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
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
"Angle_8cpp.html",
"classsupernovas_1_1Equatorial.html#a084ea74c4e83b73ef43a43e273770270",
"classsupernovas_1_1Position.html#a8158b16437d04f06a8f4e5213ca82a92",
"classsupernovas_1_1Track.html#ad855778035b6f474a30d1536971839f2",
"group__apparent.html#gaec64bf67d5a03b2b4fb2583e5918f1de",
"novas_8h.html#a0408e0c92b505e9914753930ea3f047f",
"novas_8h.html#a98c96461c56c17948f6ba1cc746f85eb",
"place_8c.html",
"transform_8c.html#a3fa57a154f2f423612736e5e3a5addbf"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';