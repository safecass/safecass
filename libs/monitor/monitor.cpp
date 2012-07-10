/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "monitor.h"
#include "json.h"

#include <map>
#include <sstream>

namespace SF {

std::map<Monitor::KeyTypes, std::string> dictionary;

Monitor::MonitorMapType MonitorMap;

Monitor::Monitor()
{
}

Monitor::~Monitor()
{
}

std::string Monitor::GetKeyString(KeyTypes keyType)
{
    if (dictionary.empty()) {
        dictionary[CONFIG]                  = "Config";
        dictionary[EVENT]                   = "Event";
        dictionary[IDENTIFIER]              = "Identifier";
        dictionary[MARGIN]                  = "Margin";
        dictionary[NAME]                    = "Name";
        dictionary[NAME_PROCESS]            = "Process";
        dictionary[NAME_COMPONENT]          = "Component";
        dictionary[NAME_INTERFACE_PROVIDED] = "InterfaceProvided";
        dictionary[NAME_INTERFACE_REQUIRED] = "InterfaceRequired";
        dictionary[NAME_EVENT_GENERATOR]    = "EventGenerator";
        dictionary[NAME_EVENT_HANDLER]      = "EventHandler";
        dictionary[OUTPUT]                  = "Output";
        dictionary[SAMPLING_RATE]           = "SamplingRate";
        dictionary[STATE]                   = "State";
        dictionary[STREAM]                  = "Stream";
        dictionary[TARGET]                  = "Target";
        dictionary[THRESHOLD]               = "Threshold";
        dictionary[TYPE]                    = "Type";
    }

    return dictionary[keyType];
}

bool Monitor::AddMonitor(const std::string & targetUID, const std::string & monitorJson)
{
    JSON json;
    json.Read(monitorJson.c_str());

    // Check if same monitoring target is already registered
    MonitorMapType::const_iterator it = MonitorMap.find(targetUID);
    if (it != MonitorMap.end()) {
        std::cerr << "Duplicated target UID: " << targetUID << std::endl;
        return false;
    }

    // create new monitor with (uid, json)
    // insert new monitor to monitor list
    // embed new monitor to cisst system

    // 2. run-time management
    // Q> how to fetch monitor's output?
    // Q> should SF use separate network middleware? pros vs. cons?
    // Q> if not, pros vs. cons?

    std::cout << "SUCCESS: " << targetUID << "\n, JSON: " << json.GetJSON() << std::endl;

    MonitorMap[targetUID] = monitorJson;

    return true;
}

};
