#-----------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
#
#-----------------------------------------------------------------------------------
#
# Created on   : Oct 18, 2015
# Last revision: Oct 18, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
# URL          : https://github.com/safecass/safecass
#

#
# Determine OS
#
set (SAFECASS_ON_MAC     FALSE)
set (SAFECASS_ON_WINDOWS FALSE)
set (SAFECASS_ON_LINUX   FALSE)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set (SAFECASS_ON_MAC TRUE)
  message ("OS: Apple/Mac detected")
elseif (UNIX)
  set (SAFECASS_ON_LINUX TRUE)
  message ("OS: Linux detected")
elseif (WIN32)
  set (SAFECASS_ON_WINDOWS TRUE)
  message ("OS: Windows detected")
endif()

# if (SAFECASS_ON_MAC)
# elseif (SAFECASS_ON_WINDOWS)
# elseif (SAFECASS_ON_LINUX)
# endif()

#
# Determine compiler
#
# This might be useful:
# http://stackoverflow.com/questions/19774778/when-is-it-necessary-to-use-use-the-flag-stdlib-libstdc
set (SAFECASS_COMPILER_NAME  "")
set (SAFECASS_COMPILER_CLANG FALSE)
set (SAFECASS_COMPILER_GCC   FALSE)
set (SAFECASS_COMPILER_MSVC  FALSE)

# Check if clang
string (FIND ${CMAKE_CXX_COMPILER_ID} "Clang" CLANG_DETECTED)
if (NOT ${CLANG_DETECTED} MATCHES -1)
  set (SAFECASS_COMPILER_NAME "Clang")
  set (SAFECASS_COMPILER_CLANG TRUE)
# Check if gcc
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (SAFECASS_COMPILER_NAME "GCC")
  set (SAFECASS_COMPILER_GCC TRUE)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set (SAFECASS_COMPILER_NAME "MSVC")
  set (SAFECASS_COMPILER_MSVC TRUE)
else()
  message(FATAL_ERROR "Failed to determine compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

#include(CheckCXXCompilerFlag)
#check_cxx_compiler_flag(-std=c++11 COMPILER_SUPPORTS_CXX11)
#check_cxx_compiler_flag(-std=c++0x COMPILER_SUPPORTS_CXX0X)
#if (COMPILER_SUPPORTS_CXX11)
#  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#elseif (COMPILER_SUPPORTS_CXX0X)
#  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#else()
#  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
#endif()

message ("Compiler: ${SAFECASS_COMPILER_NAME} detected")

#
# Determine number of processors available
#
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
  set(CTEST_BUILD_FLAGS -j${N})
  set(ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${N})
  message("Number of processors/cores detected: ${N}")
endif()
