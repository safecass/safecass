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
# FindG2Log.cmake
#
# Looks for g2log library for high-performance, safe, and asynchronous logging 
# and defines following cmake variables:
#
#   G2LOG_FOUND
#   G2LOG_INCLUDE_DIR
#   G2LOG_LIBRARIES
#
# References:
#
#   - g2log library:
#     https://sites.google.com/site/kjellhedstrom2/g2log-efficient-background-io-processign-with-c11
#

find_path(G2LOG_INCLUDE_DIR NAMES g2log.h 
          DOC "Directory containing g2log header files (e.g., g2log.h)")
find_library(G2LOG_LIBRARY_CORE NAMES lib_g2logger
             DOC "Path to g2log library (lib_g2logger)")
find_library(G2LOG_LIBRARY_AUX NAMES lib_activeobject 
             DOC "Path to g2log auxiliary library (lib_activeobject)")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(G2LOG DEFAULT_MSG G2LOG_LIBRARY G2LOG_INCLUDE_DIR)

if(G2LOG_FOUND)
  set(G2LOG_LIBRARIES ${G2LOG_LIBRARY_CORE} ${G2LOG_LIBRARY_AUX} PARENT_SCOPE)
  set(G2LOG_INCLUDE_DIR ${G2LOG_INCLUDE_DIR} PARENT_SCOPE)
endif(G2LOG_FOUND)
