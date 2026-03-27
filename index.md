---
excerpt: SuperNOVAS is a light-weight, high-precision astrometry C/C++ library, based on NOVAS C 3.1.
---

<img src="/SuperNOVAS/images/banner-620x240.png" alt="SuperNOVAS banner" width="620" height="240" align="center">
<br clear="all">

<img src="https://img.shields.io/github/v/release/Sigmyne/SuperNOVAS?label=github" class="badge" alt="GitHub release version" align="left">
<img src="https://img.shields.io/fedora/v/supernovas?color=lightblue" class="badge" alt="Fedora package version" align="left">
<img src="https://img.shields.io/ubuntu/v/supernovas?color=lightblue" class="badge" alt="Ubuntu package version" align="left">
<img src="https://img.shields.io/debian/v/supernovas?color=lightblue" class="badge" alt="Debian package version" align="left">
<br clear="all">

<img src="/SuperNOVAS/resources/Sigmyne-logo-200x44.png" alt="Sigmyne logo" width="200" height="44" align="right">
<br clear="all">

__SuperNOVAS__ is a C/C++ astronomy software library, providing high-precision astrometry such as one might need for 
running an observatory, a precise planetarium program, or for analyzing astronomical datasets. It started as a fork of 
the Naval Observatory Vector Astrometry Software ([NOVAS](https://aa.usno.navy.mil/software/novas_info)) C version 
3.1, but since then it has grown into its own, providing bug fixes, tons of new features, and a much improved API 
compared to the original NOVAS.

__SuperNOVAS__ is easy to use and it is very fast, providing 3--5 orders of magnitude faster position calculations than 
[astropy](https://www.astropy.org/) 7.0.0 in a single thread (see the [benchmarks](#benchmarks)), and its performance 
will scale with the number of CPUs when calculations are performed in parallel threads.

__SuperNOVAS__ is entirely free to use without licensing restrictions. Its source code is compatible with the C99 
standard, and hence should be suitable for old and new platforms alike. And, despite it being a light-weight library,
it fully supports the IAU 2000/2006 standards for sub-microarcsecond position calculations. 

__SuperNOVAS__ is maintained by [Attila Kovacs](https://github.com/attipaci) (Sigmyne LLC), and it is freely available 
via the [Sigmyne/SuperNOVAS](https://github.com/Sigmyne/SuperNOVAS/) repository on GitHub.

This site contains various online resources that support the library:


__Documentation__

 - [User's guide](doc/README.md) (`README.md`)
 - [API Documentation](doc/html/files.html)
 - [History of changes](doc/CHANGELOG.md) (`CHANGELOG.md`)
 - [Issues](https://github.com/Sigmyne/SuperNOVAS/issues) affecting SuperNOVAS releases (past and/or present)
 - [Community Forum](https://github.com/Sigmyne/SuperNOVAS/discussions) &ndash; ask a question, provide feedback, or 
   check announcements.

 
__Downloads__

 - [Releases](https://github.com/Sigmyne/SuperNOVAS/releases) from GitHub


__Linux Packages__

SuperNOVAS is also available in packaged for for Fedora / EPEL and Debian-based Linux distros. It has the following 
package structure, which allows non-bloated installations of just the parts that are needed for the particular use 
case(s):


 | __Fedora / EPEL RPM__                 |  __Debian package__                          |
 |---------------------------------------|----------------------------------------------|
 | `supernovas`                          | `libsupernovas1`                             |
 | `supernovas-solsys-calceph`           | `libsolsys-calceph1`                         |
 | `supernovas-devel`                    | `libsupernovas-dev`                          |
 | `supernovas-doc`                      | `libsupernovas-doc`                          |
 

The differences in package naming are due to the different naming policies for RedHat/Fedora vs Debian. Otherwise, the 
RPM and Debian packages provide identical contents and features.
 

__vcpkg Registry__

You can install the core SuperNOVAS library with `vcpkg` on Linux, MacOS, Windows, and Android as:

```bash
  $ vcpkg install supernovas
```

Or, including the `solsys-calceph` plugin library as:

```bash
  $ vcpkg install supernovas[solsys-calceph]
```

__Homebrew__ 

Or, install via the Homebrew package manager (MacOS and Linux) through the maintainer's own Tap, including the 
`solsys-calceph` plugin:

```bash
  $ brew tap attipaci/pub
  $ brew install supernovas
```


