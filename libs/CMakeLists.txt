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

# Set group files
set (LIB_DIR ${PROJECT_SOURCE_DIR})
set (SF_LIB_COMMON ${LIB_DIR}/common/common.h
                   ${LIB_DIR}/common/common.cpp
                   ${LIB_DIR}/common/json.h
                   ${LIB_DIR}/common/json.cpp)

# Find JSON (for JSON-based data exchange)
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package (JSONCPP REQUIRED)
if (JSONCPP_FOUND)
  message ("JSONCPP found")
elseif (JSONCPP_FOUND)
  message (FATAL_ERROR "JSONCPP not found")
endif (JSONCPP_FOUND)

include_directories (${JSONCPP_INCLUDE_DIR})
#set_property (GLOBAL PROPERTY JSONCPP_INCLUDE_DIR ${JSONCPP_INCLUDE_DIR})
#set_property (GLOBAL PROPERTY JSONCPP_LIBRARIES ${JSONCPP_LIBRARIES})

# MJ TODO: deal with shared libs later
#add_library(SFLib ${IS_SHARED} ${SF_LIB_COMMON})
add_library(SFLib ${SF_LIB_COMMON})
