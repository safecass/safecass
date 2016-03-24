#------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : Mar 2, 2016
# Last revision: Mar 23, 2016
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/safecass/safecass
#

#
# FindGLog.cmake
#
# Looks for Google log library and defines following cmake variables:
#
#   GLOG_FOUND
#   GLOG_INCLUDE_DIRS
#   GLOG_LIBRARIES
#
# References:
#
#   - Github project: https://github.com/google/glog
#   - How to use Google logging library: https://google-glog.googlecode.com/svn/trunk/doc/glog.html
#
include(FindPackageHandleStandardArgs)

if (SC_ON_MAC OR SC_ON_LINUX)
  set (GLOG_INCLUDE_DEFAULT_PATH
       /opt/local/include # mac port
       /usr/local/include
       /usr/include
       )
  set (GLOG_LIB_DEFAULT_PATH
       /opt/local/lib # mac port
       )
elseif (SC_ON_WINDOWS)
  # TODO
endif ()

find_path(GLOG_INCLUDE_DIR NAMES glog/logging.h
                           DOC "Directory containing glog/logging.h"
                           PATHS ${GLOG_INCLUDE_DEFAULT_PATH})

find_library(GLOG_LIBRARY NAME glog
                          DOC "Path to glog library (libglog)"
                          PATHS ${GLOG_LIB_DEFAULT_PATH})
                          #PATH_SUFFIXES lib lib64)

find_package_handle_standard_args(GLOG GLOG_INCLUDE_DIR GLOG_LIBRARY)

if (GLOG_FOUND)
  set(GLOG_LIBRARIES ${GLOG_LIBRARY})
  set(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
else()
  message("GLOG not found")
endif(GLOG_FOUND)
