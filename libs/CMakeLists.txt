#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

# Project name
project(SFLib)

# Project settings
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")

# set cmake module path
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Set group files
set (LIB_DIR ${PROJECT_SOURCE_DIR})

# common
set (SF_LIB_COMMON_SRC ${LIB_DIR}/common/common.h
                       ${LIB_DIR}/common/common.cpp
                       ${LIB_DIR}/common/json.h
                       ${LIB_DIR}/common/json.cpp)
set (SF_LIB_COMMON_INCLUDE_DIR ${LIB_DIR}/common)

# monitor
set (SF_LIB_MONITOR_SRC ${LIB_DIR}/monitor/monitor.h
                        ${LIB_DIR}/monitor/monitor.cpp)
set (SF_LIB_MONITOR_INCLUDE_DIR ${LIB_DIR}/monitor)

# FDD
#set (SF_LIB_FDD_SRC ${LIB_DIR}/FDD/fdd.h)
set (SF_LIB_FDD_INCLUDE_DIR ${LIB_DIR}/FDD)

# Collect files and paths
set (SF_LIB_SRC ${SF_LIB_COMMON_SRC} 
                ${SF_LIB_MONITOR_SRC} 
                ${SF_LIB_FDD_SRC})
set (SF_LIB_INCLUDE_DIR ${SF_LIB_COMMON_INCLUDE_DIR}
                        ${SF_LIB_MONITOR_INCLUDE_DIR}
                        ${SF_LIB_FDD_INCLUDE_DIR})

# Set project-wide paths
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)
set (SF_LIB_INCLUDE_DIR ${SF_LIB_INCLUDE_DIR} ${CMAKE_BINARY_DIR})

# Find JSON (for JSON-based data exchange)
find_package (JSONCPP REQUIRED)
if (JSONCPP_FOUND)
  set (SF_LIB_INCLUDE_DIR ${SF_LIB_INCLUDE_DIR} ${JSONCPP_INCLUDE_DIR})
elseif (JSONCPP_FOUND)
  message (FATAL_ERROR "JSONCPP not found")
endif (JSONCPP_FOUND)

# Find g2log (for high-performance, safe, asynchronous logging)
if (ENABLE_G2LOG)
  find_package (G2LOG REQUIRED)
  if (G2LOG_FOUND)
    message ("g2log found")
    set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11" PARENT_SCOPE)
    set (SF_LIB_INCLUDE_DIR ${SF_LIB_INCLUDE_DIR} ${G2LOG_INCLUDE_DIR})
  else (G2LOG_FOUND)
    message (FATAL_ERROR "g2log not found")
  endif (G2LOG_FOUND)
endif (ENABLE_G2LOG)

set (SF_LIB_INCLUDE_DIR ${SF_LIB_INCLUDE_DIR} PARENT_SCOPE)
include_directories (${SF_LIB_INCLUDE_DIR})

# MJ TODO: deal with shared libs later
#add_library(SFLib ${IS_SHARED} ${SF_LIB_SRC})
add_library(SFLib ${SF_LIB_SRC})
