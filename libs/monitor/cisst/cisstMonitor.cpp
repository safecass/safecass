/*

  Safety Framework for Component-based Robotics

  Created on: July 8, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstMonitor.h"
#include "json.h"

#include <sstream>

namespace SF {

cisstMonitor::cisstMonitor()
{
}

cisstMonitor::~cisstMonitor()
{
}

const std::string cisstTargetID::GetTargetUID(void) const
{
    std::stringstream ss;
    ss << "\"" << ProcessName << "\":\"" << ComponentName << "\":"
       << "[\"" << InterfaceProvidedName << "\":\"" << CommandName << "\", "
       << "\"" << EventGeneratorName << "\"]:"
       << "[\"" << InterfaceRequiredName << "\":\"" << FunctionName << "\", "
       << "\"" << EventHandlerName << "\"]";

    return ss.str();
}

const std::string cisstTargetID::GetTargetUID(Fault::FaultType faultType) const
{
    std::stringstream ss;

    switch (faultType) {
        case Fault::COMPONENT_PERIOD: {
            ss << "<"
               << "Component Fault "
               << "[\"" << ProcessName << "\":\"" << ComponentName << "\"]"
               << ">";
            break;
        }
        default:
        return GetTargetUID();
    }

    return ss.str();
}

std::string cisstMonitor::GetMonitorJSON(const std::string &        name,
                                         const Fault::FaultType     faultType,
                                         const Monitor::OutputType  outputType,
                                         const Monitor::StatusType  initialStatus,
                                         const TargetIDType &       targetId)
{
    /* Sample JSON file to specify Monitor instance in cisst
    {   
        // Monitor name
        "Name" : "Period Monitor",

        // Monitor target type
        "Target" : {
            "Type" : { "Component" : "Period" },
            "Identifier" : {
                "Process" : "LOCAL",
                "Component" : "TargetTaskName" }
        },
        
        // Monitor behaviors
        "Output" : {
            "Type" : "Stream" (or "Event"), // Could add active/passive concept
            "Config" : {
                // in case of stream
                "SamplingRate" : 0, // -1: continuous, 0: don't sample, > 1: Hz
                "State" : 0 } // 0: OFF, 1: ON
                // in case of event
                // TODO: more options can be added (exponentially weighted moving average (EWMA))
                "Threshold" : 0.001 * 0.1, 
                "Margin" : 0.001 * 0.05,
                "State" : 0 } // 0: OFF, 1: ON
            "Target" : {
                // TODO
                // output can be exported to centralized data collector / visualizer
                // output can be fed into FDD pipeline(s) as input
                }
            }
        }
    }
    */

    // Monitor should be installed first: It is a basically real-time "data collector"
    // used by real-time data visualizer or by filters and FDD pipelines. That is,
    // any filter or FDD pipeline requires a monitor to be installed first.
    Json::Value root;

    // Monitor name
    root[GetKeyString(NAME)] = name;

    // Monitor target type
    {   Json::Value _root;
        { Json::Value __root;
          __root[GetKeyString(NAME_COMPONENT)] = faultType;
          _root[GetKeyString(TYPE)] = __root;
        }

        { Json::Value __root;
          __root[GetKeyString(NAME_PROCESS)] = targetId.ProcessName;
          __root[GetKeyString(NAME_COMPONENT)] = targetId.ComponentName;
          _root[GetKeyString(IDENTIFIER)] = __root;
        }
        root[GetKeyString(TARGET)] = _root;
    }

    // Monitor behaviors
    {   Json::Value _root;
        _root[GetKeyString(TYPE)] = outputType;
        {   Json::Value __root;
            {
                __root[GetKeyString(STATE)] = initialStatus;
                if (outputType == OUTPUT_STREAM) {
                    __root[GetKeyString(SAMPLING_RATE)] = 1;
                } else if (outputType == OUTPUT_EVENT) {
                    Json::Value array(Json::arrayValue);
                    array.append(0.1);
                    array.append(0.5);
                    array.append(2.5);
                    //__root[GetKeyString(THRESHOLD)] = 3;
                    __root[GetKeyString(THRESHOLD)] = array;
                    __root[GetKeyString(MARGIN)] = 4;
                }
            }
            _root[GetKeyString(CONFIG)] = __root;
        }

        {   Json::Value __root;
            _root[GetKeyString(TARGET)] = __root;
        }
        root[GetKeyString(OUTPUT)] = _root;
    }

    std::stringstream ss;
    ss << root;

    return ss.str();
}

};
