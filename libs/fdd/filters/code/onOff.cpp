//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jun 30, 2014
// Last revision: Jul 3, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "onOff.h"
#include "filterFactory.h"
#include "dict.h"
#include "coordinator.h"

using namespace SF;

SF_IMPLEMENT_FACTORY(FilterOnOff);

FilterOnOff::FilterOnOff(void)
{
    // This default constructor should not be used.
}

FilterOnOff::FilterOnOff(const std::string &             targetComponentName,
                         const FilterBase::FilteringType monitoringType,
                         const std::string &             inputSignalName)
    : FilterBase(FilterOnOff::Name, targetComponentName, monitoringType),
      NameOfInputSignal(inputSignalName),
      LastValue(0.0),
      EventNameOn(NONAME),
      EventNameOff(NONAME),
      Initialized(false)
{
    Initialize();
}

FilterOnOff::FilterOnOff(const JSON::JSONVALUE & jsonNode)
    : FilterBase(FilterOnOff::Name, jsonNode),
      NameOfInputSignal(JSON::GetSafeValueString(
          jsonNode[Dict::Filter::argument], Dict::Filter::input_signal)),
      LastValue(0.0),
      EventNameOn(NONAME),
      EventNameOff(NONAME),
      Initialized(false)
{
    Initialize();
}

FilterOnOff::~FilterOnOff()
{
}

bool FilterOnOff::ConfigureFilter(const JSON::JSONVALUE & jsonNode)
{
    EventNameOn  = JSON::GetSafeValueString(jsonNode["argument"], "event_on");
    EventNameOff = JSON::GetSafeValueString(jsonNode["argument"], "event_off");
    
    return true;
}

void FilterOnOff::Initialize(void)
{
    FilterBase::Initialize();

    // filters that casros provides do not need this; this is only for user-defined filters.
    //SF_REGISTER_FILTER_TO_FACTORY(FilterOnOff);

    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal, SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(NameOfInputSignal,
                                       FilterOnOff::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName, SignalElement::SCALAR));
}

bool FilterOnOff::InitFilter(void)
{
    InputSignals[0]->SetPlaceholderScalar(0.0);

    return true;
}

void FilterOnOff::CleanupFilter(void)
{
}

void FilterOnOff::RunFilter(void)
{
    if (!FilterBase::RefreshSamples())
        return;

    // Debug log if enabled
    if (this->PrintDebugLog)
        std::cout << *this << std::endl << std::flush;

    // Filtering algorithm: if the new input is different from the local cache,
    // output becomes non-zero.  If they are the same, output is zero.
    SignalElement::ScalarType newInput = InputSignals[0]->GetPlaceholderScalar();
    if (newInput == LastValue) {
        OutputSignals[0]->SetPlaceholderScalar(0.0);
        return;
    }

    // value changes; edge is detected
    if (newInput == 0.0) {
        if (!SafetyCoordinator)
            SFLOG_ERROR << "NULL Safety Coordinator: OFF event is discarded" << std::endl;
        else
            SafetyCoordinator->OnEvent(EventNameOff);
    } else {
        if (LastValue == 0.0) {
            std::cout << "####### New input: " << newInput << std::endl;
            // Inform Safety Coordinator of the ON event
            if (!SafetyCoordinator)
                SFLOG_ERROR << "NULL Safety Coordinator: ON event is discarded" << std::endl;
            else
                SafetyCoordinator->OnEvent(EventNameOn);
        }
    }

    // Set output
    OutputSignals[0]->SetPlaceholderScalar(newInput ? 1.0 : -1.0);
    // Update local cache
    LastValue = newInput;
}

void FilterOnOff::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);

    outputStream << "----- Filter-specifics: " << std::endl 
                 << "Signal Type    : SCALAR" << std::endl
                 << "Last input     : " << LastValue << std::endl
                 << "Current reading: " << InputSignals[0]->GetPlaceholderScalar() << std::endl
                 << "Event on name  : " << EventNameOn << std::endl
                 << "Event off name : " << EventNameOff << std::endl
                 << std::endl;
}
