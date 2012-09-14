/*

  Safety Framework for Component-based Robotics

  Created on: Sep 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstEventLocation.h"

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

void cisstEventLocation::ToStream(std::ostream & outputStream) const
{
    outputStream << "Process: " << this->ProcessName << ", "
                 << "Component: " << this->ComponentName << ", "
                 << "InterfaceProvided: " << this->InterfaceProvidedName << ", "
                 << "InterfaceRequired: " << this->InterfaceRequiredName << ", "
                 << "Command: " << CommandName << ", "
                 << "Function: " << FunctionName << ", "
                 << "EventGenerator: " << EventGeneratorName << ", "
                 << "EventHandler: " << EventHandlerName;
}

};
