# Additional CMake configuration file for building the C++ supernovas++
# library
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DENABLE_CPP_LIBS=ON option.
#
# Author: Attila Kovacs


FILE(GLOB CPP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(cpp ${CPP_SOURCES})
add_library(supernovas::cpp ALIAS cpp)

set_target_properties(cpp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME supernovas++
)

target_include_directories(cpp PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_property(TARGET cpp PROPERTY CXX_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(cpp PRIVATE -x c++)
endif()

# Link with math libs and core SuperNOVAS as necessary
target_link_libraries(cpp PUBLIC core)


