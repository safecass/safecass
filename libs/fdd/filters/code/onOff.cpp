//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jun 30, 2014
// Last revision: Jun 30, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "onOff.h"
#include "filterFactory.h"
#include "dict.h"

using namespace SF;

SF_IMPLEMENT_FACTORY(FilterOnOff);

FilterOnOff::FilterOnOff(void)
{
    // This default constructor should not be used.
}

FilterOnOff::FilterOnOff(const FilterBase::FilterCategory category, 
                         const std::string &              targetComponentName,
                         const FilterBase::FilteringType  monitoringType,
                         const std::string &              inputSignalName)
  : FilterBase(FilterOnOff::Name, category, targetComponentName, monitoringType),
    NameOfInputSignal(inputSignalName),
    LastValue(0.0),
    Initialized(false)
{
    Initialize();
}

FilterOnOff::FilterOnOff(const JSON::JSONVALUE & jsonNode)
    : FilterBase(FilterOnOff::Name, jsonNode),
      NameOfInputSignal(JSON::GetSafeValueString(
          jsonNode[Dict::Filter::argument], Dict::Filter::input_signal)),
      LastValue(0.0),
      Initialized(false)
{
    Initialize();
}

FilterOnOff::~FilterOnOff()
{
}

void FilterOnOff::Initialize(void)
{
    FilterBase::Initialize();

    // filters that casros provides do not need this; this is only for user-defined filters.
    //SF_REGISTER_FILTER_TO_FACTORY(FilterOnOff);

    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal, SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(NameOfInputSignal,
                                       FilterOnOff::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName, SignalElement::SCALAR));
}

bool FilterOnOff::InitFilter(void)
{
    return true;
}

void FilterOnOff::CleanupFilter(void)
{
}

void FilterOnOff::RunFilter(void)
{
    if (!this->IsEnabled() || !Initialized) return;

    // Fetch new value from history buffer
    if (!InputSignals[0]->FetchNewValueScalar((this->Type == FilterBase::ACTIVE))) {
        SFLOG_ERROR << "FilterOnOff: failed to read input from history buffer" << std::endl;
        this->Enable(false); // suppress further error messages due to the same issue
        // TODO: RESOLVE THIS ISSUE: once Enable(false) is called, a filter is no longer
        // is usable.  There should be explicit call to Enable(true).
        return;
    }

    // Filtering algorithm: if the current input is different from local cache,
    // determine non-zero output.  If they are the same, output is zero.
    SignalElement::ScalarType newInput = InputSignals[0]->GetPlaceholderScalar();

    if (newInput == LastValue) {
        OutputSignals[0]->SetPlaceholderScalar(0.0);
        return;
    }

    // Set output
    OutputSignals[0]->SetPlaceholderScalar(newInput ? 1.0 : -1.0);
    // Update local cache
    LastValue = newInput;
    // TODO: revise event publisher to use Safety Coordinator instead of cisst event
    //if (LastFilterOfPipeline) {}

    // Debug log if enabled
    if (this->PrintDebugLog)
        std::cout << *this << std::endl;
}

void FilterOnOff::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);

    outputStream << "----- Filter-specifics: " << std::endl 
                 << "Signal Type    : SCALAR" << std::endl
                 << "Last input     : " << LastValue << std::endl
                 << "Current reading: " << InputSignals[0]->GetPlaceholderScalar() << std::endl;
}
