/*

  Safety Framework for Component-based Robotics

  Created on: July 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstCoordinator.h"

namespace SF {

cisstCoordinator::cisstCoordinator()
{
    std::cout << "CISST SAFETY" << std::endl;
}

cisstCoordinator::~cisstCoordinator()
{
    std::cout << "CISST SAFETY end" << std::endl;
}

bool cisstCoordinator::AddMonitor(const std::string & monitorJsonSpec)
{
    std::cout << "AddMonitor" << std::endl;

    return true;
}

};
