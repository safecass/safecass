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

const std::string NameOfThresholdFilter = "Threshold";

FilterThreshold::FilterThreshold(BaseType::FilterCategory category, 
                                 const std::string & inputName,
                                 SignalElement::ScalarType threshold,
                                 SignalElement::ScalarType output0,
                                 SignalElement::ScalarType output1)
    : FilterBase(category, NameOfThresholdFilter),
      Threshold(threshold), Output0(output0), Output1(output1)
{
    // Define inputs
    SFASSERT(this->AddInputSignal(inputName, SignalElement::SCALAR));

    // Define outputs
    std::stringstream ss;
    ss << NameOfThresholdFilter << ":" << inputName << ":" << this->UID;
    SFASSERT(this->AddOutputSignal(ss.str(), SignalElement::SCALAR));
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

    // Filtering algorithm: thresholding
    if (InputSignals[0]->GetPlaceholderScalar() > Threshold) {
        // MJ TODO: Generate event if necessary (this may need to be done only for
        // filters of type FAULT_DETECTOR category)
        OutputSignals[0]->SetPlaceholderScalar(Output1);
    } else {
        OutputSignals[0]->SetPlaceholderScalar(Output0);
    }

    if (debug) {
        std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
            << InputSignals[0]->GetPlaceholderScalar() << " => " << OutputSignals[0]->GetPlaceholderScalar() << std::endl;
    }
}

void FilterThreshold::ToStream(std::ostream & outputStream) const
{
    outputStream << "Filter Name: " << this->GetFilterName() << ", "
                 << (this->IsEnabled() ? "Enabled" : "Disabled")
                 << ", Input: \"" << InputSignals[0]->GetName()<< "\""
                 << ", Output: \"" << OutputSignals[0]->GetName() << "\"";
}

};
