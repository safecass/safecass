//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: Aug 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "dict.h"
#include "utils.h"
#include "filterBase.h"
#include "eventPublisherBase.h"

#include <iomanip>

using namespace SF;

FilterBase::FilterIDType FilterBase::FilterUID = 0;

const FilterBase::FilterIDType FilterBase::INVALID_FILTER_UID = 0;

const std::string InvalidSignalName = "INVALID_SIGNAL";

//-------------------------------------------------- 
//  Constructors and Destructor
//-------------------------------------------------- 
FilterBase::FilterBase(void)
    : UID(INVALID_FILTER_UID),
      Name("NONAME"),
      FilterTarget(CreateFilterTarget(NONAME, NONAME, NONAME)),
      FilterType(ACTIVE),
      LastFilterOfPipeline(false)
{
    Initialize();
}

FilterBase::FilterBase(const std::string     & filterName,
                       FilteringType           filteringType,
                       State::StateMachineType targetStateMachineType,
                       const std::string     & targetComponentName,
                       const std::string     & targetInterfaceName)
    : UID(++FilterUID),
      Name(filterName),
      FilterTarget(CreateFilterTarget(targetStateMachineType, targetComponentName, targetInterfaceName)),
      Initialized(false),
      FilterType(filteringType),
      LastFilterOfPipeline(false)
{
    Initialize();
}

FilterBase::FilterBase(const std::string & filterName, const JSON::JSONVALUE & jsonNode)
    : UID(++FilterUID),
      Name(filterName),
      FilterTarget(CreateFilterTarget(JSON::GetSafeValueString(jsonNode["target"], "type"),
                                      JSON::GetSafeValueString(jsonNode["target"], "component"),
                                      JSON::GetSafeValueString(jsonNode["target"], "interface"))),
      Initialized(false),
      FilterType(GetFilteringTypeFromString(JSON::GetSafeValueString(jsonNode, Dict::Json::type))),
      LastFilterOfPipeline(JSON::GetSafeValueBool(jsonNode, Dict::Filter::last_filter))
{
    const std::string targetStateMachineType = JSON::GetSafeValueString(jsonNode["target"], "type");
    const std::string targetComponentName    = JSON::GetSafeValueString(jsonNode["target"], "component");
    const std::string targetInterfaceName    = JSON::GetSafeValueString(jsonNode["target"], "interface");

    Initialize();
}

FilterBase::FilterTargetType FilterBase::CreateFilterTarget(State::StateMachineType targetStateMachineType,
                                                            const std::string     & targetComponentName,
                                                            const std::string     & targetInterfaceName)
{
    FilterTargetType t;

    t.StateMachineType = targetStateMachineType;
    t.ComponentName    = targetComponentName;
    if (t.StateMachineType == State::STATEMACHINE_PROVIDED || t.StateMachineType == State::STATEMACHINE_REQUIRED)
        t.InterfaceName = targetInterfaceName;
    else
        t.InterfaceName = State::STATEMACHINE_INVALID;

    return t;
}

FilterBase::FilterTargetType FilterBase::CreateFilterTarget(const std::string & targetStateMachineTypeName,
                                                            const std::string & targetComponentName,
                                                            const std::string & targetInterfaceName)
{
    FilterTargetType t;

    State::StateMachineType targetStateMachineType;
    if (targetStateMachineTypeName.compare("s_F") == 0)
        targetStateMachineType = State::STATEMACHINE_FRAMEWORK;
    else if (targetStateMachineTypeName.compare("s_A") == 0)
        targetStateMachineType = State::STATEMACHINE_APP;
    else if (targetStateMachineTypeName.compare("s_P") == 0)
        targetStateMachineType = State::STATEMACHINE_PROVIDED;
    else if (targetStateMachineTypeName.compare("s_R") == 0)
        targetStateMachineType = State::STATEMACHINE_REQUIRED;
    else
        targetStateMachineType = State::STATEMACHINE_INVALID;

    t.StateMachineType = targetStateMachineType;
    t.ComponentName    = targetComponentName;
    t.InterfaceName    = targetInterfaceName;

    return t;
}

void FilterBase::Initialize(void)
{
    PrintDebugLog = false;
    EventPublisher = 0;
    EventLocation  = 0;

    // To prevent filters from reading and using non-initialized values, filters are 
    // initially disabled.
    FilterState        = FilterBase::DISABLED;
    //EventDetectionMode = FilterBase::EVENT_DETECTION_EDGE;
    EventDetectionMode = FilterBase::EVENT_DETECTION_LEVEL;
    EventDetected      = 0;

    // For event generation and broadcasting
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

bool FilterBase::AddInputSignal(const std::string &       signalName, 
                                SignalElement::SignalType signalType)
{
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        if (InputSignals[i]->GetName().compare(signalName) == 0) {
            SFLOG_ERROR << "AddInputSignal: failed to add input signal (duplicate name): \"" << signalName << "\"" << std::endl;
            return false;
        }
    }

    SignalElement * newSignal = new SignalElement(signalName, signalType);
    InputSignals.push_back(newSignal);

    SFLOG_DEBUG << "AddInputSignal: Successfully added input signal \"" << signalName << "\" to filter \"" << this->Name << "\"" << std::endl;

    return true;
}

bool FilterBase::AddOutputSignal(const std::string &      signalName, 
                                SignalElement::SignalType signalType)
{
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        if (OutputSignals[i]->GetName().compare(signalName) == 0) {
            SFLOG_ERROR << "AddOutputSignal: failed to add output signal (duplicate name): \"" << signalName << "\"" << std::endl;
            return false;
        }
    }

    SignalElement * newSignal = new SignalElement(signalName, signalType);
    OutputSignals.push_back(newSignal);

    SFLOG_DEBUG << "AddOutputSignal: Successfully added output signal \"" << signalName << "\" to filter \"" << this->Name << "\"" << std::endl;

    return true;
}

bool FilterBase::RefreshSamples(void)
{
    if (!Initialized) {
        SFLOG_WARNING << "FilterBase: Filter is not properly initialized: " << *this << std::endl;
        return false;
    }
    if (!IsEnabled())
        return false;

    // If the input queue for fault injection is not emtpy, dequeue one element from the
    // queue and use it as the next input.
    // TODO: currently, this deals with only ONE input signal.  This should be extended to
    // multiple input signals to support multiple inputs.

    // Scalar type signal
    if (InputSignals[0]->GetSignalType() == SignalElement::SCALAR) {
        if (InputQueueScalar.size() > 0) {
            SignalElement::ScalarType injectedInputScalar = InputQueueScalar.front();
            InputQueueScalar.pop();
            InputSignals[0]->SetPlaceholderScalar(injectedInputScalar);
        } else {
            // Fetch new value from history buffer
            if (!InputSignals[0]->FetchNewValueScalar((FilterType == FilterBase::ACTIVE))) {
                SFLOG_ERROR << "failed to read input from history buffer: filter => " << *this << std::endl;
                this->Enable(false); // suppress further error messages due to the same issue
                // TODO: RESOLVE THIS ISSUE: once Enable(false) is called, a filter is no longer
                // is usable.  There should be another way(s) to enable this filter again.
                return false;
            }
        }
    }
    // Vector type signal
    else {
        if (InputQueueVector.size() > 0) {
            SignalElement::VectorType vec = InputQueueVector.front();
            InputQueueVector.pop();
            InputSignals[0]->SetPlaceholderVector(vec);
        } else {
            // Fetch new value from history buffer
            if (!InputSignals[0]->FetchNewValueVector((FilterType == FilterBase::ACTIVE))) {
                SFLOG_ERROR << "failed to read input from history buffer: filter => " << *this << std::endl;
                this->Enable(false); // suppress further error messages due to the same issue
                // TODO: RESOLVE THIS ISSUE: once Enable(false) is called, a filter is no longer
                // is usable.  There should be another way(s) to enable this filter again.
                return false;
            }
        }
    }

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

bool FilterBase::IsDisabled(void) const {
    return !IsEnabled();
}

bool FilterBase::IsEnabled(void) const {
    return (FilterState != FilterBase::DISABLED);
}

#if 0 // obsolete
bool FilterBase::HasPendingEvent(void) const {
    // integrity check
    if (FilterState == FilterBase::DETECTED) {
        SFASSERT(EventDetected);
    }
    return (FilterState == FilterBase::DETECTED);
}
#endif

void FilterBase::Enable(bool enable)
{
    if (enable) {
        // When enabling a filter, it can be either in the ENABLED or DETECTED state.
        if (IsEnabled())
            return;
        FilterState = FilterBase::ENABLED;
    } else {
#if 0
        // If a filter detected an event which has not been resolved yet, SF should inform
        // the user of the pending event.
        if (HasPendingEvent()) {
            // TODO: print out detailed information about pending event
            SFLOG_WARNING << "Warning: filter [ " << *this << " ] has pending event" << std::endl;
        }
#endif
        FilterState = FilterBase::DISABLED;
    }
}

std::string FilterBase::GetInputSignalName(size_t index) const
{
    if (index >= InputSignals.size()) {
        SFLOG_ERROR << "GetInputSignalName: index " << index << " out of range (total input signal count: " << InputSignals.size() << ")" << std::endl;
        return InvalidSignalName;
    }

    return InputSignals[index]->GetName();
}

std::string FilterBase::GetOutputSignalName(size_t index) const
{
    if (index >= OutputSignals.size()) {
        SFLOG_ERROR << "GetOutputSignalName: index " << index << " out of range (total output signal count: " << OutputSignals.size() << ")" << std::endl;
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
    if (index >= OutputSignals.size()) return 0;

    return OutputSignals[index];
}

SignalElement * FilterBase::GetInputSignalElement(size_t index) const
{
    if (index >= InputSignals.size()) return 0;

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
    if (type == ACTIVE)  return Dict::ACTIVE;
    if (type == PASSIVE) return Dict::PASSIVE;

    return Dict::INVALID;
}

FilterBase::FilteringType FilterBase::GetFilteringTypeFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(Dict::ACTIVE) == 0)  return ACTIVE;
    if (_str.compare(Dict::PASSIVE) == 0) return PASSIVE;

    return ACTIVE;
}

const std::string FilterBase::GetFilterStateString(const FilterStateType state)
{
    switch (state) {
    case DISABLED: return Dict::DISABLED;
    case ENABLED:  return Dict::ENABLED;
    case DETECTED: return Dict::DETECTED;
    }

    return Dict::INVALID;
}

FilterBase::FilterStateType FilterBase::GetFilterStateFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(Dict::DISABLED) == 0)  return DISABLED;
    if (_str.compare(Dict::ENABLED) == 0)   return ENABLED;
    if (_str.compare(Dict::DETECTED) == 0)  return DETECTED;

    return DISABLED;
}

#if 0 // obsolete
bool FilterBase::SetEventDetected(Event * event)
{
    if (!event)
        return false;

    EventDetected = event;

    return true;
}

bool FilterBase::SetEventDetected(const std::string & json)
{
    if (!HasPendingEvent()) {
        SFLOG_ERROR << "SetEventDetected: this filter already found event: \n" << json << std::endl;
        return false;
    }

    // TODO
    // 1. parse JSON
    // 2. create event instance
    // 3. call the other SetEventDetected method

    return false; // FIXME
}
#endif

void FilterBase::InjectInputScalar(SignalElement::ScalarType input, bool deepInjection)
{
    // TODO: this works only for single input: should be extended to multiple inputs later
    // Also see comments in FilterBase::RefreshSamples(void)

    if (deepInjection)
        InputSignals[0]->PushNewValueScalar(input);
    else
        InputQueueScalar.push(input);
}

void FilterBase::InjectInputScalar(const std::vector<SignalElement::ScalarType> & inputs, bool deepInjection)
{
    if (deepInjection)
        for (size_t i = 0; i < inputs.size(); ++i)
            InputSignals[0]->PushNewValueScalar(inputs[i]);
    else
        for (size_t i = 0; i < inputs.size(); ++i)
            InputQueueScalar.push(inputs[i]);
}

void FilterBase::InjectInputScalar(const std::list<SignalElement::ScalarType> & inputs, bool deepInjection)
{
    std::list<SignalElement::ScalarType>::const_iterator it = inputs.begin();

    if (deepInjection)
        for (; it != inputs.end(); ++it)
            InputSignals[0]->PushNewValueScalar(*it);
    else
        for (; it != inputs.end(); ++it)
            InputQueueScalar.push(*it);
}

void FilterBase::InjectInputVector(const SignalElement::VectorType & input, bool deepInjection)
{
    if (deepInjection)
        InputSignals[0]->PushNewValueVector(input);
    else
        InputQueueVector.push(input);
}

void FilterBase::InjectInputVector(const std::vector<SignalElement::VectorType> & inputs, bool deepInjection)
{
    std::vector<SignalElement::VectorType>::const_iterator it = inputs.begin();

    if (deepInjection)
        for (; it != inputs.end(); ++it)
            InputSignals[0]->PushNewValueVector(*it);
    else
        for (; it != inputs.end(); ++it)
            InputQueueVector.push(*it);
}

const std::string FilterBase::ShowInputQueueScalar(void) const
{
    // TODO => add deepInjection option

    if (InputQueueScalar.size() == 0)
        return "null";

    std::stringstream ss;
    ss << std::setprecision(5);
    InputQueueScalarType copy(InputQueueScalar);
    while (!copy.empty()) {
        ss << copy.front() << " ";
        copy.pop();
    }

    return ss.str();
}

const std::string FilterBase::ShowInputQueueVector(void) const
{
    if (InputQueueVector.size() == 0)
        return "null";

    std::stringstream ss;
    ss << std::setprecision(5);
    InputQueueVectorType copy(InputQueueVector);
    while (!copy.empty()) {
        for (size_t i = 0; i < copy.front().size(); ++i)
            ss << copy.front()[i] << " ";
        copy.pop();
    }

    return ss.str();
}

const std::string FilterBase::GenerateEventInfo(void) const
{
    // MJTEMP
    return "FilterBase::GenerateEventInfo:TODO";
}

void FilterBase::ToStream(std::ostream & out, bool verbose) const
{
    if (verbose) {
        out << "[" << UID << "] "
            << "Name: \"" << Name << "\", ";
        out << "Target: \"";
        switch (FilterTarget.StateMachineType) {
        case State::STATEMACHINE_FRAMEWORK: out << "s_F"; break;
        case State::STATEMACHINE_APP:       out << "s_A"; break;
        case State::STATEMACHINE_PROVIDED:  out << "s_P"; break;
        case State::STATEMACHINE_REQUIRED:  out << "s_R"; break;
        case State::STATEMACHINE_INVALID:   out << "INVALID"; break;
        }
        out << "\", component: \"" << FilterTarget.ComponentName
            << "\", interface: \"" << FilterTarget.InterfaceName
            << "\", ";
        out << "Filter type: " << (FilterType == ACTIVE ? "ACTIVE" : "PASSIVE") << ", "
            << "State: " << GetFilterStateString(FilterState);
        out << ", Event location: " << (EventLocation ? "Available" : "n/a");
        if (LastFilterOfPipeline)
            out << ", Event publisher: " << (EventPublisher ? "installed" : "n/a");
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
        if (InputQueueScalar.size())
            out << ShowInputQueueScalar() << std::endl;
        if (InputQueueVector.size())
            out << ShowInputQueueVector() << std::endl;
    } else {
        out << "[ " << UID << " ] ";
        switch (FilterTarget.StateMachineType) {
        case State::STATEMACHINE_FRAMEWORK: out << "s_F "; break;
        case State::STATEMACHINE_APP:       out << "s_A "; break;
        case State::STATEMACHINE_PROVIDED:  out << "s_P "; break;
        case State::STATEMACHINE_REQUIRED:  out << "s_R "; break;
        case State::STATEMACHINE_INVALID:   out << "N/A "; break;
        }
        out << "\"" << FilterTarget.ComponentName << "\"";
        if (FilterTarget.InterfaceName.size())
            out << ":\"" << FilterTarget.InterfaceName << "\"";
        out << "  " << Name << "  ";
        // Input signals
        for (size_t i = 0; i < InputSignals.size(); ++i)
            out << "\"" << InputSignals[i]->GetName() << "\"  ";
        // Input queue
        if (InputQueueScalar.size())
            out << "scalar [ " << ShowInputQueueScalar() << " ]";
        if (InputQueueVector.size())
            out << "vector [ " << ShowInputQueueVector() << " ]";
    }
}
