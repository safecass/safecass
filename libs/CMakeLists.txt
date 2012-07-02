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
set(SF_LIB_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/common/common.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/common/common.cpp")

#set(SF_LIB_FDD FDD/common.h FDD/common/common.cpp)

# MJ TODO: deal with shared libs later
#add_library(SFLib ${IS_SHARED} ${SF_LIB_COMMON})
add_library(SFLib ${SF_LIB_COMMON})
