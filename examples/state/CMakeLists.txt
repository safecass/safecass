#
# Safety Framework for Component-based Robotics
#
# Created on: October 10, 2012
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

project(state)

# Find required dependencies
set (REQUIRED_SF_LIBS SFLib ${JSONCPP_LIBRARIES})
if (ENABLE_G2LOG)
    set (REQUIRED_SF_LIBS ${REQUIRED_SF_LIBS} ${G2LOG_LIBRARIES})
endif (ENABLE_G2LOG)

set (INCLUDE_DIR ${SF_LIB_INCLUDE_DIR})

if (SF_HAS_CISST)
    set (REQUIRED_CISST_LIBS cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

    # find cisst and make sure the required libraries have been compiled
    find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

    if (cisst_FOUND_AS_REQUIRED)
        set (INCLUDE_DIR ${SF_LIB_INCLUDE_DIR} ${cisst_INCLUDE_DIR})
        include_directories (${INCLUDE_DIR})
        add_executable (state main.cpp)
        cisst_target_link_libraries(state ${REQUIRED_CISST_LIBS})
        target_link_libraries (state ${REQUIRED_SF_LIBS})
    else (cisst_FOUND_AS_REQUIRED)
        cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
    endif (cisst_FOUND_AS_REQUIRED)
endif (SF_HAS_CISST)
