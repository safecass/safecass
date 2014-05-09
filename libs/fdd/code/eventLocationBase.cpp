/*

  Safety Framework for Component-based Robotics

  Created on: September 14, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "eventLocationBase.h"
#include "dict.h"

namespace SF {

using namespace Dict::Json;

EventLocationBase::EventLocationBase(void)
    : ProcessName(""), ComponentName(""), InterfaceProvidedName(""), InterfaceRequiredName("")
{}

EventLocationBase::EventLocationBase(const std::string & processName,
                                     const std::string & componentName,
                                     const std::string & interfaceProvidedName,
                                     const std::string & interfaceRequiredName)
    : ProcessName(processName), ComponentName(componentName),
      InterfaceProvidedName(interfaceProvidedName), InterfaceRequiredName(interfaceRequiredName)
{}

EventLocationBase::~EventLocationBase()
{}

EventLocationBase & EventLocationBase::operator=(const EventLocationBase & rhs)
{
    this->ProcessName           = rhs.GetProcessName();
    this->ComponentName         = rhs.GetComponentName();
    this->InterfaceProvidedName = rhs.GetInterfaceProvidedName();
    this->InterfaceRequiredName = rhs.GetInterfaceRequiredName();

    return (*this);
}

const std::string EventLocationBase::GetIDString(void) const
{
    std::stringstream ss;

    if (!ProcessName.empty())
        ss << "[P]" << ProcessName << ":";
    if (!ComponentName.empty())
        ss << "[C]" << ComponentName << ":";
    if (!InterfaceProvidedName.empty())
        ss << "[IP]" << InterfaceProvidedName << ":";
    if (!InterfaceRequiredName.empty())
        ss << "[IR]" << InterfaceRequiredName << ":";

    // Remove trailing delimeter
    std::string s(ss.str());
    s = s.substr(0, s.size() - 1);

    return s;
}

void EventLocationBase::ExportToJSON(::Json::Value & root) const
{
    if (!ProcessName.empty())
        root[process] = ProcessName;
    if (!ComponentName.empty())
        root[component] = ComponentName;
    if (!InterfaceProvidedName.empty())
        root[interface_provided] = InterfaceProvidedName;
    if (!InterfaceRequiredName.empty())
        root[interface_required] = InterfaceRequiredName;
}

void EventLocationBase::ImportFromJSON(const ::Json::Value & value)
{
    ProcessName           = JSON::GetSafeValueString(value, process);
    ComponentName         = JSON::GetSafeValueString(value, component);
    InterfaceProvidedName = JSON::GetSafeValueString(value, interface_provided);
    InterfaceRequiredName = JSON::GetSafeValueString(value, interface_required);
}

void EventLocationBase::ToStream(std::ostream & outputStream) const
{
    outputStream << GetIDString();
}

};
