//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 7, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// URL          : https://github.com/minyang/safecass
//
#include "monitor.h"
#include "dict.h"
#include "jsonwrapper.h"
#include "utils.h"

namespace SC {

// unique id within process
Monitor::UIDType UIDCounter = 0;

using namespace Dict;

Monitor::Monitor() 
    : UID(++UIDCounter), // UID=0 means invalid monitor target
      Target(TARGET_INVALID), LocationID(0), State(STATE_INVALID),
      Output(OUTPUT_INVALID), SamplingRate(0), 
      LastSamplingTick(0)
{
    Initialize();
}

Monitor::Monitor(const TargetType target, 
                 EventLocationBase * locationId,
                 const StateType state,
                 const OutputType output,
                 const SamplingRateType samplingRate,
                 double lastSamplingTick)
    : UID(++UIDCounter),
      Target(target), LocationID(locationId), State(state),
      Output(output), SamplingRate(samplingRate), 
      LastSamplingTick(lastSamplingTick)
{
    Initialize();
}

Monitor::Monitor(const JsonWrapper::JsonValue & jsonNode)
    : UID(++UIDCounter),
      LastSamplingTick(0)
{
    Initialize();

    // Extract and set common (base) fields
    this->Target       = GetTargetTypeFromString(JsonWrapper::GetSafeValueString(jsonNode, Json::type));
    this->State        = GetStateTypeFromString(JsonWrapper::GetSafeValueString(jsonNode, Json::state));
    this->Output       = GetOutputTypeFromString(JsonWrapper::GetSafeValueString(jsonNode, Json::output_type));
    this->SamplingRate = JsonWrapper::GetSafeValueUInt(jsonNode, Json::sampling_rate);

    // Extract target information
    const JsonWrapper::JsonValue location(jsonNode[Json::location]);
    const std::string processName           = JsonWrapper::GetSafeValueString(location, Json::process);
    const std::string componentName         = JsonWrapper::GetSafeValueString(location, Json::component);
    const std::string interfaceProvidedName = JsonWrapper::GetSafeValueString(location, Json::interface_provided);
    const std::string interfaceRequiredName = JsonWrapper::GetSafeValueString(location, Json::interface_required);

    this->LocationID = new EventLocationBase(processName, componentName,
                                             interfaceProvidedName, interfaceRequiredName);
}

Monitor::~Monitor()
{
    if (LocationID) {
        delete LocationID;
    }
}

void Monitor::Initialize(void)
{
    Samples.PeriodActual  = 0.0;
    Samples.PeriodNominal = 0.0;
    Samples.ExecTimeUser  = 0.0;
    Samples.ExecTimeTotal = 0.0;

    AttachedToActiveFilter = false;
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

    return Dict::INVALID;
}

Monitor::TargetType Monitor::GetTargetTypeFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(STR_TARGET_CUSTOM) == 0)                 return TARGET_CUSTOM;
    if (_str.compare(STR_TARGET_THREAD_PERIOD) == 0)          return TARGET_THREAD_PERIOD;
    if (_str.compare(STR_TARGET_THREAD_DUTYCYCLE_USER) == 0)  return TARGET_THREAD_DUTYCYCLE_USER;
    if (_str.compare(STR_TARGET_THREAD_DUTYCYCLE_TOTAL) == 0) return TARGET_THREAD_DUTYCYCLE_TOTAL;
    if (_str.compare(STR_TARGET_FILTER_EVENT) == 0)           return TARGET_FILTER_EVENT;

    return TARGET_INVALID;
}

const std::string Monitor::GetStateTypeString(const StateType type)
{
    if (type == STATE_OFF) return Dict::OFF;
    if (type == STATE_ON) return Dict::ON;

    return Dict::INVALID;
}

Monitor::StateType Monitor::GetStateTypeFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(Dict::OFF) == 0) return STATE_OFF;
    if (_str.compare(Dict::ON) == 0) return STATE_ON;

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
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(Dict::STREAM) == 0) return OUTPUT_STREAM;
    if (_str.compare(Dict::EVENT) == 0) return OUTPUT_EVENT;

    return OUTPUT_INVALID;
}

void Monitor::ToStream(std::ostream & outputStream, bool includeLocation) const
{
    outputStream << "Target type: " << GetTargetTypeString(Target) << ", ";
    if (includeLocation) {
        outputStream << "LocationID: ";
        if (LocationID == 0)
            outputStream << "NULL";
        else
            outputStream << *LocationID;
        outputStream << ", ";
    }
    outputStream << "State: " << GetStateTypeString(State) << ", "
                 << "OutputType: " << GetOutputTypeString(Output) << ", "
                 << "SamplingRate: " << SamplingRate << ", "
                 << "Last sampling tick: " << LastSamplingTick << ", "
                 << "Active filter: " << AttachedToActiveFilter;
    if (!includeLocation)
        outputStream << ", ";
}

};
