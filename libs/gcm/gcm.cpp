//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: Apr 22, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "gcm.h"
#include "config.h"
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsManagerLocal.h>

namespace SF {

using namespace SF;

GCM::GCM(void) : ComponentName(NONAME)
{
    // Default constructor should not be used
    SFASSERT(false);
}

GCM::GCM(const std::string & componentName)
    : ComponentName(componentName)
{
    States.ComponentFramework   = new StateMachine(componentName);
    States.ComponentApplication = new StateMachine(componentName);
}

GCM::~GCM(void)
{
    if (States.ComponentFramework)
        delete States.ComponentFramework;
    if (States.ComponentApplication)
        delete States.ComponentApplication;

    InterfaceStateMachinesType::iterator it;
    while (!States.RequiredInterfaces.empty()) {
        it = States.RequiredInterfaces.begin();
        States.RequiredInterfaces.erase(it);
        delete it->second;
    }
    while (!States.ProvidedInterfaces.empty()) {
        it = States.ProvidedInterfaces.begin();
        States.ProvidedInterfaces.erase(it);
        delete it->second;
    }
}

void GCM::ToStream(std::ostream & out) const
{
    out << "Owner component: " << this->ComponentName << std::endl;
    out << "Required interfaces: ";
    InterfaceStateMachinesType::const_iterator it = States.RequiredInterfaces.begin();
    for (; it != States.RequiredInterfaces.end(); ++it)
        out << "\"" << it->first << "\" ";
    out << std::endl;

    out << "Provided interfaces: ";
    it = States.ProvidedInterfaces.begin();
    for (; it != States.ProvidedInterfaces.end(); ++it)
        out << "\"" << it->first << "\" ";
    out << std::endl;
}

bool GCM::AddInterface(const std::string & name, const GCM::InterfaceTypes type)
{
    if (type == PROVIDED_INTERFACE) {
        if (States.ProvidedInterfaces.find(name) != States.ProvidedInterfaces.end()) {
#if ENABLE_UNIT_TEST
            std::cout << *this << std::endl;
#endif
            return false;
        }
        States.ProvidedInterfaces.insert(std::make_pair(name, new StateMachine(name)));
    } else {
        if (States.RequiredInterfaces.find(name) != States.RequiredInterfaces.end()) {
#if ENABLE_UNIT_TEST
            std::cout << *this << std::endl;
#endif
            return false;
        }
        States.RequiredInterfaces.insert(std::make_pair(name, new StateMachine(name)));
    }
    
    return true;
}

bool GCM::FindInterface(const std::string & name, const GCM::InterfaceTypes type) const
{
    if (type == PROVIDED_INTERFACE)
        return (States.ProvidedInterfaces.find(name) != States.ProvidedInterfaces.end());
    else
        return (States.RequiredInterfaces.find(name) != States.RequiredInterfaces.end());
}

bool GCM::RemoveInterface(const std::string & name, const GCM::InterfaceTypes type)
{
    InterfaceStateMachinesType::iterator it;
    if (type == PROVIDED_INTERFACE) {
        it = States.ProvidedInterfaces.find(name);
        if (it == States.ProvidedInterfaces.end())
            return false;
        SFASSERT(it->second);
        delete it->second;
        States.ProvidedInterfaces.erase(it);
    } else {
        it = States.RequiredInterfaces.find(name);
        if (it == States.RequiredInterfaces.end())
            return false;
        SFASSERT(it->second);
        delete it->second;
        States.RequiredInterfaces.erase(it);
    }

    return true;
}

StrVecType GCM::GetNamesOfInterfaces(InterfaceTypes type) const
{
    const InterfaceStateMachinesType & interfaces = 
        (type == PROVIDED_INTERFACE ? States.ProvidedInterfaces : States.RequiredInterfaces);

    StrVecType names;

    InterfaceStateMachinesType::const_iterator it = interfaces.begin();
    const InterfaceStateMachinesType::const_iterator itEnd = interfaces.end();
    for (; it != itEnd; ++it)
        names.push_back(it->first);

    return names;
}

void GCM::GetNamesOfInterfaces(InterfaceTypes type, StrVecType & names) const
{
    names = GetNamesOfInterfaces(type);
}
    
void GCM::ProcessEvent_ComponentFramework(const SF::State::TransitionType transition)
{
    SFASSERT(States.ComponentFramework);
    States.ComponentFramework->ProcessEvent(transition);
}

void GCM::ProcessEvent_ComponentApplication(const SF::State::TransitionType transition)
{
    SFASSERT(States.ComponentApplication);
    States.ComponentApplication->ProcessEvent(transition);
}

void GCM::ProcessEvent_Interface(const std::string & interfaceName, 
                                const GCM::InterfaceTypes interfaceType,
                                const SF::State::TransitionType transition)
{
    InterfaceStateMachinesType::iterator it;
    if (interfaceType == PROVIDED_INTERFACE) {
        it = States.ProvidedInterfaces.find(interfaceName);
        if (it == States.ProvidedInterfaces.end()) {
            SFLOG_ERROR << "Failed to handle transition event: " << State::GetString(transition) << std::endl;
            return;
        }
        it->second->ProcessEvent(transition);
    } else {
        it = States.RequiredInterfaces.find(interfaceName);
        if (it == States.RequiredInterfaces.end()) {
            SFLOG_ERROR << "Failed to handle transition event: " << State::GetString(transition) << std::endl;
            return;
        }
        it->second->ProcessEvent(transition);
    }
}

State::StateType GCM::GetComponentState(const ComponentStateViews view) const
{
    SFASSERT(States.ComponentFramework);
    SFASSERT(States.ComponentApplication);

    State stateFrameworkView(States.ComponentFramework->GetCurrentState());
    State stateApplicationView(States.ComponentApplication->GetCurrentState());

    switch (view) {
    case GCM::SYSTEM_VIEW:
        return (stateFrameworkView * stateApplicationView).GetState();
    case GCM::FRAMEWORK_VIEW:
        return stateFrameworkView.GetState();
    case GCM::APPLICATION_VIEW:
        return stateApplicationView.GetState();
    }
}

State::StateType GCM::GetInterfaceState(const std::string & name, const GCM::InterfaceTypes type) const
{
    InterfaceStateMachinesType::const_iterator it;
    if (type == PROVIDED_INTERFACE) {
        it = States.ProvidedInterfaces.find(name);
        if (it == States.ProvidedInterfaces.end())
            return State::INVALID;
        return it->second->GetCurrentState();
    } else {
        it = States.RequiredInterfaces.find(name);
        if (it == States.RequiredInterfaces.end())
            return State::INVALID;
        return it->second->GetCurrentState();
    }
}

};
