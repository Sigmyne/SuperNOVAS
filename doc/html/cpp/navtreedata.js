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
    [ "User's guide", "index.html#autotoc_md1", [
      [ "Table of Contents", "index.html#autotoc_md2", null ],
      [ "Introduction", "index.html#autotoc_md4", [
        [ "Related links", "index.html#autotoc_md5", null ]
      ] ],
      [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md7", null ],
      [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md9", null ],
      [ "Building and installation", "index.html#autotoc_md11", [
        [ "Build SuperNOVAS using GNU make", "index.html#autotoc_md12", null ],
        [ "Build SuperNOVAS using CMake", "index.html#autotoc_md13", null ],
        [ "Install SuperNOVAS via <span class=\"tt\">vcpkg</span>", "index.html#autotoc_md14", null ],
        [ "Linux packages", "index.html#autotoc_md15", null ],
        [ "Homebrew package", "index.html#autotoc_md16", null ],
        [ "Nix package", "index.html#autotoc_md17", null ]
      ] ],
      [ "Celestial coordinate systems (old vs. new)", "index.html#autotoc_md19", null ],
      [ "Example use cases", "index.html#autotoc_md21", [
        [ "Recommendation: set up an ephemeris provider", "index.html#autotoc_md22", null ],
        [ "Pick your flavor (C or C++)", "index.html#autotoc_md23", null ]
      ] ],
      [ "Incorporating Solar-system ephemeris data or services", "index.html#autotoc_md25", [
        [ "CALCEPH integration", "index.html#autotoc_md26", null ],
        [ "NAIF CSPICE toolkit integration", "index.html#autotoc_md27", null ],
        [ "Universal ephemeris data / service integration", "index.html#autotoc_md31", null ]
      ] ],
      [ "Notes on precision", "index.html#autotoc_md35", [
        [ "Prerequisites to precise results", "index.html#autotoc_md37", null ],
        [ "Reduced accuracy shortcuts", "index.html#autotoc_md43", null ]
      ] ],
      [ "Runtime debug support", "index.html#autotoc_md45", null ],
      [ "Representative benchmarks", "index.html#autotoc_md49", null ],
      [ "SuperNOVAS added features", "index.html#autotoc_md57", [
        [ "New functionality highlights", "index.html#autotoc_md58", null ],
        [ "Refinements to the NOVAS C API", "index.html#autotoc_md69", null ]
      ] ],
      [ "Release schedule", "index.html#autotoc_md72", null ]
    ] ],
    [ "C99 Usage", "md__2github_2workspace_2doc_2USAGE-C99.html", [
      [ "Building your application with SuperNOVAS (C99)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md29", [
        [ "Using a GNU <span class=\"tt\">Makefile</span>", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md30", null ],
        [ "Using CMake", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md32", null ],
        [ "Deprecated API", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md33", null ],
        [ "Legacy linking <span class=\"tt\">solarsystem()</span> / <span class=\"tt\">solarsystem_hp()</span> and <span class=\"tt\">readeph()</span> modules", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md34", null ],
        [ "Legacy modules: a better way", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md36", null ]
      ] ],
      [ "Example C99 usage", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md39", [
        [ "Calculating positions for a sidereal source", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md40", [
          [ "Specify the object of interest", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md41", null ],
          [ "Specify the observer location", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md42", null ],
          [ "Specify the time of observation", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md46", null ],
          [ "Set up the observing frame", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md47", null ],
          [ "Calculate an apparent place on sky", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md50", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md51", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md52", [
          [ "Planets and/or ephemeris type objects", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md53", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md54", null ],
          [ "Approximate planet orbitals", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md55", null ],
          [ "Moon's position and phase", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md59", null ]
        ] ],
        [ "Going in reverse...", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md60", null ],
        [ "Calculate rise, set, and transit times", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md61", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md62", null ]
      ] ],
      [ "Tips and tricks (C99)", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md64", [
        [ "Multi-threaded calculations", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md65", null ],
        [ "Physical units", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md66", null ],
        [ "String times and angles", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md67", null ],
        [ "String dates", "md__2github_2workspace_2doc_2USAGE-C99.html#autotoc_md68", null ]
      ] ]
    ] ],
    [ "C++ Usage", "md__2github_2workspace_2doc_2USAGE-CPP.html", [
      [ "Building your application with SuperNOVAS (C++)", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md76", [
        [ "Using a GNU <span class=\"tt\">Makefile</span>", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md77", null ],
        [ "Using CMake", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md78", null ]
      ] ],
      [ "C++ Fundamentals", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md80", [
        [ "Namespace", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md81", null ],
        [ "Validation", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md82", null ],
        [ "Thread safety", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md83", null ],
        [ "Operator overloading", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md84", null ]
      ] ],
      [ "Example C++ usage", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md86", [
        [ "Calculating positions for a sidereal source", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md87", [
          [ "Specify the object of interest", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md88", null ],
          [ "Specify the observer location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md89", [
            [ "A. Earth-based observer location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md90", null ],
            [ "B. other observer locations", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md91", null ]
          ] ],
          [ "Specify the time of observation", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md92", null ],
          [ "Set up the observing frame", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md93", null ],
          [ "Calculate an apparent place on sky", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md94", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md95", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md96", [
          [ "Planets and/or ephemeris type objects", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md98", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md99", null ],
          [ "Approximate planet orbitals", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md100", null ],
          [ "Moon's position and phase", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md102", null ]
        ] ],
        [ "Going in reverse...", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md103", null ],
        [ "Calculate rise, set, and transit times", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md104", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "md__2github_2workspace_2doc_2USAGE-CPP.html#autotoc_md105", null ]
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
"classsupernovas_1_1Track.html#ad88920e8a678fc24904c0013e50ef718",
"group__earth.html",
"novas_8h.html#a044f1359abcf0a039ac6452a95729b5a",
"novas_8h.html#a98daa33604665eda41d9a9de964d7cc2",
"place_8c.html#a0f307c2a37d594a68e99c5c8af6d50c9",
"transform_8c.html#a6205edb1361e56cc0c7aed0f088f7437"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';