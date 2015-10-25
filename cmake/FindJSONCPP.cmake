#------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung
#
#------------------------------------------------------------------------
#
# Created on   : Oct 23, 2015
# Last revision: Oct 23, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/safecass/casros
#

#
# FindJsonCpp.cmake
#
# Looks for jsoncpp library and defines following cmake variables:
#
#   JSONCPP_FOUND
#   JSONCPP_INCLUDE_DIR
#   JSONCPP_LIBRARIES
#
#   https://github.com/open-source-parsers/jsoncpp
#

include (FindPackageHandleStandardArgs)

#set (JSONCPP_FOUND OFF)

if (SC_ON_MAC OR SC_ON_LINUX)
  set (JSONCPP_INCLUDE_DEFAULT_PATH
       /opt/local/include/json # mac port
       /usr/local/include
       /usr/include
       )
  set (JSONCPP_LIB_DEFAULT_PATH
       /opt/local/lib # mac port
       )
elseif (SC_ON_WINDOWS)
  # TODO
endif ()

find_path(JSONCPP_INCLUDE_DIR NAMES json.h
                              DOC "Directory containing json.h"
                              PATHS ${JSONCPP_INCLUDE_DEFAULT_PATH})
find_library(JSONCPP_LIBRARY NAMES jsoncpp
                             DOC "Path to jsoncpp library"
                             PATHS ${JSONCPP_LIB_DEFAULT_PATH})

# if (DEFINED JSONCPP_INCLUDE_DIR AND DEFINED JSONCPP_LIBRARY)
  # message("JSONCPP found")
  # set (JSONCPP_FOUND TRUE)
# else()
  # message("JSONCPP not found")
# endif ()

# if (JSONCPP_FOUND)
  # set(JSONCPP_INCLUDE_DIR ${JSONCPP_INCLUDE_DIR})# PARENT_SCOPE)
  # set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
# endif ()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(JSONCPP JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY)

if (JSONCPP_FOUND)
  set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
  # debug log
  message("JSONCPP found")
  message("JSONCPP include dir: ${JSONCPP_INCLUDE_DIR}")
  message("JSONCPP library: ${JSONCPP_LIBRARIES}")
else()
  message("JSONCPP not found")
  mark_as_advanced(JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY)
endif()
