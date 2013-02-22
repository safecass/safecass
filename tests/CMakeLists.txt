#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

project (tests)

message("Unit-tests enabled")

# Find CppTest
find_package (CPPTEST REQUIRED)
if (CPPTEST_FOUND)
    message ("CppTest found")
elseif ()
    message (FATAL_ERROR "JSONCPP not found")
endif ()

include_directories (${SF_LIB_INCLUDE_DIR} ${CPPTEST_INCLUDE_DIR})

add_executable (tests main.cpp testJson.h testJson.cpp)

target_link_libraries (tests SFLib ${SF_LIB_DEPENDENCY_LIBS} ${CPPTEST_LIBRARY})
