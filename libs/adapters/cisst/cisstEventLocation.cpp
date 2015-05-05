/*

  Safety Framework for Component-based Robotics

  Created on: Sep 14, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstEventLocation.h"
#include "cisstDic.h"

using namespace SC::Dict::Json;

namespace SC {

cisstEventLocation::cisstEventLocation(void)
    : EventLocationBase(),
      CommandName(""), FunctionName(""), EventGeneratorName(""), EventHandlerName("")
{}

cisstEventLocation::cisstEventLocation(const std::string & processName,
                                       const std::string & componentName,
                                       const std::string & interfaceProvidedName,
                                       const std::string & interfaceRequiredName,
                                       const std::string & commandName,
                                       const std::string & functionName,
                                       const std::string & eventGeneratorName,
                                       const std::string & eventHandlerName)
    : EventLocationBase(processName, componentName, interfaceProvidedName, interfaceRequiredName),
      CommandName(commandName), FunctionName(functionName),
      EventGeneratorName(eventGeneratorName), EventHandlerName(eventHandlerName)
{}

cisstEventLocation::~cisstEventLocation()
{}

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

const std::string cisstEventLocation::GetIDString(void) const
{
    std::stringstream ss;
    ss << EventLocationBase::GetIDString();

    ss << ":";
    if (!CommandName.empty())
        ss << "[Cm]" << CommandName << ":";
    if (!FunctionName.empty())
        ss << "[Fn]" << FunctionName << ":";
    if (!EventGeneratorName.empty())
        ss << "[Eg]" << EventGeneratorName << ":";
    if (!EventHandlerName.empty())
        ss << "[Eh]" << EventHandlerName << ":";

    // Remove trailing delimeter
    std::string s(ss.str());
    s = s.substr(0, s.size() - 1);

    return s;
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

    // FIXME
#if 0
    CommandName        = JsonWrapper::GetSafeValueString(value, cisst::command);
    FunctionName       = JsonWrapper::GetSafeValueString(value, cisst::function);
    EventGeneratorName = JsonWrapper::GetSafeValueString(value, cisst::event_generator);
    EventHandlerName   = JsonWrapper::GetSafeValueString(value, cisst::event_handler);
#endif
}

void cisstEventLocation::ToStream(std::ostream & outputStream) const
{
    EventLocationBase::ToStream(outputStream);
    //outputStream << GetIDString();
}

};
