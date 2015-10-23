#-----------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
#
#-----------------------------------------------------------------------------------
#
# Created on   : Jul 1, 2012
# Last revision: Oct 18, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
# URL          : https://github.com/safecass/safecass
#

# Minimum 3.0.0 required to support download-and-build external packages
cmake_minimum_required (VERSION 3.0.0)

# Project name
project (SAFECASS)

# Set the version number
set (SAFECASS_VERSION_MAJOR "0")
set (SAFECASS_VERSION_MINOR "1")
set (SAFECASS_VERSION_PATCH "0")
set (SAFECASS_VERSION "${SAFECASS_VERSION_MAJOR}.${SAFECASS_VERSION_MINOR}.${SAFECASS_VERSION_PATCH}")

# Set cmake module path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Include safecass cmake utility macros
include (cmake/utils.cmake)
include (cmake/add_dl_project.cmake)

# Enable or disable casros (used by external project where SC is deployed)
#set (SAFECASS_ACTIVATED TRUE)

# Set the current project as casros
#set (CURRENT_PROJECT_IS_SC ON)

# Project settings for output (generated) files
set (BUILD_ROOT             ${CMAKE_BINARY_DIR})
set (EXECUTABLE_OUTPUT_PATH ${BUILD_ROOT}/bin)
set (LIBRARY_OUTPUT_PATH    ${BUILD_ROOT}/libs)

# Aliasing paths
set (SAFECASS_BUILD_ROOT  ${BUILD_ROOT})
set (SAFECASS_SOURCE_ROOT ${CMAKE_SOURCE_DIR})
set (SAFECASS_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH}) # where to find safecass library (libsafecass)
set (SAFECASS_INCLUDE_DIR ${SAFECASS_SOURCE_ROOT}/libs) # where to find header files

# Option to build shared library
option (SAFECASS_BUILD_SHARED_LIBS "Build shared libraries" OFF)

#
# Setting up required dependencies
#
# JSONCPP
add_dl_project(PROJ jsoncpp
               URL  https://github.com/open-source-parsers/jsoncpp/archive/1.6.5.tar.gz
               #CMAKE_OPTIONS "-DJSONCPP_WITH_CMAKE_PACKAGE:BOOL=ON"#"-DBUILD_SHARED_LIBS:BOOL=OFF;-DBUILD_TESTING:BOOL=OFF"
               EXCLUDE_FROM_ALL)
# To enable this, SAFECASS shared lib should be selected first, and then enable
# jsoncpp_lib.
#if (SAFECASS_BUILD_SHARED_LIBS)
#  target_include_directories(jsoncpp_lib INTERFACE "${SAFECASS_BUILD_ROOT}/jsoncpp/src/include")
#endif()
target_include_directories(jsoncpp_lib_static INTERFACE "${SAFECASS_BUILD_ROOT}/jsoncpp/src/include")

# Boost: Check if environment variable (BOOST_ROOT) is defined
#if (NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
#  set(BOOST_ROOT $ENV{BOOST_ROOT})
#  message("BOOST_ROOT is found from environment variable: ${BOOST_ROOT}")
#endif()

#
# Build SAFECASS library
#
#add_subdirectory(libs)

#
# Optional dependencies
#

# Option to use external logger (g2log)
option (SAFECASS_USE_G2LOG "Enable and use g2log as logging framework. Requires the g2log library" OFF)
if (SAFECASS_USE_G2LOG)
  # Download and import gtest
  add_dl_project(PROJ         g2log_by_kjellkod
                 URL          https://bitbucket.org/KjellKod/g2log/get/tip.zip
                 PATH_SUFFIX  g2log
                 EXCLUDE_FROM_ALL)
  target_include_directories(lib_g2logger INTERFACE "${CMAKE_BINARY_DIR}/g2log/src/g2log/src")
  target_include_directories(lib_g2logger_shared INTERFACE "${CMAKE_BINARY_DIR}/g2log/src/g2log/src")
endif()

# Option to enable unit tests
option (SAFECASS_BUILD_UNIT_TEST "Enable and generate unit test suites." OFF)
if (SAFECASS_BUILD_UNIT_TEST)
  # Enable ctest support
  enable_testing()
  include (CTest)

  # Add unit tests
  add_subdirectory(tests)
endif()

#if (BUILD_TOOLS)
#  #add_subdirectory(programs)
#  add_subdirectory(tools)
#endif()
#if (BUILD_EXAMPLES)
#  add_subdirectory(examples)
#endif ()
#if (BUILD_UNIT_TEST)
#  add_subdirectory(tests)
#endif ()


#
# Misc. option
#
# option to organize targets in folders
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

return()





# Options to enable visualizataion framework using Qt and D3/cubism
option (ENABLE_VIS_FRAMEWORK "Enable visualization framework using Qt and visualization packages" OFF)
if (ENABLE_VIS_FRAMEWORK)
    message("Visualization framework enabled")
endif ()

# Option to compile examples separately
option (BUILD_EXAMPLES "Build examples.  Requires casros-enabled component-based framework." OFF)

# Option to compile programs separately
option (BUILD_TOOLS "Build tools.  Requires casros-enabled component-based framework." OFF)

# Option to use component-based software framework in robotics
option (SAFECASS_HAS_CISST "Enable support for cisst. Requires the cisst package" ON)
if (SAFECASS_HAS_CISST)
  message("cisst support enabled")
  # find cisst
  set (CISST_LIBRARIES_REQUIRED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
  find_package(cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

  if (cisst_FOUND_AS_REQUIRED)
    # Modify CMake configuration to use cisst
    include (${CISST_USE_FILE})
    # Print out cisst information
    message("CISST ${CISST_VERSION} found: ${CISST_BINARY_DIR}")
    add_definitions(-DSAFECASS_HAS_CISST)
    #
    # TODO
    #
  else ()
    message(FATAL_ERROR "Failed to find cisst")
  endif ()
endif (SAFECASS_HAS_CISST)


# Update compiler configuration accordingly
if (SAFECASS_USE_G2LOG)
  set (SAFECASS_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

# Create configuration files
configure_file ("${SAFECASS_CMAKE_MODULE_PATH}/SCConfig.cmake.in"
                "${SAFECASS_BUILD_ROOT}/SCConfig.cmake"
                @ONLY)
