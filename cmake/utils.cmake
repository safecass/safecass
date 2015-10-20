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
set (SC_ON_MAC     FALSE)
set (SC_ON_WINDOWS FALSE)
set (SC_ON_LINUX   FALSE)

# Mac/Apple
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set (SC_ON_MAC TRUE)
  message ("OS: Apple/Mac detected")
elseif (UNIX)
  set (SC_ON_LINUX TRUE)
  message ("OS: Linux detected")
elseif (WIN32)
  set (SC_ON_WINDOWS TRUE)
  message ("OS: Windows detected")
endif()

if (SC_ON_MAC)
elseif (SC_ON_WINDOWS)
elseif (SC_ON_LINUX)
endif()

#
# Determine compiler
#
set (SC_COMPILER "")
# Check if clang
string (FIND ${CMAKE_CXX_COMPILER_ID} "Clang" CLANG_DETECTED)
if (NOT ${CLANG_DETECTED} MATCHES -1)
  set (SC_COMPILER "Clang")
# Check if gcc
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set (SC_COMPILER "GCC")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set (SC_COMPILER "MSVC")
else()
  message(FATAL_ERROR "Failed to determine compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

message ("Compiler: ${SC_COMPILER} detected")
