#-----------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
#
#-----------------------------------------------------------------------------------
#
# Created on   : Jul 2, 2012
# Last revision: Oct 30, 2016
# Author       : Min Yang Jung <myj@jhu.edu>
# URL          : https://github.com/safecass/safecass
#

# TODO:
#
# 1. shared lib support
#   => option (SAFECASS_BUILD_SHARED_LIBS "Build shared libraries" OFF)
#
# 2. Boost: Use official release archive from https://github.com/boostorg/boost/tags
#           For some reason, this lightweight downloaded package doesn't work
#           with the current build setup.
#
# 3. Boost.Graph: Selectively enable/disable GraphViz reader/exporter feature
#                 If not enabled, two boost libs are not required (regex, graph)

# NOTE:
# - MD5 HASH generator: http://onlinemd5.com

#--------------------------------------------------------------------
#
# Required dependencies
# - Try to find first
# - If not found, download and build from source as external project
#
#--------------------------------------------------------------------
include(add_external_project)

#
# 1) JSONCPP
#
find_package(JSONCPP)
if (JSONCPP_FOUND)
  message("JSONCPP found")
else(JSONCPP_FOUND)
  # Download and configure at configure-time
  add_dl_project(PROJ jsoncpp
                 URL  https://github.com/open-source-parsers/jsoncpp/archive/1.6.5.tar.gz
                 #CMAKE_OPTIONS -DBUILD_SHARED_LIBS:BOOL=ON
                 #              -DJSONCPP_WITH_TESTS:BOOL=OFF
                 EXCLUDE_FROM_ALL)

  # To enable this, SAFECASS shared lib should be selected first, and then enable
  # jsoncpp_lib.
  #if (SAFECASS_BUILD_SHARED_LIBS)
  #  target_include_directories(jsoncpp_lib INTERFACE "${SAFECASS_BUILD_ROOT}/jsoncpp/src/include")
  #endif()
  target_include_directories(jsoncpp_lib_static INTERFACE
                             ${SAFECASS_BUILD_ROOT}/external_packages/jsoncpp/src/include)

  # Download and configure at build-time
  #add_external_project(NAME          jsoncpp
  #                     URL           https://github.com/open-source-parsers/jsoncpp/archive/1.6.5.tar.gz
  #                     BUILD_IN_SRC  FALSE
  #                     EXCLUDE_FROM_ALL
  #                     CMAKE_OPTIONS -DCMAKE_INSTALL_PREFIX:FILEPATH=${SAFECASS_BUILD_ROOT}/external_packages/jsoncpp
  #                                   # #-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  #                                   # #-DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
  #                                   -DBUILD_SHARED_LIBS:BOOL=OFF
  #                                   -DBUILD_STATIC_LIBS:BOOL=ON
  #                                   -DJSONCPP_WITH_TESTS:BOOL=OFF
  #                                   -DJSONCPP_WITH_WARNING_AS_ERROR:BOOL=OFF
  #                                   -DJSONCPP_WITH_POST_BUILD_UNITTEST:BOOL=OFF
  #                                   -DJSONCPP_WITH_PKGCONFIG_SUPPORT:BOOL=OFF)

  # Set JSON-related variables
  set (JSONCPP_INCLUDE_DIR  ${SAFECASS_BUILD_ROOT}/external_packages/jsoncpp/src/include)
  set (JSONCPP_LIBRARY_DIRS ${SAFECASS_LIBRARY_DIR})
  set (JSONCPP_FOUND        ON)

  find_library(JSONCPP_LIBRARIES
               NAME  jsoncpp #jsoncpp_lib_shared
               PATHS ${SAFECASS_BUILD_ROOT}/external_packages/jsoncpp
               NO_DEFAULT_PATH
               NO_CMAKE_FIND_ROOT_PATH)

  message(STATUS "JSONCPP_INCLUDE_DIR: ${JSONCPP_INCLUDE_DIR}")
  message(STATUS "JSONCPP_LIBRARY_DIRS: ${JSONCPP_LIBRARY_DIRS}")
  message(STATUS "JSONCPP_LIBRARIES: ${JSONCPP_LIBRARIES}")
  message(STATUS "JSONCPP_FOUND: ${JSONCPP_FOUND}")
endif()

#
# 2) Boost
#
set (BOOST_COMPONENTS_REQUIRED chrono graph system regex program_options)

# TEMP to test boost external project
#find_package(Boost 1.60.0 COMPONENTS ${BOOST_COMPONENTS_REQUIRED})
if (FALSE) #Boost_FOUND)
  message("Boost found")
else()
  message("Boost not found -- Boost 1.60.0 will be downloaded and built")

  # Define boost bootstrap shell command
  if (SAFECASS_ON_LINUX OR SAFECASS_ON_MAC)
    set(BOOST_BOOTSTRAP_CMD "./bootstrap.sh")
    set(BOOST_B2_CMD        "./b2")
  elseif (SAFECASS_ON_WINDOWS)
    set(BOOST_BOOTSTRAP_CMD "bootstrap.bat")
    set(BOOST_B2_CMD        "b2.exe")
  endif()

  # Travis build on Linux with gcc/clang occasionally fail due to limited
  # memory: "g++: internal compiler error: Killed (program cc1plus)"
  # This error appears to be reported many times already.
  #
  # Without optimization: fails on Linux/clang
  # With -O3: fails on Linux/gcc
  # With -O2: fails on Linux/clang
  #
  # Linux container-based: 4 GB / 2 Cores
  # OS X: 3 GB / 1 Core
  #
  # In Travis, the number of cores reported by CMake on Linux is 32, whereas
  # the number is 2 on OS X.  Compiler crash may have something to do with
  # -j32 flag during Boost build process.
  if (TRAVIS_CI)
    # Compile with optimization for BGL (recommended by BGL)
    #if (SAFECASS_ON_LINUX)
    #  list (APPEND CMAKE_CXX_FLAGS "-O2")
    #endif()
    set (N 2)
  endif()

  # Download and configure at build-time
  add_external_project(NAME          boost
                       URL           http://cs.jhu.edu/~minyang/boost_1_60_0.tar.bz2
                       #URL           /Users/MJ/project/tools/boost_1_60_0.tar.bz2 # Local folder
                       URL_HASH      65a840e1a0b13a558ff19eeb2c4f0cbe # 1.60.0
                       BUILD_IN_SRC  TRUE
                       CONFIG_CMD    ${BOOST_BOOTSTRAP_CMD}
                       BUILD_CMD     ${BOOST_B2_CMD} install
                              -d0
                              --prefix=${SAFECASS_BUILD_ROOT}/external_packages/boost/build
                              --build-dir=${SAFECASS_BUILD_ROOT}/external_packages/boost/build
                              --variant=release # debug|release
                              --link=static # static|shared
                              --threading=multi #single|multi
                              --with-program_options
                              #--with-thread
                              --with-system
                              --with-chrono
                              # graph and regex is only required for graph with GraphViz
                              --with-graph
                              --with-regex
                              -j${N})

  set (Boost_INCLUDE_DIRS ${SAFECASS_BUILD_ROOT}/external_packages/boost/build/include)
  set (Boost_LIBRARY_DIRS  ${SAFECASS_BUILD_ROOT}/external_packages/boost/build/lib)
  set (Boost_FOUND ON CACHE BOOL "If boost package was downloaded")

  include (find_boost_library)
  find_boost_library(LIB_NAME program_options) # setting Boost_PROGRAM_OPTIONS_LIBRARY
  #find_boost_library(LIB_NAME thread) # setting Boost_THREAD_LIBRARY
  find_boost_library(LIB_NAME system) # setting Boost_SYSTEM_LIBRARY
  find_boost_library(LIB_NAME chrono) # setting Boost_CHRONO_LIBRARY
  find_boost_library(LIB_NAME graph) # setting Boost_GRAPH_LIBRARY
  find_boost_library(LIB_NAME regex) # setting Boost_REGEX_LIBRARY
  set (Boost_PROGRAM_OPTIONS_FOUND ON)
  #set (Boost_THREAD_FOUND ON)
  set (Boost_SYSTEM_FOUND ON)
  set (Boost_CHRONO_FOUND ON)

  set (Boost_LIBRARIES "") # clear out existing cache just in case
  list (APPEND Boost_LIBRARIES ${Boost_PROGRAM_OPTIONS_LIBRARY}
                               #${Boost_THREAD_LIBRARY}
                               ${Boost_SYSTEM_LIBRARY}
                               ${Boost_CHRONO_LIBRARY}
                               ${Boost_GRAPH_LIBRARY}
                               ${Boost_REGEX_LIBRARY})

  set (Boost_VERSION       106000)
  set (Boost_LIB_VERSION   1_60)
  set (Boost_MAJOR_VERSION 1)
  set (Boost_MINOR_VERSION 60)
  set (Boost_SUBMINOR_VERSION 0)

  message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
  message("Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
  message("Boost_LIBRARIES: ${Boost_LIBRARIES}")
  message("Boost_VERSION: ${Boost_VERSION}")
  message("Boost_LIB_VERSION: ${Boost_LIB_VERSION}")
  message("Boost_MAJOR_VERSION: ${Boost_MAJOR_VERSION}")
  message("Boost_MINOR_VERSION: ${Boost_MINOR_VERSION}")
  message("Boost_SUBMINOR_VERSION: ${Boost_SUBMINOR_VERSION}")
  foreach (COMPONENT ${BOOST_COMPONENTS_REQUIRED})
    string(TOUPPER ${COMPONENT} COMPONENT_NAME)
    #string(TOUPPER ${COMPONENT}_FOUND COMPONENT_FOUND)
    #string(TOUPPER ${COMPONENT}_LIBRARY COMPONENT_LIBRARY)
    message("Boost_${COMPONENT_NAME}_FOUND: ${Boost_${COMPONENT_NAME}_FOUND}")
    message("Boost_${COMPONENT_NAME}_LIBRARY: ${Boost_${COMPONENT_NAME}_LIBRARY}")
    set (Boost_${COMPONENT_NAME}_FOUND ${Boost_${COMPONENT_NAME}_FOUND} PARENT_SCOPE)
    set (Boost_${COMPONENT_NAME}_LIBRARY ${Boost_${COMPONENT_NAME}_LIBRARY} PARENT_SCOPE)
  endforeach()

  set (Boost_FOUND        ${Boost_FOUND}        PARENT_SCOPE)
  set (Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} PARENT_SCOPE)
  set (Boost_LIBRARIES    ${Boost_LIBRARIES}    PARENT_SCOPE)
endif()

#
# 3) glog (Google Logger)
#
if (NOT GLOG_FOUND)
  # Download and build glog at build-time
  #add_dl_project(PROJ glog
                 #URL           https://github.com/google/glog/archive/v0.3.4.tar.gz
                 #URL_HASH      MD5=DF92E05C9D02504FB96674BC776A41CB
                 #CMAKE_OPTIONS "-DWITH_GFLAGS=OFF")

  add_external_project(NAME          glog
                       URL           https://github.com/google/glog/archive/v0.3.4.tar.gz
                       URL_HASH      df92e05c9d02504fb96674bc776a41cb
                       BUILD_IN_SRC  TRUE
                       CONFIG_CMD    "./configure"
                       BUILD_CMD     "make" -j${N})

  # Import glog options from the build
  set (GLOG_INCLUDE_DIRS ${SAFECASS_BUILD_ROOT}/external_packages/glog/src/src)
  set (GLOG_LIBRARY_DIRS ${SAFECASS_BUILD_ROOT}/external_packages/glog/src/.libs)
  set (GLOG_LIBRARIES ${GLOG_LIBRARY_DIRS}/libglog.a)
  set (GLOG_FOUND ON)
  # find_library(GLOG_LIBRARIES
               # # On UNIX-like systems, force CMake to find static library
               # # FIXME when supporting shared library, this has to be updated.
               # NAME glog
               # #NAME libglog.a
               # PATHS ${GLOG_LIBRARY_DIRS}
               # NO_DEFAULT_PATH
               # NO_CMAKE_FIND_ROOT_PATH)

  set (GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIRS} PARENT_SCOPE)
  set (GLOG_LIBRARY_DIRS ${GLOG_LIBRARY_DIRS} PARENT_SCOPE)
  set (GLOG_LIBRARIES    ${GLOG_LIBRARIES} PARENT_SCOPE)
  set (GLOG_FOUND        ${GLOG_FOUND} PARENT_SCOPE)
endif()

#
# 4) Eigen
#
if (NOT EIGEN3_FOUND)
  # Download and configure Eigen3 at configure-time
  add_dl_project(PROJ     eigen3
                 URL      http://bitbucket.org/eigen/eigen/get/3.2.8.tar.bz2
                 URL_HASH MD5=9e3bfaaab3db18253cfd87ea697b3ab1
                 EXCLUDE_FROM_ALL)

  # Set JSON-related variables
  set (EIGEN3_INCLUDE_DIR ${SAFECASS_BUILD_ROOT}/external_packages/eigen3/src
       CACHE PATH "Eigen3 include directory")
  set (EIGEN3_FOUND ON
       CACHE BOOL "If Eigen3 package was downloaded")
endif()





#--------------------------------------------------------------------
#
# Optional dependencies
# - Try to find first
# - If not found, download and build from source as external project
#
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#
# SAFECASS Library
#
#--------------------------------------------------------------------
include (add_safecass_library)

# Generate config.h
configure_file(config.h.in ${BUILD_ROOT}/include/config.h)

# Set include directories
set (SAFECASS_LIBRARY_INCLUDE_DIR # safecass headers
                                  ${SAFECASS_INCLUDE_DIR}
                                  # generated safecass headers
                                  ${BUILD_ROOT}/include
                                  # dependencies
                                  ${JSONCPP_INCLUDE_DIR}
                                  ${Boost_INCLUDE_DIRS}
                                  ${GLOG_INCLUDE_DIRS}
                                  ${EIGEN3_INCLUDE_DIR}
                                  )

# Define libraries to compile
set (SAFECASS_LIBRARIES common safecass)
foreach (lib ${SAFECASS_LIBRARIES})
  add_safecass_library(${lib})
  target_include_directories(${lib} PUBLIC ${SAFECASS_LIBRARY_INCLUDE_DIR})
endforeach()

# Set dependencies of common library
set (SAFECASS_LIBRARY_COMMON_DEPENDENCY ${Boost_LIBRARIES})
if (SAFECASS_COMPILER_GCC)
  if (SAFECASS_ON_MAC)
    # libc++ is only required for gcc on Mac
    list (APPEND SAFECASS_LIBRARY_COMMON_DEPENDENCY c++)
  endif()
endif()
target_link_libraries(common ${SAFECASS_LIBRARY_COMMON_DEPENDENCY})

# For gcc compilation
# On Mac, g++ is by default aliased to clang that uses libc++ (-stdlib=libstdc++).
# On Linux, (the actual) g++ uses libstdc++ (-lstdc++).
# if (SAFECASS_COMPILER_GCC)
  # list (APPEND SC_CXX_FLAGS "-lstdc++")
  #list (APPEND CMAKE_CXX_FLAGS "-stdlib=libc++")
# endif()

# Set dependencies
add_dependencies(common external-glog external-boost) # common requires glog and boost
add_dependencies(safecass common) # common requires glog

# Make include directories available to other projects within safecass
set (SAFECASS_LIBRARY_INCLUDE_DIR ${SAFECASS_LIBRARY_INCLUDE_DIR} PARENT_SCOPE)

# Define link library dependencies
target_link_libraries(common jsoncpp_lib_static)
target_link_libraries(safecass common)

return()


#
#
# BELOW ARE SKIPPED
#
#


# Set directories

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
add_library(SCLib ${SC_LIB_TYPE} ${SC_LIB_SRC})
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
