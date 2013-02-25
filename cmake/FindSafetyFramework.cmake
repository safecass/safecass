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
# Looks for the Safety Framework, loads its configuartions, and defines 
# following cmake variables:
#
#   SF_FOUND        : If SF is found
#   SF_CONFIG_FILE  : Configuration variables (e.g., versions, build path)
#   SF_LIBRARY_FILE : Path to SF lib file (SFLib)
#

find_file(SF_CONFIG_FILE SFConfig.cmake NO_DEFAULT_PATH 
          DOC "Path to SFConfig.cmake file in the build tree of Safety Framework")
find_library(SF_LIBRARY_FILE SFLib NO_DEFAULT_PATH
             DOC "Path to Safety Framework library (SFLib)")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SF DEFAULT_MSG SF_CONFIG_FILE SF_LIBRARY_FILE)

if (SF_FOUND)
    # MJ: PARENT_SCOPE appears to be redundant here
    #set (SF_CONFIG_FILE ${SF_CONFIG_FILE} PARENT_SCOPE)
    #set (SF_LIBRARY_FILE ${SF_LIBRARY_FILE} PARENT_SCOPE)
    include(${SF_CONFIG_FILE})
endif(SF_FOUND)
