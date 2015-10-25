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
list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Include safecass cmake utility macros
include (cmake/utils.cmake)
include (cmake/add_dl_project.cmake)

# Enable or disable casros (used by external project where SC is deployed)
#set (SAFECASS_ACTIVATED TRUE)

# Set the current project as casros
#set (CURRENT_PROJECT_IS_SC ON)

# CMake settings for output (generated) files
set (BUILD_ROOT             ${CMAKE_BINARY_DIR})
set (EXECUTABLE_OUTPUT_PATH ${BUILD_ROOT}/bin)
set (LIBRARY_OUTPUT_PATH    ${BUILD_ROOT}/libs)

# Aliasing paths
set (SAFECASS_BUILD_ROOT  ${BUILD_ROOT})
set (SAFECASS_SOURCE_ROOT ${CMAKE_SOURCE_DIR})
set (SAFECASS_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH}) # where to find safecass library (libsafecass)
set (SAFECASS_INCLUDE_DIR ${SAFECASS_SOURCE_ROOT}/libs) # where to find header files

# Misc settings
# Option to organize targets in folders (mostly for MSVS)
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Build library
add_subdirectory(libs)

# Option to enable unit tests
option (SAFECASS_BUILD_UNIT_TEST "Enable unit testing facility" OFF)
if (SAFECASS_BUILD_UNIT_TEST)
  # Enable ctest support
  enable_testing()
  include (CTest)

  # Add unit tests
  add_subdirectory(tests)
endif()

# Option to compile programs separately
#option (BUILD_TOOLS "Build tools.  Requires casros-enabled component-based framework." OFF)
#if (BUILD_TOOLS)
#  #add_subdirectory(programs)
#  add_subdirectory(tools)
#endif()

# Option to compile examples separately
#option (BUILD_EXAMPLES "Build examples.  Requires casros-enabled component-based framework." OFF)
#if (BUILD_EXAMPLES)
#  add_subdirectory(examples)
#endif ()

#if (BUILD_UNIT_TEST)
#  add_subdirectory(tests)
#endif ()



# Options to enable visualizataion framework using Qt and D3/cubism
#option (ENABLE_VIS_FRAMEWORK "Enable visualization framework using Qt and visualization packages" OFF)
#if (ENABLE_VIS_FRAMEWORK)
#    message("Visualization framework enabled")
#endif ()

# Option to use component-based software framework in robotics
# option (SAFECASS_HAS_CISST "Enable support for cisst. Requires the cisst package" ON)
# if (SAFECASS_HAS_CISST)
  # message("cisst support enabled")
  # # find cisst
  # set (CISST_LIBRARIES_REQUIRED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
  # find_package(cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

  # if (cisst_FOUND_AS_REQUIRED)
    # # Modify CMake configuration to use cisst
    # include (${CISST_USE_FILE})
    # # Print out cisst information
    # message("CISST ${CISST_VERSION} found: ${CISST_BINARY_DIR}")
    # add_definitions(-DSAFECASS_HAS_CISST)
    # #
    # # TODO
    # #
  # else ()
    # message(FATAL_ERROR "Failed to find cisst")
  # endif ()
# endif (SAFECASS_HAS_CISST)


# Create configuration files
# configure_file ("${SAFECASS_CMAKE_MODULE_PATH}/SCConfig.cmake.in"
                # "${SAFECASS_BUILD_ROOT}/SCConfig.cmake"
                # @ONLY)
