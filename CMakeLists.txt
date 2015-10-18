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
set (SC_VERSION_MAJOR "0")
set (SC_VERSION_MINOR "1")
set (SC_VERSION_PATCH "0")
set (SC_VERSION "${SC_VERSION_MAJOR}.${SC_VERSION_MINOR}.${SC_VERSION_PATCH}")

# Set cmake module path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Include utility macros (e.g., detecting OS, ...)
include (cmake/utils.cmake)

# Enable or disable casros (used by external project where SC is deployed)
#set (SC_ACTIVATED TRUE)

# Set the current project as casros
#set (CURRENT_PROJECT_IS_SC ON)

# Project settings for output (generated) files
set (BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set (EXECUTABLE_OUTPUT_PATH "${BUILD_ROOT}/bin")
set (LIBRARY_OUTPUT_PATH "${BUILD_ROOT}/lib")

# option to organize targets in folders
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Project settings for source files
set (SC_BUILD_ROOT ${BUILD_ROOT})
set (SC_SOURCE_ROOT ${CMAKE_SOURCE_DIR})
set (SC_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH})   # where to find safecass library (libsafecass)
set (SC_INCLUDE_DIR "${SC_SOURCE_ROOT}/libs") # where to find header files

#
# Handle external packages
#
# Boost: Check if environment variable (BOOST_ROOT) is defined
if (NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
  set(BOOST_ROOT $ENV{BOOST_ROOT})
  message("BOOST_ROOT is found from environment variable: ${BOOST_ROOT}")
endif()

# Google Test:


# Option to enable unit-testing with CppTest
option (ENABLE_UNIT_TEST "Enable and generate unit test suites. Requires the CppTest library" OFF)
# Option to use g2log
option (ENABLE_G2LOG "Enable and use g2log as logging framework. Requires the g2log library" OFF)
if (ENABLE_G2LOG)
  set (SC_USE_G2LOG ${ENABLE_G2LOG})
endif()

#
# Build options
#
# Option to build shared library
option (BUILD_SHARED_LIBS "Build shared libraries" OFF)
if (BUILD_SHARED_LIBS)
  set (SC_LIB_TYPE SHARED)
elseif ()
  set (SC_LIB_TYPE STATIC)
endif ()

# Option to use robot middleware packages
option (SC_HAS_CISST "Enable support for cisst. Requires the cisst package" ON)
if (SC_HAS_CISST)
  message("cisst support enabled")
  # find cisst
  set (CISST_LIBRARIES_REQUIRED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
  find_package(cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

  if (cisst_FOUND_AS_REQUIRED)
    # Modify CMake configuration to use cisst
    include (${CISST_USE_FILE})
    # Print out cisst information
    message("CISST ${CISST_VERSION} found: ${CISST_BINARY_DIR}")
    add_definitions(-DSC_HAS_CISST)
    #
    # TODO
    #
  else ()
    message(FATAL_ERROR "Failed to find cisst")
  endif ()
endif (SC_HAS_CISST)

# Options to enable visualizataion framework using Qt and D3/cubism
option (ENABLE_VIS_FRAMEWORK "Enable visualization framework using Qt and visualization packages" OFF)
if (ENABLE_VIS_FRAMEWORK)
    message("Visualization framework enabled")
endif ()

# Option to compile examples separately
option (BUILD_EXAMPLES "Build examples.  Requires casros-enabled component-based framework." OFF)
# Option to compile programs separately
option (BUILD_TOOLS "Build tools.  Requires casros-enabled component-based framework." OFF)

# Include sub-directories
add_subdirectory(libs)
if (BUILD_TOOLS)
  #add_subdirectory(programs)
  add_subdirectory(tools)
endif()
if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()
if (ENABLE_UNIT_TEST)
  add_subdirectory(tests)
endif ()

# Update compiler configuration accordingly
if (ENABLE_G2LOG)
    set (SC_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

# Create configuration files
configure_file ("${SC_CMAKE_MODULE_PATH}/SCConfig.cmake.in"
                "${SC_BUILD_ROOT}/SCConfig.cmake"
                @ONLY)
