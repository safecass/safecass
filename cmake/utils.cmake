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
set (SAFECASS_COMPILER "")
# Check if clang
string (FIND ${CMAKE_CXX_COMPILER_ID} "Clang" CLANG_DETECTED)
if (NOT ${CLANG_DETECTED} MATCHES -1)
  set (SAFECASS_COMPILER "Clang")
# Check if gcc
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (SAFECASS_COMPILER "GCC")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set (SAFECASS_COMPILER "MSVC")
else()
  message(FATAL_ERROR "Failed to determine compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

message ("Compiler: ${SAFECASS_COMPILER} detected")

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
