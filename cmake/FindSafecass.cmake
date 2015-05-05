#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2013 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

#
# FindSafetyFramework.cmake
#
# Looks for SAFECASS, loads its configuartions, and defines 
# following cmake variables:
#
#   SC_FOUND        : If SAFECASS is found
#   SC_CONFIG_FILE  : Configuration variables (e.g., versions, build path)
#   SC_LIBRARY_FILE : Path to SC lib file (SCLib)
#

find_file(SC_CONFIG_FILE SCConfig.cmake NO_DEFAULT_PATH 
          DOC "Path to SCConfig.cmake file in the build tree of Safety Framework")
find_library(SC_LIBRARY_FILE SCLib NO_DEFAULT_PATH
             DOC "Path to Safety Framework library (SCLib)")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SC DEFAULT_MSG SC_CONFIG_FILE SC_LIBRARY_FILE)

if (SC_FOUND)
    # MJ: PARENT_SCOPE appears to be redundant here
    #set (SC_CONFIG_FILE ${SC_CONFIG_FILE} PARENT_SCOPE)
    #set (SC_LIBRARY_FILE ${SC_LIBRARY_FILE} PARENT_SCOPE)
    include(${SC_CONFIG_FILE})
endif(SC_FOUND)
