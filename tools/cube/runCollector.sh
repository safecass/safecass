#!/bin/bash
#
# Safety Framework for Component-based Robotics
#
# Created on: August 15, 2012
#
# Copyright (C) 2012 Min Yang Jung, Peter Kazanzides
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

#
# bash script to run Cube collector
#
#NODE_BINARY_DIR=/usr/local/bin
CUBE_NODE_MODULE_DIR=/Users/MJ/project/tools/node_modules/cube

#export NODE_PATH=$CUBE_NODE_MODULE_DIR

echo "Running Cube collector... Press Ctrl+C to quit"
node "$CUBE_NODE_MODULE_DIR"/bin/collector
