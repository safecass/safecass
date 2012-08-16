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

const std::string Monitor::GetStatusString(StatusType type)
{
    if (type == MONITOR_OFF) return OFF;
    else if (type == MONITOR_ON) return ON;

    return Dict::INVALID;
}

Monitor::StatusType Monitor::GetStatusFromString(const std::string & str)
{
    if (str.compare(Dict::OFF) == 0) return MONITOR_OFF;
    else if (str.compare(Dict::ON) == 0) return MONITOR_ON;

    return MONITOR_INVALID;
}

const std::string Monitor::GetOutputString(OutputType type)
{
    if (type == OUTPUT_STREAM) return Dict::STREAM;
    else if (type == OUTPUT_EVENT) return Dict::EVENT;
    
    return Dict::INVALID;
}

Monitor::OutputType Monitor::GetOutputFromString(const std::string & str)
{
    if (str.compare(Dict::STREAM) == 0) return OUTPUT_STREAM;
    else if (str.compare(Dict::EVENT) == 0) return OUTPUT_EVENT;

    return OUTPUT_INVALID;
}

};
