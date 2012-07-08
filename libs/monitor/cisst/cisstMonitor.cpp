/*

  Safety Framework for Component-based Robotics

  Created on: July 8, 2012

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstMonitor.h"
#include "json.h"

namespace SF {

cisstMonitor::cisstMonitor()
{
}

cisstMonitor::~cisstMonitor()
{
}

bool cisstMonitor::CreateMonitor(const std::string & monitorSpecJson)
{
    JSON json;
    json.Read(monitorSpecJson.c_str());

    std::cout << json.GetJSON() << std::endl;

    return false;
}

};
