include (ExternalProject)

macro(add_external_project)
  # Set up named macro arguments
  set(options        EXCLUDE_FROM_ALL)
  set(oneValueArgs   NAME URL URL_HASH CONFIG_CMD)
  set(multiValueArgs CMAKE_OPTIONS BUILD_CMD INSTALL_CMD)
  cmake_parse_arguments(EXT_PROJ_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(EXT_PROJ_ARGS_EXCLUDE_FROM_ALL)
    set(EXCLUDE_FROM_ALL "EXCLUDE_FROM_ALL")
  else()
    unset(EXCLUDE_FROM_ALL)
  endif()

  ExternalProject_Add(external-${EXT_PROJ_ARGS_NAME}
                      URL               ${EXT_PROJ_ARGS_URL}
                      URL_MD5           ${EXT_PROJ_ARGS_URL_HASH}
                      BUILD_IN_SOURCE   1
                      SOURCE_DIR        ${SAFECASS_BUILD_ROOT}/external_packages/${EXT_PROJ_ARGS_NAME}/src
                      DOWNLOAD_DIR      ${SAFECASS_BUILD_ROOT}/external_packages/${EXT_PROJ_ARGS_NAME}/download
                      CONFIGURE_COMMAND ${EXT_PROJ_ARGS_CONFIG_CMD}
                      BUILD_COMMAND     ${EXT_PROJ_ARGS_BUILD_CMD}
                      INSTALL_COMMAND   ${EXT_PROJ_ARGS_INSTALL_CMD}
                      TEST_COMMAND      ""
                      CMAKE_CACHE_ARGS  ${EXT_PROJ_ARGS_CMAKE_OPTIONS})
endmacro()
