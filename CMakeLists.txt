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

# option to organize targets in folders
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

#add_subdirectory(libs)
#add_subdirectory(plugins)
#add_subdirectory(tools)
add_subdirectory(examples)
