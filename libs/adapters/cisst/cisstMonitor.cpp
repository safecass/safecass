/*

  Safety Framework for Component-based Robotics

  Created on: July 8, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstMonitor.h"

#include "dict.h"
#include "cisstDic.h"
#include "json.h"

#include <sstream>

namespace SF {

using namespace Dict;
using namespace Dict::Json;

cisstMonitor::cisstMonitor(const Monitor::TargetType target,
                           cisstEventLocation *      locationID,
                           const Monitor::StateType  state,
                           const Monitor::OutputType output,
                           const SamplingRateType    samplingRate)
    : Monitor()
{
    this->Target = target;
    this->LocationID = locationID;
    this->State = state;
    this->Output = output;
    this->SamplingRate = samplingRate;
}

cisstMonitor::~cisstMonitor()
{
}

const std::string cisstMonitor::GetMonitorJSON(void) const
{
    ::Json::Value root;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(LocationID);

    root[NAME] = this->GetUIDAsString();

    // Monitor target type
    {   ::Json::Value _root;
        _root[TYPE] = Monitor::GetTargetTypeString(Target);

        { ::Json::Value __root;
          __root[NAME_PROCESS]                = locationID->GetProcessName();
          __root[NAME_COMPONENT]              = locationID->GetComponentName();
          __root[NAME_INTERFACE_PROVIDED]     = locationID->GetInterfaceProvidedName();
          __root[NAME_INTERFACE_REQUIRED]     = locationID->GetInterfaceRequiredName();
          __root[cisst::NAME_COMMAND]         = locationID->GetCommandName();
          __root[cisst::NAME_FUNCTION]        = locationID->GetFunctionName();
          __root[cisst::NAME_EVENT_GENERATOR] = locationID->GetEventGeneratorName();
          __root[cisst::NAME_EVENT_HANDLER]   = locationID->GetEventHandlerName();
          _root[IDENTIFIER] = __root;
        }
        root[TARGET] = _root;
    }

    // Monitor behaviors
    {   ::Json::Value _root;
        _root[TYPE] = Monitor::GetOutputTypeString(Output);
        {   ::Json::Value __root;
            {
                __root[STATE] = Monitor::GetStateTypeString(this->State);
                if (Output == OUTPUT_STREAM) {
                    __root[SAMPLING_RATE] = SamplingRate;
                } else if (Output == OUTPUT_EVENT) {
#if 0 // MJ TEMP: not yet decided how to use these fields
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
            _root[CONFIG] = __root;
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
}

const std::string cisstMonitor::GetJsonForPublishingPeriod(double sample) const
{
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
} 

const std::string cisstMonitor::GetJsonForPublishingDutyCycleUser(double dutyCycle) const
{
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
}

const std::string cisstMonitor::GetJsonForPublishingDutyCycleTotal(double dutyCycle) const
{
    ::Json::Value root;
    cisstEventLocation * locationID = dynamic_cast<cisstEventLocation*>(LocationID)
        ;

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
}

void cisstMonitor::ToStream(std::ostream & outputStream) const
{
    Monitor::ToStream(outputStream);
}

};
