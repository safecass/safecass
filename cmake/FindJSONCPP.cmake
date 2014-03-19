#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung
#
#------------------------------------------------------------------------
#
# Created on   : Sep 24, 2012
# Last revision: Mar 19, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#

#
# FindJsonCpp.cmake
#
# Looks for jsoncpp library for JSON integration and defines following
# cmake variables:
#
#   JSONCPP_FOUND
#   JSONCPP_INCLUDE_DIR
#   JSONCPP_LIBRARIES
#
# References:
#
#   - JSON: http://www.json.org
#   - jsoncpp library: 
#       http://sourceforge.net/projects/jsoncpp
#       http://jsoncpp.sourceforge.net
#

set (JSONCPP_FOUND OFF)

if (SF_ON_MAC)
  set (JSONCPP_INCLUDE_DEFAULT_PATH "/opt/local/include/json") # mac port
  set (JSONCPP_LIB_DEFAULT_PATH "/opt/local/lib/") # mac port
elseif (SF_ON_WINDOWS)
  # TODO
elseif (SF_ON_LINUX)
  # TODO
endif ()

find_path(JSONCPP_INCLUDE_DIR NAMES json.h 
                              DOC "Directory containing json.h"
                              PATHS ${JSONCPP_INCLUDE_DEFAULT_PATH})
find_library(JSONCPP_LIBRARY NAMES jsoncpp 
                             DOC "Path to jsoncpp library"
                             PATHS ${JSONCPP_LIB_DEFAULT_PATH})

if (DEFINED ${JSONCPP_INCLUDE_DIR} AND DEFINED ${JSONCPP_LIBRARY})
  message("JSONCPP found")
  set (JSONCPP_FOUND TRUE)
else()
  message("JSONCPP not found -- will be downloaded and compiled during build")
endif ()

if (JSONCPP_FOUND)
  set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
  set(JSONCPP_INCLUDE_DIR ${JSONCPP_INCLUDE_DIR})# PARENT_SCOPE)
else ()
  # if not found, try downloading and building JSON automatically
  include (ExternalProject)
  set (JSONCPP_SVN_REPOSITORY svn://svn.code.sf.net/p/jsoncpp/code/trunk/jsoncpp)
  set (JSONCPP_SVN_REVISION 276)
  ExternalProject_Add (jsoncppExternal
                       PREFIX casrosJSON
                       SVN_REPOSITORY ${JSONCPP_SVN_REPOSITORY}
                       SVN_REVISION   -r ${JSONCPP_SVN_REVISION}
                       CMAKE_CACHE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                                        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                                        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
                                        -DOPTION_BUILD_EXAMPLES:BOOL=OFF
                                        -DJSONCPP_WITH_TESTS:BOOL=OFF
                                        -DJSONCPP_LIB_BUILD_SHARED:BOOL=ON
                                        -DJSONCPP_WITH_POST_BUILD_UNITTEST:BOOL=OFF
                                        -DCMAKE_INSTALL_PREFIX:FILEPATH=${SF_BUILD_ROOT}/external/json
                       INSTALL_DIR ${SF_BUILD_ROOT}/external/json
                       )
  
  # Set all JSON variables based on install directory
  set (JSONCPP_INCLUDE_DIR "${SF_BUILD_ROOT}/external/json/include")
  set (JSONCPP_INCLUDE_DIR ${JSONCPP_INCLUDE_DIR} PARENT_SCOPE)

  set (JSONCPP_LIBRARY_DIRS "${SF_BUILD_ROOT}/external/json/lib")
  set (JSONCPP_LIBRARY_DIRS ${JSONCPP_LIBRARY_DIRS} PARENT_SCOPE)
  set (JSONCPP_LIBRARIES jsoncpp)
  set (JSONCPP_LIBRARIES ${JSONCPP_LIBRARIES} PARENT_SCOPE)

  set (JSONCPP_FOUND ON)
  set (JSONCPP_FOUND ON PARENT_SCOPE)

  message(STATUS "JSONCPP include dir: ${JSONCPP_INCLUDE_DIR}")
  message(STATUS "JSONCPP libs:${JSONCPP_LIBRARIES}")
  message(STATUS "JSONCPP library dir:${JSONCPP_LIBRARY_DIRS}")
endif ()
