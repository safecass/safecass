#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
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

find_path (ZEROC_ICE_INCLUDE_DIR NAMES Ice/Ice.h DOC "Directory containing Ice/Ice.h")
find_library (ZEROC_ICE_LIBRARY_ICE NAMES Ice DOC "Path to Ice libraries")
find_library (ZEROC_ICE_LIBRARY_ICEUTIL NAMES IceUtil DOC "Path to IceUtil libraries")
find_library (ZEROC_ICE_LIBRARY_ICESTORM NAMES IceStorm DOC "Path to IceStorm libraries")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZEROC_ICE DEFAULT_MSG 
    ZEROC_ICE_INCLUDE_DIR ZEROC_ICE_LIBRARY_ICE ZEROC_ICE_LIBRARY_ICEUTIL ZEROC_ICE_LIBRARY_ICESTORM)

if (ZEROC_ICE_FOUND)
    set (ZEROC_ICE_HOME "${ZEROC_ICE_INCLUDE_DIR}/..")
    set (ZEROC_ICE_LIBRARIES ${ZEROC_ICE_LIBRARY_ICE} ${ZEROC_ICE_LIBRARY_ICEUTIL}
                             ${ZEROC_ICE_LIBRARY_ICESTORM})
    set (ZEROC_ICE_INCLUDE_DIR ${ZEROC_ICE_INCLUDE_DIR})
    set (ZEROC_SLICE2CPP "${ZEROC_ICE_HOME}/bin/slice2cpp")
    set (ZEROC_SLICE_INCLUDE_DIR "${ZEROC_ICE_HOME}/../slice")
    #message(">>>>> ZEROC_ICE_FOUND: ${ZEROC_ICE_FOUND}")
    #message(">>>>> ZEROC_ICE_HOME: ${ZEROC_ICE_HOME}")
    #message(">>>>> ZEROC_ICE_INCLUDE_DIR: ${ZEROC_ICE_INCLUDE_DIR}")
    #message(">>>>> ZEROC_ICE_LIBRARIES: ${ZEROC_ICE_LIBRARIES}")
    #message(">>>>> ZEROC_SLICE2CPP: ${ZEROC_SLICE2CPP}")
    #message(">>>>> ZEROC_SLICE_INCLUDE_DIR: ${ZEROC_SLICE_INCLUDE_DIR}")
    message("ZeroC ICE found")
else (ZEROC_ICE_FOUND)
    message(FATAL_ERROR "Failed to find ZeroC Ice")
endif(ZEROC_ICE_FOUND)
