//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Sep 3, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
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
      OutputAbove(0.0),
      OutputBelow(0.0),
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
      OutputAbove(outputAbove),
      OutputBelow(outputBelow),
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
      OutputAbove(outputAbove),
      OutputBelow(outputBelow),
      EventNameAbove(eventNameAbove),
      EventNameBelow(eventNameBelow)
{
    Initialize();
}

FilterThreshold::FilterThreshold(const JsonWrapper::JsonValue & jsonNode)
    : FilterBase(FilterThreshold::Name, jsonNode),
      NameOfInputSignal(JsonWrapper::GetSafeValueString(jsonNode["argument"], "input_signal")),
      Threshold(JsonWrapper::GetSafeValueDouble(jsonNode["argument"], "threshold")),
      Tolerance(JsonWrapper::GetSafeValueDouble(jsonNode["argument"], "tolerance")),
      OutputAbove(JsonWrapper::GetSafeValueDouble(jsonNode["argument"], "output_above")),
      OutputBelow(JsonWrapper::GetSafeValueDouble(jsonNode["argument"], "output_below")),
      EventNameAbove(JsonWrapper::GetSafeValueString(jsonNode["argument"], "event_onset")),
      EventNameBelow(JsonWrapper::GetSafeValueString(jsonNode["argument"], "event_completion"))
{
    Initialize();
}

FilterThreshold::~FilterThreshold()
{
}

void FilterThreshold::Initialize(void)
{
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

bool FilterThreshold::ConfigureFilter(const JsonWrapper::JsonValue & jsonNode)
{
    Threshold = JsonWrapper::GetSafeValueDouble(jsonNode["argument"], "threshold");
    Tolerance = JsonWrapper::GetSafeValueDouble(jsonNode["argument"], "tolerance");
    OutputAbove = JsonWrapper::GetSafeValueDouble(jsonNode["argument"], "output_above");
    OutputBelow = JsonWrapper::GetSafeValueDouble(jsonNode["argument"], "output_below");
    EventNameAbove = JsonWrapper::GetSafeValueString(jsonNode["argument"], "event_onset");
    EventNameBelow = JsonWrapper::GetSafeValueString(jsonNode["argument"], "event_completion");

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
        bool generate = (GetFilterState() == FilterBase::DETECTED);
        if (generate) {
            // Generate completion event
            const std::string evt = GenerateEventInfo(FilterThreshold::BELOW_THRESHOLD);
            SafetyCoordinator->OnEvent(evt);
            OutputSignals[0]->SetPlaceholderScalar(OutputBelow);

            // Reset filter status to undetected (i.e., enabled)
            SetFilterState(FilterBase::ENABLED);
        }
        return;
    }

    if (this->EventDetectionMode == FilterBase::EVENT_DETECTION_LEVEL) {
        // Above threshold with tolerance; Generate event
        // TODO: Right now FilterBase::LastFilterOfPipeline is not being used, which allows
        // any filter to generate events.  Should filter pipelines be used, it may be
        // necessary to allow only the last filter to generate events to avoid potential 
        // event flooding issue.
        const std::string evt = GenerateEventInfo(FilterThreshold::ABOVE_THRESHOLD);
        SafetyCoordinator->OnEvent(evt); // Onset event detected
        OutputSignals[0]->SetPlaceholderScalar(OutputAbove);
    } else {
        SFASSERT(this->EventDetectionMode == FilterBase::EVENT_DETECTION_EDGE);
        SFASSERT(GetFilterState() != FilterBase::DISABLED);

        if (GetFilterState() != FilterBase::DETECTED) {
            // Generate onset event
            const std::string evt = GenerateEventInfo(FilterThreshold::ABOVE_THRESHOLD);
            SafetyCoordinator->OnEvent(evt); // Onset event detected
            OutputSignals[0]->SetPlaceholderScalar(OutputAbove);

        } else {
            // NOP; onset event has already been generated
        }
    }

    // Set filter status to detected
    SetFilterState(FilterBase::DETECTED);
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
                    << "EventBelow : " << EventNameBelow << std::endl;
}

const std::string FilterThreshold::GenerateEventInfo(EVENT_TYPE eventType) const
{
    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.GetRoot();

    // Populate common attributes
    BaseType::GenerateEventInfo(json);

    json["event"]["name"] = ((eventType == FilterThreshold::ABOVE_THRESHOLD) ? EventNameAbove : EventNameBelow);
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
