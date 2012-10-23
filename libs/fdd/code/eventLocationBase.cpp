/*

  Safety Framework for Component-based Robotics

  Created on: September 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "eventLocationBase.h"
#include "dict.h"

namespace SF {

using namespace Dict::Json;

void EventLocationBase::ExportToJSON(::Json::Value & root) const
{
    root[process]            = ProcessName;
    root[component]          = ComponentName;
    root[interface_provided] = InterfaceProvidedName;
    root[interface_required] = InterfaceRequiredName;
}

void EventLocationBase::ImportFromJSON(const ::Json::Value & value)
{
    ProcessName           = value.get(process, "").asString();
    ComponentName         = value.get(component, "").asString();
    InterfaceProvidedName = value.get(interface_provided, "").asString();
    InterfaceRequiredName = value.get(interface_required, "").asString();
}

void EventLocationBase::ToStream(std::ostream & outputStream) const
{
    outputStream << "[Proc]" << ProcessName << ":"
                 << "[Comp]" << ComponentName << ":"
                 << "[IntP]" << InterfaceProvidedName << ":"
                 << "[IntR]" << InterfaceRequiredName;
}

EventLocationBase & EventLocationBase::operator=(const EventLocationBase & rhs)
{
    ProcessName           = rhs.GetProcessName();
    ComponentName         = rhs.GetComponentName();
    InterfaceProvidedName = rhs.GetInterfaceProvidedName();
    InterfaceRequiredName = rhs.GetInterfaceRequiredName();
}

};