/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "filterBase.h"

namespace SF {

FilterBase::FilterIdType FilterBase::FilterUID = 0;

const FilterBase::FilterIdType InvalidFilterUID = -1;
const std::string InvalidSignalName = "INVALID_SIGNAL";

//-------------------------------------------------- 
//  Constructors and Destructor
//-------------------------------------------------- 
FilterBase::FilterBase(void)
    : UID(InvalidFilterUID),
      Category(FilterBase::INVALID),
      Name("NONAME"), 
      Enabled(false)
{}

FilterBase::FilterBase(FilterCategory category, const std::string & name)
    : UID(FilterUID++),
      Category(category),
      Name(name), 
      // MJ: if filter is enabled when constructed, the first few inputs and outputs could
      // be corrupted.
      Enabled(false)
{
}

FilterBase::~FilterBase()
{
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        delete InputSignals[i];
    }
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        delete OutputSignals[i];
    }
}

bool FilterBase::AddInputSignal(const std::string & name, SignalElement::SignalType type)
{
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        if (InputSignals[i]->GetName().compare(name) == 0) {
            SFLOG_ERROR << "AddInputSignal: failed to add input signal (duplicate name): \"" << name << "\"" << std::endl;
            return false;
        }
    }

    SignalElement * newSignal = new SignalElement(type, name);
    InputSignals.push_back(newSignal);
    SFLOG_DEBUG << "AddInputSignal: Successfully added input signal \"" << name << "\" to filter \"" << this->Name << "\"" << std::endl;

    return true;
}

bool FilterBase::AddOutputSignal(const std::string & name, SignalElement::SignalType type)
{
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        if (OutputSignals[i]->GetName().compare(name) == 0) {
            SFLOG_ERROR << "AddOutputSignal: failed to add output signal (duplicate name): \"" << name << "\"" << std::endl;
            return false;
        }
    }

    SignalElement * newSignal = new SignalElement(type, name);
    OutputSignals.push_back(newSignal);
    SFLOG_DEBUG << "AddOutputSignal: Successfully added output signal \"" << name << "\" to filter \"" << this->Name << "\"" << std::endl;

    return true;
}

std::string FilterBase::GetInputSignalName(size_t index) const
{
    if (index >= InputSignals.size()) {
        SFLOG_ERROR << "GetInputSignalName: index " << index << " out of range (total input signal count: " << InputSignals.size() << ")" << std::endl;
        return InvalidSignalName;
    }

    return InputSignals[index]->GetName();
}

std::string FilterBase::GetOutputSignalName(size_t index) const
{
    if (index >= OutputSignals.size()) {
        SFLOG_ERROR << "GetOutputSignalName: index " << index << " out of range (total output signal count: " << OutputSignals.size() << ")" << std::endl;
        return InvalidSignalName;
    }

    return OutputSignals[index]->GetName();
}

#if 0 // not used now
FilterBase::SignalNamesType FilterBase::GetInputSignalNames(void) const
{
    SignalNamesType names;
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        names.push_back(InputSignals[i]->GetName());
    }
    return names;
}

FilterBase::SignalNamesType FilterBase::GetOutputSignalNames(void) const
{
    SignalNamesType names;
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        names.push_back(OutputSignals[i]->GetName());
    }
    return names;
}
#endif

SignalElement * FilterBase::GetOutputSignalElement(size_t index) const
{
    if (index >= OutputSignals.size()) return 0;

    return OutputSignals[index];
}

SignalElement * FilterBase::GetInputSignalElement(size_t index) const
{
    if (index >= InputSignals.size()) return 0;

    return InputSignals[index];
}

};