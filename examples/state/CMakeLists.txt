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

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

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
        # find Boost
        # Disable auto-linking. 
        #add_definitions(-DBOOST_ALL_NO_LIB)
        # In case of a Shared Boost install (dlls), you should then enable this
        #if (BUILD_SHARED_LIBS)
        #    add_definitions(-DBOOST_ALL_DYN_LINK)
        #    # Explicitly tell find-package to search for Static Boost libs (if needed)
        #    set(Boost_USE_STATIC_LIBS OFF) 
        #else (BUILD_SHARED_LIBS)
        #    set(Boost_USE_STATIC_LIBS ON) 
        #endif (BUILD_SHARED_LIBS)

        #find_package(Boost REQUIRED COMPONENTS msm)
        #set (Boost_ADDITIONAL_VERSIONS "1.51.0")
        #find_package (BoostMSM)
        #if (BoostMSM_FOUND)
            set (INCLUDE_DIR ${SF_LIB_INCLUDE_DIR} ${cisst_INCLUDE_DIR})
            #file(GLOB_RECURSE cppFiles src/*.cpp)
            #message("+++++ ${cppFiles}")

            #add_executable(state ${cppFiles})
            #target_link_libraries(state ${Boost_LIBRARIES})

            include_directories (${INCLUDE_DIR})
            add_executable (state main.cpp) # ${cppFiles})
            cisst_target_link_libraries(state ${REQUIRED_CISST_LIBS})
            target_link_libraries (state ${REQUIRED_SF_LIBS})
       #else(BoostMSM_FOUND)
       #     message(FATAL_ERROR "Boost is not found.")
       #endif(BoostMSM_FOUND)
    else (cisst_FOUND_AS_REQUIRED)
        cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
    endif (cisst_FOUND_AS_REQUIRED)
endif (SF_HAS_CISST)
