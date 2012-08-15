/*

  Safety Framework for Component-based Robotics

  Created on: August 15, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

//
// Cube evaluator (imported from cube)
//
var options = require("./evaluator-config"),
    cube = require("../"),
    server = cube.server(options);

server.register = function(db, endpoints) {
  cube.evaluator.register(db, endpoints);
};

server.start();
