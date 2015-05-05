#-----------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
#
#-----------------------------------------------------------------------------------
#
# Created on   : Jul 1, 2012
# Last revision: May 4, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
#
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Project name
project(SAFECASS)

# Set the version number
set (SC_VERSION_MAJOR "0")
set (SC_VERSION_MINOR "0")
set (SC_VERSION_PATCH "1")
set (SC_VERSION "${SC_VERSION_MAJOR}.${SC_VERSION_MINOR}.${SC_VERSION_PATCH}")

# Enable or disable casros (used by external project where SC is deployed)
set (SC_ACTIVATED TRUE)

# Set the current project as casros
set (CURRENT_PROJECT_IS_SC ON)

# Project settings for output (generated) files
set (BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set (EXECUTABLE_OUTPUT_PATH "${BUILD_ROOT}/bin")
set (LIBRARY_OUTPUT_PATH "${BUILD_ROOT}/lib")

# OS recognition
set (SC_ON_MAC     FALSE)
set (SC_ON_WINDOWS FALSE)
set (SC_ON_LINUX   FALSE)
# Mac/Apple
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set (SC_ON_MAC TRUE)
  message("OS: Apple/Mac detected")
else ()
  if (UNIX)
    set (SC_ON_LINUX TRUE)
    message("OS: Linux detected")
  endif ()
endif ()
# Windows
if (WIN32)
  set (SC_ON_WINDOWS TRUE)
  message("OS: Windows detected")
endif ()
# QNX
#if ("${CMAKE_SYSTEM_NAME}" strequal "QNX")
#    set (SC_ON_QNX ON)
#else if (UNIX)

# Detecting compiler
set (SC_COMPILER "")
if (SC_ON_WINDOWS)
  if (MINGW) # MinGW compiler in Windows
    set (SC_COMPILER "MinGW") 
  elseif (MSYS) # MSYS developer environment in Windows
    set (SC_COMPILER "MSYS")
  elseif (BORLAND) # Borland compiler in Windows
    set (SC_COMPILER "Borland")
  elseif (WATCOM) # Open Watcom compiler in Windows
    set (SC_COMPILER "Watcom")
    #elseif (MSVC OR MSVC_IDE OR MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 OR
    #        CMAKE_COMPILER_2005 OR MSVC90) # Microsoft Visual Studio
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set (SC_COMPILER "MSVC")
  endif()
else (SC_ON_WINDOWS)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (SC_COMPILER "Clang")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set (SC_COMPILER "GCC")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set (SC_COMPILER "Intel")
    #if (CMAKE_COMPILER_IS_GNUCC) # variant of GCC
    #      set (SC_COMPILER "gcc variant")
    #  elseif (CMAKE_COMPILER_IS_GNUCXX) # variant of g++
    #      set (SC_COMPILER "g++ variant")
  elseif (CYGWIN) # Cygwin
    set (SC_COMPILER "Cygwin")
  endif()
endif (SC_ON_WINDOWS)

message("Compiler: ${SC_COMPILER} detected")

# option to organize targets in folders
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Project settings for source files
set (SC_BUILD_ROOT ${BUILD_ROOT})
set (SC_SOURCE_DIR ${Safecass_SOURCE_DIR})
set (SC_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH}) # where to find SCLib build library
set (SC_INCLUDE_DIR "${SC_SOURCE_DIR}/libs") # where to find header files

# Set cmake module path
set (SC_CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SC_CMAKE_MODULE_PATH})

# If BOOST_ROOT was defined in the environment, use it.
if (NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
  set(BOOST_ROOT $ENV{BOOST_ROOT})
else ()
  if (NOT BOOST_ROOT AND NOT $ENV{BOOSTROOT} STREQUAL "")
    set(BOOST_ROOT $ENV{BOOSTROOT})
  endif()
endif()

if (BOOST_ROOT)
  message("BOOST_ROOT is set as ${BOOST_ROOT}")
endif ()

# Option to enable unit-testing with CppTest
option (ENABLE_UNIT_TEST "Enable and generate unit test suites. Requires the CppTest library" OFF)
# Option to use g2log
option (ENABLE_G2LOG "Enable and use g2log as logging framework. Requires the g2log library" OFF)
if (ENABLE_G2LOG)
  set (SC_USE_G2LOG ${ENABLE_G2LOG})
endif()
# Option to build shared library
option (BUILD_SHARED_LIBS "Build shared libraries" OFF)
if (BUILD_SHARED_LIBS)
  set (LIB_TYPE SHARED)
elseif ()
  set (LIB_TYPE STATIC)
endif ()

# Option to use robot middleware packages
option (SC_HAS_CISST "Enable support for cisst. Requires the cisst package" ON)
if (SC_HAS_CISST)
  message("cisst support enabled")
  # find cisst
  set (CISST_LIBRARIES_REQUIRED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
  find_package(cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

  if (cisst_FOUND_AS_REQUIRED)
    # Modify CMake configuration to use cisst
    include (${CISST_USE_FILE})
    # Print out cisst information
    message("CISST ${CISST_VERSION} found: ${CISST_BINARY_DIR}")
    add_definitions(-DSC_HAS_CISST)
    #
    # TODO
    #
  else ()
    message(FATAL_ERROR "Failed to find cisst")
  endif ()
endif (SC_HAS_CISST)

# Options to enable visualizataion framework using Qt and D3/cubism
option (ENABLE_VIS_FRAMEWORK "Enable visualization framework using Qt and visualization packages" OFF)
if (ENABLE_VIS_FRAMEWORK)
    message("Visualization framework enabled")
endif ()

# Option to compile examples separately
option (BUILD_EXAMPLES "Build examples.  Requires casros-enabled component-based framework." OFF)
# Option to compile programs separately
option (BUILD_TOOLS "Build tools.  Requires casros-enabled component-based framework." OFF)

# Include sub-directories
add_subdirectory(libs)
if (BUILD_TOOLS)
  #add_subdirectory(programs)
  add_subdirectory(tools)
endif()
if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()
if (ENABLE_UNIT_TEST)
  add_subdirectory(tests)
endif ()

# Update compiler configuration accordingly
if (ENABLE_G2LOG)
    set (SC_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

# Create configuration files
configure_file ("${SC_CMAKE_MODULE_PATH}/SCConfig.cmake.in"
                "${SC_BUILD_ROOT}/SCConfig.cmake"
                @ONLY)
