macro(find_boost_library)
  #set(options        EXCLUDE_FROM_ALL)
  set(oneValueArgs LIB_NAME)
  #set(multiValueArgs CMAKE_OPTIONS)
  cmake_parse_arguments(BOOST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  # According to cmake FindBoost's convention
  string(TOUPPER ${BOOST_LIB_NAME} BOOST_LIB_NAME_UPPER)

  # This will fail unless boost package is found by find_boost, because
  # there is no boost package installed at configuration time.
  # Currently, boost is always downloaded and compiled at build time.
  # find_library(Boost_${BOOST_LIB_NAME_UPPER}_LIBRARY
               # NAME  boost_${BOOST_LIB_NAME}
               # PATHS ${Boost_LIBRARY_DIRS}
               # NO_DEFAULT_PATH
               # NO_CMAKE_FIND_ROOT_PATH)

  # Manually set location of boost package
  set (Boost_${BOOST_LIB_NAME_UPPER}_LIBRARY
       # TODO adjust suffix depending on static/shared compilation mode
       ${Boost_LIBRARY_DIRS}/libboost_${BOOST_LIB_NAME}.a)
endmacro()
