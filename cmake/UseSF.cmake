#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

# Include this file only outside Safety Framework
if (NOT CURRENT_PROJECT_IS_SF)

    # Extend CMake Module Path to include SF cmake modules
    set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SF_CMAKE_MODULE_PATH})

    # Add the include and lib paths for Safety Framework
    include_directories (${SF_INCLUDE_DIR})
    link_directories (${SF_BUILD_LIBRARY_PATH})

    # Load all settings for external dependencies
    #cisst_load_package_setting (${CISST_LIBRARIES_REQUIRED_INTERNAL})

endif (NOT CURRENT_PROJECT_IS_SF)

# Add link and include directories based on required libraries
#cisst_set_directories (${CISST_LIBRARIES_REQUIRED_INTERNAL})
