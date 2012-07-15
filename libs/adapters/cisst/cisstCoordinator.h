/*

  Safety Framework for Component-based Robotics

  Created on: July 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _cisstCoordinator_h
#define _cisstCoordinator_h

#include "coordinator.h"

namespace SF {

class cisstCoordinator: public Coordinator {
public:
    cisstCoordinator();
    ~cisstCoordinator();

    bool AddMonitor(const std::string & monitorJsonSpec);
};

};

#endif // _cisstCoordinator_h
