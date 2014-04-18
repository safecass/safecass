#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : Jul 6, 2012
# Last revision: Apr 18, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
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

add_executable (tests main.cpp 
                      testJson.h testJson.cpp
                      testGCM.h testGCM.cpp
                      )

target_link_libraries (tests SFLib ${SF_LIB_DEPENDENCY_LIBS} 
                                   ${CPPTEST_LIBRARY}
                                   ${BOOST_LIB_PROGRAM_OPTION})
