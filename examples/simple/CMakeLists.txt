#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung, Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : July 31, 2012
# Last revision: March 19, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#
project(simple)

set (REQUIRED_SC_LIBS SCLib ${JSONCPP_LIBRARIES})
if (ENABLE_G2LOG)
  set (REQUIRED_SC_LIBS ${REQUIRED_SC_LIBS} ${G2LOG_LIBRARIES})
endif (ENABLE_G2LOG)

set (INCLUDE_DIR ${SC_LIB_INCLUDE_DIR})

if (SC_HAS_CISST)
  set (REQUIRED_CISST_LIBS cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

  # find cisst and make sure the required libraries have been compiled
  find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

  if (cisst_FOUND_AS_REQUIRED)
    set (INCLUDE_DIR ${INCLUDE_DIR} ${cisst_INCLUDE_DIR})
    include_directories (${INCLUDE_DIR})
    add_executable (simple main_cisst.cpp)
    cisst_target_link_libraries(simple ${REQUIRED_CISST_LIBS})
    target_link_libraries (simple ${REQUIRED_SC_LIBS})
  else (cisst_FOUND_AS_REQUIRED)
    cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
  endif (cisst_FOUND_AS_REQUIRED)
endif (SC_HAS_CISST)
