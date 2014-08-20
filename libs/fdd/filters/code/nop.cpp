//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Aug 20, 2014
// Last revision: Aug 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "nop.h"
#include "filterFactory.h"
//#include "dict.h"
//#include "coordinator.h"

using namespace SF;

SF_IMPLEMENT_FACTORY(FilterNOP);

FilterNOP::FilterNOP(void)
    : FilterBase(FilterNOP::Name,    // filter name
                 FilterBase::ACTIVE, // filtering type (active vs. passive)
                 State::STATEMACHINE_INVALID, // type of state machine
                 NONAME,             // target component name
                 NONAME),            // target interface name
      NameOfInputSignal(NONAME)
{
    // This default constructor should not be used by user
    Initialize();
}

FilterNOP::FilterNOP(const std::string &       targetComponentName, 
                     const std::string &       inputSignalName,
                     SignalElement::SignalType inputSignalType)
    : FilterBase(FilterNOP::Name,
                 FilterBase::ACTIVE,
                 State::STATEMACHINE_INVALID,
                 targetComponentName,
                 NONAME),
      NameOfInputSignal(inputSignalName), InputSignalType(inputSignalType)
{
    Initialize();
}

FilterNOP::FilterNOP(const JSON::JSONVALUE & json)
    : FilterBase(FilterNOP::Name, json),
      NameOfInputSignal(JSON::GetSafeValueString(json["argument"], "input_signal"))
{
    Initialize();
}

FilterNOP::~FilterNOP()
{
}

void FilterNOP::Initialize(void)
{
    SFASSERT(this->AddInputSignal(NameOfInputSignal, InputSignalType));

#if 0
    const std::string outputSignalName(
        this->GenerateOutputSignalName(NameOfInputSignal,
                                       FilterNOP::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName, SignalElement::SCALAR));
#endif
}

bool FilterNOP::InitFilter(void)
{
    this->Initialized = true;
    return true;
}

bool FilterNOP::ConfigureFilter(const JSON::JSONVALUE & json)
{
    return true;
}

void FilterNOP::RunFilter(void)
{
    return; // NOP
#if 0
    if (!FilterBase::RefreshSamples())
        return;

    // Filtering algorithm: 
    // Output is 1 if the new input is different from the previous value and 
    // the input is non-zero (edge-trigerred).  Otherwise, output is zero.
    //int newInput = (int) InputSignals[0]->GetPlaceholderScalar();
    double i = InputSignals[0]->GetPlaceholderScalar();
    int newInput = (int) i;

    if (this->PrintDebugLog) {
        if (newInput != 0)
            std::cout << "FilterNOP [ \"" << FilterTarget.ComponentName << "\":\""
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
        const std::string evt = GenerateEventInfo(FilterNOP::OFFSET);
        SafetyCoordinator->OnEvent(evt);
        newOutput = 0.0;
    } else {
        if (LastValue == 0) {
            // onset event
            const std::string evt = GenerateEventInfo(FilterNOP::ONSET);
            SafetyCoordinator->OnEvent(evt);
            newOutput = 1.0;
        }
    }

    // Set output
    OutputSignals[0]->SetPlaceholderScalar(newOutput);
    // Update local cache
    LastValue = newInput;
#endif
}
