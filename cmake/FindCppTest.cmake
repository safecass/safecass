#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

#
# FindCppTest.cmake
#
# Looks for CppTest library (unit-testing framework) and defines following
# cmake variables:
#
#   CPPTEST_FOUND
#   CPPTEST_INCLUDE_DIR
#   CPPTEST_LIBRARIES
#
# Reference:
#
#   - CppTest: http://cpptest.sourceforge.net
#

find_path(CPPTEST_INCLUDE_DIR cpptest.h 
                              PATHS /usr/local/include
                                    /usr/include
                                    /opt/local/include
                                    /opt/include
                              PATH_SUFFIXES cppunit
                              DOC "Directory containing cpptest.h")
find_library(CPPTEST_LIBRARY cpptest
                             PATHS /usr/local/lib
                                   /usr/lib
                                   /opt/local/lib
                                   /opt/lib
                             DOC "Path to cpptest library")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPPTEST DEFAULT_MSG CPPTEST_LIBRARY CPPTEST_INCLUDE_DIR)

if(CPPTEST_FOUND)
    set (CPPTEST_LIBRARY ${CPPTEST_LIBRARY} PARENT_SCOPE)
    set (CPPTEST_INCLUDE_DIR ${CPPTEST_INCLUDE_DIR} PARENT_SCOPE)
endif(CPPTEST_FOUND)
