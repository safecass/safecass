#---------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
#
#---------------------------------------------------------------------------------
#
# Created on   : Jul 6, 2012
# Last revision: Mar 28, 2016
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/safecass/safecass
#
project (sctest)

message (STATUS "Unit-tests enabled")

# No need to find glog here (GLOG settings are available at the top cmake level)

# load safecass macro to help ctest test entry registration
include (add_ctest)

# Unit testing requires gtest; download, build, and import gtest in the build tree
add_dl_project(PROJ     gtest
               URL      https://googletest.googlecode.com/files/gtest-1.7.0.zip
               URL_HASH SHA1=f85f6d2481e2c6c4a18539e391aa4ea8ab0394af
               EXCLUDE_FROM_ALL)
target_include_directories(gtest INTERFACE
                           ${SAFECASS_BUILD_ROOT}/external_packages/gtest/src/include)

# Define test suites
add_executable (testParams main.cpp testParams.cpp)
add_executable (testSignalElement main.cpp testSignalElement.cpp)
add_executable (testInput main.cpp testInput.cpp)
add_executable (testState main.cpp testState.cpp)

# Register each test to test suites
set (TEST_SUITES testParams testSignalElement testInput testState)

# Set up for each test suite
foreach (TEST ${TEST_SUITES})
  target_link_libraries (${TEST} # safecass libs
                                 common safecass
                                 # 3rd party libs
                                 ${GLOG_LIBRARIES}
                                 ${Boost_LIBRARIES}
                                 # TODO Replace raw library name with cmake string var
                                 gtest
                                 jsoncpp_lib_static)

  target_include_directories(${TEST} INTERFACE ${SAFECASS_LIBRARY_INCLUDE_DIR})
  # Register to CTest
  add_ctest(${TEST})
endforeach()


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
