#---------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
#
#---------------------------------------------------------------------------------
#
# Created on   : Jul 6, 2012
# Last revision: Apr 20, 2016
# Author       : Min Yang Jung <myj@jhu.edu>
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

# Define dependencies
set (TEST_DEPENDENCY # safecass libs
                     common
                     safecass
                     # 3rd party libs
                     ${GLOG_LIBRARIES}
                     ${Boost_LIBRARIES}
                     gtest
                     # TODO This may need to be update when adding support for shared library
                     jsoncpp_lib_static)
# Boost.Chrono uses clock_gettime, which requires rt on Linux
# Note: Since glibc 2.17, -rt is no longer required because clock_* functions
# are now part of the main C library.  Older glibc < 2.17, -lrt is still required
# (how to check: ldd --version)
if (SAFECASS_ON_LINUX)
  list (APPEND TEST_DEPENDENCY rt)
endif()

# Define test cases and register each test to test suites
file(GLOB TEST_SUITES ${SAFECASS_SOURCE_ROOT}/tests/test*.cpp)
foreach (TEST ${TEST_SUITES})
  # Extract file name
  get_filename_component (TEST_FILE_NAME ${TEST} NAME)
  get_filename_component (TEST_UNIT ${TEST} NAME_WE)
  # Define test unit
  add_executable (${TEST_UNIT} main.cpp ${TEST_FILE_NAME})
  # Define dependencies
  target_link_libraries (${TEST_UNIT} ${TEST_DEPENDENCY})
  target_include_directories(${TEST_UNIT} INTERFACE ${SAFECASS_LIBRARY_INCLUDE_DIR})
  # Register to CTest
  add_ctest(${TEST_UNIT})
endforeach()
