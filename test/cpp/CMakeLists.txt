# Additional CMake configuration file for building the C++ regression testing 
# programs of the SuperNOVAS library. The programs are added to the test suite.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_TESTING=ON -DENABLE_CPP=ON options.
#
# Author: Attila Kovacs

# test all sources
FILE(GLOB TEST_PROGRAMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# Test programs, matching the test sources
list(TRANSFORM TEST_PROGRAMS REPLACE "[.]cpp$" "")

# Build each example
foreach(TEST ${TEST_PROGRAMS})
    set(TEST_SOURCE ${TEST}.cpp)

    add_executable(${TEST} ${TEST_SOURCE})
              
    target_include_directories(${TEST} PRIVATE
        ${PROJECT_SOURCE_DIR}/include
    )
        
    # Link against the supernovas library
    target_link_libraries(${TEST} PRIVATE
        supernovas::cpp
        supernovas::core
        ${MATH}
    )

    add_test(NAME ${TEST} COMMAND ${TEST})
endforeach()

