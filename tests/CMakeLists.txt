#------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : Jul 6, 2012
# Last revision: Oct 19, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/safecass/safecass 
#
project (sctest)

message (STATUS "SAFECASS: Unit-tests enabled")

# load safecass macro to help ctest test entry registration
include (${CMAKE_SOURCE_DIR}/cmake/add_ctest.cmake)

# Unit testing requires gtest; download and import gtest
add_dl_project(PROJ     gtest
               URL      https://googletest.googlecode.com/files/gtest-1.7.0.zip
               URL_HASH SHA1=f85f6d2481e2c6c4a18539e391aa4ea8ab0394af
               EXCLUDE_FROM_ALL)
target_include_directories(gtest INTERFACE
                           ${SAFECASS_BUILD_ROOT}/external_packages/gtest/src/include)


#set (UNIT_TEST_SRC test_json.h test_json.cpp)

#include_directories (${SC_INCLUDE_DIR}/common)

add_executable (sctest_json main.cpp ${UNIT_TEST_SRC})

target_link_libraries (sctest_json gtest)

add_ctest(sctest_json)

return()




# Find CppTest
find_package (CPPTEST REQUIRED)
if (CPPTEST_FOUND)
  message ("CppTest found")
elseif ()
  message (FATAL_ERROR "JSONCPP not found")
endif ()

set (UNIT_TEST_INCLUDE_DIR ${SC_LIB_INCLUDE_DIR} ${CPPTEST_INCLUDE_DIR})
set (UNIT_TEST_SRC testJson.h testJson.cpp)

# Enable cisst-specific unit-tests if cisst is ON
if (SC_HAS_CISST)
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
                     #testJson.h testJson.cpp
                     testState.h testState.cpp
                     testUtil.h testUtil.cpp)
endif (SC_HAS_CISST)

# Define project settings
include_directories (${UNIT_TEST_INCLUDE_DIR})

add_executable (tests main.cpp ${UNIT_TEST_SRC})

target_link_libraries (tests SCLib ${SC_LIB_DEPENDENCY_LIBS} 
                                   ${CPPTEST_LIBRARY}
                                   ${BOOST_LIB_PROGRAM_OPTION})
if (SC_HAS_CISST)
  cisst_target_link_libraries(tests ${REQUIRED_CISST_LIBS})
endif()
