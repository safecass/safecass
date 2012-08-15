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

# MJ TEMP: bash script example:
#   http://tldp.org/LDP/abs/html/sha-bang.html

#
# bash script to launch the visualization framework which consists of
#   
#   1) DB backend (MongoDB)
#   2) Cube collector
#   3) Cube evaluator

# Launch MongoDB
# Check if db/MongoDB/ folder exists. If not, create one.
MONGDB_BINARY_DIR=/Users/MJ/project/tools/mongodb-osx-x86_64-2.0.7/bin
if [ ! -d "db/MongoDB" ]; then
    echo "Creating database for MongoDB/Cube ..."
    mkdir -p db/MongoDB
    # Run MongoDB daemon as background process
    "$MONGDB_BINARY_DIR"/mongod --dbpath db/MongoDB
    # Create and initialize MongoDB
    "$MONGDB_BINARY_DIR"/mongo < createMongoDB.js
fi
if [ -d "db/MongoDB" ]; then
    echo "MongoDB folder detected"
    # Run MongoDB daemon as background process
    "$MONGDB_BINARY_DIR"/mongod --dbpath db/MongoDB
fi

echo "Running MongoDB... Press Ctrl+C to quit"

# mongo --host="the.server.ip:port" dbname script.js
# mongo < script.js
#mongo test --eval "printjson(db.getCollectionNames())"
#
# script.js:
#   db.mycollection.findOne()
#   db.getCollectionNames().forEach(function(collection) {
#     print(collection);
#   });
#
# another js example:
#   db = connect("localhost:27017/test");
#   var cursor = db.unicorns.find({gender : "m"});
#   var total = 0;
#   while(cursor.hasNext()) {
#      var obj = cursor.next();
#        total += (obj.hits || 0);
#   }
#   print(total);
