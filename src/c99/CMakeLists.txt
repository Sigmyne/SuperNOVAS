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
target_link_libraries(core PUBLIC ${MATHLIB})

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
        core
        ${EPHEM_LIBRARY}
        Threads::Threads
    )
endfunction()

# Process plugin options...
if(ENABLE_CALCEPH)
    find_package(calceph COMPONENTS Development REQUIRED CONFIG)
    set_package_properties(calceph PROPERTIES
        URL "https://calceph.imcce.fr"
        DESCRIPTION  "is designed to access the binary planetary ephemeris files"
        TYPE REQUIRED
        PURPOSE "Provides ephemeris support for supernovas"
    )

    solsys_plugin(calceph)
    find_path(CALCEPH_INCLUDE_DIR "calceph.h" REQUIRED)
    target_include_directories(solsys-calceph PRIVATE "${CALCEPH_INCLUDE_DIR}")
    string(APPEND PC_LIBS "-lsolsys-calceph ")
    string(APPEND PC_LIBS_PRIVATE "-lcalceph ")
endif()

if(ENABLE_CSPICE)
    find_library(cspice cspice REGISTRY_VIEW TARGET REQUIRED)


    solsys_plugin(cspice)
    find_path(CSPICE_INCLUDE_DIR "cspice/SpiceUsr.h" REQUIRED)
    target_include_directories(solsys-cspice PRIVATE "${CSPICE_INCLUDE_DIR}")
    string(APPEND PC_LIBS "-lsolsys-cspice ")
    string(APPEND PC_LIBS_PRIVATE "-lcspice ")
endif()

