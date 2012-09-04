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
                                 // below are filter-specific arguments
                                 const std::string &           inputSignalName,
                                 SignalElement::ScalarType     threshold,
                                 SignalElement::ScalarType     output0,
                                 SignalElement::ScalarType     output1)
    : FilterBase(FilterThreshold::Name, category, targetComponentName, monitoringType),
      NameOfInputSignal(inputSignalName),
      Threshold(threshold),
      Output0(output0),
      Output1(output1)
{
    // Define inputs
    SFASSERT(this->AddInputSignal(NameOfInputSignal,
                                  SignalElement::SCALAR,
                                  monitoringType));

    // Define outputs
    const std::string outputSignalName(
        this->GenerateOutputSignalName(inputSignalName,
                                       FilterThreshold::Name,
                                       this->UID,
                                       0));
    SFASSERT(this->AddOutputSignal(outputSignalName,
                                   SignalElement::SCALAR,
                                   monitoringType));
}

FilterThreshold::~FilterThreshold()
{
}

#if 0
const std::string FilterThreshold::GenerateJSON(void) const
{
    ::Json::Value root;
    cisstTargetID * targetID = dynamic_cast<cisstTargetID*>(TargetID);

    root[NAME] = this->GetUIDAsString();

    // Monitor target type
    {   ::Json::Value _root;
        _root[TYPE] = Monitor::GetTargetTypeString(Target);

        { ::Json::Value __root;
          __root[NAME_PROCESS]                = targetID->ProcessName;
          __root[NAME_COMPONENT]              = targetID->ComponentName;
          __root[NAME_INTERFACE_PROVIDED]     = targetID->InterfaceProvidedName;
          __root[NAME_INTERFACE_REQUIRED]     = targetID->InterfaceRequiredName;
          __root[cisst::NAME_COMMAND]         = targetID->CommandName;
          __root[cisst::NAME_FUNCTION]        = targetID->FunctionName;
          __root[cisst::NAME_EVENT_GENERATOR] = targetID->EventGeneratorName;
          __root[cisst::NAME_EVENT_HANDLER]   = targetID->EventHandlerName;
          _root[IDENTIFIER] = __root;
        }
        root[TARGET] = _root;
    }

    // Monitor behaviors
    {   ::Json::Value _root;
        _root[TYPE] = Monitor::GetOutputTypeString(Output);
        {   ::Json::Value __root;
            {
                __root[STATE] = Monitor::GetStateTypeString(this->State);
                if (Output == OUTPUT_STREAM) {
                    __root[SAMPLING_RATE] = SamplingRate;
                } else if (Output == OUTPUT_EVENT) {
#if 0 // MJ TEMP: not yet decided how to use these fields
                    ::Json::Value array(::Json::arrayValue);
                    array.append(0.1);
                    array.append(0.5);
                    array.append(2.5);
                    //__root[THRESHOLD] = 3;
                    __root[THRESHOLD] = array;
                    __root[MARGIN] = 4;
#endif
                }
            }
            _root[CONFIG] = __root;
        }

#if 0 // MJ: where to publish is determined by Ice configuration files
        {   ::Json::Value __root;
            {
                ::Json::Value array(::Json::arrayValue);
                for (size_t i = 0; i < AddressesToPublish.size(); ++i)
                    array.append(AddressesToPublish[i]);
                __root[PUBLISH] = array;
            }
            _root[TARGET] = __root;
        }
        root[OUTPUT] = _root;
#endif
    }

    std::stringstream ss;
    ss << root;

    return ss.str();
}
#endif

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
