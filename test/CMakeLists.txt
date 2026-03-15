# Additional CMake configuration file for building the regression testing programs
# of the SuperNOVAS library. The programs are added to the test suite.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_TESTING=ON option.
#
# Author: Attila Kovacs

# List of test programs to build
set(TEST_PROGRAMS
    test-compat
    test-super
    test-errors
)

if(ENABLE_CALCEPH)
    list(APPEND TEST_PROGRAMS test-calceph)
endif()

if(ENABLE_CSPICE)
    list(APPEND TEST_PROGRAMS test-cspice)
endif()

if(ENABLE_CPP_LIBS)
    add_subdirectory(cpp)
endif()

include_directories(${supernovas_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/legacy)

# Link against binaries in the local supernovas lib/ directory
link_directories(${CMAKE_BINARY_DIR}/lib)

# Directory where ephemeris data for the tests are found
set(EPHEM_DIR ${PROJECT_SOURCE_DIR}/test/ephem)

# Create a data/ directory for the compat test output
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)

# Build each example
foreach(TEST ${TEST_PROGRAMS})
    set(TEST_SOURCE src/${TEST}.c)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE})
        add_executable(${TEST} ${TEST_SOURCE})
              
        target_include_directories(${TEST} PRIVATE
            ${PROJECT_SOURCE_DIR}/include
        )
        
        # Link against the supernovas library
        target_link_libraries(${TEST} PRIVATE
            supernovas::core
            ${MATH}
        )

        # Special handling for CALCEPH example
        if(${TEST} STREQUAL "test-calceph")
            find_library(CALCEPH_LIB calceph REQUIRED)
            target_link_libraries(${TEST} PRIVATE
                supernovas::solsys-calceph
                ${CALCEPH_LIB}
            )
     	    add_test(NAME ${TEST} COMMAND ${TEST} ${EPHEM_DIR})
     	    
        # Special handling for CSPICE example
        elseif(${TEST} STREQUAL "test-cspice")
            find_library(CSPICE_LIB cspice REQUIRED)
            target_link_libraries(${TEST} PRIVATE
                supernovas::solsys-cspice
                ${CSPICE_LIB}
            )
            add_test(NAME ${TEST} COMMAND ${TEST} ${EPHEM_DIR})
        
        # All other examples
     	else()
     	    add_test(NAME ${TEST} COMMAND ${TEST})
        endif()
        
    else()
        message(WARNING "Source file ${TEST_SOURCE} not found - ${TEST} will not be built")
    endif()
endforeach()

