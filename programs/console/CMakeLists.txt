#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2014 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : May 7, 2014
# Last revision: Jul 23, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#
# console is a command line interpreter utility for casros.  It provides basic
# features to inspect running systems, but also enables interacting with the
# system with casros.
#
# This project uses the Clipo project as the front-end, a command line interpreter 
# based on the boost Program Option library in C++.  The Clipo project URL is
# https://code.google.com/p/clipo .
# 
project(console)

if (SF_HAS_CISST)
  set (REQUIRED_CISST_LIBS cisstCommon cisstOSAbstraction)
  set (REQUIRED_SF_LIBS SFLib)

  # find cisst and make sure the required libraries have been compiled
  find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)
  #find_package (Curses REQUIRED QUIET)

  if (cisst_FOUND_AS_REQUIRED)
    include_directories (${SF_LIB_INCLUDE_DIR})
    add_executable (console main_cisst.cpp
                            # casros network accessor
                            accessor.h accessor.cpp
                            # command handlers
                            cmd_filter.cpp 
                            cmd_state.cpp
                            cmd_event.cpp
                            cmd_connection.cpp
                            cmd_service.cpp)
    cisst_target_link_libraries(console ${REQUIRED_CISST_LIBS})
    target_link_libraries (console ${REQUIRED_SF_LIBS} 
                                   ${SF_LIB_DEPENDENCY_LIBS}
                                   ${BOOST_LIB_PROGRAM_OPTION})# ncurses)
  else ()
    cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
  endif ()
endif (SF_HAS_CISST)
