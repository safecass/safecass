#-----------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
#
#-----------------------------------------------------------------------------------
#
# Created on   : Jul 14, 2014
# Last revision: May 4, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
#
project(viewerQt)

set (REQUIRED_SC_LIBS SCLib ${JSONCPP_LIBRARIES})

# List cisst libraries needed
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstCommonQt
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstMultiTaskQt
                              cisstQt)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})
if (cisst_FOUND_AS_REQUIRED)
  # load cisst configuration
  include (${CISST_USE_FILE})
  # wrap all Qt files
  qt4_wrap_ui (QT_VIEWER_WRAPPED_UI viewerQtWidget.ui)
  qt4_wrap_cpp (QT_VIEWER_WRAPPED_CPP viewerQtComponent.h
                                      QtJsBridge/jsApiHandler.h
                                      QtJsBridge/jsApiProxy.h
                                      QtJsBridge/jsCppBridge.h)
  # needed to include Qt generated header files
  include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QT_QTWEBKIT_INCLUDE_DIR})

  # create example using a single process
  add_executable (viewerQt
                  main.cpp
                  accessor.h accessor.cpp
                  viewerQtComponent.h viewerQtComponent.cpp 
                  QtJsBridge/jsApiHandler.cpp
                  QtJsBridge/jsApiProxy.cpp
                  QtJsBridge/jsCppBridge.cpp
                  ${QT_VIEWER_WRAPPED_UI}
                  ${QT_VIEWER_WRAPPED_CPP})
  cisst_target_link_libraries (viewerQt ${REQUIRED_CISST_LIBRARIES})
  target_link_libraries (viewerQt ${REQUIRED_SC_LIBS} ${QT_QTWEBKIT_LIBRARY})
else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
