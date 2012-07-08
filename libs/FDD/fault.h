/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

namespace SF {

class Fault {
public:
    typedef enum {
        COMPONENT_THREAD_SCHEDULING_LATENCY
    } FaultType;

public:
    Fault() {}
};

};
