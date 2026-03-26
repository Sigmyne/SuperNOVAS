# CMake configuration file for the SuperNOVAS library
#
# Authors: Kiran Shila, Attila Kovacs
#
# Usage: 
#   
#  To build under a 'build/' directory:
#
#    $ cmake -B build [OPTIONS]
#    $ cmake --build build
#  
#  And then to install, e.g. under '/usr/local':
#    $ [sudo ] cmake --install --prefix=/usr/local
#
#  You may install just a specific component via the --component option, such
#  as 'Runtime' or 'Development'.
#


cmake_minimum_required(VERSION 3.24...4.2)

# Project definition
project(supernovas
    VERSION 1.6.0
    DESCRIPTION "SuperNOVAS C/C++ astrometry library"
    HOMEPAGE_URL "https://sigmyne.github.io/SuperNOVAS/"
    LANGUAGES C CXX
)

# Include required modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FeatureSummary)

# Build options
option(BUILD_SHARED_LIBS "Build as shared libraries instead of static" OFF)
option(BUILD_DOC "Build HTML documetation" OFF)
option(BUILD_TESTING "Build regression test suite" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)
option(BUILD_BENCHMARK "Build benchmark programs" OFF)
option(ENABLE_CPP "Enable C++ library build (supernovas++ library)" OFF)
option(ENABLE_CALCEPH "Enable CALCEPH support (solsys-calceph plugin)" OFF)
option(ENABLE_CSPICE "Enable CSPICE support (solsys-cspice plugin)" OFF)


# Set feature descriptions for summary
add_feature_info(SharedLibs BUILD_SHARED_LIBS "Build as shared libraries")
add_feature_info(Documentation BUILD_DOC "Developer documentation (HTML)")
add_feature_info(Examples BUILD_EXAMPLES "Build and test example programs")
add_feature_info(Testing BUILD_TESTING "Run regression testing")
add_feature_info(Benchmarks BUILD_BENCHMARK "Build benchmarking programs")

add_feature_info(C++-Libs ENABLE_CPP "Build C++ library also (supernovas++)")
add_feature_info(Calceph-Plugin ENABLE_CALCEPH "Optional ephemeris support via CALCEPH (solsys-calceph)")
add_feature_info(CSPICE-Plugin ENABLE_CSPICE "Optional ephemeris support via CSPICE (solsys-cspice)")


# ----------------------------------------------------------------------------
# pkg-config variables

# [.pc] Libs -- linker flags for apps
set(PC_LIBS "-lsupernovas ")

# [.pc] Libs.private -- linker flags for supernovas itself
set(PC_LIBS_PRIVATE "")


# ----------------------------------------------------------------------------
# Global build configuration

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Debug postfix for libraries
set(CMAKE_DEBUG_POSTFIX d)

# Build time locations for sub-configs
link_directories(${CMAKE_BINARY_DIR}/lib)
set(supernovas_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)

if(WIN32)
    # Windows-specific settings
    message("Configuring for Windows...")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

if(WIN32)
    set(MATH "")
else()
    find_library(MATH_LIBRARY m REQUIRED)
    set(MATH m)
    
    # [.pc] Link applications against math lib...
    string(APPEND PC_LIBS_PRIVATE -l${MATH} " ")
endif()

if(ENABLE_CALCEPH OR ENABLE_CSPICE)
    find_package(Threads)
    
    if(CMAKE_THREAD_LIBS_INIT)
        # [.pc] apps should be linked against threads libs also...
        string(APPEND PC_LIBS_PRIVATE ${CMAKE_THREAD_LIBS_INIT} " ")
    endif()
endif()

# ----------------------------------------------------------------------------
# Extenal plugin configuration

# CALCEPH plugin
if(ENABLE_CALCEPH)
    find_package(calceph COMPONENTS Development REQUIRED CONFIG)
    set_package_properties(calceph PROPERTIES
        URL "https://calceph.imcce.fr"
        DESCRIPTION  "is designed to access the binary planetary ephemeris files"
        TYPE REQUIRED
        PURPOSE "Provides ephemeris support for supernovas"
    )
    string(APPEND PC_LIBS "-lsolsys-calceph ")
    string(APPEND PC_LIBS_PRIVATE "-lcalceph ")
endif()

# CSPICE plugin
if(ENABLE_CSPICE)
    find_library(cspice cspice REGISTRY_VIEW TARGET REQUIRED)
    string(APPEND PC_LIBS "-lsolsys-cspice ")
    string(APPEND PC_LIBS_PRIVATE "-lcspice ")
endif()

# ----------------------------------------------------------------------------
# Link applications against the C++ library also, if enabled

if(ENABLE_CPP)
    string(APPEND PC_LIBS "-lsupernovas++ ")
endif()

# ----------------------------------------------------------------------------
# Build libraries from source

add_subdirectory(src)

# ----------------------------------------------------------------------------
# Testing, examples, and benchmarks...

# Run tests for the enabled options below...
enable_testing()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

# Benchmarking
if(BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()

# Documentation
if(BUILD_DOC) 
    add_subdirectory(doc)
endif()

# ----------------------------------------------------------------------------
# Install Runtime component

set(INSTALL_TARGETS core)
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME Runtime)

if(ENABLE_CALCEPH)
    list(APPEND INSTALL_TARGETS solsys-calceph)
endif()

if(ENABLE_CSPICE)
    list(APPEND INSTALL_TARGETS solsys-cspice)
endif()

if(ENABLE_CPP)
    list(APPEND INSTALL_TARGETS cpp)
endif()

install(TARGETS ${INSTALL_TARGETS} EXPORT supernovasTargets)


# ------------------------------------------------------------------------
# Install Development component

install(FILES include/novas.h
    TYPE INCLUDE
    COMPONENT Development
)

if(ENABLE_CALCEPH)
    install(FILES include/novas-calceph.h
        TYPE INCLUDE
        COMPONENT Development
    )
endif()

if(ENABLE_CSPICE)
    install(FILES include/novas-cspice.h
        TYPE INCLUDE
        COMPONENT Development
    )
endif()

if(ENABLE_CPP)
    install(FILES include/supernovas.h
        TYPE INCLUDE
        COMPONENT Development
    )
endif()

# Install markdown files
install(FILES CHANGELOG.md CONTRIBUTING.md
    TYPE DOC
    COMPONENT Development   
)

# For examples/ dir, list just the directory, not the individual files installed.
install(CODE "message(STATUS \"Installing: <prefix>/${CMAKE_INSTALL_DOCDIR}/examples/\")")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples
    TYPE DOC
    MESSAGE_NEVER
    COMPONENT Development
)

# For legacy/ dir, list just the directory, not the individual files installed.
install(CODE "message(STATUS \"Installing: <prefix>/${CMAKE_INSTALL_DOCDIR}/legacy/*\")")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/legacy
    TYPE DOC
    MESSAGE_NEVER
    COMPONENT Development
)

install(EXPORT supernovasTargets
    COMPONENT Development
    FILE supernovasTargets.cmake
    NAMESPACE supernovas::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/supernovas
)


# ------------------------------------------------------------------------
# Package config

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/supernovasConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/supernovasConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/supernovas
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/supernovasConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/supernovasConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/supernovasConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/supernovas
    COMPONENT Development
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/supernovas.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/supernovas.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/supernovas.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT Development
)

# ----------------------------------------------------------------------------
# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

# ----------------------------------------------------------------------------
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE supernovas)


# ----------------------------------------------------------------------------
# Summary
feature_summary(WHAT ALL)

message(" - Compile flags added with CMAKE_BUILD_TYPE:")
message("   Debug:             ${CMAKE_C_FLAGS_DEBUG}")
message("   Release:           ${CMAKE_C_FLAGS_RELEASE}")
message("   RelWithDebInfo:    ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message("   MinSizeRel:        ${CMAKE_C_FLAGS_MINSIZEREL}")
message("")

message(" - supernovas Configuration Summary:")
message("   Version:           ${PROJECT_VERSION}")
if(CMAKE_BUILD_TYPE)
    message("   Build type:        ${CMAKE_BUILD_TYPE}")
else()
    message("   Build type:        (default) -- external CFLAGS only")
endif()
message("   Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message("   Compiler:          ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message("")

