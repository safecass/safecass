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
# copy files from [SRC]/tools/IceBox to [BUILD]/bin
set (ICEBOX_BASE_DIR "${SafetyFramework_SOURCE_DIR}/tools/IceBox")
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
# Visualization back-end framework: MongoDB and Cube
#
# copy files from [SRC]/tools/cube to [BUILD]/bin
set (MONGODB_BASE_DIR "${SafetyFramework_SOURCE_DIR}/tools/cube")
set (MONGODB_SRC_FILES # MongoDB
                       "createMongoDB.js"
                       "runDBCube.sh"
                       "runCollector.sh"
                       "runEvaluator.sh"
                       # cube collector and its configuration
                       "collector-config.js" "collector.js"
                       # cube evaluator and its configuration
                       "evaluator-config.js" "evaluator.js"
                       )

foreach (MONGODB_SRC_FILE ${MONGODB_SRC_FILES})
    # MJ: Don't create db here: it should be created by runDBCube.sh
    # create MongoDB folder for MongoDB
    #file (MAKE_DIRECTORY "${TARGET_DIR}/db/MongoDB")
    # copy scripts to bin folder
    file (COPY ${MONGODB_BASE_DIR}/${MONGODB_SRC_FILE} 
               DESTINATION ${TARGET_DIR})
endforeach (MONGODB_SRC_FILE)
