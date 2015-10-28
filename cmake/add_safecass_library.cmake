macro (add_safecass_library libname)
  file(GLOB sources ${SAFECASS_SOURCE_ROOT}/libs/${libname}/code/*.cpp)
  # TODO: update this after handling shared library properly
  #add_library(${libname} SHARED ${sublib_sources})
  add_library(${libname} ${sources})
  target_include_directories(${libname} PUBLIC ${SAFECASS_SOURCE_ROOT}/libs/${libname})
endmacro()
