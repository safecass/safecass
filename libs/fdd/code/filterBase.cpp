//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: May 6, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "dict.h"
#include "utils.h"
#include "filterBase.h"
#include "eventPublisherBase.h"

namespace SF {

using namespace Dict;

FilterBase::FilterIDType FilterBase::FilterUID = 0;

const FilterBase::FilterIDType InvalidFilterUID = -1;
const std::string InvalidSignalName = "INVALID_SIGNAL";

//-------------------------------------------------- 
//  Constructors and Destructor
//-------------------------------------------------- 
FilterBase::FilterBase(void)
    : UID(InvalidFilterUID),
      Name("NONAME"),
      Category(INVALID),
      NameOfTargetComponent("NONAME"), 
      Type(ACTIVE),
      LastFilterOfPipeline(false)
{
    Initialize();
}

FilterBase::FilterBase(const std::string  & filterName,
                       const FilterCategory filterCategory,
                       const std::string  & targetComponentName,
                       const FilteringType  monitoringType)
    : UID(++FilterUID),
      Name(filterName),
      ClassName(""),
      Category(filterCategory),
      NameOfTargetComponent(targetComponentName), 
      Type(monitoringType),
      LastFilterOfPipeline(false)
{
    Initialize();
}

FilterBase::FilterBase(const std::string & filterName, const JSON::JSONVALUE & jsonNode)
    : UID(++FilterUID),
      Name(filterName),
      ClassName( JSON::GetSafeValueString( jsonNode, Json::class_name)),
      Category( GetFilterCategoryFromString( JSON::GetSafeValueString( jsonNode, Json::category) ) ),
      NameOfTargetComponent( JSON::GetSafeValueString( jsonNode, Json::target_component ) ), 
      Type( GetFilteringTypeFromString(JSON::GetSafeValueString( jsonNode, Json::type ) ) ),
      LastFilterOfPipeline( JSON::GetSafeValueBool( jsonNode, Filter::LastFilter) )
{
    Initialize();
}

void FilterBase::Initialize(void)
{
    PrintDebugLog = false;
    EventPublisher = 0;
    EventLocation  = 0;

    // To prevent filters from reading and using non-initialized values, filters are 
    // initially disabled.
    FilterState   = FilterBase::DISABLED;
    EventDetected = 0;
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

std::string FilterBase::GenerateOutputSignalName(const std::string & prefix,
                                                 const std::string & root1,
                                                 const FilterIDType  root2,
                                                 size_t              suffix) const
{
    std::stringstream ss;
    ss << prefix << ":" << root1 << ":" << root2 << ":" << suffix;

    return ss.str();
}

const std::string FilterBase::GenerateFDIJSON(double severity, double timestamp) const
{
    return std::string("n/a (not implemented)");
}

bool FilterBase::IsDisabled(void) const {
    return !IsEnabled();
}

bool FilterBase::IsEnabled(void) const {
    return (FilterState != FilterBase::DISABLED);
}

bool FilterBase::HasPendingEvent(void) const {
    // integrity check
    if (FilterState == FilterBase::DETECTED) {
        SFASSERT(EventDetected);
    }
    return (FilterState == FilterBase::DETECTED);
}

void FilterBase::Enable(bool enable)
{
    if (enable) {
        // When enabling a filter, it can be either in the ENABLED or DETECTED state.
        if (IsEnabled())
            return;
        FilterState = FilterBase::ENABLED;
    } else {
        // If a filter detected an event which has not been resolved yet, SF should inform
        // the user of the pending event.
        if (HasPendingEvent()) {
            // TODO: print out detailed information about pending event
            SFLOG_WARNING << "Warning: filter [ " << *this << " ] has pending event" << std::endl;
        }
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

std::string FilterBase::ToString(void) const
{
    std::stringstream ss;
    ToStream(ss);

    return ss.str();
}

void FilterBase::ToStream(std::ostream & outputStream) const
{
    outputStream << "[" << UID << "] "
                 << "Name: \"" << Name << "\", "
                 << "Category: ";
    switch (Category) {
    case INVALID:        outputStream << "INVALID"; break; 
    case FEATURE:        outputStream << "FEATURE"; break; 
    case FEATURE_VECTOR: outputStream << "FEATURE_VECTOR"; break; 
    case SYMPTOM:        outputStream << "SYMPTOM"; break; 
    case SYMPTOM_VECTOR: outputStream << "SYMPTOM_VECTOR"; break; 
    case FAULT_DETECTOR: outputStream << "FAULT_DETECTOR"; break; 
    default:             SFASSERT(false);
    }
    outputStream << ", ";
    outputStream << "Target component: \"" << NameOfTargetComponent << "\", "
                 << "Filtering type: " << (Type == ACTIVE ? "ACTIVE" : "PASSIVE") << ", "
                 << "State: " << GetFilterStateString(FilterState);
    outputStream << ", Event location: " << (EventLocation ? "Available" : "n/a");
    if (LastFilterOfPipeline)
        outputStream << ", Event publisher: " << (EventPublisher ? "installed" : "n/a");
    outputStream << std::endl;

    // Input signals
    outputStream << "----- Input Signals:" << std::endl;
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        outputStream << "[" << i << "] " << (*InputSignals[i]) << std::endl;
    }
    // Output signals
    outputStream << "----- Output Signals:" << std::endl;
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        outputStream << "[" << i << "] " << (*OutputSignals[i]) << std::endl;
    }
}

//-----------------------------------------------
// Misc. Getters
//-----------------------------------------------
static const std::string STR_FILTER_INVALID        = "INVALID";
static const std::string STR_FILTER_FEATURE        = "FEATURE";
static const std::string STR_FILTER_FEATURE_VECTOR = "FEATURE_VECTOR";
static const std::string STR_FILTER_SYMPTOM        = "SYMPTOM";
static const std::string STR_FILTER_SYMPTOM_VECTOR = "SYMPTOM_VECTOR";
static const std::string STR_FILTER_FAULT_DETECTOR = "FAULT_DETECTOR";

const std::string FilterBase::GetFilterCategoryString(const FilterCategory type)
{
    if (type == FEATURE)        return STR_FILTER_FEATURE;
    if (type == FEATURE_VECTOR) return STR_FILTER_FEATURE_VECTOR;
    if (type == SYMPTOM)        return STR_FILTER_SYMPTOM;
    if (type == SYMPTOM_VECTOR) return STR_FILTER_SYMPTOM_VECTOR;
    if (type == FAULT_DETECTOR) return STR_FILTER_FAULT_DETECTOR;

    return Dict::INVALID;
}

FilterBase::FilterCategory FilterBase::GetFilterCategoryFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(STR_FILTER_FEATURE) == 0)        return FEATURE;
    if (_str.compare(STR_FILTER_FEATURE_VECTOR) == 0) return FEATURE_VECTOR;
    if (_str.compare(STR_FILTER_SYMPTOM) == 0)        return SYMPTOM;
    if (_str.compare(STR_FILTER_SYMPTOM_VECTOR) == 0) return SYMPTOM_VECTOR;
    if (_str.compare(STR_FILTER_FAULT_DETECTOR) == 0) return FAULT_DETECTOR;

    return INVALID;
}

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

};
