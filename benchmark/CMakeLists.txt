# Additional CMake configuration file for building the benchmarking programs
# of the SuperNOVAS library.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_BENCHMARK=ON option.
#
# Author: Attila Kovacs

# List of benchmarking programs to build
set(BENCHMARK_PROGRAMS
    benchmark-nutation
    benchmark-place
)

include_directories(${supernovas_INCLUDE_DIRS})

# Build each example
foreach(BENCHMARK ${BENCHMARK_PROGRAMS})
    set(BENCHMARK_SOURCE ${BENCHMARK}.c)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK_SOURCE})
        add_executable(${BENCHMARK} ${BENCHMARK_SOURCE})

        # Link against the supernovas library
        target_link_libraries(${BENCHMARK} PRIVATE core)
    else()
        message(WARNING "Source file ${BENCHMARK_SOURCE} not found - ${BENCHMARK} will not be built")
    endif()
endforeach()
