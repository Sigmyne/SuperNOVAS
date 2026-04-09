# Additional CMake configuration file for building the C99 supernovas library
#
# To invoke simply configure the cmake build in the supernovas directory.
#
# Author: Attila Kovacs

# ----------------------------------------------------------------------------
# Build the 'supernovas' library as a static OR as a shared lib.
# depending on BUILD_SHARED_LIBS

add_library(core ${SUPERNOVAS_CORE_SOURCES})
add_library(supernovas::core ALIAS core)

set_target_properties(core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME supernovas
)

target_include_directories(core PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Link with math libs as necessary
if(MATH_LIB)
    target_link_libraries(core PUBLIC ${MATH})
endif()

# ----------------------------------------------------------------------------
# Plugins

# plugins via external libraries (e.g. calceph or cspice). 
function(solsys_plugin EPHEM_LIB_NAME)
    set(PLUGIN solsys-${EPHEM_LIB_NAME})
    
    add_library(${PLUGIN} ${PLUGIN}.c)
    add_library(supernovas::${PLUGIN} ALIAS ${PLUGIN})

    set_target_properties(${PLUGIN} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    if(CMAKE_USE_PTHREADS_INIT)
        # if Threads is provided via pthread, then compile source accordingly
        target_compile_definitions(${PLUGIN} PRIVATE SUPERNOVAS_USE_PTHREAD)
    endif()
    
    target_include_directories(${PLUGIN} PRIVATE ${supernovas_INCLUDE_DIRS})
    
    find_library(EPHEM_LIBRARY ${EPHEM_LIB_NAME} REQUIRED)
    
    target_link_libraries(solsys-${EPHEM_LIB_NAME} PUBLIC
        supernovas::core
        ${EPHEM_LIBRARY}
        ${MATH}
        Threads::Threads
    )
endfunction()

# Process plugin options...
if(ENABLE_CALCEPH)
    solsys_plugin(calceph)
    find_path(CALCEPH_INCLUDE_DIR "calceph.h" REQUIRED)
    target_include_directories(solsys-calceph PRIVATE "${CALCEPH_INCLUDE_DIR}")
endif()

if(ENABLE_CSPICE)
    solsys_plugin(cspice)
    find_path(CSPICE_INCLUDE_DIR "cspice/SpiceUsr.h" REQUIRED)
    target_include_directories(solsys-cspice PRIVATE "${CSPICE_INCLUDE_DIR}")
endif()

