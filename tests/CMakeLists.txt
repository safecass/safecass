#---------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
#
#---------------------------------------------------------------------------------
#
# Created on   : Jul 6, 2012
# Last revision: Sep 21, 2016
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
               URL      https://github.com/google/googletest/archive/release-1.7.0.zip
               URL_HASH SHA1=F89BC9F55477DF2FDE082481E2D709BFAFDB057B
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
# TODO: If cmake can detect glibc version, this can be improved such that rt
# dependency is defined only for older glibc (< 2.17)
# In the meantime, add rt dependency anyway to avoid linker failure with older glibc.
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

  # Set compiler flags on Linux to resolve linker errors
  # http://stackoverflow.com/questions/17150075/undefined-reference-to-clock-gettime-although-lrt-is-given
  if (SAFECASS_ON_LINUX)
    set_target_properties(${TEST_UNIT} PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")
  endif()

  # Define dependencies
  target_include_directories(${TEST_UNIT} INTERFACE ${SAFECASS_LIBRARY_INCLUDE_DIR})
  target_link_libraries (${TEST_UNIT} ${TEST_DEPENDENCY})

  # Register to CTest
  add_ctest(${TEST_UNIT})
endforeach()

# add_executable (boost boost.cpp)
# target_include_directories(boost INTERFACE ${SAFECASS_LIBRARY_INCLUDE_DIR})
# target_link_libraries (boost ${TEST_DEPENDENCY})
