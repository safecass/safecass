#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

#
# FindBoostMSM.cmake
#
# Looks for the Boost Meta State Machine (MSM) library and defines following cmake variables:
#
#   BoostMSM_FOUND
#   Boost_INCLUDE_DIR
#
# References:
#   [1] Boost Meta State Machine (MSM):
#       http://www.boost.org/doc/libs/1_45_0/libs/msm/doc/HTML/index.html
#
find_path(Boost_INCLUDE_DIR NAMES boost/msm/common.hpp DOC "Directory containing boost/msm/common.hpp")

if (Boost_INCLUDE_DIR STREQUAL "Boost_INCLUDE_DIR-NOTFOUND")
  message(FATAL_ERROR "Boost MSM not found")
else ()
  set (BoostMSM_FOUND TRUE)
endif ()

if(BoostMSM_FOUND)
  set(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIR})
endif(BoostMSM_FOUND)
