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

MongoDB::MongoDB(void)
{}

MongoDB::~MongoDB(void)
{}

const std::string MongoDB::ConvertTopicMessageToDBEntry(const JSONSerializer::TopicType topic, 
                                                         JSONSerializer & jsonSerializer)
{
    switch (topic) {
        case JSONSerializer::MONITOR:
            return ConvertTopicMessageToDBEntry_Monitor(jsonSerializer);
        case JSONSerializer::EVENT:
            return ConvertTopicMessageToDBEntry_Event(jsonSerializer);
        case JSONSerializer::SUPERVISOR:
        case JSONSerializer::INVALID:
        default:
            return std::string("SF::MongoDB::ConvertTopicMessageToDBEntry - invalid topic type");
    }
}

const std::string MongoDB::ConvertTopicMessageToDBEntry_Monitor(JSONSerializer & jsonSerializer)
{
    // Json placeholder for DB entry
    JSON::JSONVALUE entry;

    // MJ TODO: Should convert timestamp from json to UTC; right now current UTC is used instead.
    entry[SF::Dict::Json::time] = GetCurrentUTCTimeString();

    // Monitor data sample
    JSON::JSONVALUE _data;
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

        case Monitor::TARGET_CUSTOM:
            {
                entry[type] = custom;
                _data[custom] = jsonSerializer.GetMonitorFields().get(custom, 0.0).asDouble();
            }
            break;

        // [SFUPDATE]

        case Monitor::TARGET_INVALID:
        default:
            SFLOG_ERROR << "Failed to convert json serializer to JSON string: " << jsonSerializer.GetMonitorTargetType() << std::endl;
            return "";
    }

    entry[data] = _data;

    std::stringstream ss;
    ss << entry;

    return ss.str();
}

const std::string MongoDB::ConvertTopicMessageToDBEntry_Event(JSONSerializer & jsonSerializer)
{
    // FIXME
#if 0
    // Json placeholder for DB entry
    JSON::JSONVALUE entry;

    // MJ TODO: Should convert timestamp from json to UTC; right now current UTC is used instead.
    entry[SF::Dict::Json::time] = GetCurrentUTCTimeString();

    // Fault information
    JSON::JSONVALUE _data;
    _data[process]   = jsonSerializer.GetEventLocation()->GetProcessName();
    _data[component] = jsonSerializer.GetEventLocation()->GetComponentName();

    switch (jsonSerializer.GetFaultType()) {
        
        case Fault::FAULT_COMPONENT_PERIOD:
            {   
#if 0 // MJ TODO: implement this
                entry[type] = thread_period;
                _data[period_nominal] = jsonSerializer.GetMonitorFields().get(period_nominal, 0.0).asDouble();
                _data[period_actual]  = jsonSerializer.GetMonitorFields().get(period_actual, 0.0).asDouble();
#endif
            }
            break;

        case Fault::FAULT_COMPONENT_OVERRUN:
            {
#if 0 // MJ TODO: implement this
                entry[type] = thread_dutycycle_user;
                _data[dutycycle_user] = jsonSerializer.GetMonitorFields().get(dutycycle_user, 0.0).asDouble();
                _data[dutycycle_user_ratio] = jsonSerializer.GetMonitorFields().get(dutycycle_user_ratio, 0.0).asDouble();
#endif
            }
            break;

        case Fault::FAULT_APPLICATION:
            {
                entry[type] = fault_application;
                _data[detector] = jsonSerializer.GetFaultDetectorName();
                _data[timestamp] = jsonSerializer.GetTimestamp();
                _data[severity] = jsonSerializer.GetFaultFields().get(severity, 0.0).asDouble();
            }
            break;
        // [SFUPDATE]

        default:
            SFLOG_ERROR << "Failed to convert json serializer to JSON string: " << jsonSerializer.GetMonitorTargetType() << std::endl;
            return "";
    }

    entry[data] = _data;

    std::stringstream ss;
    ss << entry;

    return ss.str();
#endif
    return std::string("FIXME");
}

}
