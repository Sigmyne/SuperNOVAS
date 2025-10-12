# Additional CMake configuration file for building the C++ supernovas++
# library
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_CPP_LIBS=ON option.
#
# Author: Attila Kovacs


FILE(GLOB CPP_SOURCES *.cpp)

set(C_SOURCES ${SUPERNOVAS_CORE_SOURCES})

function(list_prepend var prefix)
    set(temp "")
    foreach(f ${${var}})
        list(APPEND temp "${prefix}${f}")
    endforeach()
    set(${var} "${temp}" PARENT_SCOPE)
endfunction()

list_prepend(C_SOURCES "../")

SET(ALL_SOURCES ${CPP_SOURCES} ${C_SOURCES})

set_source_files_properties(${ALL_SOURCES}
    PROPERTIES LANGUAGE CXX)
    
add_library(supernovas-cpp ${ALL_SOURCES})

set_target_properties(supernovas-cpp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME supernovas++
    OUTPUT_NAME supernovas++
    CLEAN_DIRECT_OUTPUT ON
)

target_include_directories(supernovas-cpp PRIVATE
    ${PROJECT_SOURCE_DIR}/include/eop
    ${PROJECT_SOURCE_DIR}/include/nutation
)

target_include_directories(supernovas-cpp PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(supernovas-cpp PRIVATE NOVAS_NAMESPACE)
    
set_property(TARGET supernovas-cpp PROPERTY CXX_STANDARD 17)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(supernovas-cpp PRIVATE -x c++)
endif()

# Link with math libs as necessary
target_link_libraries(supernovas-cpp PRIVATE $<$<BOOL:${HAVE_LIBM}>:m>)
