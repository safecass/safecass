#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung
#
#------------------------------------------------------------------------
#
# Created on   : July 31, 2012
# Last revision: March 19, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#

#
# FindIce.cmake
#
# Looks for ZeroC ICE library for inter-process communication and defines 
# following cmake variables:
#
#   ZEROC_ICE_FOUND
#   ZEROC_ICE_HOME
#   ZEROC_ICE_INCLUDE_DIR
#   ZEROC_ICE_LIBRARIES
#   ZEROC_SLICE2CPP
#   ZEROC_SLICE_INCLUDE_DIR
#
# References:
#
#   - ZeroC ICE: http://www.zeroc.com
#

# define ice versions
set (ICE_VERSIONS 3.3.0 3.3.1
                  3.4.0 3.4.1
                  3.5.0 3.5.1)

# set default search paths
# TODO: expand default search paths on other platforms
set (ICE_DEFAULT_PATH)
if (SF_ON_MAC)
  set (ICE_INCLUDE_DEFAULT_PATH "/opt/local/include") # mac port
  set (ICE_LIB_DEFAULT_PATH "/opt/local/lib/") # mac port
elseif (SF_ON_WINDOWS)
  # TODO
elseif (SF_ON_LINUX)
  # TODO
endif ()

find_path (ZEROC_ICE_INCLUDE_DIR NAMES Ice/Ice.h 
                                 DOC "Directory containing Ice/Ice.h"
                                 PATHS ${ICE_INCLUDE_DEFAULT_PATH})
find_library (ZEROC_ICE_LIBRARY_ICE NAMES Ice ZerocIce
                                    DOC "Path to Ice libraries"
                                    HINTS ${ICE_LIB_DEFAULT_PATH})
find_library (ZEROC_ICE_LIBRARY_ICEUTIL NAMES IceUtil ZerocIceUtil
                                        DOC "Path to IceUtil libraries"
                                        HINTS ${ICE_LIB_DEFAULT_PATH})
find_library (ZEROC_ICE_LIBRARY_ICESTORM NAMES IceStorm ZerocIceStorm
                                         DOC "Path to IceStorm libraries"
                                         HINTS ${ICE_LIB_DEFAULT_PATH})

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZEROC_ICE 
  DEFAULT_MSG 
  ZEROC_ICE_INCLUDE_DIR 
  ZEROC_ICE_LIBRARY_ICE 
  ZEROC_ICE_LIBRARY_ICEUTIL 
  ZEROC_ICE_LIBRARY_ICESTORM)

if (ZEROC_ICE_FOUND)
  set (ZEROC_ICE_HOME "${ZEROC_ICE_INCLUDE_DIR}/..")
  set (ZEROC_ICE_LIBRARIES ${ZEROC_ICE_LIBRARY_ICE} ${ZEROC_ICE_LIBRARY_ICEUTIL}
                            ${ZEROC_ICE_LIBRARY_ICESTORM})
  set (ZEROC_ICE_INCLUDE_DIR ${ZEROC_ICE_INCLUDE_DIR})
  set (ZEROC_SLICE2CPP "${ZEROC_ICE_HOME}/bin/slice2cpp")
  set (ZEROC_SLICE_INCLUDE_DIR "${ZEROC_ICE_HOME}/../slice")
  message(STATUS "ZEROC_ICE_FOUND: ${ZEROC_ICE_FOUND}")
  message(STATUS "ZEROC_ICE_HOME: ${ZEROC_ICE_HOME}")
  message(STATUS "ZEROC_ICE_INCLUDE_DIR: ${ZEROC_ICE_INCLUDE_DIR}")
  message(STATUS "ZEROC_ICE_LIBRARIES: ${ZEROC_ICE_LIBRARIES}")
  message(STATUS "ZEROC_SLICE2CPP: ${ZEROC_SLICE2CPP}")
  message(STATUS "ZEROC_SLICE_INCLUDE_DIR: ${ZEROC_SLICE_INCLUDE_DIR}")
  message("ZeroC ICE found")
else (ZEROC_ICE_FOUND)
  message(FATAL_ERROR "Failed to find ZeroC Ice")
endif(ZEROC_ICE_FOUND)
