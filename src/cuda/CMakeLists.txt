

if(NOT DEFINED CMAKE_CUDA_STANDARD)
    set(CMAKE_CUDA_STANDARD 11)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()

# Additional CMake configuration file for building the C++ supernovas++
# library
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DENABLE_CPP_LIBS=ON option.
#
# Author: Attila Kovacs


FILE(GLOB CUDA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cu)

function(list_prepend var prefix)
    set(temp "")
    foreach(f ${${var}})
        list(APPEND temp "${prefix}${f}")
    endforeach()
    set(${var} "${temp}" PARENT_SCOPE)
endfunction()

add_library(cuda ${CUDA_SOURCES})
add_library(supernovas::cuda ALIAS cpp)

set_target_properties(cuda PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME supernovas-cuda
)

target_include_directories(cpp PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Link with math libs and core SuperNOVAS as necessary
target_link_libraries(cuda PUBLIC core)
set_target_properties(cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

 string(APPEND PC_LIBS "-lsupernovas++ ")
