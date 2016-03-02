#------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : Jul 8, 2012
# Last revision: Mar 2, 2016
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/safecass/safecass
#

#
# FindJsonCpp.cmake
#
# Looks for g2log library for high-performance and asynchronous logging
# and defines following cmake variables:
#
#   G2LOG_FOUND
#   G2LOG_INCLUDE_DIR
#   G2LOG_LIBRARIES
#
#   https://github.com/open-source-parsers/jsoncpp
#
# References:
#
#   - g2log library:
#     https://sites.google.com/site/kjellhedstrom2/g2log-efficient-background-io-processign-with-c11
#
include(FindPackageHandleStandardArgs)

if (SC_ON_MAC OR SC_ON_LINUX)
  set (G2LOG_INCLUDE_DEFAULT_PATH
       /opt/local/include # mac port
       /usr/local/include
       /usr/include
       )
  set (G2LOG_LIB_DEFAULT_PATH
       /opt/local/lib # mac port
       )
elseif (SC_ON_WINDOWS)
  # TODO
endif ()

find_path(G2LOG_INCLUDE_DIR NAMES g2log.h
                            DOC "Directory containing g2log.h"
                            PATHS ${G2LOG_INCLUDE_DEFAULT_PATH})

find_library(G2LOG_LIBRARY_CORE NAME lib_g2logger
                                DOC "Path to g2log library (lib_g2logger)"
                                PATHS ${G2LOG_LIB_DEFAULT_PATH})
find_library(G2LOG_LIBRARY_AUX NAME lib_activeobject
                               DOC "Path to g2log auxiliary library (lib_activeobject)"
                               PATHS ${G2LOG_LIB_DEFAULT_PATH})

find_package_handle_standard_args(G2LOG G2LOG_INCLUDE_DIR G2LOG_LIBRARY_CORE G2LOG_LIBRARY_AUX)

if (G2LOG_FOUND)
  set(G2LOG_LIBRARIES ${G2LOG_LIBRARY_CORE} ${G2LOG_LIBRARY_AUX})
else()
  message("G2LOG not found")
endif(G2LOG_FOUND)
