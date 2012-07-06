#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

project (tests)

# path to common files
set (COMMON_FILES_PATH "${CMAKE_SOURCE_DIR}/common")

# Find CppTest (as unit-test framework)
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package (CPPTEST REQUIRED)
if (CPPTEST_FOUND)
  message ("JSONCPP found")
elseif (CPPTEST_FOUND)
  message (FATAL_ERROR "JSONCPP not found")
endif (CPPTEST_FOUND)

configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})

add_executable (tests main.cpp 
                      # unit-test files
                      testJson.h testJson.cpp)

target_link_libraries (tests SFLib ${CPPTEST_LIBRARY} ${JSONCPP_LIBRARIES})
                                    
include_directories(${COMMON_FILES_PATH} ${CPPTEST_INCLUDE_DIR})

