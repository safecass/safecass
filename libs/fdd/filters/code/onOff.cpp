//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jun 30, 2014
// Last revision: Aug 5, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "onOff.h"
#include "filterFactory.h"
#include "dict.h"
#include "coordinator.h"
#include "utils.h" // GetCurrentTimeTick

using namespace SC;

SC_IMPLEMENT_FACTORY(FilterOnOff);

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

FilterOnOff::FilterOnOff(const JsonWrapper::JsonValue & jsonNode)
    : FilterBase(FilterOnOff::Name, jsonNode),
      NameOfInputSignal(JsonWrapper::GetSafeValueString(jsonNode["argument"], "input_signal")),
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
    //SC_REGISTER_FILTER_TO_FACTORY(FilterOnOff);

    // Define inputs
    SCASSERT(this->AddInputSignal(NameOfInputSignal, SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(NameOfInputSignal,
                                       FilterOnOff::Name,
                                       this->UID,
                                       0));
    SCASSERT(this->AddOutputSignal(outputSignalName, SignalElement::SCALAR));
}

bool FilterOnOff::ConfigureFilter(const JsonWrapper::JsonValue & jsonNode)
{
    EventNameOn  = JsonWrapper::GetSafeValueString(jsonNode["argument"], "event_on");
    EventNameOff = JsonWrapper::GetSafeValueString(jsonNode["argument"], "event_off");
    
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
    double newOutput = 0.0;
    SCASSERT(SafetyCoordinator);
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

void FilterOnOff::ToStream(std::ostream & outputStream, bool verbose) const
{
    BaseType::ToStream(outputStream, verbose);

    if (!verbose)
        outputStream << EventNameOn << ", " << EventNameOff;
    else {
        outputStream << "----- Filter-specifics: " << std::endl 
                    << "Signal Type    : SCALAR" << std::endl
                    << "Last input     : " << LastValue << std::endl
                    << "Next reading   : " << InputSignals[0]->GetPlaceholderScalar() << std::endl
                    << "Event on name  : " << EventNameOn << std::endl
                    << "Event off name : " << EventNameOff << std::endl;
    }
}

const std::string FilterOnOff::GenerateEventInfo(EVENT_TYPE eventType) const
{
    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.GetRoot();

    // Populate common attributes
    BaseType::GenerateEventInfo(json);

    json["event"]["name"] = ((eventType == FilterOnOff::ONSET) ? EventNameOn : EventNameOff);
    //json["event"]["severity"] = 255; // TEMP
#if 0
    json["event"]["target"]["type"]      = this->FilterTarget.StateMachineType;
    json["event"]["target"]["component"] = this->FilterTarget.ComponentName;
    json["event"]["target"]["interface"] = this->FilterTarget.InterfaceName;
#endif
    json["target"]["type"]      = this->FilterTarget.StateMachineType;
    json["target"]["component"] = this->FilterTarget.ComponentName;
    json["target"]["interface"] = this->FilterTarget.InterfaceName;

    return JsonWrapper::GetJSONString(json);
}
