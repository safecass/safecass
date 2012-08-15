#
# Safety Framework for Component-based Robotics
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

set (TARGET_DIR "${BUILD_ROOT}/bin")

#
# IceBox
#
# copy files from [SRC]/tools/launchIceBox to [BUILD]/bin
set (ICEBOX_BASE_DIR "${SafetyFramework_SOURCE_DIR}/tools/launchIceBox")
set (ICEBOX_SRC_FILES "config.icebox"
                      "config.service" 
                      "launchIceBox.sh")

foreach (ICEBOX_SRC_FILE ${ICEBOX_SRC_FILES})
    # create Freeze db folder for IceBox
    file (MAKE_DIRECTORY "${TARGET_DIR}/db/Freeze")
    # copy IceBox launch script and setting files to bin folder
    file (COPY ${ICEBOX_BASE_DIR}/${ICEBOX_SRC_FILE} 
               DESTINATION ${TARGET_DIR})
endforeach (ICEBOX_SRC_FILE)

#
# MongoDB
#
# copy files from [SRC]/tools/scripts to [BUILD]/bin
set (MONGODB_BASE_DIR "${SafetyFramework_SOURCE_DIR}/tools/scripts")
set (MONGODB_SRC_FILES "createMongoDB.js"
                       "runDBCube.sh")

foreach (MONGODB_SRC_FILE ${MONGODB_SRC_FILES})
    # create MongoDB folder for MongoDB
    file (MAKE_DIRECTORY "${TARGET_DIR}/db/MongoDB")
    # copy scripts to bin folder
    file (COPY ${MONGODB_BASE_DIR}/${MONGODB_SRC_FILE} 
               DESTINATION ${TARGET_DIR})
endforeach (MONGODB_SRC_FILE)
