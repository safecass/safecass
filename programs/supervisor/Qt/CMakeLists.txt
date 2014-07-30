#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : Aug 7, 2012
# Last revision: Jul 29, 2014
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#
project(supervisorQt)

# List cisst libraries needed
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstCommonQt
                              cisstVector
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
  qt4_wrap_ui (QT_SUPERVISOR_WRAPPED_UI supervisorQtWidget.ui)
  qt4_wrap_cpp (QT_SUPERVISOR_WRAPPED_CPP supervisorQtComponent.h)

  # needed to include Qt generated header files
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  #include_directories(/Users/MJ/QtSDK/Desktop/Qt/4.8.1/gcc/include/QtWebKit)

  # create example using a single process
  add_executable (supervisorQt
                  main.cpp
                  supervisorQtComponent.cpp
                  supervisorQtComponent.h
                  ${QT_SUPERVISOR_WRAPPED_UI}
                  ${QT_SUPERVISOR_WRAPPED_CPP})
  #set_property (TARGET supervisorQt PROPERTY FOLDER "cisstMultiTask/examples")
  cisst_target_link_libraries (supervisorQt ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
