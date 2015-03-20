#------------------------------------------------------------------------
#
# CASROS: Component-based Architecture for Safe Robotic Systems
#
# Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
#
#------------------------------------------------------------------------
#
# Created on   : Jul 29, 2012
# Last revision: Mar 19, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
# Github       : https://github.com/minyang/casros
#
set (SF_BUILD_BIN_ROOT "${SF_BUILD_ROOT}/bin")

##############################
# console
##############################
add_subdirectory(console)

##############################
# viewer
##############################
add_subdirectory(viewer)

##############################
# IceBox
##############################
# copy files from [SRC]/tools/IceBox to [BUILD]/bin
set (ICEBOX_BASE_DIR "${SafetyFramework_SOURCE_DIR}/tools/IceBox")
set (ICEBOX_SRC_FILES "config.icebox"
                      "config.service" 
                      "launchIceBox.sh")

foreach (ICEBOX_SRC_FILE ${ICEBOX_SRC_FILES})
    # create Freeze db folder for IceBox
    if (NOT EXISTS "${SF_BUILD_BIN_ROOT}/db/Freeze")
        file (MAKE_DIRECTORY "${SF_BUILD_BIN_ROOT}/db/Freeze")
    endif ()
    # copy IceBox launch script and setting files to bin folder
    if (NOT EXISTS ${SF_BUILD_BIN_ROOT}/${ICEBOX_SRC_FILE})
        file (COPY ${ICEBOX_BASE_DIR}/${ICEBOX_SRC_FILE} 
                DESTINATION ${SF_BUILD_BIN_ROOT})
    endif ()
endforeach (ICEBOX_SRC_FILE)

##############################
# Visualization back-end framework: MongoDB and Cube
##############################
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
    #file (MAKE_DIRECTORY "${SF_BUILD_BIN_ROOT}/db/MongoDB")
    # copy scripts to bin folder
    file (COPY ${MONGODB_BASE_DIR}/${MONGODB_SRC_FILE} 
               DESTINATION ${SF_BUILD_BIN_ROOT})
endforeach (MONGODB_SRC_FILE)
