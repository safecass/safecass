//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: Jul 15, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "gcm.h"
#include "config.h"
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsManagerLocal.h>

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

const std::string GCM::ProcessStateTransition(State::StateMachineType type,
                                              const Event *           event,
                                              const std::string &     componentName,
                                              const std::string &     interfaceName)
{
    StateMachine * sm = 0;
    bool interfaceStateChange = false;

    if (type == State::STATEMACHINE_FRAMEWORK || type == State::STATEMACHINE_APP) {
        sm = (type == State::STATEMACHINE_FRAMEWORK ? States.ComponentFramework : States.ComponentApplication);
    } else if (type == State::STATEMACHINE_PROVIDED || type == State::STATEMACHINE_REQUIRED) {
        interfaceStateChange = true;
        InterfaceStateMachinesType & interfaceStateMachines =
            (type == State::STATEMACHINE_PROVIDED ? States.ProvidedInterfaces : States.RequiredInterfaces);

        GCM::InterfaceStateMachinesType::const_iterator it;
        it = interfaceStateMachines.find(interfaceName);
        if (it == interfaceStateMachines.end()) {
            SFLOG_ERROR << "GCM::ProcessStateTransition: no interface found: \"" << interfaceName << "\"" << std::endl;
            return "";
        }
        sm = it->second;
    } else {
        SFLOG_ERROR << "GCM::ProcessStateTransition: invalid state machine type" << std::endl;
        return "";
    }

    // NOTE: Depending on event handling policy in terms of severity and criticality,
    // the logics in the following codes and StateMachine::ProcessEvent() should behave
    // accordingly.  For example, should the criticality (N/W/E) has higher priority than
    // severity (1-255), casros has to look at criticality first.  This is the current
    // design and implementation.  Should the severity does, however, severity has to be 
    // considered first.

    // Get current state
    const State::StateType currentState = sm->GetCurrentState();
    // Look for possible transitions from the current state
    const State::TransitionType transition = event->GetTransition(currentState);
    if (transition == State::INVALID_TRANSITION) {
        SFLOG_ERROR << "GCM::ProcessStateTransition: invalid transition: " 
                    << "current state: " << State::GetStringState(currentState) << ", "
                    << "transition: " << State::GetStringTransition(transition)
                    << std::endl;
        return "";
    }
    // Make transition for a single statemachine of interest. Depending on type of transition 
    // (getting worse vs. better), transition event may be ignored.
    bool transitionProcessed = sm->ProcessEvent(transition, event);
    if (!transitionProcessed) {
        SFLOG_INFO << "GCM::ProcessStateTransition: event \"" << event->GetName() << "\" is ignored" << std::endl;
        return "";
    }

    // TODO: Make subsequent transitions resulted from the transition above and propagate state 
    // changes/events to CONNECTED interfaces.  Note that the projected states do not have
    // actual statemachines and thus we don't need to propagate state transitions to S_P,
    // S_R, and S.

    // TODO: build up JSON string that only contains state changes (delta)
    return "TODO: BUILD UP JSON";
}

#if 0
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
#endif

State::StateType GCM::GetComponentState(const ComponentStateViews view) const
{
    SFASSERT(States.ComponentFramework);
    SFASSERT(States.ComponentApplication);

    State stateFrameworkView(States.ComponentFramework->GetCurrentState());
    State stateApplicationView(States.ComponentApplication->GetCurrentState());

    switch (view) {
    case GCM::FRAMEWORK_VIEW:
        return stateFrameworkView.GetState();
    case GCM::APPLICATION_VIEW:
        return stateApplicationView.GetState();
    case GCM::SYSTEM_VIEW:
        {
            State stateInterfaceProvided(GetInterfaceState(GCM::PROVIDED_INTERFACE));
            State stateInterfaceRequired(GetInterfaceState(GCM::REQUIRED_INTERFACE));
            return (((stateFrameworkView * stateApplicationView) * stateInterfaceProvided) * stateInterfaceRequired).GetState();
        }
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

State::StateType GCM::GetInterfaceState(const GCM::InterfaceTypes type) const
{
    // NOTE: Different policies on the "projected" state of two kinds of interfaces
    //
    // S_P is always in the NORMAL state unless all provided interfaces are the non-NORMAL state
    // because a change of the provided interface state does not have significance WITHIN
    // the component unless all provided interface states become non-NORMAL.
    // In contrast, any state change in any required interface affects S_R because a
    // component "requires" all of required interfaces to work properly; otherwise, the
    // component cannot operate correctly.
    const InterfaceStateMachinesType & interfaces = 
        (type == PROVIDED_INTERFACE ? States.ProvidedInterfaces : States.RequiredInterfaces);

    bool allProvidedInterfacesNonNormal = true;

    InterfaceStateMachinesType::const_iterator it = interfaces.begin();
    const InterfaceStateMachinesType::const_iterator itEnd = interfaces.end();
    if (it == itEnd)
        return State::NORMAL; // there can be no provided or required interfaces

    State finalState(it->second->GetCurrentState());
    allProvidedInterfacesNonNormal &= (it->second->GetCurrentState() != State::NORMAL);
    ++it;
    for (; it != itEnd; ++it) {
        finalState = finalState * it->second->GetCurrentState();
        if (allProvidedInterfacesNonNormal)
            allProvidedInterfacesNonNormal &= (it->second->GetCurrentState() != State::NORMAL);
    }

    if (type == REQUIRED_INTERFACE)
        return finalState.GetState();
    else {
        if (allProvidedInterfacesNonNormal) {
            return finalState.GetState();
        } else {
            return State::NORMAL;
        }
    }
}
