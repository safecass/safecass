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
# FindJsonCpp.cmake
#
# Looks for jsoncpp library for JSON integration and defines following
# cmake variables:
#
#   JSONCPP_FOUND
#   JSONCPP_INCLUDE_DIR
#   JSONCPP_LIBRARIES
#
# References:
#
#   - JSON: http://www.json.org
#   - jsoncpp library: 
#       http://sourceforge.net/projects/jsoncpp
#       http://jsoncpp.sourceforge.net
#

find_path(JSONCPP_INCLUDE_DIR NAMES jsoncpp/json.h DOC "Directory containing jsoncpp/json.h")
find_library(JSONCPP_LIBRARY NAMES json_linux-gcc-4.2.1_libmt DOC "Path to jsoncpp library")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JSONCPP DEFAULT_MSG JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR)

if(JSONCPP_FOUND)
  set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY} PARENT_SCOPE)
  set(JSONCPP_INCLUDE_DIR ${JSONCPP_INCLUDE_DIR} PARENT_SCOPE)
endif(JSONCPP_FOUND)
