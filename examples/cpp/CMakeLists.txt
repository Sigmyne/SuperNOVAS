# Additional CMake configuration file for building the example programs of the 
# supernovas library. The examples are also added to the test suite so they 
# may be checked for functionality.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_EXAMPLES=ON option.
#
# Author: Attila Kovacs

cmake_minimum_required(VERSION 3.20...4.2)

# Configuration in case this is a standalone project....
if(NOT PROJECT_NAME)
    project(supernovas-examples-c++
        VERSION 1.6.0
        DESCRIPTION "SuperNOVAS C++ example programs"
        HOMEPAGE_URL "https://sigmyne.github.io/SuperNOVAS/"
        LANGUAGES CXX
    )
    
    include(GNUInstallDirs)
        
    find_package(supernovas)

    if(WIN32)
        set(MATH "")
    else()
        find_library(MATH_LIBRARY m REQUIRED)
        set(MATH m)
    endif()
endif()


# Build all examples
FILE(GLOB EXAMPLE_PROGRAMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# Test programs, matching the test sources
list(TRANSFORM EXAMPLE_PROGRAMS REPLACE "[.]cpp$" "")

# Build each example
foreach(EXAMPLE ${EXAMPLE_PROGRAMS})
    set(EXAMPLE_SOURCE ${EXAMPLE}.cpp)

    add_executable(${EXAMPLE} ${EXAMPLE_SOURCE})

    target_include_directories(${EXAMPLE} PRIVATE ${supernovas_INCLUDE_DIRS})

    # Link against the supernovas library
    target_link_libraries(${EXAMPLE} PRIVATE
       supernovas::core
       supernovas::cpp
       ${MATH}
    )

    add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE})
endforeach()
