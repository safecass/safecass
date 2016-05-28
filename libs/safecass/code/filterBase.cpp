//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: May 1, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "safecass/filterBase.h"

#include "common/dict.h"
#include "common/utils.h"
#include "common/jsonwrapper.h"
#include "safecass/eventPublisherBase.h"

#include <iomanip>

using namespace SC;

FilterBase::FilterIDType FilterBase::FilterUID = 0;

const FilterBase::FilterIDType FilterBase::INVALID_FILTER_UID = 0;

const std::string InvalidSignalName = "INVALID_SIGNAL";

//--------------------------------------------------
//  FilterBase::StateMachineInfo
//--------------------------------------------------
FilterBase::StateMachineInfo::StateMachineInfo(void)
    : StateMachineType(State::STATEMACHINE_INVALID),
      ComponentName(""), InterfaceName("")
{}

FilterBase::StateMachineInfo::StateMachineInfo(State::StateMachineType type,
                                               const std::string & componentName,
                                               const std::string & interfaceName)
    : StateMachineType(type), ComponentName(componentName)
{
    // Only interface-type statemachines (STATEMACHINE_PROVIDED and STATEMACHINE_REQUIRED)
    // is allowed to have non-empty interface name.
    if (StateMachineType == State::STATEMACHINE_PROVIDED || StateMachineType == State::STATEMACHINE_REQUIRED)
        InterfaceName = interfaceName;
    else
        InterfaceName = "";
}

FilterBase::StateMachineInfo::StateMachineInfo(const std::string & stateMachineTypeName,
                                               const std::string & componentName,
                                               const std::string & interfaceName)
{
    if (stateMachineTypeName.compare("s_F") == 0)
        StateMachineType = State::STATEMACHINE_FRAMEWORK;
    else if (stateMachineTypeName.compare("s_A") == 0)
        StateMachineType = State::STATEMACHINE_APP;
    else if (stateMachineTypeName.compare("s_P") == 0)
        StateMachineType = State::STATEMACHINE_PROVIDED;
    else if (stateMachineTypeName.compare("s_R") == 0)
        StateMachineType = State::STATEMACHINE_REQUIRED;
    else
        StateMachineType = State::STATEMACHINE_INVALID;

    ComponentName = componentName;

    // Only interface-type statemachines (STATEMACHINE_PROVIDED and STATEMACHINE_REQUIRED)
    // is allowed to have non-empty interface name.
    if (StateMachineType == State::STATEMACHINE_PROVIDED || StateMachineType == State::STATEMACHINE_REQUIRED)
        InterfaceName = interfaceName;
    else
        InterfaceName = "";
}

bool FilterBase::StateMachineInfo::operator==(const StateMachineInfo & rhs) const
{
    return ((this->StateMachineType == rhs.StateMachineType) &&
            (this->ComponentName.compare(rhs.ComponentName) == 0) &&
            (this->InterfaceName.compare(rhs.InterfaceName) == 0));
}

bool FilterBase::StateMachineInfo::operator!=(const StateMachineInfo & rhs) const
{
    return !(*this == rhs);
}

//--------------------------------------------------
//  Constructors and Destructor
//--------------------------------------------------
FilterBase::FilterBase(void)
    : FilterID(INVALID_FILTER_UID),
      Name(NONAME),
      StateMachineRegistered(StateMachineInfo()),
      FilterType(FILTERING_INTERNAL),
      //LastFilterOfPipeline(false),
      EventDetectionMode(EVENT_DETECTION_EDGE)
{
    Initialize();
}

FilterBase::FilterBase(const std::string     & filterName,
                       FilteringType           filteringType,
                       State::StateMachineType targetStateMachineType,
                       const std::string     & targetComponentName,
                       const std::string     & targetInterfaceName,
                       EventDetectionModeType  eventDetectionMode)
    : FilterID(++FilterUID),
      Name(filterName),
      StateMachineRegistered(StateMachineInfo(targetStateMachineType, targetComponentName, targetInterfaceName)),
      FilterType(filteringType),
      //LastFilterOfPipeline(false),
      EventDetectionMode(eventDetectionMode)
{
    Initialize();
}

FilterBase::FilterBase(const std::string     & filterName,
                       FilteringType           filteringType,
                       const StateMachineInfo & stateMachineInfo,
                       EventDetectionModeType  eventDetectionMode)
    : FilterID(++FilterUID),
      Name(filterName),
      StateMachineRegistered(stateMachineInfo),
      FilterType(filteringType),
      //LastFilterOfPipeline(false),
      EventDetectionMode(eventDetectionMode)
{
    Initialize();
}

FilterBase::FilterBase(const std::string & filterName, const Json::Value & json)
    : FilterID(++FilterUID),
      Name(filterName),
      StateMachineRegistered(StateMachineInfo(json["target"]["type"].asString(),
                                              json["target"]["component"].asString(),
                                              json["target"]["interface"].asString())),
      FilterType(GetFilteringTypeFromString(json["type"].asString())),
      EventDetectionMode(GetEventDetectionTypeFromString(json["event_type"].asString()))
{
    Initialize();
}

void FilterBase::Initialize(void)
{
    EventPublisher = 0;
    EventLocation  = 0;

    // Disable filter at start up in order not to read and use non-initialized values
    FilterState = FilterBase::STATE_INIT;

    // Initialize safety coordinator instance (used for event generation by derived filter classes)
    SafetyCoordinator = 0;
}

FilterBase::~FilterBase()
{
    for (size_t i = 0; i < InputSignals.size(); ++i)
        delete InputSignals[i];
    for (size_t i = 0; i < OutputSignals.size(); ++i)
        delete OutputSignals[i];

    if (EventPublisher) delete EventPublisher;
    if (EventLocation) delete EventLocation;
}

bool FilterBase::AddInputSignal(ParamBase & signalObject, const std::string & signalName)
{
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        SCASSERT(InputSignals[i]);
        if (InputSignals[i]->GetName().compare(signalName) == 0) {
            SCLOG_ERROR << "Failed to add input signal (duplicate name): \"" << signalName << "\"" << std::endl;
            return false;
        }
    }

    // FIXME where to set history buffer instance???
    InputSignals.push_back(new SignalElement(signalName, signalObject));

    SCLOG_DEBUG << "Added input signal: \"" << signalName << "\" to filter ID " << this->FilterUID
                << " (\"" << this->Name << "\")" << std::endl;

    return true;
}

bool FilterBase::AddOutputSignal(ParamBase & signalObject, const std::string & signalName)
{
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        SCASSERT(OutputSignals[i]);
        if (OutputSignals[i]->GetName().compare(signalName) == 0) {
            SCLOG_ERROR << "Failed to add output signal (duplicate name): \"" << signalName << "\"" << std::endl;
            return false;
        }
    }

    // FIXME where to set history buffer instance???
    OutputSignals.push_back(new SignalElement(signalName, signalObject));

    SCLOG_DEBUG << "Added input signal: \"" << signalName << "\" to filter ID " << this->FilterUID
                << " (\"" << this->Name << "\")" << std::endl;

    return true;
}

bool FilterBase::RefreshSamples(void)
{
    if (FilterState == STATE_INIT) {
        SCLOG_WARNING << "FilterBase: Filter is not properly initialized: " << *this << std::endl;
        return false;
    }
    if (!IsEnabled())
        return false;

    // If the input queue for fault injection is not emtpy, dequeue one element from the
    // queue and use it as the next input.
    // TODO: currently, this deals with only ONE input signal.  This should be extended to
    // multiple input signals to support multiple inputs.

    /*
    // Scalar type signal
    if (InputSignals[0]->GetSignalType() == SignalElement::SCALAR) {
        if (InjectionQueue.size() > 0) {
            SignalElement::ScalarType injectedInputScalar = InjectionQueue.front();
            InjectionQueue.pop();
            InputSignals[0]->SetPlaceholderScalar(injectedInputScalar);
        } else {
            // Fetch new value from history buffer
            if (!InputSignals[0]->FetchNewValueScalar((FilterType == FilterBase::FILTERING_INTERNAL))) {
                SCLOG_ERROR << "failed to read input from history buffer: filter => " << *this << std::endl;
                this->Enable(false); // suppress further error messages due to the same issue
                // TODO: RESOLVE THIS ISSUE: once Enable(false) is called, a filter is no longer
                // is usable.  There should be another way(s) to enable this filter again.
                return false;
            }
        }
    }
    // Vector type signal
    else {
        // FIXME
#if 0
        if (InputQueueVector.size() > 0) {
            SignalElement::VectorType vec = InputQueueVector.front();
            InputQueueVector.pop();
            InputSignals[0]->SetPlaceholderVector(vec);
        } else {
            // Fetch new value from history buffer
            if (!InputSignals[0]->FetchNewValueVector((FilterType == FilterBase::FILTERING_INTERNAL))) {
                SCLOG_ERROR << "failed to read input from history buffer: filter => " << *this << std::endl;
                this->Enable(false); // suppress further error messages due to the same issue
                // TODO: RESOLVE THIS ISSUE: once Enable(false) is called, a filter is no longer
                // is usable.  There should be another way(s) to enable this filter again.
                return false;
            }
        }
#endif
    }
*/

    return true;
}

std::string FilterBase::GenerateOutputSignalName(const std::string & prefix,
                                                 const std::string & root1,
                                                 const FilterIDType  root2,
                                                 size_t              suffix) const
{
    std::stringstream ss;
    ss << prefix << ":" << root1 << ":" << root2 << ":" << suffix;

    return ss.str();
}

void FilterBase::Enable(bool enable)
{
    // FIXME add state transition handler

    if (enable) {
        // When enabling a filter, it can be either in the ENABLED or DETECTED state.
        if (IsEnabled())
            return;
        FilterState = FilterBase::STATE_ENABLED;
    } else {
#if 0
        // If a filter detected an event which has not been resolved yet, SC should inform
        // the user of the pending event.
        if (HasPendingEvent()) {
            // TODO: print out detailed information about pending event
            SCLOG_WARNING << "Warning: filter [ " << *this << " ] has pending event" << std::endl;
        }
#endif
        FilterState = FilterBase::STATE_DISABLED;
    }
}

std::string FilterBase::GetInputSignalName(size_t index) const
{
    if (index >= InputSignals.size()) {
        SCLOG_ERROR << "GetInputSignalName: index " << index << " out of range (total input signal count: " << InputSignals.size() << ")" << std::endl;
        return InvalidSignalName;
    }

    return InputSignals[index]->GetName();
}

std::string FilterBase::GetOutputSignalName(size_t index) const
{
    if (index >= OutputSignals.size()) {
        SCLOG_ERROR << "GetOutputSignalName: index " << index << " out of range (total output signal count: " << OutputSignals.size() << ")" << std::endl;
        return InvalidSignalName;
    }

    return OutputSignals[index]->GetName();
}

#if 0 // not used now
FilterBase::SignalNamesType FilterBase::GetInputSignalNames(void) const
{
    SignalNamesType names;
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        names.push_back(InputSignals[i]->GetName());
    }
    return names;
}

FilterBase::SignalNamesType FilterBase::GetOutputSignalNames(void) const
{
    SignalNamesType names;
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        names.push_back(OutputSignals[i]->GetName());
    }
    return names;
}
#endif

SignalElement * FilterBase::GetOutputSignalElement(size_t index) const
{
    if (OutputSignals.empty() || index >= OutputSignals.size())
        return 0;

    return OutputSignals[index];
}

SignalElement * FilterBase::GetInputSignalElement(size_t index) const
{
    if (InputSignals.empty() || index >= InputSignals.size())
        return 0;

    return InputSignals[index];
}


void FilterBase::SetEventPublisherInstance(EventPublisherBase * publisher)
{
    if (!publisher) return;
    if (EventPublisher)
        delete EventPublisher;

    EventPublisher = publisher;
}

void FilterBase::SetEventLocationInstance(EventLocationBase * location)
{
    if (!location) return;
    if (EventLocation)
        delete EventLocation;

    EventLocation = location;
}

const std::string FilterBase::ToString(bool verbose) const
{
    std::stringstream ss;
    ToStream(ss, verbose);
    return ss.str();
}

//-----------------------------------------------
// Misc. Getters
//-----------------------------------------------
const std::string FilterBase::GetFilteringTypeString(const FilteringType type)
{
    if (type == FILTERING_INTERNAL) return Dict::FILTERING_INTERNAL;
    if (type == FILTERING_EXTERNAL) return Dict::FILTERING_EXTERNAL;

    return Dict::INVALID;
}

FilterBase::FilteringType FilterBase::GetFilteringTypeFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(Dict::FILTERING_INTERNAL) == 0) return FILTERING_INTERNAL;
    if (_str.compare(Dict::FILTERING_EXTERNAL) == 0) return FILTERING_EXTERNAL;

    return FILTERING_INTERNAL;
}

const std::string FilterBase::GetFilterStateString(const FilterStateType state)
{
    switch (state) {
    case STATE_INIT:     return Dict::STATE_INIT;
    case STATE_DISABLED: return Dict::STATE_DISABLED;
    case STATE_ENABLED:  return Dict::STATE_ENABLED;
    case STATE_DETECTED: return Dict::STATE_DETECTED;
    }

    return Dict::INVALID;
}

FilterBase::FilterStateType FilterBase::GetFilterStateFromString(const std::string & str)
{
    std::string s = to_lowercase(str);

    if (s.compare(Dict::STATE_INIT) == 0)     return STATE_INIT;
    if (s.compare(Dict::STATE_DISABLED) == 0) return STATE_DISABLED;
    if (s.compare(Dict::STATE_ENABLED) == 0)  return STATE_ENABLED;
    if (s.compare(Dict::STATE_DETECTED) == 0) return STATE_DETECTED;

    return STATE_INIT;
}

FilterBase::EventDetectionModeType FilterBase::GetEventDetectionTypeFromString(const std::string & str)
{
    std::string s = to_lowercase(str);

    if (s.compare(Dict::EVENT_DETECTION_LEVEL) == 0)
        return EVENT_DETECTION_LEVEL;

    return EVENT_DETECTION_EDGE;
}

void FilterBase::InjectInput(const std::string & inputSignalName, const ParamBase & arg, bool deepInjection)
{
    // FIXME
}

/*
void FilterBase::InjectInputScalar(SignalElement::ScalarType input, bool deepInjection)
{
    // TODO: this works only for single input: should be extended to multiple inputs later
    // Also see comments in FilterBase::RefreshSamples(void)

    if (deepInjection)
        InputSignals[0]->PushNewValueScalar(input);
    else
        InjectionQueue.push(input);
}

void FilterBase::InjectInputScalar(const std::vector<SignalElement::ScalarType> & inputs, bool deepInjection)
{
    if (deepInjection)
        for (size_t i = 0; i < inputs.size(); ++i)
            InputSignals[0]->PushNewValueScalar(inputs[i]);
    else
        for (size_t i = 0; i < inputs.size(); ++i)
            InjectionQueue.push(inputs[i]);
}
*/

const std::string FilterBase::PrintInjectionQueue(void) const
{
    // TODO => add deepInjection option

    if (InjectionQueue.size() == 0)
        return "null";

    std::stringstream ss;
    ss << std::setprecision(5);
    InjectionQueueType copy(InjectionQueue);
    while (!copy.empty()) {
        ss << copy.front() << " ";
        copy.pop();
    }

    return ss.str();
}

void FilterBase::GenerateEventInfo(Json::Value & json) const
{
    json["event"]["fuid"] = (unsigned int)FilterID;
    // TODO: Currently, event generation timestamp is set to the timing when
    // event json is generated.  Alternatively, we could use the timestamp of
    // the sample retrieved from the history buffer.  
    // In case of cisst, note that the state table maintains elapsed time, rather 
    // than absolute time.
    // FIXME
    json["event"]["timestamp"] = GetCurrentTimestampString();//GetCurrentTimestamp();
}

// TODO: improve second parameter to handle other options (e.g., json, raw, 
// verbose, terse, etc.)
void FilterBase::ToStream(std::ostream & out, bool verbose) const
{
    if (verbose) {
        out << "[" << FilterID << "] "
            << "Name: \"" << Name << "\", ";
        out << "Target: \"";
        switch (StateMachineRegistered.GetStateMachineType()) {
        case State::STATEMACHINE_FRAMEWORK: out << "s_F"; break;
        case State::STATEMACHINE_APP:       out << "s_A"; break;
        case State::STATEMACHINE_PROVIDED:  out << "s_P"; break;
        case State::STATEMACHINE_REQUIRED:  out << "s_R"; break;
        case State::STATEMACHINE_SERVICE:   out << "INVALID (service)"; break;
        case State::STATEMACHINE_INVALID:   out << "INVALID"; break;
        }
        out << "\", component: \"" << StateMachineRegistered.GetComponentName()
            << "\", interface: \"" << StateMachineRegistered.GetInterfaceName()
            << "\", ";
        out << "Filter type: " << (FilterType == FILTERING_INTERNAL ? "FILTERING_INTERNAL" : "FILTERING_EXTERNAL") << ", "
            << "State: " << GetFilterStateString(FilterState);
        out << ", Event location: " << (EventLocation ? "Available" : "n/a");
        // if (LastFilterOfPipeline)
            // out << ", Event publisher: " << (EventPublisher ? "installed" : "n/a");
        out << std::endl;

        // Input signals
        out << "----- Input Signals:" << std::endl;
        for (size_t i = 0; i < InputSignals.size(); ++i) {
            out << "[" << i << "] " << (*InputSignals[i]) << std::endl;
        }
        // Output signals
        out << "----- Output Signals:" << std::endl;
        for (size_t i = 0; i < OutputSignals.size(); ++i) {
            out << "[" << i << "] " << (*OutputSignals[i]) << std::endl;
        }
        // Input queue
        out << "----- Input queue: ";
        if (InjectionQueue.size())
            out << PrintInjectionQueue() << std::endl;
        // FIXME
        // if (InputQueueVector.size())
            // out << ShowInputQueueVector() << std::endl;
    } else {
        out << "[ " << FilterID << " ] ";
        switch (StateMachineRegistered.GetStateMachineType()) {
        case State::STATEMACHINE_FRAMEWORK: out << "s_F "; break;
        case State::STATEMACHINE_APP:       out << "s_A "; break;
        case State::STATEMACHINE_PROVIDED:  out << "s_P "; break;
        case State::STATEMACHINE_REQUIRED:  out << "s_R "; break;
        case State::STATEMACHINE_SERVICE:   out << "N/A (service) "; break;
        case State::STATEMACHINE_INVALID:   out << "N/A "; break;
        }
        out << "\"" << StateMachineRegistered.GetComponentName() << "\"";
        if (!StateMachineRegistered.GetInterfaceName().empty())
            out << ":\"" << StateMachineRegistered.GetInterfaceName() << "\"";
        out << "  " << Name << "  ";
        // Input signals
        for (size_t i = 0; i < InputSignals.size(); ++i)
            out << "\"" << InputSignals[i]->GetName() << "\"  ";
        // Input queue
        if (InjectionQueue.size())
            out << "scalar [ " << PrintInjectionQueue() << " ]";
        // FIXME
        // if (InputQueueVector.size())
            // out << "vector [ " << ShowInputQueueVector() << " ]";
    }
}
