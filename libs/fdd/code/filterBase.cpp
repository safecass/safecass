/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "filterBase.h"
#include "eventPublisherBase.h"

namespace SF {

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
      LastFilterOfPipeline(false),
      PrintDebugLog(false),
      Enabled(false),
      EventPublisher(0),
      EventLocation(0)
{}

FilterBase::FilterBase(const std::string & filterName,
                       FilterCategory      filterCategory,
                       const std::string & targetComponentName,
                       FilteringType       monitoringType)
    : UID(FilterUID++),
      Name(filterName),
      Category(filterCategory),
      NameOfTargetComponent(targetComponentName), 
      Type(monitoringType),
      LastFilterOfPipeline(false),
      PrintDebugLog(false),
      // MJ: if filter is enabled when constructed, the first few inputs and outputs could
      // be corrupted.
      Enabled(false),
      EventPublisher(0),
      EventLocation(0)
{
}

FilterBase::~FilterBase()
{
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        delete InputSignals[i];
    }
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        delete OutputSignals[i];
    }

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
                 << "State: " << (Enabled ? "ENABLED" : "DISABLED");
    if (LastFilterOfPipeline)
        outputStream << ", Event publisher: " << (EventPublisher ? "Avaiable" : "Unavailable");
    outputStream << ", Event location: " << (EventLocation ? "Available" : "Unavailable") << std::endl;

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

};
