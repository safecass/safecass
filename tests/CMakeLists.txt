#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

project (tests)

set (REQUIRED_SF_LIBS SFLib ${JSONCPP_LIBRARIES} 
                            ${CPPTEST_LIBRARY})

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

include_directories(${CMAKE_BINARY_DIR})

add_executable (tests main.cpp 
                      # unit-test files
                      testJson.h testJson.cpp)

target_link_libraries (tests ${REQUIRED_SF_LIBS})
                                    
include_directories(${COMMON_FILES_PATH} ${CPPTEST_INCLUDE_DIR})

