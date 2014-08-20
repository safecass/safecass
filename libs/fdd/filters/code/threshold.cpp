//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Sep 3, 2012
// Last revision: Aug 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "threshold.h"
#include "filterFactory.h"
#include "dict.h"
#include "coordinator.h"
#include <iomanip>

using namespace SF;

SF_IMPLEMENT_FACTORY(FilterThreshold);

FilterThreshold::FilterThreshold(void)
    : FilterBase(FilterThreshold::Name,  // filter name
                 FilterBase::ACTIVE, // filtering type (active vs. passive)
                 State::STATEMACHINE_INVALID, // type of state machine
                 NONAME,             // target component name
                 NONAME),            // target interface name
      NameOfInputSignal(NONAME),
      Threshold(0.0),
      Tolerance(0.0),
      OutputBelow(0.0),
      OutputAbove(0.0),
      EventNameAbove(NONAME),
      EventNameBelow(NONAME)
{
    Initialize();
}

FilterThreshold::FilterThreshold(FilterBase::FilteringType monitoringType,
                                 State::StateMachineType   targetStateMachineType,
                                 const std::string &       targetComponentName,
                                 const std::string &       targetInterfaceName,
                                 const std::string &       inputSignalName,
                                 SignalElement::ScalarType threshold,
                                 SignalElement::ScalarType tolerance,
                                 SignalElement::ScalarType outputBelow,
                                 SignalElement::ScalarType outputAbove,
                                 const std::string &       eventNameAbove,
                                 const std::string &       eventNameBelow)
    : FilterBase(FilterThreshold::Name,
                 monitoringType,
                 targetStateMachineType,
                 targetComponentName,
                 targetInterfaceName),
      NameOfInputSignal(inputSignalName),
      Threshold(threshold),
      Tolerance(tolerance),
      OutputBelow(outputBelow),
      OutputAbove(outputAbove),
      EventNameAbove(eventNameAbove),
      EventNameBelow(eventNameBelow)
{
    Initialize();
}

FilterThreshold::FilterThreshold(FilterBase::FilteringType monitoringType,
                                 State::StateMachineType   targetStateMachineType,
                                 const std::string &       targetComponentName,
                                 const std::string &       inputSignalName,
                                 SignalElement::ScalarType threshold,
                                 SignalElement::ScalarType tolerance,
                                 SignalElement::ScalarType outputBelow,
                                 SignalElement::ScalarType outputAbove,
                                 const std::string &       eventNameAbove,
                                 const std::string &       eventNameBelow)
    : FilterBase(FilterThreshold::Name,
                 monitoringType,
                 targetStateMachineType,
                 targetComponentName,
                 NONAME),
      NameOfInputSignal(inputSignalName),
      Threshold(threshold),
      Tolerance(tolerance),
      OutputBelow(outputBelow),
      OutputAbove(outputAbove),
      EventNameAbove(eventNameAbove),
      EventNameBelow(eventNameBelow)
{
    Initialize();
}

FilterThreshold::FilterThreshold(const JSON::JSONVALUE & jsonNode)
    : FilterBase(FilterThreshold::Name, jsonNode),
      NameOfInputSignal(JSON::GetSafeValueString(jsonNode["argument"], "input_signal")),
      Threshold(JSON::GetSafeValueDouble(jsonNode["argument"], "threshold")),
      Tolerance(JSON::GetSafeValueDouble(jsonNode["argument"], "tolerance")),
      OutputAbove(JSON::GetSafeValueDouble(jsonNode["argument"], "output_above")),
      OutputBelow(JSON::GetSafeValueDouble(jsonNode["argument"], "output_below")),
      EventNameAbove(JSON::GetSafeValueString(jsonNode["argument"], "event_above")),
      EventNameBelow(JSON::GetSafeValueString(jsonNode["argument"], "event_below"))
{
    Initialize();
}

FilterThreshold::~FilterThreshold()
{
}

void FilterThreshold::Initialize(void)
{
    IsAboveThreshold = false;

    // Register this filter to the filter factory
    // filters that casros provides do not need this; this is only for user-defined filters.
    //SF_REGISTER_FILTER_TO_FACTORY(FilterThreshold);

    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal, SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(NameOfInputSignal,
                                       FilterThreshold::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName, SignalElement::SCALAR));
}

bool FilterThreshold::ConfigureFilter(const JSON::JSONVALUE & jsonNode)
{
    Threshold = JSON::GetSafeValueDouble(jsonNode["argument"], "threshold");
    Tolerance = JSON::GetSafeValueDouble(jsonNode["argument"], "tolerance");
    OutputAbove = JSON::GetSafeValueDouble(jsonNode["argument"], "output_above");
    OutputBelow = JSON::GetSafeValueDouble(jsonNode["argument"], "output_below");
    EventNameAbove = JSON::GetSafeValueString(jsonNode["argument"], "event_above");
    EventNameBelow = JSON::GetSafeValueString(jsonNode["argument"], "event_below");

    return true;
}

bool FilterThreshold::InitFilter(void)
{
    this->Initialized = true;

    return true;
}

void FilterThreshold::CleanupFilter(void)
{
}

void FilterThreshold::RunFilter(void)
{
    if (!FilterBase::RefreshSamples())
        return;

    const double newInput = InputSignals[0]->GetPlaceholderScalar();

    if (this->PrintDebugLog) {
        std::cout << "FilterThreshold[ \"" << FilterTarget.ComponentName << "\":\""
                  << FilterTarget.InterfaceName << "\" ] : " 
                  << std::setprecision(5) << newInput << std::endl << std::flush;
    }

    // Below threshold
    if (newInput <= Threshold + Tolerance) {
        if (IsAboveThreshold) {
            const std::string evt = GenerateEventInfo(FilterThreshold::BELOW_THRESHOLD);
            SafetyCoordinator->OnEvent(evt); // Offset event detected
            IsAboveThreshold = false;
        }
        OutputSignals[0]->SetPlaceholderScalar(OutputBelow);
        return;
    }

    // Above threshold with tolerance; Generate event
    // TODO: Right now FilterBase::LastFilterOfPipeline is not being used, which allows
    // any filter to generate events.  Should filter pipelines be used, it may be
    // necessary to allow only the last filter to generate events to avoid potential 
    // event flooding issue.
    const std::string evt = GenerateEventInfo(FilterThreshold::ABOVE_THRESHOLD);
    SafetyCoordinator->OnEvent(evt); // Onset event detected
    OutputSignals[0]->SetPlaceholderScalar(OutputAbove);

    IsAboveThreshold = true;
}

void FilterThreshold::ToStream(std::ostream & outputStream, bool verbose) const
{
    BaseType::ToStream(outputStream, verbose);

    if (!verbose)
        outputStream << EventNameAbove << ", " << EventNameBelow;
    else
        outputStream << "----- Filter-specifics: " << std::endl
                    << "Threshold  : " << Threshold << std::endl
                    << "Tolerance  : " << Tolerance << std::endl
                    << "OutputAbove: " << OutputAbove << std::endl
                    << "OutputBelow: " << OutputBelow << std::endl
                    << "EventAbove : " << EventNameAbove << std::endl
                    << "EventBelow : " << EventNameBelow << std::endl
                    << "Above Th.? : " << (IsAboveThreshold ? "true" : "false") << std::endl;
}

const std::string FilterThreshold::GenerateEventInfo(EVENT_TYPE eventType) const
{
    JSON json;
    JSON::JSONVALUE & root = json.GetRoot();
    root["event"]["name"] = ((eventType == FilterThreshold::ABOVE_THRESHOLD) ? EventNameAbove : EventNameBelow);
    root["event"]["timestamp"] = InputSignals[0]->GetTimeLastSampleFetched();
    //root["event"]["severity"] = 255; // TEMP
    root["event"]["fuid"] = this->UID;
#if 0
    root["event"]["target"]["type"]      = this->FilterTarget.StateMachineType;
    root["event"]["target"]["component"] = this->FilterTarget.ComponentName;
    root["event"]["target"]["interface"] = this->FilterTarget.InterfaceName;
#endif
    root["target"]["type"]      = this->FilterTarget.StateMachineType;
    root["target"]["component"] = this->FilterTarget.ComponentName;
    root["target"]["interface"] = this->FilterTarget.InterfaceName;

    return JSON::GetJSONString(root);
}
