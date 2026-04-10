# Additional CMake configuration file for building the C/C++ libratries of 
# SuperNOVAS.
#
# To invoke simply configure and run the cmake build in the supernovas root 
# directory
#
# Author: Attila Kovacs

# Sources
set(SUPERNOVAS_CORE_SOURCES
    target.c
    observer.c
    earth.c
    equator.c
    system.c
    transform.c
    cio.c
    orbital.c
    spectral.c
    grav.c
    nutation.c
    timescale.c
    frames.c
    place.c
    calendar.c
    refract.c
    naif.c
    parse.c
    util.c
    planets.c
    itrf.c
    ephemeris.c
    solsys3.c
    solsys-ephem.c
    moon.c
)


add_subdirectory(c99)

if(ENABLE_CPP)
    add_subdirectory(cpp)
endif()

if(ENABLE_CUDA)
    add_subdirectory(cuda)
endif()
