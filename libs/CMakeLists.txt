#-----------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
#
#-----------------------------------------------------------------------------------
#
# Created on   : Jul 2, 2012
# Last revision: May 4, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
# URL          : https://github.com/minyang/safecass
#

#---------------------------------
# Project Settings
#---------------------------------
project(SCLib)

# Set directories
set (LIB_DIR ${PROJECT_SOURCE_DIR})

#---------------------------------
# Collect project files
#---------------------------------
# common
set (SC_LIB_COMMON_SRC ${LIB_DIR}/common/common.h ${LIB_DIR}/common/common.cpp
                       ${LIB_DIR}/common/common.h ${LIB_DIR}/common/utils.cpp
                       ${LIB_DIR}/common/dict.h ${LIB_DIR}/common/dict.cpp
                       ${LIB_DIR}/common/jsonwrapper.h ${LIB_DIR}/common/jsonwrapper.cpp
                       #${LIB_DIR}/common/jsonSerializer.h ${LIB_DIR}/common/jsonSerializer.cpp
                       ${LIB_DIR}/common/color_console.h
                       )
set (SC_LIB_COMMON_INCLUDE_DIR ${LIB_DIR} ${LIB_DIR}/common)

# supervisor
set (SC_LIB_SUPERVISOR_SRC ${LIB_DIR}/supervisor/coordinator.h
                           ${LIB_DIR}/supervisor/coordinator.cpp
                           ${LIB_DIR}/supervisor/supervisor.h
                           ${LIB_DIR}/supervisor/supervisor.cpp)
set (SC_LIB_SUPERVISOR_INCLUDE_DIR ${LIB_DIR} ${LIB_DIR}/supervisor)

# monitor
set (SC_LIB_MONITOR_SRC ${LIB_DIR}/monitor/monitor.h
                        ${LIB_DIR}/monitor/monitor.cpp)
set (SC_LIB_MONITOR_INCLUDE_DIR ${LIB_DIR}/monitor)

# FDD
set (SC_LIB_FDD_SRC # faults
                    ${LIB_DIR}/fdd/event.h ${LIB_DIR}/fdd/code/event.cpp
                    # event location
                    ${LIB_DIR}/fdd/eventLocationBase.h ${LIB_DIR}/fdd/code/eventLocationBase.cpp
                    # filter
                    ${LIB_DIR}/fdd/signal.h ${LIB_DIR}/fdd/code/signal.cpp
                    ${LIB_DIR}/fdd/filterBase.h ${LIB_DIR}/fdd/code/filterBase.cpp
                    ${LIB_DIR}/fdd/filterFactory.h ${LIB_DIR}/fdd/code/filterFactory.cpp
                    # filters
                    #${LIB_DIR}/fdd/filters/bypass.h ${LIB_DIR}/fdd/filters/bypass.cpp
                    ${LIB_DIR}/fdd/filters/threshold.h ${LIB_DIR}/fdd/filters/code/threshold.cpp
                    #${LIB_DIR}/fdd/filters/changeDetection.h ${LIB_DIR}/fdd/filters/code/changeDetection.cpp
                    ${LIB_DIR}/fdd/filters/onOff.h ${LIB_DIR}/fdd/filters/code/onOff.cpp
                    ${LIB_DIR}/fdd/filters/nop.h ${LIB_DIR}/fdd/filters/code/nop.cpp
                    )
set (SC_LIB_FDD_INCLUDE_DIR ${LIB_DIR}/fdd ${LIB_DIR}/fdd/filters)

# Generic Component Model (GCM)
set (SC_LIB_GCM_SRC ${LIB_DIR}/gcm/gcm.h ${LIB_DIR}/gcm/gcm.cpp)
set (SC_LIB_GCM_INCLUDE_DIR ${LIB_DIR}/gcm)

# adapters
set (SC_LIB_ADAPTER_BASE_DIR ${LIB_DIR}/adapters)
if (SC_HAS_CISST)
  set (SC_LIB_ADAPTER_DIR "${SC_LIB_ADAPTER_BASE_DIR}/cisst")
  set (SC_LIB_ADAPTER_CISST_SRC ${SC_LIB_ADAPTER_DIR}/cisstMonitor.h
                                ${SC_LIB_ADAPTER_DIR}/cisstMonitor.cpp
                                ${SC_LIB_ADAPTER_DIR}/cisstEventLocation.h
                                ${SC_LIB_ADAPTER_DIR}/cisstEventLocation.cpp
                                ${SC_LIB_ADAPTER_DIR}/cisstAccessor.h
                                ${SC_LIB_ADAPTER_DIR}/cisstAccessor.cpp)

  set (SC_LIB_MONITOR_INCLUDE_DIR ${SC_LIB_MONITOR_INCLUDE_DIR} ${SC_LIB_ADAPTER_DIR})
  set (SC_LIB_MONITOR_SRC ${SC_LIB_MONITOR_SRC} ${SC_LIB_ADAPTER_CISST_SRC})
  set (SC_LIB_FDD_SRC ${SC_LIB_FDD_SRC} ${SC_LIB_ADAPTER_CISST_SRC})
endif (SC_HAS_CISST)

# Add Curl wrapper if visualization is enabled
# set (SC_LIB_COMMUNICATOR_SRC ${LIB_DIR}/db/MongoDB.h
                             # ${LIB_DIR}/db/MongoDB.cpp)
if (ENABLE_VIS_FRAMEWORK)
    find_package(CURL REQUIRED)
    set (SC_LIB_COMMUNICATOR_SRC ${SC_LIB_COMMUNICATOR_SRC}
                                 ${LIB_DIR}/communicator/curlWrapper.h
                                 ${LIB_DIR}/communicator/curlWrapper.cpp)
    set (SC_LIB_COMMUNICATOR_INCLUDE_DIR ${LIB_DIR}/db
                                         ${LIB_DIR}/communicator)
endif (ENABLE_VIS_FRAMEWORK)

# Boost
find_package (BOOST REQUIRED)
if (BOOST_FOUND)
  set (SC_LIB_STATEMACHINE_INCLUDE_DIR ${BOOST_INCLUDE_DIR}
                                       ${LIB_DIR}/statemachine)
  set (SC_LIB_STATEMACHINE_SRC ${SC_LIB_STATEMACHINE_INCLUDE_DIR}/state.h
                               ${SC_LIB_STATEMACHINE_INCLUDE_DIR}/state.cpp
                               ${SC_LIB_STATEMACHINE_INCLUDE_DIR}/statemachine.h
                               ${SC_LIB_STATEMACHINE_INCLUDE_DIR}/statemachine.cpp
                               ${SC_LIB_STATEMACHINE_INCLUDE_DIR}/stateEventHandler.h
                               ${SC_LIB_STATEMACHINE_INCLUDE_DIR}/stateEventHandler.cpp)
  set (SC_LIB_DEPENDENCY_LIBS ${SC_LIB_DEPENDENCY_LIBS} 
                              ${BOOST_LIB_PROGRAM_OPTION}
                              ${BOOST_LIB_THREAD}
                              ${BOOST_LIB_SYSTEM})
endif()

# communicator for message exchange (publish-subscribe)
# Find ZeroC Ice
find_package(Ice REQUIRED)
set (SC_LIB_COMMUNICATOR_SRC ${SC_LIB_COMMUNICATOR_SRC}
                             ${LIB_DIR}/communicator/topic_def.h
                             ${LIB_DIR}/communicator/baseIce.h
                             ${LIB_DIR}/communicator/baseIce.cpp
                             ${LIB_DIR}/communicator/publisher.h
                             ${LIB_DIR}/communicator/publisher.cpp
                             ${LIB_DIR}/communicator/subscriber.h
                             ${LIB_DIR}/communicator/subscriber.cpp)
set (SC_LIB_COMMUNICATOR_INCLUDE_DIR ${SC_LIB_COMMUNICATOR_INCLUDE_DIR} 
                                     ${LIB_DIR}/communicator)

# slice integration
set (SLICE_FILES messages)
set (SLICE_INCLUDE_DIR ${ZEROC_SLICE_INCLUDE_DIR})
foreach (slice_file ${SLICE_FILES})
    set (GENERATED_CPP_FILE "${SC_BUILD_ROOT}/libs/${slice_file}.cpp")
    set (GENERATED_H_FILE "${SC_BUILD_ROOT}/libs/${slice_file}.h")
    set (SLICE_SRC_DIR  ${LIB_DIR}/communicator/slice)
    set (SLICE_SRC_FILE ${SLICE_SRC_DIR}/${slice_file}.ice)
    add_custom_command (
        OUTPUT ${GENERATED_CPP_FILE} ${GENERATED_H_FILE} 
               ${SC_BUILD_ROOT}/include/${slice_file}.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        # first run slice2cpp
        COMMAND ${ZEROC_SLICE2CPP}
        ARGS -I ${SLICE_INCLUDE_DIR} -I ${SLICE_SRC_DIR} ${SLICE_SRC_FILE}
        # then create a directory to copy the header file
        COMMAND ${CMAKE_COMMAND}
        ARGS -E make_directory ${SC_BUILD_ROOT}/include
        # and finally copy the header file
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_if_different ${GENERATED_H_FILE} ${SC_BUILD_ROOT}/include
        COMMENT "Compiling ${slice_file}.ice"
        MAIN_DEPENDENCY ${SLICE_SRC_FILE})
    include_directories (${SC_BUILD_ROOT})
    set (GENERATED_HEADER_FILES ${GENERATED_HEADER_FILES} ${SC_BUILD_ROOT}/include/${slice_file}.h)
    set (GENERATED_SOURCE_FILES ${GENERATED_SOURCE_FILES} ${GENERATED_CPP_FILE})
endforeach (slice_file ${SLICE_FILES})

set (SC_LIB_COMMUNICATOR_SRC ${SC_LIB_COMMUNICATOR_SRC} ${GENERATED_SOURCE_FILES})
set (SC_LIB_COMMUNICATOR_INCLUDE_DIR ${SC_LIB_COMMUNICATOR_INCLUDE_DIR}
                                     ${SC_BUILD_ROOT}/include)
set (SC_LIB_DEPENDENCY_LIBS ${SC_LIB_DEPENDENCY_LIBS} ${ZEROC_ICE_LIBRARIES})

#---------------------------------
# Set global INCLUDE and LIB
#---------------------------------
set (SC_LIB_SRC ${SC_LIB_COMMON_SRC} 
                ${SC_LIB_SUPERVISOR_SRC}
                ${SC_LIB_MONITOR_SRC} 
                ${SC_LIB_FDD_SRC}
                ${SC_LIB_STATEMACHINE_SRC}
                ${SC_LIB_COMMUNICATOR_SRC}
                ${SC_LIB_GCM_SRC})
set (SC_LIB_INCLUDE_DIR ${SC_LIB_INCLUDE_DIR} 
                        ${SC_LIB_COMMON_INCLUDE_DIR}
                        ${SC_LIB_SUPERVISOR_INCLUDE_DIR}
                        ${SC_LIB_MONITOR_INCLUDE_DIR}
                        ${SC_LIB_FDD_INCLUDE_DIR}
                        ${SC_LIB_STATEMACHINE_INCLUDE_DIR}
                        ${SC_LIB_COMMUNICATOR_INCLUDE_DIR}
                        ${SC_LIB_GCM_INCLUDE_DIR})

# Set project-wide paths
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)
set (SC_LIB_INCLUDE_DIR ${SC_LIB_INCLUDE_DIR} ${CMAKE_BINARY_DIR})

#---------------------------------
# Include 3rd-party modules
#---------------------------------
# Find JSON (for JSON-based data exchange)
find_package (JSONCPP REQUIRED)
if (JSONCPP_FOUND)
  set (SC_LIB_INCLUDE_DIR ${SC_LIB_INCLUDE_DIR} ${JSONCPP_INCLUDE_DIR})
  set (SC_LIB_DEPENDENCY_LIBS ${SC_LIB_DEPENDENCY_LIBS} ${JSONCPP_LIBRARIES})
endif (JSONCPP_FOUND)

# Find g2log (for high-performance, safe, asynchronous logging)
if (ENABLE_G2LOG)
  find_package (G2LOG REQUIRED)
  if (G2LOG_FOUND)
    message ("g2log found")
    set (SC_CXX_FLAGS "-std=c++11")
    set (SC_LIB_INCLUDE_DIR ${SC_LIB_INCLUDE_DIR} ${G2LOG_INCLUDE_DIR})
    set (SC_LIB_DEPENDENCY_LIBS ${SC_LIB_DEPENDENCY_LIBS} ${G2LOG_LIBRARIES})
    # suppress warnings
    if (UNIX OR APPLE)
        add_definitions(-Wdeprecated-declarations)
    endif()
  else (G2LOG_FOUND)
    message (FATAL_ERROR "g2log not found")
  endif (G2LOG_FOUND)
endif (ENABLE_G2LOG)

# Find CURL (for http commnunication)
if (ENABLE_VIS_FRAMEWORK)
  if (CURL_FOUND)
    message ("CURL found")
    set (SC_LIB_INCLUDE_DIR ${SC_LIB_INCLUDE_DIR} ${CURL_INCLUDE_DIR})
    set (SC_LIB_DEPENDENCY_LIBS ${SC_LIB_DEPENDENCY_LIBS} ${CURL_LIBRARIES})
  else (CURL_FOUND)
    message (FATAL_ERROR "CURL not found")
  endif (CURL_FOUND)
endif (ENABLE_VIS_FRAMEWORK)

include_directories (${SC_LIB_INCLUDE_DIR})
add_library(SCLib ${LIB_TYPE} ${SC_LIB_SRC})
target_link_libraries(SCLib ${SC_LIB_DEPENDENCY_LIBS})

if (SC_HAS_CISST)# AND NOT ENABLE_G2LOG)
  set (REQUIRED_CISST_LIBS cisstCommon cisstOSAbstraction)

  # find cisst and make sure the required libraries have been compiled
  find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

  if (cisst_FOUND_AS_REQUIRED)
    include_directories (${SC_LIB_INCLUDE_DIR})
    cisst_target_link_libraries(SCLib ${REQUIRED_CISST_LIBS})
  else ()
    cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
  endif ()
endif (SC_HAS_CISST)# AND NOT ENABLE_G2LOG)

# Set variables in the parent scope
set (SC_LIB_DEPENDENCY_LIBS ${SC_LIB_DEPENDENCY_LIBS} PARENT_SCOPE)
set (SC_LIB_INCLUDE_DIR ${SC_LIB_INCLUDE_DIR} PARENT_SCOPE)

#set (SC_CXX_FLAGS ${SC_CXX_FLAGS} "-Wno-unused-parameter")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SC_CXX_FLAGS}")

# Disable annoying warnings
if (SC_ON_WINDOWS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-Wno-write-strings)  # deprecated conversion from string constant to char *
add_definitions(-Wno-pointer-arith)  # NULL used in arithmetic
add_definitions(-Wconversion-null)   # converting to non-pointer type 'char' from NULL
#add_definitions(-Wunused-parameter)
