#-----------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
#
#-----------------------------------------------------------------------------------
#
# Created on   : July 31, 2012
# Last revision: May 4, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
#
# Include this file only outside SAFECASS
if (NOT CURRENT_PROJECT_IS_SC)

  # Extend CMake Module Path to include SAFECASS cmake modules
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SC_CMAKE_MODULE_PATH})

  # Add the include and lib paths for SAFECASS
  include_directories (${SC_INCLUDE_DIR})
  link_directories (${SC_BUILD_LIBRARY_PATH})

endif (NOT CURRENT_PROJECT_IS_SC)
