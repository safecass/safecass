//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Sep 14, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
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

void EventLocationBase::ImportFromJSON(const ::Json::Value & UNUSED(value))
{
#if 0
    ProcessName           = JsonWrapper::GetSafeValueString(value, process);
    ComponentName         = JsonWrapper::GetSafeValueString(value, component);
    InterfaceProvidedName = JsonWrapper::GetSafeValueString(value, interface_provided);
    InterfaceRequiredName = JsonWrapper::GetSafeValueString(value, interface_required);
#endif
}

void EventLocationBase::ToStream(std::ostream & outputStream) const
{
    outputStream << GetIDString();
}

};
