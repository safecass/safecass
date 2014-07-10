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
    : FilterBase(FilterOnOff::Name,  // filter name
                 FilterBase::ACTIVE, // filtering type (active vs. passive)
                 State::STATEMACHINE_INVALID, // type of state machine
                 NONAME,             // target component name
                 NONAME),            // target interface name
      NameOfInputSignal(NONAME),
      LastValue(0),
      EventNameOn(NONAME),
      EventNameOff(NONAME)
{
    // This default constructor should not be used by user
    Initialize();
}

FilterOnOff::FilterOnOff(FilterBase::FilteringType monitoringType,
                         State::StateMachineType   targetStateMachineType,
                         const std::string &       targetComponentName,
                         const std::string &       targetInterfaceName,
                         const std::string &       inputSignalName)
    : FilterBase(FilterOnOff::Name,
                 monitoringType,
                 targetStateMachineType,
                 targetComponentName,
                 targetInterfaceName),
      NameOfInputSignal(inputSignalName),
      LastValue(0),
      EventNameOn(NONAME),
      EventNameOff(NONAME)
{
    Initialize();
}

FilterOnOff::FilterOnOff(const JSON::JSONVALUE & jsonNode)
    : FilterBase(FilterOnOff::Name, jsonNode),
      NameOfInputSignal(JSON::GetSafeValueString(jsonNode["argument"], "input_signal")),
      LastValue(0),
      EventNameOn(NONAME),
      EventNameOff(NONAME)
{
    Initialize();
}

FilterOnOff::~FilterOnOff()
{
}

void FilterOnOff::Initialize(void)
{
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

bool FilterOnOff::ConfigureFilter(const JSON::JSONVALUE & jsonNode)
{
    EventNameOn  = JSON::GetSafeValueString(jsonNode["argument"], "event_on");
    EventNameOff = JSON::GetSafeValueString(jsonNode["argument"], "event_off");
    
    return true;
}

bool FilterOnOff::InitFilter(void)
{
    this->Initialized = true;

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
    //if (this->PrintDebugLog)
    //    std::cout << *this << std::endl << std::flush;

    // Filtering algorithm: 
    // Output is 1 if the new input is different from the previous value and 
    // the input is non-zero (edge-trigerred).  Otherwise, output is zero.
    //int newInput = (int) InputSignals[0]->GetPlaceholderScalar();
    double i = InputSignals[0]->GetPlaceholderScalar();
    int newInput = (int) i;

    if (this->PrintDebugLog) {
        if (newInput != 0)
            std::cout << "FilterOnOff [ \"" << FilterTarget.ComponentName << "\":\""
                << FilterTarget.InterfaceName << "\" ] : " << newInput << std::endl << std::flush;
    }

    if (newInput == LastValue) {
        OutputSignals[0]->SetPlaceholderScalar(0.0);
        return;
    }

    // value changes; edge is detected
    double newOutput;
    SFASSERT(SafetyCoordinator);
    if (newInput == 0) {
        // offset event
        const std::string evt = GenerateEventInfo(FilterOnOff::OFFSET);
        SafetyCoordinator->OnEvent(evt);
        newOutput = 0.0;
    } else {
        if (LastValue == 0) {
            // onset event
            const std::string evt = GenerateEventInfo(FilterOnOff::ONSET);
            SafetyCoordinator->OnEvent(evt);
            newOutput = 1.0;
        }
    }

    // Set output
    OutputSignals[0]->SetPlaceholderScalar(newOutput);
    // Update local cache
    LastValue = newInput;
}

void FilterOnOff::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);

    outputStream << "----- Filter-specifics: " << std::endl 
                 << "Signal Type    : SCALAR" << std::endl
                 << "Last input     : " << LastValue << std::endl
                 << "Next reading   : " << InputSignals[0]->GetPlaceholderScalar() << std::endl
                 << "Event on name  : " << EventNameOn << std::endl
                 << "Event off name : " << EventNameOff << std::endl
                 << std::endl;
}

const std::string FilterOnOff::GenerateEventInfo(EVENT_TYPE eventType) const
{
    JSON json;
    JSON::JSONVALUE & root = json.GetRoot();
    root["event"]["name"] = ((eventType == FilterOnOff::ONSET) ? EventNameOn : EventNameOff);
    // TODO: integrate boost time/date library
    root["event"]["timestamp"] = 1234.5678; // TEMP
    root["event"]["target"]["type"]      = this->FilterTarget.StateMachineType;
    root["event"]["target"]["component"] = this->FilterTarget.ComponentName;
    root["event"]["target"]["interface"] = this->FilterTarget.InterfaceName;
    root["event"]["severity"] = 255; // TEMP
    root["event"]["fuid"] = this->UID;

    return JSON::GetJSONString(root);
}
