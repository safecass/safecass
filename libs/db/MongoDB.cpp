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
        SFLOG_ERROR << "Failed to parse json from topic message: " << topic << std::endl;
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
    double nominalPeriod;
    switch (targetType) {
        case Monitor::TARGET_THREAD_PERIOD: {   
            // entry type
            entry["type"] = "period";
            // measurement data
            Json::Value _data;
            _data["process"]   = sample[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
            _data["component"] = sample[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
            _data["nominal"] = sample[TARGET].get(PERIOD_EXPECTED, 0.0).asDouble();
            _data["actual"] = sample.get(SAMPLE, 0.0).asDouble();
            entry["data"] = _data;
        }
        break;

        case Monitor::TARGET_THREAD_DUTYCYCLE_USER: {
            // entry type
            entry["type"] = "exectime_user";
            // measurement data
            Json::Value _data;
            _data["process"]   = sample[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
            _data["component"] = sample[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
            _data["exectime"] = sample.get(SAMPLE, 0.0).asDouble();
            nominalPeriod = sample[TARGET].get(PERIOD_EXPECTED, 0.0).asDouble();
            if (nominalPeriod != 0.0)
                _data["exectime_ratio"] = (sample.get(SAMPLE, 0.0).asDouble() / nominalPeriod) * 100.0;
            else
                _data["exectime_ratio"] = 0.0;
            entry["data"] = _data;
        }
        break;

        case Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL: {
            // entry type
            entry["type"] = "exectime_total";
            // measurement data
            Json::Value _data;
            _data["process"]   = sample[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
            _data["component"] = sample[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
            _data["exectime"] = sample.get(SAMPLE, 0.0).asDouble();
            nominalPeriod = sample[TARGET].get(PERIOD_EXPECTED, 0.0).asDouble();
            if (nominalPeriod != 0.0)
                _data["exectime_ratio"] = (sample.get(SAMPLE, 0.0).asDouble() / nominalPeriod) * 100.0;
            else
                _data["exectime_ratio"] = 0.0;
            entry["data"] = _data;
        }
        break;
        // [SFUPDATE]

        default:
            SFLOG_ERROR << "Failed to convert topic message to MongoDB entry due to invalid fault type string: "
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
