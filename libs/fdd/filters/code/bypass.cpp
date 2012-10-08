/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

/*
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/FilterBasics.h>
#include <cisstMultiTask/mtsFaultDetectorThresholding.h>
*/

#include "bypass.h"
#include "historyBufferBase.h"

namespace SF {

const std::string NameOfBypassFilter = "Bypass";

FilterBypass::FilterBypass(BaseType::FilterCategory  category, 
                           const std::string &       inputName,
                           SignalElement::SignalType inputType,
                           HistoryBufferBase * historyBuffer)
    : FilterBase(NameOfBypassFilter, category, historyBuffer),
      InputType(inputType)
{
    // Define inputs
    this->AddInputSignal(inputName, inputType);

    // Define outputs
    std::stringstream ss;
    ss << inputName << ":" << NameOfBypassFilter << "[" << this->UID << "]";
    this->AddOutputSignal(ss.str(), inputType);
}

FilterBypass::~FilterBypass()
{
}

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

void FilterBypass::ToStream(std::ostream & outputStream) const
{
    outputStream << "Filter Name: " << this->GetFilterName() << " ("
                 << (InputType == SignalElement::SCALAR ? "SCALAR" : "VECTOR")
                 << "), " << (this->IsEnabled() ? "Enabled" : "Disabled")
                 << ", Input: \"" << InputSignals[0]->GetName() << "\", Output: \"" << OutputSignals[0]->GetName() << "\"";
}

};
