#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

# copy files from [SRC]/tools/launchIceBox to [BUILD]/tools/launchIceBox
set (BASE_DIR "${SafetyFramework_SOURCE_DIR}/tools/launchIceBox")
set (TARGET_DIR "${BUILD_ROOT}/bin")
set (SRC_FILES "config.icebox"
               "config.service" 
               "launch.sh")

foreach (SRC_FILE ${SRC_FILES})
    # create Freeze db folder for IceBox
    file (MAKE_DIRECTORY "${TARGET_DIR}/db")
    # copy IceBox launch script and setting files to bin folder
    file (COPY ${BASE_DIR}/${SRC_FILE} 
               DESTINATION ${TARGET_DIR})
endforeach (SRC_FILE)
