//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Sep 14, 2012
// Last revision: Apr 28, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// URL          : https://github.com/safecass/safecass
//
#include "common/dict.h"
#include "common/jsonwrapper.h"
#include "safecass/eventLocationBase.h"

using namespace SC;

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

void EventLocationBase::ExportToJSON(::Json::Value & json) const
{
    if (!ProcessName.empty())
        json[Dict::PROCESS] = ProcessName;
    if (!ComponentName.empty())
        json[Dict::COMPONENT] = ComponentName;
    if (!InterfaceProvidedName.empty())
        json[Dict::INTERFACE_PROVIDED] = InterfaceProvidedName;
    if (!InterfaceRequiredName.empty())
        json[Dict::INTERFACE_REQUIRED] = InterfaceRequiredName;
}

void EventLocationBase::ImportFromJSON(const Json::Value & json)
{
    ProcessName           = json[Dict::PROCESS].asString();
    ComponentName         = json[Dict::COMPONENT].asString();
    InterfaceProvidedName = json[Dict::INTERFACE_PROVIDED].asString();
    InterfaceRequiredName = json[Dict::INTERFACE_REQUIRED].asString();
}

void EventLocationBase::ToStream(std::ostream & os) const
{
    os << GetIDString();
}
