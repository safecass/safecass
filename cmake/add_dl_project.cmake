# Original author: Craig Scott (http://crascit.com/2015/07/25/cmake-gtest)
include(CMakeParseArguments)

macro(add_dl_project)
  # Set up named macro arguments
  set(options        EXCLUDE_FROM_ALL)
  set(oneValueArgs   PROJ URL URL_HASH PATH_SUFFIX)
  set(multiValueArgs "")
  cmake_parse_arguments(DL_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  # Create and build a separate CMake project to carry out the download.
  # If we've already previously done these steps, they will not cause
  # anything to be updated, so extra rebuilds of the project won't occur.
  configure_file(${CMAKE_SOURCE_DIR}/cmake/add_dl_project.CMakeLists.cmake.in
                 ${CMAKE_BINARY_DIR}/${DL_ARGS_PROJ}/download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${DL_ARGS_PROJ}/download)
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${DL_ARGS_PROJ}/download)

  # Now add the downloaded source directory to the build as normal.
  # The EXCLUDE_FROM_ALL option is useful if you only want to build
  # the downloaded project if something in the main build needs it.
  if(DL_ARGS_EXCLUDE_FROM_ALL)
    set(EXCLUDE_FROM_ALL "EXCLUDE_FROM_ALL")
  else()
    unset(EXCLUDE_FROM_ALL)
  endif()

  add_subdirectory(${CMAKE_BINARY_DIR}/${DL_ARGS_PROJ}/src/${DL_ARGS_PATH_SUFFIX}
                   ${CMAKE_BINARY_DIR}/${DL_ARGS_PROJ}/build
                   ${EXCLUDE_FROM_ALL})
endmacro()
