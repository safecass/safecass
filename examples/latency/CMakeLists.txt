#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

#include_directories (${sawOpenAL_INCLUDE_DIR})
#link_directories (${sawOpenAL_LIBRARY_DIR})
#target_link_libraries (sawDataPlayerComponents ${sawOpenAL_LIBRARIES})

add_executable (latency main.cpp)
