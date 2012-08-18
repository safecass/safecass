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

const std::string MongoDB::GetDBEntryFromMonitorTopic(const std::string & topic)
{
    // Parse source Json
    SF::JSON json;
    if (!json.Read(topic.c_str())) {
        CMN_LOG_RUN_ERROR << "Failed to parse json from topic message: " << topic << std::endl;
        return "";
    }

    Json::Value & sample = json.GetRoot();

    // Generate Json for MongoDB entry
    Json::Value entry;

    // MJ TODO: this timestamp should be set by mtsMonitorComponent when collecting samples
    entry["time"] = GetCurrentUTCTimeString();

    // Get monitor type
    const std::string targetTypeString = sample[TARGET].get(TYPE, "n/a").asString();
    Monitor::TargetType targetType = Monitor::GetTargetTypeFromString(targetTypeString);

    // Monitor data sample
    switch (targetType) {
        case Monitor::TARGET_THREAD_PERIOD: {   
            // entry type
            entry["type"] = "monitor_period";
            // measurement data
            Json::Value _data;
            _data["process"]   = sample[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
            _data["component"] = sample[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
            _data["period_expected"] = 0.1; // MJ TODO: FIXME: this should be provided by monitor component
            _data["period_actual"] = sample.get(SAMPLE, 0.0).asDouble();
            entry["data"] = _data;
        }
        break;

        case Monitor::TARGET_THREAD_DUTYCYCLE_USER: {
            // entry type
            entry["type"] = "monitor_exectime_user";
            // measurement data
            Json::Value _data;
            _data["process"]   = sample[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
            _data["component"] = sample[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
            _data["exec_time"] = sample.get(SAMPLE, 0.0).asDouble();
            entry["data"] = _data;
        }
        break;

        case Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL: {
            // entry type
            entry["type"] = "monitor_exectime_total";
            // measurement data
            Json::Value _data;
            _data["process"]   = sample[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
            _data["component"] = sample[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
            _data["exec_time"] = sample.get(SAMPLE, 0.0).asDouble();
            entry["data"] = _data;
        }
        break;
        // [SFUPDATE]

        default:
            CMN_LOG_RUN_ERROR << "Failed to convert topic message to MongoDB entry due to invalid fault type string: "
                << "\"" << targetTypeString << "\"" << std::endl;
    }

    std::stringstream ss;
    ss << entry;

    return ss.str();
}

MongoDB::MongoDB()
{
}

MongoDB::~MongoDB()
{
}

}
