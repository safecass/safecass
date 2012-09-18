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

const std::string cisstEventLocation::GetLocationID(void) const
{
    std::stringstream ss;

    ss << this->ProcessName.empty() ? "-" : this->ProcessName;
    ss << ":" << this->ComponentName.empty() ? "-" : this->ComponentName;
    if (!this->InterfaceProvidedName.empty() || !CommandName.empty() || !EventGeneratorName.empty()) {
        ss << ":[P]" << this->InterfaceProvidedName << ":" << CommandName << ":" << EventGeneratorName;
    }
    if (!this->InterfaceRequiredName.empty() || !FunctionName.empty() || !EventHandlerName.empty()) {
        ss << ":[R]" << this->InterfaceRequiredName << ":" << FunctionName << ":" << EventHandlerName;
    }

    return ss.str();
}

void cisstEventLocation::PopulateJSONValues(::Json::Value & root) const
{
    EventLocationBase::PopulateJSONValues(root);

    root[cisst::command]         = CommandName;
    root[cisst::function]        = FunctionName;
    root[cisst::event_generator] = EventGeneratorName;
    root[cisst::event_handler]   = EventHandlerName;
}

void cisstEventLocation::ToStream(std::ostream & outputStream) const
{
    EventLocationBase::ToStream(outputStream);
    outputStream << ":[Comm]" << CommandName << ":"
                 << "[Func]" << FunctionName << ":"
                 << "[EvtG]" << EventGeneratorName << ":"
                 << "[EvtH]" << EventHandlerName;
}

};
