//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 6, 2014
// Last revision: May 6, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "changeDetection.h"
#include "filterFactory.h"
#include "dict.h"

using namespace SF;

const std::string FilterChangeDetection::Name = "ChangeDetection";

FilterChangeDetection::FilterChangeDetection(const FilterBase::FilterCategory category, 
                                             const std::string &              targetComponentName,
                                             const FilterBase::FilteringType  monitoringType,
                                             const std::string &              inputSignalName)
    : FilterBase(FilterChangeDetection::Name, category, targetComponentName, monitoringType),
      NameOfInputSignal(inputSignalName),
      LastValue(0.0),
      Initialized(false)
{
    SF_REGISTER_FILTER_TO_FACTORY(FilterChangeDetection);

    // Define inputs
    // TODO: this filter only supports scalar-type input/output for now.
    SFASSERT(this->AddInputSignal(inputSignalName, SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(inputSignalName,
                                       FilterChangeDetection::Name,
                                       this->UID,
                                       0));
    // TODO: this filter only supports scalar-type input/output for now.
    SFASSERT(this->AddOutputSignal(outputSignalName, SignalElement::SCALAR));
}

FilterChangeDetection::FilterChangeDetection(const JSON::JSONVALUE & jsonNode)
    : FilterBase(FilterChangeDetection::Name, jsonNode),
      NameOfInputSignal(JSON::GetSafeValueString(
          jsonNode[Dict::Filter::Arguments], Dict::Filter::InputSignalName))
{
    Initialize();
}

FilterChangeDetection::~FilterChangeDetection()
{
}

bool FilterChangeDetection::InitFilter(void)
{
    return true;
}

void FilterChangeDetection::CleanupFilter(void)
{
}

void FilterChangeDetection::RunFilter(void)
{
    if (!this->IsEnabled()) return;

    // Fetch new value from history buffer
    if (!InputSignals[0]->FetchNewValueScalar((this->Type == FilterBase::ACTIVE))) {
        SFLOG_ERROR << "FilterChangeDetection: failed to read input from history buffer" << std::endl;
        this->Enable(false); // suppress further error messages due to the same issue
        // TODO: RESOLVE THIS ISSUE: once Enable(false) is called, a filter is no longer
        // is usable.  There should be explicit call to Enable(true).
        return;
    }

    // Filtering algorithm: if the current input is different from local cache,
    // output is 1; otherwise, 0.
    SignalElement::ScalarType newInput = InputSignals[0]->GetPlaceholderScalar();
    if (Initialized)
        OutputSignals[0]->SetPlaceholderScalar( (newInput != LastValue) ? 1.0 : 0.0 );

    // Update local cache
    LastValue = newInput;

    // TODO: check if event publisher works
    //if (LastFilterOfPipeline) {}

    // Debug log if enabled
    if (this->PrintDebugLog)
        std::cout << *this << std::endl;
}

void FilterChangeDetection::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);

    outputStream << "----- Filter-specifics: " << std::endl 
                 << "Signal Type    : SCALAR" << std::endl
                 << "Last input     : " << LastValue
                 << "Current reading: " << InputSignals[0]->GetPlaceholderScalar()
                 << std::endl;
}
