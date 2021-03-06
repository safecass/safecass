//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : July 8, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#include "cisstMonitor.h"

#include "dict.h"
#include "cisstDic.h"
#include "jsonSerializer.h"

#include <sstream>

using namespace SC;

cisstMonitor::cisstMonitor(const Monitor::TargetType target,
                           cisstEventLocation *      locationID,
                           const Monitor::StateType  state,
                           const Monitor::OutputType output,
                           const SamplingRateType    samplingRate)
    : Monitor(target, locationID, state, output, samplingRate)
{
}

cisstMonitor::cisstMonitor(const JsonWrapper::JsonValue & jsonNode)
    : Monitor(jsonNode)
{
    // Extract target information
    const JsonWrapper::JsonValue location = jsonNode["location"];

    std::string processName, componentName;
    std::string interfaceProvidedName, interfaceRequiredName;
    std::string commandName, eventHandlerName;
    std::string functionName, eventGeneratorName;

    commandName           = JsonWrapper::GetSafeValueString(location, Dict::Json::command);
    eventHandlerName      = JsonWrapper::GetSafeValueString(location, Dict::Json::event_handler);
    functionName          = JsonWrapper::GetSafeValueString(location, Dict::Json::function);
    eventGeneratorName    = JsonWrapper::GetSafeValueString(location, Dict::Json::event_generator);

    cisstEventLocation * locationID =
        new cisstEventLocation(this->LocationID->GetProcessName(),
                               this->LocationID->GetComponentName(),
                               this->LocationID->GetInterfaceProvidedName(),
                               this->LocationID->GetInterfaceRequiredName(),
                               commandName,
                               functionName,
                               eventGeneratorName,
                               eventHandlerName);

    // Replace eventLocationBase instance that base class (SC::Monitor) created
    // with cisstEventLocation instance.
    delete this->LocationID;
    this->LocationID = locationID;
}

cisstMonitor::~cisstMonitor()
{
}

cisstEventLocation * cisstMonitor::GetLocationID(void) const
{
    return dynamic_cast<cisstEventLocation *>(this->LocationID);
}

const std::string cisstMonitor::GetMonitorJSON(void) const
{
    return "";
    // TEMP
#if 0
    JsonWrapper::JsonValue root;
    cisstEventLocation * locationID = GetLocationID();
    if (!locationID)
        return std::string("ERROR: no location information");

    root[name] = this->GetUIDAsString();

    // Monitor target type
    {   JsonWrapper::JsonValue _root;
        _root[type] = Monitor::GetTargetTypeString(Target);

        { ::Json::Value __root;
          __root[process]                = locationID->GetProcessName();
          __root[component]              = locationID->GetComponentName();
          __root[interface_provided]     = locationID->GetInterfaceProvidedName();
          __root[interface_required]     = locationID->GetInterfaceRequiredName();
          __root[cisst::command]         = locationID->GetCommandName();
          __root[cisst::function]        = locationID->GetFunctionName();
          // __root[cisst::event_generator] = locationID->GetEventGeneratorName();
          // __root[cisst::event_handler]   = locationID->GetEventHandlerName();
          // _root[location] = __root;
        }
        root[localization] = _root;
    }

    // Monitor behaviors
    {   JsonWrapper::JsonValue _root;
        _root[type] = Monitor::GetOutputTypeString(Output);
        {   ::Json::Value __root;
            {
                __root[state] = Monitor::GetStateTypeString(this->State);
                if (Output == OUTPUT_STREAM) {
                    __root[sampling_rate] = SamplingRate;
                } else if (Output == OUTPUT_EVENT) {
#if 0 // MJ TEMP: not yet decided how to use these fields
// 02/08/13: isn't this what filters do???
                    ::Json::Value array(::Json::arrayValue);
                    array.append(0.1);
                    array.append(0.5);
                    array.append(2.5);
                    //__root[THRESHOLD] = 3;
                    __root[THRESHOLD] = array;
                    __root[MARGIN] = 4;
#endif
                }
            }
            _root[config] = __root;
        }

#if 0 // MJ: where to publish is determined by Ice configuration files
        {   ::Json::Value __root;
            {
                ::Json::Value array(::Json::arrayValue);
                for (size_t i = 0; i < AddressesToPublish.size(); ++i)
                    array.append(AddressesToPublish[i]);
                __root[PUBLISH] = array;
            }
            _root[TARGET] = __root;
        }
        root[OUTPUT] = _root;
#endif
    }

    std::stringstream ss;
    ss << root;

    return ss.str();
#endif
}

const std::string cisstMonitor::GetJsonForPublish(double UNUSED(sample), double UNUSED(currentTick)) const
{
    return "FIXME";
#if 0
    // Create JSONSerializer instance 
    JSONSerializer serializer;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(this->LocationID);

    // Populate common fields
    serializer.SetTopicType(Topic::DATA);
    serializer.SetCategoryTypeData(Topic::Data::MONITOR);
    serializer.SetEventLocation(locationID);
    serializer.SetTimestamp(currentTick);

    // Populate monitor information
    serializer.SetMonitorTargetType(this->Target);

    // Populate monitor-specific fields
    ::Json::Value & fields = serializer.GetMonitorFields();
    switch (serializer.GetMonitorTargetType()) {
        case TARGET_THREAD_PERIOD:
            fields[period_nominal] = GetSamplingPeriod();
            fields[period_actual] = sample;
            break;
        case TARGET_THREAD_DUTYCYCLE_USER:
            fields[dutycycle_user] = sample;
            fields[dutycycle_user_ratio] = sample / GetSamplingPeriod();
            break;
        case TARGET_THREAD_DUTYCYCLE_TOTAL:
            fields[dutycycle_total] = sample;
            fields[dutycycle_total_ratio] = sample / GetSamplingPeriod();
            break;
        case TARGET_FILTER_EVENT:
            // MJ: Filter events (i.e., faults) are handled separately
            SCASSERT(false);
            break;
            // [SCUPDATE]
        case TARGET_INVALID:
        default:
            break;
    }

    return serializer.GetJSON();
#endif
}

/*
const std::string cisstMonitor::GetJsonForPublishingPeriod(double sample) const
{
#if 0
    ::Json::Value root;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(LocationID);

    ::Json::Value _root;
    _root[TYPE] = Monitor::GetTargetTypeString(Target);
    _root[PERIOD_EXPECTED] = GetSamplingPeriod();

    { ::Json::Value __root;
        __root[NAME_PROCESS] = locationID->GetProcessName();
        __root[NAME_COMPONENT] = locationID->GetComponentName();
        _root[IDENTIFIER] = __root;
    }
    root[TARGET] = _root;
    root[SAMPLE] = sample;

    std::stringstream ss;
    ss << root;

    return ss.str();
#endif

    // Create JSONSerializer instance 
    JSONSerializer serializer;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(this->LocationID);

    // Populate common fields
    serializer.SetTopicType(JSONSerializer::MONITOR);
    serializer.SetEventLocation(locationID);
    serializer.SetTimestamp(timestamp);

    // Populate monitor information
    serializer.SetMonitorTargetType(this->Target);

    // Populate monitor-specific fields
    ::Json::Value & fields = serializer.GetMonitorFields();
    fields[SC::Dict::Json::period_expected] = GetSamplingPeriod();
    fields[SC::Dict::Json::sample] = sample;

    return serializer.GetJSON();
} 

const std::string cisstMonitor::GetJsonForPublishingDutyCycleUser(double dutyCycle) const
{
#if 0
    ::Json::Value root;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(LocationID);

    ::Json::Value _root;
    _root[TYPE] = Monitor::GetTargetTypeString(Target);
    _root[PERIOD_EXPECTED] = GetSamplingPeriod();

    { ::Json::Value __root;
        __root[NAME_PROCESS] = locationID->GetProcessName();
        __root[NAME_COMPONENT] = locationID->GetComponentName();
        _root[IDENTIFIER] = __root;
    }
    root[TARGET] = _root;
    root[SAMPLE] = dutyCycle;

    std::stringstream ss;
    ss << root;

    return ss.str();
#endif

    // Create JSONSerializer instance 
    JSONSerializer serializer;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(this->LocationID);

    // Populate common fields
    serializer.SetTopicType(JSONSerializer::MONITOR);
    serializer.SetEventLocation(locationID);
    serializer.SetTimestamp(timestamp);

    // Populate monitor information
    serializer.SetMonitorTargetType(this->Target);

    // Populate monitor-specific fields
    ::Json::Value & fields = serializer.GetMonitorFields();
    fields[SC::Dict::Json::period_expected] = GetSamplingPeriod();
    fields[SC::Dict::Json::sample] = dutyCycle;

    return serializer.GetJSON();
}

const std::string cisstMonitor::GetJsonForPublishingDutyCycleTotal(double dutyCycle) const
{
#if 0
    ::Json::Value root;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(LocationID);

    ::Json::Value _root;
    _root[TYPE] = Monitor::GetTargetTypeString(Target);
    _root[PERIOD_EXPECTED] = GetSamplingPeriod();

    { ::Json::Value __root;
        __root[NAME_PROCESS] = locationID->GetProcessName();
        __root[NAME_COMPONENT] = locationID->GetComponentName();
        _root[IDENTIFIER] = __root;
    }
    root[TARGET] = _root;
    root[SAMPLE] = dutyCycle;

    std::stringstream ss;
    ss << root;

    return ss.str();
#endif

    // Create JSONSerializer instance 
    JSONSerializer serializer;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(this->LocationID);

    // Populate common fields
    serializer.SetTopicType(JSONSerializer::MONITOR);
    serializer.SetEventLocation(locationID);
    serializer.SetTimestamp(timestamp);

    // Populate monitor information
    serializer.SetMonitorTargetType(this->Target);

    // Populate monitor-specific fields
    ::Json::Value & fields = serializer.GetMonitorFields();
    fields[SC::Dict::Json::period_expected] = GetSamplingPeriod();
    fields[SC::Dict::Json::sample] = dutyCycle;

    return serializer.GetJSON();
}
*/

void cisstMonitor::ToStream(std::ostream & outputStream, bool includeLocation) const
{
    Monitor::ToStream(outputStream, includeLocation);

    if (!includeLocation) {
        outputStream << "LocationID: ";
        if (LocationID == 0)
            outputStream << "NULL";
        else
            outputStream << *LocationID;
    }
}
