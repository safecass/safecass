/*

  Safety Framework for Component-based Robotics

  Created on: August 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "supervisor.h"

namespace SC {

Supervisor::Supervisor()
{
}

Supervisor::~Supervisor()
{
}

const std::string Supervisor::GetSupervisorName(void) {
    return std::string("Safety Framework Supervisor");
}

};
