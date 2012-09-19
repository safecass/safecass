/*

  Safety Framework for Component-based Robotics

  Created on: August 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "MongoDB.h"

#include "json.h"
#include "dict.h"
#include "monitor.h"

#include <iostream>

using namespace SF::Dict::Json;

namespace SF {

const std::string MongoDB::GetDBEntryFromMonitorTopic(JSONSerializer & jsonSerializer)
{
    // Json placeholder for DB entry
    Json::Value entry;

    // MJ TODO: Should convert timestamp from json to UTC; right now current UTC is used instead.
    entry[SF::Dict::Json::time] = GetCurrentUTCTimeString();

    // Monitor data sample
    Json::Value _data;
    _data[process]   = jsonSerializer.GetEventLocation()->GetProcessName();
    _data[component] = jsonSerializer.GetEventLocation()->GetComponentName();

    switch (jsonSerializer.GetMonitorTargetType()) {
        case Monitor::TARGET_THREAD_PERIOD:
            {   
                entry[type] = thread_period;
                _data[period_nominal] = jsonSerializer.GetMonitorFields().get(period_nominal, 0.0).asDouble();
                _data[period_actual]  = jsonSerializer.GetMonitorFields().get(period_actual, 0.0).asDouble();
            }
            break;

        case Monitor::TARGET_THREAD_DUTYCYCLE_USER:
            {
                entry[type] = thread_dutycycle_user;
                _data[dutycycle_user] = jsonSerializer.GetMonitorFields().get(dutycycle_user, 0.0).asDouble();
                _data[dutycycle_user_ratio] = jsonSerializer.GetMonitorFields().get(dutycycle_user_ratio, 0.0).asDouble();
            }
            break;

        case Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL:
            {
                entry[type] = thread_dutycycle_total;
                _data[dutycycle_total] = jsonSerializer.GetMonitorFields().get(dutycycle_total, 0.0).asDouble();
                _data[dutycycle_total_ratio] = jsonSerializer.GetMonitorFields().get(dutycycle_total_ratio, 0.0).asDouble();
            }
            break;
        // [SFUPDATE]

        default:
            SFLOG_ERROR << "Failed to convert json serializer to JSON string" << std::endl;
            return "";
    }

    entry[data] = _data;

    std::stringstream ss;
    ss << entry;

    return ss.str();
}

}
