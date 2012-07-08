#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Project name
project(SafetyFramework)

# Project settings for output files
set (BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set (EXECUTABLE_OUTPUT_PATH "${BUILD_ROOT}/bin")
set (LIBRARY_OUTPUT_PATH "${BUILD_ROOT}/lib")

set_property (GLOBAL PROPERTY USE_FOLDERS ON) # option to organize targets in folders

# shared library support
# MJ TODO: deal with shared libs later
#set (IS_SHARED OFF)

# Set where to look for header files
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${SafetyFramework_SOURCE_DIR}/libs/Common")
include_directories("${SafetyFramework_SOURCE_DIR}/libs/FDD")
include_directories("${SafetyFramework_SOURCE_DIR}/libs/FDD/filters")

# Options to enable unit-testing with CppTest
option (ENABLE_UNIT_TEST "Enable and generate unit test suites. Requires the CppTest library" ON)
# Options to use g2log
option (ENABLE_G2LOG "Enable and use g2log as logging framework. Requires the g2log library" OFF)

# Options to use robot middleware packages
option (SF_HAS_CISST "Enable support for cisst. Requires the cisst package" ON)
if (SF_HAS_CISST)
    message("cisst support enabled")
    # find cisst
    set (CISST_LIBRARIES_REQUIRED cisstCommon cisstVector cisstOSAbstraction
                                  cisstMultiTask cisstParameterTypes)
    find_package(cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

    if (cisst_FOUND_AS_REQUIRED)
        # Modify CMake configuration to use cisst
        include (${CISST_USE_FILE})
        # Print out cisst information
        message("CISST found (ver. ${CISST_VERSION}): ${CISST_BINARY_DIR}")
        add_definitions(-DSF_HAS_CISST)
        #
        # TODO
        #
    else (cisst_FOUND_AS_REQUIRED)
        message(FATAL_ERROR "Failed to find cisst")
    endif (cisst_FOUND_AS_REQUIRED)
endif (SF_HAS_CISST)

# Include sub-directories
add_subdirectory(libs)
#add_subdirectory(plugins)
#add_subdirectory(tools)
add_subdirectory(examples)
if (ENABLE_UNIT_TEST)
  message("Unit-test enabled")
  add_subdirectory(tests)
endif (ENABLE_UNIT_TEST)
