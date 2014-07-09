//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 6, 2014
// Last revision: Jul 8, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "changeDetection.h"
#include "filterFactory.h"
#include "dict.h"

using namespace SF;

SF_IMPLEMENT_FACTORY(FilterChangeDetection);

FilterChangeDetection::FilterChangeDetection(void)
{
    // This default constructor should not be used.
    Initialize();
}

FilterChangeDetection::FilterChangeDetection(const std::string &              targetComponentName,
                                             const FilterBase::FilteringType  monitoringType,
                                             const std::string &              inputSignalName)
    : FilterBase(FilterChangeDetection::Name, targetComponentName, monitoringType),
      NameOfInputSignal(inputSignalName),
      LastValue(0.0)
{
    Initialize();
}

FilterChangeDetection::FilterChangeDetection(const JSON::JSONVALUE & jsonNode)
    : FilterBase(FilterChangeDetection::Name, jsonNode),
      NameOfInputSignal(JSON::GetSafeValueString(
          jsonNode[Dict::Filter::argument], Dict::Filter::input_signal)),
      LastValue(0.0)
{
    Initialize();
}

FilterChangeDetection::~FilterChangeDetection()
{
}

void FilterChangeDetection::Initialize(void)
{
    // filters that casros provides do not need this; this is only for user-defined filters.
    //SF_REGISTER_FILTER_TO_FACTORY(FilterChangeDetection);

    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal, SignalElement::SCALAR));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(NameOfInputSignal,
                                       FilterChangeDetection::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName, SignalElement::SCALAR));
}

bool FilterChangeDetection::ConfigureFilter(const JSON::JSONVALUE & jsonNode)
{
    EventName = JSON::GetSafeValueString(jsonNode["argument"], "event");

    return true;
}

bool FilterChangeDetection::InitFilter(void)
{
    this->Initialized = true;

    return true;
}

void FilterChangeDetection::CleanupFilter(void)
{
}

void FilterChangeDetection::RunFilter(void)
{
    if (!FilterBase::RefreshSamples())
        return;

    // Debug log if enabled
    if (this->PrintDebugLog)
        std::cout << *this << std::endl;

    // Filtering algorithm: if the current input is different from local cache,
    // output is 1; otherwise, 0.
    SignalElement::ScalarType newInput = InputSignals[0]->GetPlaceholderScalar();
    if (Initialized)
        OutputSignals[0]->SetPlaceholderScalar( (newInput != LastValue) ? 1.0 : 0.0 );

    // Update local cache
    LastValue = newInput;

    // TODO: check if event publisher works
    //if (LastFilterOfPipeline) {}
}

void FilterChangeDetection::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);

    outputStream << "----- Filter-specifics: " << std::endl 
                 << "Signal Type    : SCALAR" << std::endl
                 << "Last input     : " << LastValue << std::endl
                 << "Next reading   : " << InputSignals[0]->GetPlaceholderScalar() << std::endl;
}
