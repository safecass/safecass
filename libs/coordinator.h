/*

  Safety Framework for Component-based Robotics

  Created on: July 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _coordinator_h
#define _coordinator_h

#include "common.h"

namespace SF {

// Adapter class: almost empty but pure virtual methods
class Coordinator {
public:
    Coordinator();
    virtual ~Coordinator();

    virtual bool AddMonitor(const std::string & monitorJsonSpec) = 0;
};

};

#endif // _coordinator_h
