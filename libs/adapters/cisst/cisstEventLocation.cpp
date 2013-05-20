/*

  Safety Framework for Component-based Robotics

  Created on: Sep 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstEventLocation.h"
#include "cisstDic.h"

using namespace SF::Dict::Json;

namespace SF {

cisstEventLocation::cisstEventLocation(void) : EventLocationBase()
{
}

const std::string cisstEventLocation::GetLocationID(void) const
{
    std::stringstream ss;

    ss << (this->ProcessName.empty() ? "-" : this->ProcessName) << ":";
    ss << (this->ComponentName.empty() ? "-" : this->ComponentName);
    if (!this->InterfaceProvidedName.empty() || !CommandName.empty() || !EventGeneratorName.empty()) {
        ss << ":[P]" << this->InterfaceProvidedName << ":" << CommandName;
        if (!EventGeneratorName.empty())
            ss << ":" << EventGeneratorName;
    }
    if (!this->InterfaceRequiredName.empty() || !FunctionName.empty() || !EventHandlerName.empty()) {
        ss << ":[R]" << this->InterfaceRequiredName << ":" << FunctionName;
        if (!EventHandlerName.empty())
            ss << ":" << EventHandlerName;
    }

    return ss.str();
}

void cisstEventLocation::ExportToJSON(::Json::Value & root) const
{
    EventLocationBase::ExportToJSON(root);

    root[cisst::command]         = CommandName;
    root[cisst::function]        = FunctionName;
    root[cisst::event_generator] = EventGeneratorName;
    root[cisst::event_handler]   = EventHandlerName;
}

void cisstEventLocation::ImportFromJSON(const ::Json::Value & value)
{
    EventLocationBase::ImportFromJSON(value);

    CommandName        = value.get(cisst::command, "").asString();
    FunctionName       = value.get(cisst::function, "").asString();
    EventGeneratorName = value.get(cisst::event_generator, "").asString();
    EventHandlerName   = value.get(cisst::event_handler, "").asString();
}

void cisstEventLocation::ToStream(std::ostream & outputStream) const
{
    EventLocationBase::ToStream(outputStream);

    outputStream << ":[Comm]" << CommandName << ":"
                 << "[Func]" << FunctionName << ":"
                 << "[EvtG]" << EventGeneratorName << ":"
                 << "[EvtH]" << EventHandlerName;
}

cisstEventLocation & cisstEventLocation::operator=(const cisstEventLocation& rhs)
{
    this->ProcessName           = rhs.GetProcessName();
    this->ComponentName         = rhs.GetComponentName();
    this->InterfaceProvidedName = rhs.GetInterfaceProvidedName();
    this->InterfaceRequiredName = rhs.GetInterfaceRequiredName();
    CommandName        = rhs.GetCommandName();
    FunctionName       = rhs.GetFunctionName();
    EventGeneratorName = rhs.GetEventGeneratorName();
    EventHandlerName   = rhs.GetEventHandlerName();

    return (*this);
}

};
