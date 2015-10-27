macro(find_boost_library)
  #set(options        EXCLUDE_FROM_ALL)
  set(oneValueArgs LIB_NAME)
  #set(multiValueArgs CMAKE_OPTIONS)
  cmake_parse_arguments(BOOST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  # According to cmake FindBoost's convention
  string(TOUPPER ${BOOST_LIB_NAME} BOOST_LIB_NAME_UPPER)

  find_library(Boost_${BOOST_LIB_NAME_UPPER}_LIBRARY
               NAME  boost_${BOOST_LIB_NAME}
               PATHS ${Boost_LIBRARY_DIRS}
               NO_DEFAULT_PATH
               NO_CMAKE_FIND_ROOT_PATH)
  list (APPEND Boost_LIBRARIES ${BOOST_LIB})
endmacro()
