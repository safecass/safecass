/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "monitor.h"
#include "dict.h"
#include "json.h"

namespace SF {

using namespace Dict;

Monitor::Monitor()
{
}

Monitor::~Monitor()
{
}

const std::string Monitor::GetString(OutputType type)
{
    if (type == OUTPUT_STREAM) return Dict::STREAM;
    else if (type == OUTPUT_EVENT) return Dict::EVENT;
}

const std::string Monitor::GetString(StatusType type)
{
    if (type == MONITOR_OFF) return Dict::OFF;
    else if (type == MONITOR_ON) return Dict::ON;
}

};
