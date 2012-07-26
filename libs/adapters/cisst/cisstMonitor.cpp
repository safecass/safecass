/*

  Safety Framework for Component-based Robotics

  Created on: July 8, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "dict.h"
#include "cisstMonitor.h"
#include "json.h"

#include <sstream>

namespace SF {

using namespace Dict;

//-----------------------------------------------
// cisstTargetID
//-----------------------------------------------
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
        case Fault::FAULT_COMPONENT_PERIOD: {
            ss << "<"
               << Fault::GetFaultString(faultType)
               << "[\"" << ProcessName << "\":\"" << ComponentName << "\"]"
               << ">";
            break;
        }
        default:
        return GetTargetUID();
    }

    return ss.str();
}

void cisstTargetID::ToStream(std::ostream & outputStream) const
{
    outputStream << "Process: " << ProcessName << ", "
                 << "Component: " << ComponentName << ", "
                 << "InterfaceProvided: " << InterfaceProvidedName << ", "
                 << "InterfaceRequired: " << InterfaceRequiredName << ", "
                 << "Command: " << CommandName << ", "
                 << "Function: " << FunctionName << ", "
                 << "EventGenerator: " << EventGeneratorName << ", "
                 << "EventHandler: " << EventHandlerName;
}

//-----------------------------------------------
// cisstMonitor
//-----------------------------------------------
cisstMonitor::cisstMonitor(): Status(MONITOR_OFF), OutputType(OUTPUT_EVENT)
{
}

cisstMonitor::~cisstMonitor()
{
}

std::string cisstMonitor::GetMonitorJSON(const std::string &       name,
                                         const Fault::FaultType    faultType,
                                         const Monitor::OutputType outputType,
                                         const Monitor::StatusType initialStatus,
                                         const TargetIDType &      targetId)
{
    // Monitor should be installed first: It is a basically real-time "data collector"
    // used by real-time data visualizer or by filters and FDD pipelines. That is,
    // any filter or FDD pipeline requires a monitor to be installed first.
    Json::Value root;

    // Monitor name
    /*
        "Name" : "Period Monitor"
    */
    root[NAME] = name;

    // Monitor target type
    /*
        "Target" : {
            //"Type" : { "Component" : "Period" },
            "Type" : "FAULT_COMPONENT_PERIOD",
            "Identifier" : {
                "Process" : "LOCAL",
                "Component" : "TargetTaskName" }
        },
    */
    {   Json::Value _root;
        /*
        { Json::Value __root;
          __root[NAME_COMPONENT] = Fault::GetFaultString(faultType);
          _root[TYPE] = __root;
        }
        */
        _root[TYPE] = Fault::GetFaultString(faultType);

        { Json::Value __root;
          __root[NAME_PROCESS] = targetId.ProcessName;
          __root[NAME_COMPONENT] = targetId.ComponentName;
          _root[IDENTIFIER] = __root;
        }
        root[TARGET] = _root;
    }

    // Monitor behaviors
    /*
        "Output" : {
            "Type" : "Stream" (or "Event"), // Could add active/passive concept
            "Config" : {
                // in case of stream
                "SamplingRate" : 0, // -1: continuous, 0: don't sample, > 1: Hz
                "State" : "ON" }
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
    */
    {   Json::Value _root;
        _root[TYPE] = Monitor::GetOutputString(outputType);
        {   Json::Value __root;
            {
                __root[STATE] = Monitor::GetStatusString(initialStatus);
                if (outputType == OUTPUT_STREAM) {
                    __root[SAMPLING_RATE] = 1; // MJ TEMP
                } else if (outputType == OUTPUT_EVENT) {
                    Json::Value array(Json::arrayValue);
                    array.append(0.1);
                    array.append(0.5);
                    array.append(2.5);
                    //__root[THRESHOLD] = 3;
                    __root[THRESHOLD] = array;
                    __root[MARGIN] = 4;
                }
            }
            _root[CONFIG] = __root;
        }

        {   Json::Value __root;
            {
                // MJ TEMP: ip of IceStorm service?
                Json::Value array(Json::arrayValue);
                array.append("127.0.0.1");
                array.append("10.162.34.118"); // my desktop
                __root[PUBLISH] = array;
            }
            _root[TARGET] = __root;
        }
        root[OUTPUT] = _root;
    }

    std::stringstream ss;
    ss << root;

    return ss.str();
}

bool cisstMonitor::IsActive(void) const
{
    return (Status == Monitor::MONITOR_ON);
}

bool cisstMonitor::IsStream(void) const
{
    return (OutputType == OUTPUT_STREAM);
}

bool cisstMonitor::IsEvent(void) const
{
    return (OutputType == OUTPUT_EVENT);
}

Fault::FaultType cisstMonitor::GetFaultType(void) const
{
    return FaultType;
}

TargetIDType & cisstMonitor::GetTargetID(void)
{
    return TargetID;
}

void cisstMonitor::SetFaultType(const Fault::FaultType faultType)
{
    FaultType = faultType;
}

void cisstMonitor::SetStatus(const Monitor::StatusType status)
{
    Status = status;
}

void cisstMonitor::SetOutputType(const Monitor::OutputType outputType)
{
    OutputType = outputType;
}

void cisstMonitor::SetTargetId(const TargetIDType & targetID)
{
    TargetID = targetID;
}

void cisstMonitor::SetSamplingRate(int samplingRate)
{
    SamplingRate = samplingRate;
}

void cisstMonitor::SetAddressesToPublish(const StrVecType & addresses)
{
    AddressesToPublish = addresses;
}

void cisstMonitor::ToStream(std::ostream & outputStream) const
{
    outputStream << "Fault type: " << Fault::GetFaultString(FaultType) << ", "
                 << "TargetID: " << TargetID << ", "
                 << "Status: " << Monitor::GetStatusString(Status) << ", "
                 << "OutputType: " << Monitor::GetOutputString(OutputType) << ", "
                 << "SamplingRate: " << SamplingRate << ", "
                 << "Addresses: ";
    if (AddressesToPublish.empty()) {
        outputStream << "(NONE)";
    } else {
        outputStream << "[ ";
        for (size_t i = 0; i < AddressesToPublish.size(); ++i) {
            outputStream << AddressesToPublish[i] << " ";
        }
        outputStream << "]";
    }
}

};
