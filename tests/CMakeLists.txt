#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : Jul 6, 2012
# Last revision: Jun 25, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#
project (tests)

message("Unit-tests enabled")

# Find CppTest
find_package (CPPTEST REQUIRED)
if (CPPTEST_FOUND)
  message ("CppTest found")
elseif ()
  message (FATAL_ERROR "JSONCPP not found")
endif ()

set (UNIT_TEST_INCLUDE_DIR ${SF_LIB_INCLUDE_DIR} ${CPPTEST_INCLUDE_DIR})
set (UNIT_TEST_SRC testJson.h testJson.cpp)

# Enable cisst-specific unit-tests if cisst is ON
if (SF_HAS_CISST)
  set (REQUIRED_CISST_LIBS cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

  # find cisst and make sure the required libraries have been compiled
  find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)
  if (NOT cisst_FOUND_AS_REQUIRED)
    cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
    return()
  endif()

  set (UNIT_TEST_INCLUDE_DIR ${INCLUDE_DIR} ${INCLUDE_DIR} ${cisst_INCLUDE_DIR})
  set (UNIT_TEST_SRC ${UNIT_TEST_SRC} 
                     #testGCM.h testGCM.cpp
                     #GCMHelper.h GCMHelper.cpp
                     testState.h testState.cpp)
endif (SF_HAS_CISST)

# Define project settings
include_directories (${UNIT_TEST_INCLUDE_DIR})

add_executable (tests main.cpp ${UNIT_TEST_SRC})

target_link_libraries (tests SFLib ${SF_LIB_DEPENDENCY_LIBS} 
                                   ${CPPTEST_LIBRARY}
                                   ${BOOST_LIB_PROGRAM_OPTION})
if (SF_HAS_CISST)
  cisst_target_link_libraries(tests ${REQUIRED_CISST_LIBS})
endif()
