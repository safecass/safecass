/*

  Safety Framework for Component-based Robotics

  Created on: September 3, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "threshold.h"

namespace SF {

const std::string FilterThreshold::Name = "Threshold";

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
    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal,
                                  SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(inputSignalName,
                                       FilterThreshold::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName,
                                   SignalElement::SCALAR));
}

FilterThreshold::~FilterThreshold()
{
}

void FilterThreshold::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    // Fetch new value from history buffer and update output value
    if (!InputSignals[0]->FetchNewValueScalar()) {
        SFLOG_ERROR << "FilterThreshold: failed to fetch new scalar value from history buffer" << std::endl;
        // If failed, placeholders are set as zero internally.
        this->Enable(false); // for no further error messages
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
                this->EventPublisher->PublishEvent(GenerateFDIJSON());
            } else {
                SFLOG_ERROR << "FilterThreshold: No event publisher is active and thus event cannot be published" << std::endl;
            }
        }
    } else {
        OutputSignals[0]->SetPlaceholderScalar(Output0);
    }

    if (debug) {
        std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
            << InputSignals[0]->GetPlaceholderScalar() << " => " << OutputSignals[0]->GetPlaceholderScalar() << std::endl;
    }
}

const std::string FilterThreshold::GenerateFDIJSON(void) const
{
    /*
        FDI JSON Specification: 

        {   // event identification
            "type" : {
                "fault" : "application",
                "description" : "force sensor reading (Y) threshold exceeded"
            },
            // event localization
            "localization" : {
                "location" : {
                    "process" : "LCM",
                    "component" : "component_name",
                    // below are middleware-specific
                    "provided_interface" : "",
                    "required_interface" : "",
                    "command" : "",
                    "function" : "",
                    "event_generator" : "",
                    "event_handler" : "" },
                "timestamp" : 1234567890 
            },
            // severity
            "severity" : 1.3
        }
     */
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
