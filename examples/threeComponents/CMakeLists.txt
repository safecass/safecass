#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2015 Min Yang Jung, Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : April 7, 2015
# Last revision: April 9, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#
project(threeCmponents)

set (REQUIRED_SF_LIBS SFLib ${JSONCPP_LIBRARIES})
if (ENABLE_G2LOG)
  set (REQUIRED_SF_LIBS ${REQUIRED_SF_LIBS} ${G2LOG_LIBRARIES})
endif (ENABLE_G2LOG)

set (INCLUDE_DIR ${SF_LIB_INCLUDE_DIR})

if (SF_HAS_CISST)
  # find cisst and make sure the required libraries have been compiled
  find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

  if (cisst_FOUND_AS_REQUIRED)
    add_definitions(-DJSON_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}")

    set (INCLUDE_DIR ${INCLUDE_DIR} ${cisst_INCLUDE_DIR})
    include_directories (${INCLUDE_DIR})
    add_executable (threeComponents main_cisst.cpp components.h components.cpp)

    set (REQUIRED_CISST_LIBS cisstCommon cisstOSAbstraction cisstMultiTask)
    cisst_target_link_libraries(threeComponents ${REQUIRED_CISST_LIBS})

    target_link_libraries (threeComponents ${REQUIRED_SF_LIBS})
  else (cisst_FOUND_AS_REQUIRED)
    cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
  endif (cisst_FOUND_AS_REQUIRED)
endif (SF_HAS_CISST)
