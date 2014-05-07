//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Sep 3, 2012
// Last revision: May 6, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "threshold.h"
#include "dict.h"
#include "jsonSerializer.h"
#include "eventPublisherBase.h"
#include "filterFactory.h"

namespace SF {

const std::string FilterThreshold::Name = "FilterThreshold";

FilterThreshold::FilterThreshold(BaseType::FilterCategory      category, 
                                 const std::string &           targetComponentName,
                                 SF::FilterBase::FilteringType monitoringType,
                                 // filter-specific arguments
                                 const std::string &           inputSignalName,
                                 SignalElement::ScalarType     threshold,
                                 SignalElement::ScalarType     margin,
                                 SignalElement::ScalarType     output0,
                                 SignalElement::ScalarType     output1)
    : FilterBase(FilterThreshold::Name, category, targetComponentName, monitoringType),
      NameOfInputSignal(inputSignalName),
      Threshold(threshold),
      Margin(margin),
      Output0(output0),
      Output1(output1)
{
    Initialize();
}

FilterThreshold::FilterThreshold(const JSON::JSONVALUE & jsonNode)
    : FilterBase(FilterThreshold::Name, jsonNode),
      NameOfInputSignal(JSON::GetSafeValueString(
          jsonNode[Dict::Filter::Arguments], Dict::Filter::InputSignalName)),
      Threshold(JSON::GetSafeValueDouble(
          jsonNode[Dict::Filter::Arguments], Dict::FilterThreshold::threshold)),
      Margin(JSON::GetSafeValueDouble(
          jsonNode[Dict::Filter::Arguments], Dict::FilterThreshold::margin)),
      Output0(JSON::GetSafeValueDouble(
          jsonNode[Dict::Filter::Arguments], Dict::FilterThreshold::output0)),
      Output1(JSON::GetSafeValueDouble(
          jsonNode[Dict::Filter::Arguments], Dict::FilterThreshold::output1))
{
    Initialize();
}

void FilterThreshold::Initialize(void)
{
    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal,
                                  SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(NameOfInputSignal,
                                       FilterThreshold::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName,
                                   SignalElement::SCALAR));

    // Register this filter to the filter factory
    SF_REGISTER_FILTER_TO_FACTORY(FilterThreshold);
}

FilterThreshold::~FilterThreshold()
{}

bool FilterThreshold::InitFilter(void)
{
    return true;
}

void FilterThreshold::RunFilter(void)
{
    if (!this->IsEnabled()) return;

    // Fetch new value from history buffer and update output value
    if (!InputSignals[0]->FetchNewValueScalar((this->Type == FilterBase::ACTIVE))) {
        SFLOG_ERROR << "FilterThreshold: failed to fetch new scalar value from history buffer" << std::endl;
        // If failed, placeholders are set as zero internally.
        this->Enable(false); // for no further error messages
        return;
    }

    // Filtering algorithm: thresholding with margin
    if (InputSignals[0]->GetPlaceholderScalar() > Threshold + Margin) {
        // Generate event if necessary (MJ: this may need to be done only for
        // filters of type FAULT_DETECTOR category)
        OutputSignals[0]->SetPlaceholderScalar(Output1);
        // If this filter is the last filter of a pipeline
        if (this->LastFilterOfPipeline) {
            // If there is an event publisher associated with this filter and
            // it is properly installed, publish the detected fault event to 
            // the Safety Framework.
            if (this->EventPublisher) {
                double timestamp = InputSignals[0]->GetTimeLastSampleFetched();
                //double severity = InputSignals[0]->GetPlaceholderScalar() - (Threshold + Margin);
                double severity = InputSignals[0]->GetPlaceholderScalar() - Threshold;
                this->EventPublisher->PublishEvent(GenerateFDIJSON(severity, timestamp));
            } else {
                SFLOG_ERROR << "FilterThreshold: No event publisher is active and thus event cannot be published" << std::endl;
            }
        }
    } else {
        OutputSignals[0]->SetPlaceholderScalar(Output0);
    }

    if (this->PrintDebugLog) {
        std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
            << InputSignals[0]->GetPlaceholderScalar() << " => " << OutputSignals[0]->GetPlaceholderScalar() << std::endl;
    }
}

void FilterThreshold::FilterThreshold::CleanupFilter(void)
{
    // NOP
}

const std::string FilterThreshold::GenerateFDIJSON(double severity, double timestamp) const
{
    if (!EventLocation) {
        SFLOG_ERROR << "GenerateFDIJSON: No event location instance available" << std::endl;
        return "ERROR: no event location available";
    }

    // Create JSONSerializer instance 
    JSONSerializer serializer;

    // Populate common fields
    serializer.SetTopicType(JSONSerializer::EVENT);
    serializer.SetEventLocation(EventLocation);
    serializer.SetTimestamp(timestamp);

    // Populate fault information
    // FIXME
#if 0
    serializer.SetEventType(Event::EVENT_FAULT);
    serializer.SetFaultType(Event::FAULT_APPLICATION);
    serializer.SetFaultDetectorName(this->GetFilterName()); // MJ: could use the name of filter pipeline instead

    // Populate fault-specific fields
    JSON::JSONVALUE & fields = serializer.GetFaultFields();
    fields[Dict::Json::severity] = severity;
#endif

    return serializer.GetJSON();
}

void FilterThreshold::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);

    outputStream << "----- Filter-specifics: " << std::endl
                 << "Threshold: " << Threshold << ", "
                 << "Margin: " << Margin << ", "
                 << "Output0: " << Output0 << ", "
                 << "Output1: " << Output1 << std::endl;
}

};
