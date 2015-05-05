//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jan 7, 2014
// Last revision: Aug 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "bypass.h"
#include "filterFactory.h"
//#include "historyBufferBase.h"
#include "dict.h"
#include "coordinator.h"
#include "utils.h" // GetCurrentTimeTick

//
// WARNING: THIS FILTER DOES NOT WORK -- NEEDS MORE MODIFICATION
//

using namespace SF;

SF_IMPLEMENT_FACTORY(FilterBypass);

#if 0
void FilterBypass::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    // Fetch new value from history buffer and update output value
    if (InputType == SignalElement::SCALAR) {
        if (!InputSignals[0]->FetchNewValueScalar()) {
            SFLOG_ERROR << "FilterBypass: failed to fetch new scalar value from history buffer" << std::endl;
            // If failed, placeholders are set as zero internally.
            this->Enable(false); // for no further error messages
        }
        // Bypass algorithm: just set output as the latest reading 
        OutputSignals[0]->SetPlaceholderScalar(InputSignals[0]->GetPlaceholderScalar());
    } else {
        if (!InputSignals[0]->FetchNewValueVector()) {
            SFLOG_ERROR << "FilterBypass: failed to fetch new vector value from history buffer" << std::endl;
            // If failed, placeholders are set as zero internally.
            this->Enable(false); // for no further error messages
        }
        // Bypass algorithm: just set output as the latest reading 
        OutputSignals[0]->SetPlaceholderVector(InputSignals[0]->GetPlaceholderVector());
    }

    if (debug) {
        if (InputType == SignalElement::SCALAR) {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                << InputSignals[0]->GetPlaceholderScalar() << " => " << OutputSignals[0]->GetPlaceholderScalar() << std::endl;
        } else {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": [ ";
            for (size_t i = 0; i < InputSignals[0]->GetPlaceholderVector().size(); ++i) {
                std::cout << InputSignals[0]->GetPlaceholderVector()[i] << " ";
            }
            std::cout << " ] => ";
            for (size_t i = 0; i < OutputSignals[0]->GetPlaceholderVector().size(); ++i) {
                std::cout << OutputSignals[0]->GetPlaceholderVector()[i] << " ";
            }
        }
    }
}
#endif

FilterBypass::FilterBypass(void)
    : FilterBase(FilterBypass::Name, // filter name
                 FilterBase::ACTIVE, // filtering type (active vs. passive)
                 State::STATEMACHINE_INVALID, // type of state machine
                 NONAME,             // target component name
                 NONAME),            // target interface name
      NameOfInputSignal(NONAME),
      EventNameOn(NONAME),
      EventNameOff(NONAME)
{
    // This default constructor should not be used by user
    Initialize();
}

FilterBypass::FilterBypass(FilterBase::FilteringType monitoringType,
                           State::StateMachineType   targetStateMachineType,
                           const std::string &       targetComponentName,
                           const std::string &       targetInterfaceName,
                           const std::string &       inputSignalName)
    : FilterBase(FilterBypass::Name,
                 monitoringType,
                 targetStateMachineType,
                 targetComponentName,
                 targetInterfaceName),
      NameOfInputSignal(inputSignalName),
      EventNameOn(NONAME),
      EventNameOff(NONAME)
{
    Initialize();
}

FilterBypass::FilterBypass(const JSON::JSONVALUE & jsonNode)
    : FilterBase(FilterBypass::Name, jsonNode),
      NameOfInputSignal(JSON::GetSafeValueString(jsonNode["argument"], "input_signal")),
      EventNameOn(NONAME),
      EventNameOff(NONAME)
{
    Initialize();
}

FilterBypass::~FilterBypass()
{
}

void FilterBypass::Initialize(void)
{
    // filters that casros provides do not need this; this is only for user-defined filters.
    //SF_REGISTER_FILTER_TO_FACTORY(FilterBypass);

    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal, SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(NameOfInputSignal,
                                       FilterBypass::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName, SignalElement::SCALAR));
}

bool FilterBypass::ConfigureFilter(const JSON::JSONVALUE & jsonNode)
{
    EventNameOn  = JSON::GetSafeValueString(jsonNode["argument"], "event_on");
    EventNameOff = JSON::GetSafeValueString(jsonNode["argument"], "event_off");
    
    return true;
}

bool FilterBypass::InitFilter(void)
{
    this->Initialized = true;

    return true;
}

void FilterBypass::CleanupFilter(void)
{
}

void FilterBypass::RunFilter(void)
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
            std::cout << "FilterBypass [ \"" << FilterTarget.ComponentName << "\":\""
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
        const std::string evt = GenerateEventInfo(FilterBypass::OFFSET);
        SafetyCoordinator->OnEvent(evt);
        newOutput = 0.0;
    } else {
        if (LastValue == 0) {
            // onset event
            const std::string evt = GenerateEventInfo(FilterBypass::ONSET);
            SafetyCoordinator->OnEvent(evt);
            newOutput = 1.0;
        }
    }

    // Set output
    OutputSignals[0]->SetPlaceholderScalar(newOutput);
    // Update local cache
    LastValue = newInput;
}

void FilterBypass::ToStream(std::ostream & outputStream, bool verbose) const
{
    BaseType::ToStream(outputStream, verbose);

    if (!verbose)
        outputStream << EventNameOn << ", " << EventNameOff;
    else {
        outputStream << "----- Filter-specifics: " << std::endl 
                    << "Signal Type    : SCALAR" << std::endl
                    << "Next reading   : " << InputSignals[0]->GetPlaceholderScalar() << std::endl
                    << "Event on name  : " << EventNameOn << std::endl
                    << "Event off name : " << EventNameOff << std::endl;
    }
}

const std::string FilterBypass::GenerateEventInfo(EVENT_TYPE eventType) const
{
    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.Getjson();

    // Populate common attributes
    BaseType::GenerateEventInfo(json);

    json["event"]["name"] = ((eventType == FilterBypass::ONSET) ? EventNameOn : EventNameOff);
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
