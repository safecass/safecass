/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "monitor.h"
#include "dict.h"
#include "json.h"

namespace SF {

// unique id within process
Monitor::UIDType UIDCounter = 0;

using namespace Dict;

Monitor::Monitor() 
    : UID(++UIDCounter), // UID=0 means invalid monitor target
      Target(TARGET_INVALID), LocationID(0), State(STATE_INVALID),
      Output(OUTPUT_INVALID), SamplingRate(0), LastSamplingTick(0),
      AttachedToActiveFilter(false)
{
    Initialize();
}

Monitor::Monitor(const TargetType target, 
                 EventLocationBase * locationId,
                 const StateType state,
                 const OutputType output,
                 const SamplingRateType samplingRate,
                 double lastSamplingTick)
    : Target(target), LocationID(locationId), State(state),
      Output(output), SamplingRate(samplingRate), LastSamplingTick(lastSamplingTick),
      AttachedToActiveFilter(false)
{
    Initialize();
}

Monitor::~Monitor()
{
    if (LocationID) {
        delete LocationID;
    }
}

#if 0
#define COPY(_field) this->_field = rhs._field;
Monitor & Monitor::operator=(const Monitor & rhs)
{
    COPY(Target);
    COPY(LocationID);
    COPY(State);
    COPY(Output);
    COPY(SamplingRate);
}
#undef COPY
#endif

void Monitor::Initialize(void)
{
    Samples.Period = 0.0;
    Samples.ExecTimeUser = 0.0;
    Samples.ExecTimeTotal = 0.0;
}

const std::string Monitor::GetUIDAsString(void) const
{
    if (!LocationID) {
        return "N/A";
    }

    std::stringstream ss;
    ss << LocationID->GetIDString();
    ss << ":" << GetTargetTypeString(Target);

    return ss.str();
}

Monitor::UIDType Monitor::GetUIDAsNumber(void) const
{
    if (!LocationID) {
        return INVALID_UID;
    }

    return UID;
}

bool Monitor::IsSamplingNecessary(double currentTick) const
{
    if (LastSamplingTick + GetSamplingPeriod() < currentTick)
        return true;

    return false;
}

//-----------------------------------------------
// Misc. Getters
//-----------------------------------------------
static const std::string STR_TARGET_CUSTOM                 = "CUSTOM";
static const std::string STR_TARGET_THREAD_PERIOD          = "THREAD_PERIOD";
static const std::string STR_TARGET_THREAD_DUTYCYCLE_USER  = "THREAD_DUTYCYCLE_USER";
static const std::string STR_TARGET_THREAD_DUTYCYCLE_TOTAL = "THREAD_DUTYCYCLE_TOTAL";
static const std::string STR_TARGET_FILTER_EVENT           = "FILTER_EVENT";

const std::string Monitor::GetTargetTypeString(const TargetType type)
{
    if (type == TARGET_CUSTOM)                 return STR_TARGET_CUSTOM;
    if (type == TARGET_THREAD_PERIOD)          return STR_TARGET_THREAD_PERIOD;
    if (type == TARGET_THREAD_DUTYCYCLE_USER)  return STR_TARGET_THREAD_DUTYCYCLE_USER;
    if (type == TARGET_THREAD_DUTYCYCLE_TOTAL) return STR_TARGET_THREAD_DUTYCYCLE_TOTAL;
    if (type == TARGET_FILTER_EVENT)           return STR_TARGET_FILTER_EVENT;

    return INVALID;
}

Monitor::TargetType Monitor::GetTargetTypeFromString(const std::string & str)
{
    if (str.compare(STR_TARGET_CUSTOM) == 0)                 return TARGET_CUSTOM;
    if (str.compare(STR_TARGET_THREAD_PERIOD) == 0)          return TARGET_THREAD_PERIOD;
    if (str.compare(STR_TARGET_THREAD_DUTYCYCLE_USER) == 0)  return TARGET_THREAD_DUTYCYCLE_USER;
    if (str.compare(STR_TARGET_THREAD_DUTYCYCLE_TOTAL) == 0) return TARGET_THREAD_DUTYCYCLE_TOTAL;
    if (str.compare(STR_TARGET_FILTER_EVENT) == 0)           return TARGET_FILTER_EVENT;

    return TARGET_INVALID;
}

const std::string Monitor::GetStateTypeString(const StateType type)
{
    if (type == STATE_OFF) return OFF;
    if (type == STATE_ON) return ON;

    return INVALID;
}

Monitor::StateType Monitor::GetStateTypeFromString(const std::string & str)
{
    if (str.compare(Dict::OFF) == 0) return STATE_OFF;
    if (str.compare(Dict::ON) == 0) return STATE_ON;

    return STATE_INVALID;
}

const std::string Monitor::GetOutputTypeString(OutputType type)
{
    if (type == OUTPUT_STREAM) return Dict::STREAM;
    if (type == OUTPUT_EVENT) return Dict::EVENT;
    
    return INVALID;
}

Monitor::OutputType Monitor::GetOutputTypeFromString(const std::string & str)
{
    if (str.compare(Dict::STREAM) == 0) return OUTPUT_STREAM;
    if (str.compare(Dict::EVENT) == 0) return OUTPUT_EVENT;

    return OUTPUT_INVALID;
}

void Monitor::ToStream(std::ostream & outputStream) const
{
    outputStream << "Target type: " << GetTargetTypeString(Target) << ", "
                 << "LocationID: " << *LocationID << ", "
                 << "State: " << GetStateTypeString(State) << ", "
                 << "OutputType: " << GetOutputTypeString(Output) << ", "
                 << "SamplingRate: " << SamplingRate << ", "
                 << "Last sampling tick: " << LastSamplingTick << ", "
                 << "Active filter: " << AttachedToActiveFilter;
}

};
