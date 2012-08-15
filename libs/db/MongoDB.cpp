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
#include "fault.h"

#include <iostream>

using namespace SF::Dict;

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

    entry["type"] = "monitor_period"; // MJ TODO: FIXME later
    // MJ TODO: this timestamp should be set by mtsMonitorComponent when collecting samples
    entry["time"] = GetCurrentUTCTimeString();

    // Get fault type
    const std::string faultTypeString = sample[TARGET].get(TYPE, "n/a").asString();
    Fault::FaultType faultType = Fault::GetFaultFromString(faultTypeString);

    // Monitor data sample
    switch (faultType) {
        case Fault::FAULT_COMPONENT_PERIOD: {   
            Json::Value _data;
            _data["process"]   = sample[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
            _data["component"] = sample[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
            _data["period_expected"] = 0.1; // MJ TODO: FIXME: this should be provided by monitor component
            _data["period_actual"] = sample.get(SAMPLE, 0.0).asDouble();
            entry["data"] = _data;
        }
        break;

        case Fault::FAULT_INVALID:
        default:
            CMN_LOG_RUN_ERROR << "Failed to convert topic message to MongoDB entry due to invalid fault type string: "
                << "\"" << faultTypeString << "\"" << std::endl;
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
