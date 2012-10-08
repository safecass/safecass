/*

  Safety Framework for Component-based Robotics

  Created on: October 8, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "trendVel.h"

namespace SF {

const std::string FilterTrendVel::Name = "TrendVel";

FilterTrendVel::FilterTrendVel(BaseType::FilterCategory      category, 
                               const std::string &           targetComponentName,
                               SF::FilterBase::FilteringType monitoringType,
                               // filter-specific arguments
                               const std::string &           inputSignalName,
                               SignalElement::SignalType     signalType)
    : FilterBase(FilterTrendVel::Name, category, targetComponentName, monitoringType),
      NameOfInputSignal(inputSignalName),
      SignalType(signalType)
{
    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal, SignalType));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(inputSignalName,
                                       FilterTrendVel::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName, SignalType));

    // Initialize local cache for previous value
    PreviousValue.Initialized = false;
    PreviousValue.Timestamp = 0.0;
    PreviousValue.Scalar = 0.0;
    // lazy initialize PreviousValue.Scalar (because initialization could require
    // middleware-specific codes here)
}

FilterTrendVel::~FilterTrendVel()
{
}

void FilterTrendVel::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    if (SignalType == SignalElement::SCALAR) {
        // Fetch new value from history buffer
        if (!InputSignals[0]->FetchNewValueScalar((this->Type == FilterBase::ACTIVE))) {
            SFLOG_ERROR << "FilterTrendVel: failed to fetch new scalar value from history buffer" << std::endl;
            // If failed, placeholders are set as zero internally.
            this->Enable(false); // for no further error messages
            return;
        }

        // Filtering algorithm: 1st-order differentiation
        SignalElement::ScalarType newInput = InputSignals[0]->GetPlaceholderScalar();
        const SignalElement::ScalarType newTimestamp = InputSignals[0]->GetTimeLastSampleFetched();
        if (!PreviousValue.Initialized) {
            // bypass first input as first output
            OutputSignals[0]->SetPlaceholderScalar(newInput);
            PreviousValue.Initialized = true;
        } else {
            OutputSignals[0]->SetPlaceholderScalar(
                (newInput - PreviousValue.Scalar) / (newTimestamp - PreviousValue.Timestamp));
        }

        // Update local cache
        PreviousValue.Timestamp = newTimestamp;
        PreviousValue.Scalar = newInput;
    } else {
        // Fetch new value from history buffer
        if (!InputSignals[0]->FetchNewValueVector((this->Type == FilterBase::ACTIVE))) {
            SFLOG_ERROR << "FilterTrendVel: failed to fetch new scalar value from history buffer" << std::endl;
            // If failed, placeholders are set as zero internally.
            this->Enable(false); // for no further error messages
            return;
        }

        // Filtering algorithm: 1st-order differentiation
        SignalElement::VectorType newInput = InputSignals[0]->GetPlaceholderVector();
        const SignalElement::TimestampType newTimestamp = InputSignals[0]->GetTimeLastSampleFetched();
        if (!PreviousValue.Initialized) {
            // bypass first input as first output
            OutputSignals[0]->SetPlaceholderVector(newInput);
            PreviousValue.Initialized = true;
        } else {
            SignalElement::VectorType v(newInput.size());
            for (size_t i = 0; i < v.size(); ++i) {
                v[i] = (newInput[i] - PreviousValue.Vector[i]) / (newTimestamp - PreviousValue.Timestamp);
            }
            OutputSignals[0]->SetPlaceholderVector(v);
        }

        // Update local cache
        PreviousValue.Timestamp = newTimestamp;
        PreviousValue.Vector = newInput;
    }

    // This filter cannot be the last one of a pipeline (no criteria for publishing events can be specified)
    if (this->LastFilterOfPipeline) {
        SFLOG_WARNING << "FilterTrendVel: this type of filter cannot publish envets " << std::endl;
    }

    if (debug) {
        if (SignalType == SignalElement::SCALAR) {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                      << InputSignals[0]->GetPlaceholderScalar() << "(@ " << InputSignals[0]->GetTimeLastSampleFetched()
                      << ") - "
                      << PreviousValue.Scalar << "(@ " << PreviousValue.Timestamp
                      << ") => "
                      << OutputSignals[0]->GetPlaceholderScalar() << std::endl;
        } else {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": [ ";
            SignalElement::VectorType & v = InputSignals[0]->GetPlaceholderVector();
            for (size_t i = 0; i < v.size(); ++i) {
                std::cout << v[i] << " ";
            }
            std::cout << "] (@ " << InputSignals[0]->GetTimeLastSampleFetched()
                      << ") - [ ";
            v = PreviousValue.Vector;
            for (size_t i = 0; i < v.size(); ++i) {
                std::cout << v[i] << " ";
            }
            std::cout << "] (@ " << PreviousValue.Timestamp
                      << ") => [ ";
            v = OutputSignals[0]->GetPlaceholderVector();
            for (size_t i = 0; i < v.size(); ++i) {
                std::cout << v[i] << " ";
            }
            std::cout << "]" << std::endl;
        }
    }
}

const std::string FilterTrendVel::GenerateFDIJSON(double severity, double timestamp) const
{
    return "n/a";
}

void FilterTrendVel::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);

    outputStream << "----- Filter-specifics: " << std::endl
        << "Signal Type: " << (SignalType == SignalElement::SCALAR ? "SCALAR" : "VECTOR") << std::endl;
}

};
