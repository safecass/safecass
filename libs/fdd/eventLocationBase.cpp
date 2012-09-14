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

void EventLocationBase::PopulateJSONValues(::Json::Value & root) const
{
    root[NAME_PROCESS]                = ProcessName;
    root[NAME_COMPONENT]              = ComponentName;
    root[NAME_INTERFACE_PROVIDED]     = InterfaceProvidedName;
    root[NAME_INTERFACE_REQUIRED]     = InterfaceRequiredName;
}

void EventLocationBase::ToStream(std::ostream & outputStream) const
{
    outputStream << "[Proc]" << ProcessName << ":"
                 << "[Comp]" << ComponentName << ":"
                 << "[IntP]" << InterfaceProvidedName << ":"
                 << "[IntR]" << InterfaceRequiredName;
}

};
