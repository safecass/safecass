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

const std::string cisstEventLocation::GetIDString(void) const
{
    std::stringstream ss;
    ss << *this;
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
    std::stringstream ss;
    EventLocationBase::ToStream(ss);

    if (!CommandName.empty())
        ss << "[Cm]" << CommandName << ":";
    if (!FunctionName.empty())
        ss << "[Fn]" << FunctionName << ":";
    if (!EventGeneratorName.empty())
        ss << "[Eg]" << EventGeneratorName << ":";
    if (!EventHandlerName.empty())
        ss << "[Eh]" << EventHandlerName << ":";

    // Remove trailing delimeter
    outputStream << ss.str().erase(ss.str().size() - 1, 1);
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
