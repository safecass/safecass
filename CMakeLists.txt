#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : Jul 1, 2012
# Last revision: Jul 29, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Project name
project(SafetyFramework)

# Set the version number
set (SF_VERSION_MAJOR "0")
set (SF_VERSION_MINOR "0")
set (SF_VERSION_PATCH "1")
set (SF_VERSION "${SF_VERSION_MAJOR}.${SF_VERSION_MINOR}.${SF_VERSION_PATCH}")

# Enable or disable casros (used by external project where SF is deployed)
set (SF_ACTIVATED TRUE)

# Set the current project as casros
set (CURRENT_PROJECT_IS_SF ON)

# Project settings for output (generated) files
set (BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set (EXECUTABLE_OUTPUT_PATH "${BUILD_ROOT}/bin")
set (LIBRARY_OUTPUT_PATH "${BUILD_ROOT}/lib")

# OS recognition
set (SF_ON_MAC     FALSE)
set (SF_ON_WINDOWS FALSE)
set (SF_ON_LINUX   FALSE)
# Mac/Apple
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set (SF_ON_MAC TRUE)
  message("OS: Apple/Mac detected")
else ()
  if (UNIX)
    set (SF_ON_LINUX TRUE)
    message("OS: Linux detected")
  endif ()
endif ()
# Windows
if (WIN32)
  set (SF_ON_WINDOWS TRUE)
  message("OS: Windows detected")
endif ()
# QNX
#if ("${CMAKE_SYSTEM_NAME}" strequal "QNX")
#    set (SF_ON_QNX ON)
#else if (UNIX)

# Detecting compiler
set (SF_COMPILER "")
if (SF_ON_WINDOWS)
  if (MINGW) # MinGW compiler in Windows
    set (SF_COMPILER "MinGW") 
  elseif (MSYS) # MSYS developer environment in Windows
    set (SF_COMPILER "MSYS")
  elseif (BORLAND) # Borland compiler in Windows
    set (SF_COMPILER "Borland")
  elseif (WATCOM) # Open Watcom compiler in Windows
    set (SF_COMPILER "Watcom")
    #elseif (MSVC OR MSVC_IDE OR MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 OR
    #        CMAKE_COMPILER_2005 OR MSVC90) # Microsoft Visual Studio
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set (SF_COMPILER "MSVC")
  endif()
else (SF_ON_WINDOWS)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (SF_COMPILER "Clang")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set (SF_COMPILER "GCC")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set (SF_COMPILER "Intel")
    #if (CMAKE_COMPILER_IS_GNUCC) # variant of GCC
    #      set (SF_COMPILER "gcc variant")
    #  elseif (CMAKE_COMPILER_IS_GNUCXX) # variant of g++
    #      set (SF_COMPILER "g++ variant")
  elseif (CYGWIN) # Cygwin
    set (SF_COMPILER "Cygwin")
  endif()
endif (SF_ON_WINDOWS)

message("Compiler: ${SF_COMPILER} detected")

# option to organize targets in folders
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Project settings for source files
set (SF_BUILD_ROOT ${BUILD_ROOT})
set (SF_SOURCE_DIR ${SafetyFramework_SOURCE_DIR})
set (SF_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH}) # where to find SFLib build library
set (SF_INCLUDE_DIR "${SF_SOURCE_DIR}/libs") # where to find header files

# Set cmake module path
set (SF_CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SF_CMAKE_MODULE_PATH})

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
  set (SF_USE_G2LOG ${ENABLE_G2LOG})
endif()
# Option to build shared library
option (BUILD_SHARED_LIBS "Build shared libraries" OFF)
if (BUILD_SHARED_LIBS)
  set (LIB_TYPE SHARED)
elseif ()
  set (LIB_TYPE STATIC)
endif ()

# Option to use robot middleware packages
option (SF_HAS_CISST "Enable support for cisst. Requires the cisst package" ON)
if (SF_HAS_CISST)
  message("cisst support enabled")
  # find cisst
  set (CISST_LIBRARIES_REQUIRED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
  find_package(cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

  if (cisst_FOUND_AS_REQUIRED)
    # Modify CMake configuration to use cisst
    include (${CISST_USE_FILE})
    # Print out cisst information
    message("CISST ${CISST_VERSION} found: ${CISST_BINARY_DIR}")
    add_definitions(-DSF_HAS_CISST)
    #
    # TODO
    #
  else ()
    message(FATAL_ERROR "Failed to find cisst")
  endif ()
endif (SF_HAS_CISST)

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
    set (SF_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

# Create configuration files
configure_file ("${SF_CMAKE_MODULE_PATH}/SFConfig.cmake.in"
                "${SF_BUILD_ROOT}/SFConfig.cmake"
                @ONLY)
