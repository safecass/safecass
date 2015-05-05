#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

project(supervisor)

if (SC_HAS_CISST)
    set (REQUIRED_CISST_LIBS cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
    set (REQUIRED_SC_LIBS SCLib)

    # find cisst and make sure the required libraries have been compiled
    find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

    if (cisst_FOUND_AS_REQUIRED)
        include_directories (${SC_LIB_INCLUDE_DIR})
        add_executable (supervisor main_cisst.cpp)
        cisst_target_link_libraries(supervisor ${REQUIRED_CISST_LIBS})
        target_link_libraries (supervisor ${REQUIRED_SC_LIBS} ${SC_LIB_DEPENDENCY_LIBS})
    else ()
        cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
    endif ()
endif (SC_HAS_CISST)

if (ENABLE_VIS_FRAMEWORK AND CISST_HAS_QT)
    add_subdirectory(Qt)
endif ()
