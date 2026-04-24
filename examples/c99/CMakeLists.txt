# Additional CMake configuration file for building the example programs of the 
# supernovas library. The examples are also added to the test suite so they 
# may be checked for functionality.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_EXAMPLES=ON option.
#
# Author: Attila Kovacs

cmake_minimum_required(VERSION 3.20...4.3)

# Configuration in case this is a standalone project....
if(NOT PROJECT_NAME)
    project(supernovas-examples-c99
        VERSION 1.6.0
        DESCRIPTION "SuperNOVAS C99 example programs"
        HOMEPAGE_URL "https://sigmyne.github.io/SuperNOVAS/"
        LANGUAGES C
    )
    
    include(CheckLibraryExists)
    include(GNUInstallDirs)
        
    find_package(supernovas)

    check_library_exists(m exp "" HAS_MATHLIB)
    if(HAS_MATHLIB)
        set(MATHLIB m)
    endif()
endif()


# List of example programs to build
set(EXAMPLE_PROGRAMS
    example-star
    example-time
    example-high-z
    example-orbital
    example-rise-set
    example-moon
)

if(supernovas_solsys-calceph_FOUND OR ENABLE_CALCEPH)
    list(APPEND EXAMPLE_PROGRAMS example-calceph)
endif()

if(supernovas_solsys-cspice_FOUND OR ENABLE_CSPICE)
    list(APPEND EXAMPLE_PROGRAMS example-cspice)
endif()

# The ephemeris data to use for CALCEPH / CSPICE testing
set(EPHEM ${PROJECT_SOURCE_DIR}/test/ephem/de440s-j2000.bsp)

message("include dirs is ${supernovas_INCLUDE_DIRS}")

# Build each example
foreach(EXAMPLE ${EXAMPLE_PROGRAMS})
    set(EXAMPLE_SOURCE ${EXAMPLE}.c)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_SOURCE})
        add_executable(${EXAMPLE} ${EXAMPLE_SOURCE})

        target_include_directories(${EXAMPLE} PRIVATE ${supernovas_INCLUDE_DIRS})

        # Link against the supernovas library
        target_link_libraries(${EXAMPLE} PRIVATE
            supernovas::core
            ${MATHLIB}
        )

        # Special handling for CALCEPH example
        if(${EXAMPLE} STREQUAL "example-calceph")
            find_library(CALCEPH_LIB calceph REQUIRED)
            target_link_libraries(${EXAMPLE} PRIVATE supernovas::solsys-calceph)
            if(ENABLE_CALCEPH)
                add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE} ${EPHEM})
            endif()

        # Special handling for CSPICE example
        elseif(${EXAMPLE} STREQUAL "example-cspice")
       	    find_dependency(cspice) 
            target_link_libraries(${EXAMPLE} PRIVATE supernovas::solsys-cspice)
            if(ENABLE_CSPICE)
                add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE} ${EPHEM})
            endif()

        # All other tests
        else()
            add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE})
        endif()

    else()
        message(WARNING "Source file ${EXAMPLE_SOURCE} not found - ${EXAMPLE} will not be built")
    endif()
endforeach()
