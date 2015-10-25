include (ExternalProject)

macro(add_external_project)
  # Set up named macro arguments
  set(options        EXCLUDE_FROM_ALL)
  set(oneValueArgs   NAME URL URL_HASH)
  set(multiValueArgs CMAKE_OPTIONS)
  cmake_parse_arguments(EXT_PROJ_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(EXT_PROJ_ARGS_EXCLUDE_FROM_ALL)
    set(EXCLUDE_FROM_ALL "EXCLUDE_FROM_ALL")
  else()
    unset(EXCLUDE_FROM_ALL)
  endif()

  ExternalProject_Add(external-${EXT_PROJ_ARGS_NAME}
                      URL         ${EXT_PROJ_ARGS_URL}
                      SOURCE_DIR  ${SAFECASS_BUILD_ROOT}/external_packages/${EXT_PROJ_ARGS_NAME}/src
                      BINARY_DIR  ${SAFECASS_BUILD_ROOT}/external_packages/${EXT_PROJ_ARGS_NAME}/build
                      INSTALL_DIR ${SAFECASS_BUILD_ROOT}/external_packages/${EXT_PROJ_ARGS_NAME}
                      CONFIGURE_COMMAND ""
                      BUILD_COMMAND     ""
                      INSTALL_COMMAND   ""
                      TEST_COMMAND      ""
                      CMAKE_CACHE_ARGS ${EXT_PROJ_ARGS_CMAKE_OPTIONS})
endmacro()
