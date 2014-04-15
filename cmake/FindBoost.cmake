#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung
#
#------------------------------------------------------------------------
#
# Created on   : Apr 15, 2014
# Last revision: Apr 15, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#

#
# FindBoost.cmake
#
# Looks for Boost libraries and define cmake variables for each library.
# Boost libraries used: Meta State Machine, Program Options
#
#   BOOST_FOUND: If all sub-libraries are found
#   BOOST_INCLUDE_DIR: Path to boost include dir (e.g., /opt/local/include on macports)
#   BOOST_LIB_PROGRAM_OPTION: Boost program option library
#
# References:
#
#   [1] Boost Meta State Machine (MSM):
#       http://www.boost.org/doc/libs/1_55_0/libs/msm/doc/HTML/index.html
#   [2] Boost Program Options:
#       http://www.boost.org/doc/libs/1_55_0/doc/html/program_options/tutorial.html
#

# TODO:
#   read environment var BOOST_ROOT and skip manual setting if possible
#
#   (?) other preprocessor symbols:
#     add_definitions(-DBOOST_ALL_NO_LIB) : disable auto-linking (?)       
# 
# In case of a Shared Boost install (dlls), you should then enable this
#if (BUILD_SHARED_LIBS)
#    add_definitions(-DBOOST_ALL_DYN_LINK)
#    # Explicitly tell find-package to search for Static Boost libs (if needed)
#    set(Boost_USE_STATIC_LIBS OFF) 
#else (BUILD_SHARED_LIBS)
#    set(Boost_USE_STATIC_LIBS ON) 
#endif (BUILD_SHARED_LIBS)

# Set default search paths
# TODO: expand default search paths on other platforms
if (SF_ON_MAC)
  set (BOOST_INCLUDE_DEFAULT_PATH "/opt/local/include") # mac port
  set (BOOST_LIBS_DEFAULT_PATH "/opt/local/lib/") # mac port
elseif (SF_ON_WINDOWS)
  #
elseif (SF_ON_LINUX)
  #
endif ()

find_path (BOOST_INCLUDE_DIR NAMES boost/msm/common.hpp 
                             DOC "Directory containing boost/msm/common.hpp"
                             PATHS ${BOOST_INCLUDE_DEFAULT_PATH})

if (ENABLE_UNIT_TEST)
  find_library (BOOST_LIB_PROGRAM_OPTION NAMES boost_program_options boost_program_options-mt
                                        DOC "Path to Boost program option library"
                                        HINTS ${BOOST_LIBS_DEFAULT_PATH})
endif()

include (FindPackageHandleStandardArgs)
if (ENABLE_UNIT_TEST)
  find_package_handle_standard_args(BOOST DEFAULT_MSG BOOST_INCLUDE_DIR BOOST_LIB_PROGRAM_OPTION)
else()
  find_package_handle_standard_args(BOOST DEFAULT_MSG BOOST_INCLUDE_DIR)
endif()

if (NOT BOOST_FOUND)
  message(FATAL_ERROR "Boost include directory not found (was looking for boost/msm/common.hpp")
  return()
endif ()

message("Boost libraries found")
message(STATUS "BOOST_INCLUDE_DIR: ${BOOST_INCLUDE_DIR}")
message(STATUS "BOOST_FOUND: ${BOOST_FOUND}")
if (ENABLE_UNIT_TEST)
  message(STATUS "BOOST_LIB_PROGRAM_OPTION: ${BOOST_LIB_PROGRAM_OPTION}")
endif()
