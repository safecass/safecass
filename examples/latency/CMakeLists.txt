#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

project(latency)

set (REQUIRED_SF_LIBS SFLib ${JSONCPP_LIBRARIES})

if (SF_HAS_CISST)
    set (REQUIRED_CISST_LIBS cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

    # find cisst and make sure the required libraries have been compiled
    find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

    if (cisst_FOUND_AS_REQUIRED)
        include_directories (${cisst_INCLUDE_DIR} ${JSONCPP_INCLUDE_DIR})
        add_executable (latency main_cisst.cpp)
        cisst_target_link_libraries(latency ${REQUIRED_CISST_LIBS})
        target_link_libraries (latency ${REQUIRED_SF_LIBS})
    else (cisst_FOUND_AS_REQUIRED)
        cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
    endif (cisst_FOUND_AS_REQUIRED)
endif (SF_HAS_CISST)
