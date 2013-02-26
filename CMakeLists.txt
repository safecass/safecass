#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Project name
project(SafetyFramework)

# Set the version number
set (SF_VERSION_MAJOR "0")
set (SF_VERSION_MINOR "0")
set (SF_VERSION_PATCH "1")
set (SF_VERSION "${SF_VERSION_MAJOR}.${SF_VERSION_MINOR}.${SF_VERSION_PATCH}")

# Set the current project as Safety Framework 
set (CURRENT_PROJECT_IS_SF ON)

# Project settings for output (generated) files
set (BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set (EXECUTABLE_OUTPUT_PATH "${BUILD_ROOT}/bin")
set (LIBRARY_OUTPUT_PATH "${BUILD_ROOT}/lib")

set_property (GLOBAL PROPERTY USE_FOLDERS ON) # option to organize targets in folders

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
option (ENABLE_UNIT_TEST "Enable and generate unit test suites. Requires the CppTest library" ON)
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
option (ENABLE_VIS_FRAMEWORK "Enable visualization framework using Qt and visualization packages" ON)
if (ENABLE_VIS_FRAMEWORK)
    message("Visualization framework enabled")
endif ()

# Option to compile examples separately
option (BUILD_EXAMPLES "Build examples.  Requires safety framework-enabled middleware." OFF)

# Include sub-directories
add_subdirectory(libs)
add_subdirectory(programs)
add_subdirectory(tools)
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
