/*

  Safety Framework for Component-based Robotics

  Created on: July 8, 2012

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _cisstMonitor_h
#define _cisstMonitor_h

#include "monitor.h"

namespace SF {

class cisstMonitor {
public:
    cisstMonitor();
    virtual ~cisstMonitor();

    static bool CreateMonitor(const std::string & monitorSpecJson);
};

};

#endif // _cisstMonitor_h
