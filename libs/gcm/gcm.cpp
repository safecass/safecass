//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: Jul 28, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "gcm.h"
#include "common.h"
#include "config.h"
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsManagerLocal.h>

using namespace SF;

GCM::GCM(void) : CoordinatorName(NONAME), ComponentName(NONAME)
{
    // Default constructor should not be used
    SFASSERT(false);
}

GCM::GCM(const std::string & coordinatorName, const std::string & componentName)
    : CoordinatorName(coordinatorName), ComponentName(componentName)
{
    States.ComponentFramework   = new StateMachine(componentName);
    States.ComponentApplication = new StateMachine(componentName);

    // Hopefully, nobody uses s_F or s_A as the name of required interface...
    ServiceStateDependencyInfo["s_F"] = new StrVecType;
    ServiceStateDependencyInfo["s_A"] = new StrVecType;
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

    ServiceStateDependencyInfoType::iterator it2;
    while (!ServiceStateDependencyInfo.empty()) {
        it2 = ServiceStateDependencyInfo.begin();
        ServiceStateDependencyInfo.erase(it2);
        delete it2->second;
    }
    while (!ServiceStateDependencyInfo2.empty()) {
        it2 = ServiceStateDependencyInfo2.begin();
        ServiceStateDependencyInfo2.erase(it2);
        delete it2->second;
    }

    ConnectionsType::iterator it3;
    while (!Connections.empty()) {
        it3 = Connections.begin();
        Connections.erase(it3);
        delete it3->second;
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

bool GCM::AddInterface(const std::string & name, GCM::InterfaceTypes type)
{
    if (type == PROVIDED_INTERFACE) {
        if (States.ProvidedInterfaces.find(name) != States.ProvidedInterfaces.end()) {
#if ENABLE_UNIT_TEST
            std::cout << *this << std::endl;
#endif
            return false;
        }
        States.ProvidedInterfaces.insert(std::make_pair(name, new StateMachine(name)));

        // add new column to service state dependency information table
        SFASSERT(ServiceStateDependencyInfo2.find(name) == ServiceStateDependencyInfo2.end());
        ServiceStateDependencyInfo2[name] = new StrVecType;

        SFLOG_DEBUG << "GCM:AddInterface: updated service state dependency table with provided interface: "
                    << "\"" << ComponentName << "\" - \"" << name << "\"" << std::endl;
    } else {
        if (States.RequiredInterfaces.find(name) != States.RequiredInterfaces.end()) {
#if ENABLE_UNIT_TEST
            std::cout << *this << std::endl;
#endif
            return false;
        }
        States.RequiredInterfaces.insert(std::make_pair(name, new StateMachine(name)));

        // add new column to service state dependency information table
        SFASSERT(ServiceStateDependencyInfo.find(name) == ServiceStateDependencyInfo.end());
        ServiceStateDependencyInfo[name] = new StrVecType;

        SFLOG_DEBUG << "GCM:AddInterface: updated service state dependency table with required interface: "
                    << "\"" << ComponentName << "\" - \"" << name << "\"" << std::endl;
    }
    
    return true;
}

bool GCM::FindInterface(const std::string & name, GCM::InterfaceTypes type) const
{
    if (type == PROVIDED_INTERFACE)
        return (States.ProvidedInterfaces.find(name) != States.ProvidedInterfaces.end());
    else
        return (States.RequiredInterfaces.find(name) != States.RequiredInterfaces.end());
}

bool GCM::RemoveInterface(const std::string & name, GCM::InterfaceTypes type)
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

StrVecType GCM::GetNamesOfInterfaces(GCM::InterfaceTypes type) const
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

State::TransitionType GCM::ProcessStateTransition(State::StateMachineType type,
                                                  const Event *           event,
                                                  const std::string &     interfaceName,
                                                  JSON::JSONVALUE &       json)
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
            return State::INVALID_TRANSITION;
        }
        sm = it->second;
    } else {
        SFLOG_ERROR << "GCM::ProcessStateTransition: invalid state machine type" << std::endl;
        return State::INVALID_TRANSITION;
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
        SFLOG_WARNING << "GCM::ProcessStateTransition: invalid transition: " 
                      << "current state: " << State::GetStringState(currentState) << ", "
                      << "transition: " << State::GetStringTransition(transition)
                      << std::endl;
        return State::INVALID_TRANSITION;
    }
    // Make transition for a single statemachine of interest. Depending on type of transition 
    // (getting worse vs. better), transition event may be ignored.
    bool transitionProcessed = sm->ProcessEvent(transition, event);
    if (!transitionProcessed) {
        SFLOG_INFO << "GCM::ProcessStateTransition: event \"" << event->GetName() << "\" is ignored" << std::endl;
        return State::INVALID_TRANSITION;
    }

    // Error propagation based on service states (for i-th component and j-th interface of
    // the component):
    // - Service state = [ s_R(i,k) * s_F(i) * s_A(i) * ] s_P(i,j) ([]: optional)
    // Service states change 1) whenever s_P(i,j) changes and 2) any of other states that
    // the provided interface state depends on change.

    // Make subsequent transitions resulted from the transition above and propagate state 
    // changes/events to connected interfaces via publishing state change messages.

    // Check if any service state changes due to this event/transition
    // A service state changes whenever a provided interface's "projected" state goes 
    // to Normal or Error.  That is,
    JSON _jsonState;
    JSON::JSONVALUE & jsonState = _jsonState.GetRoot();
    // Number of provided interface of which state is affcted by the transition
    size_t n = 0;
    // NORMAL_TO_WARNING and WARNING_TO_NORMAL don't have impact on the projected state
    // of a provided interface, and thus don't need to get propagated to external 
    // components.
    if (transition == State::NORMAL_TO_ERROR || transition == State::WARNING_TO_ERROR ||
        transition == State::ERROR_TO_NORMAL || transition == State::ERROR_TO_WARNING)
    {
        jsonState["request"] = "state_update";

        if (type == State::STATEMACHINE_PROVIDED) {
            JSON _jsonUpdate;
            JSON::JSONVALUE & jsonUpdate = _jsonUpdate.GetRoot();

            PopulateStateUpdateJSON(interfaceName, jsonUpdate);

            jsonState["update"][n++] = jsonUpdate;
        } else {
            // look up depedency table to check if there is any provided interface whose
            // service state is affected by the state change.
            StrVecType * vec;
            ServiceStateDependencyInfoType::const_iterator it;
            if (type == State::STATEMACHINE_FRAMEWORK) {
                it = ServiceStateDependencyInfo.find("s_F");
                SFASSERT(it != ServiceStateDependencyInfo.end());
                vec = it->second;
            }
            else if (type == State::STATEMACHINE_APP) {
                it = ServiceStateDependencyInfo.find("s_A");
                SFASSERT(it != ServiceStateDependencyInfo.end());
                vec = it->second;
            }
            else {
                it = ServiceStateDependencyInfo.find(interfaceName);
                SFASSERT(it != ServiceStateDependencyInfo.end());
                vec = it->second;
            }
            // iterate list of statemachine for provided interfaces and generates json object
            // to broadcast to inform the service state change due to this transition
            for (size_t i = 0; i < vec->size(); ++i) {
                const std::string prvName = vec->at(i);
                JSON _jsonUpdate;
                JSON::JSONVALUE & jsonUpdate = _jsonUpdate.GetRoot();

                PopulateStateUpdateJSON(prvName, jsonUpdate);

                jsonState["update"][n++] = jsonUpdate;
            }
        }
    }

    // There may be cases where a component has no application-level provided interface of which 
    // service state relies on its required interface(s).  In this case, the service state of
    // the provided interface does not get affected by the state change of the required
    // interface(s), and thus no error propagation is required.
    if (n == 0)
        json = JSON::JSONVALUE::null;
    else
        json = jsonState;

    return transition;
}

void GCM::PopulateStateUpdateJSON(const std::string & providedInterfaceName, JSON::JSONVALUE & json) const
{
    // information about provided interface of which state just changed
    json["source"]["safety_coordinator"] = CoordinatorName;
    json["source"]["component"] = ComponentName;
    json["source"]["interface"] = providedInterfaceName;
    // new state
    const Event * e = 0;
    State::StateType nextServiceState = GetServiceState(providedInterfaceName, e, true);
    json["state"] = static_cast<unsigned int>(nextServiceState);
    // event
    json["event"] = ((e == 0) ? JSON::JSONVALUE::null : e->SerializeJSON());
    // target
    {
        JSON _target;
        JSON::JSONVALUE & target = _target.GetRoot();
        int cntTarget = 0;

        ConnectionsType::const_iterator it = Connections.find(providedInterfaceName);
        if (it == Connections.end()) {
            // no connection
            json["target"][cntTarget] = JSON::JSONVALUE::null;
        } else {
            const ConnectionListType & vec = *(it->second);
            for (size_t i = 0; i < vec.size(); ++i) {
                target["safety_coordinator"] = vec[i].SafetyCoordinatorName;
                target["component"] = vec[i].ComponentName;
                target["interface"] = vec[i].RequiredInterfaceName;
                json["target"][cntTarget++] = target;
            }
        }
    }

    SFLOG_DEBUG << "PopulateStateUpdateJSON: " << providedInterfaceName << ", JSON:\n" << JSON::GetJSONString(json) << std::endl;
}

State::StateType GCM::GetComponentState(ComponentStateViews view) const
{
    SFASSERT(States.ComponentFramework);
    SFASSERT(States.ComponentApplication);

    State stateFramework(States.ComponentFramework->GetCurrentState());
    State stateApplication(States.ComponentApplication->GetCurrentState());

    switch (view) {
    case GCM::FRAMEWORK_VIEW:
        return stateFramework.GetState();
    case GCM::APPLICATION_VIEW:
        return stateApplication.GetState();
    case GCM::SYSTEM_VIEW:
        {
            State stateInterfaceProvided(GetInterfaceState(GCM::PROVIDED_INTERFACE));
            State stateInterfaceRequired(GetInterfaceState(GCM::REQUIRED_INTERFACE));
            return (((stateFramework* stateApplication) * stateInterfaceProvided) * stateInterfaceRequired).GetState();
        }
    }
}

State::StateType GCM::GetComponentState(ComponentStateViews view, const Event* & e) const
{
    SFASSERT(States.ComponentFramework);
    SFASSERT(States.ComponentApplication);

    const Event *eFramework = 0, *eApp = 0, *ePrv = 0, *eReq = 0;

    State stateFramework(States.ComponentFramework->GetCurrentState());
    State stateApp(States.ComponentApplication->GetCurrentState());

    eFramework = States.ComponentFramework->GetPendingEvent();
    eApp = States.ComponentApplication->GetPendingEvent();

    switch (view) {
    case GCM::FRAMEWORK_VIEW:
        e = eFramework;
        return stateFramework.GetState();
    case GCM::APPLICATION_VIEW:
        e = eApp;
        return stateApp.GetState();
    case GCM::SYSTEM_VIEW:
        {
            State statePrv(GetInterfaceState(GCM::PROVIDED_INTERFACE, ePrv));
            State stateReq(GetInterfaceState(GCM::REQUIRED_INTERFACE, eReq));

#define GET_HIGHER_SEVERITY_EVENT(_e1, _e2)\
            if (_e1 == 0 && _e2 == 0)\
                e = 0;\
            else if (_e1 == 0 && _e2)\
                e = _e2;\
            else if (_e1 && _e2 == 0)\
                e = _e1;\
            else\
                e = ((_e1->GetSeverity() > _e2->GetSeverity()) ? _e1 : _e2);

            GET_HIGHER_SEVERITY_EVENT(eFramework, eApp);
            GET_HIGHER_SEVERITY_EVENT(e, ePrv);
            GET_HIGHER_SEVERITY_EVENT(e, eReq);
#undef GET_HIGHER_SEVERITY_EVENT

            return (((stateFramework * stateApp) * statePrv) * stateReq).GetState();
        }
    }
}

State::StateType GCM::GetInterfaceState(const std::string & name, GCM::InterfaceTypes type) const
{
    InterfaceStateMachinesType::const_iterator it;
    if (type == GCM::PROVIDED_INTERFACE) {
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

State::StateType GCM::GetInterfaceState(const std::string & name, GCM::InterfaceTypes type, const Event* & e) const
{
    GCM::InterfaceStateMachinesType::const_iterator it;
    if (type == GCM::PROVIDED_INTERFACE) {
        it = States.ProvidedInterfaces.find(name);
        if (it == States.ProvidedInterfaces.end()) {
            e = 0;
            return State::INVALID;
        }
    } else {
        it = States.RequiredInterfaces.find(name);
        if (it == States.RequiredInterfaces.end()) {
            e = 0;
            return State::INVALID;
        }
    }

    e = it->second->GetPendingEvent();
    return it->second->GetCurrentState();
}

State::StateType GCM::GetInterfaceState(GCM::InterfaceTypes type, const Event* & e) const
{
    const InterfaceStateMachinesType & interfaces = 
        (type == GCM::PROVIDED_INTERFACE ? States.ProvidedInterfaces : States.RequiredInterfaces);

    if (interfaces.size() == 0) {
        e = 0;
        return State::NORMAL;
    }

    State state(State::NORMAL);
    const Event * event = 0;

    InterfaceStateMachinesType::const_iterator it = interfaces.begin();
    const InterfaceStateMachinesType::const_iterator itEnd = interfaces.end();
    for (; it != itEnd; ++it) {
        state = state * it->second->GetCurrentState();
        if (it->second->GetPendingEvent() == 0)
            continue;
        if (event == 0 || 
            event->GetSeverity() < it->second->GetPendingEvent()->GetSeverity())
            event = it->second->GetPendingEvent();
    }

    e = event;
    return state.GetState();

#if 0
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
#endif
}

State::StateType GCM::GetInterfaceState(GCM::InterfaceTypes type) const
{
    const Event * e = 0;

    return GetInterfaceState(type, e);
}

void GCM::AddServiceStateDependency(const JSON::JSONVALUE & services)
{
    std::string prvName;
    for (size_t i = 0; i < services.size(); ++i) {
        prvName = JSON::GetSafeValueString(services[i], "name");
        if (!FindInterface(prvName, PROVIDED_INTERFACE)) {
            SFLOG_ERROR << "GCM::AddServiceStateDependency: no provided interface found: "
                        << "\"" << prvName << "\"" << std::endl;
            continue;
        }

        bool dependent = JSON::GetSafeValueBool(services[i]["dependency"], "s_F");
        if (dependent)
            AddServiceStateDependencyEntry(prvName, "s_F");
        dependent = JSON::GetSafeValueBool(services[i]["dependency"], "s_A");
        if (dependent)
            AddServiceStateDependencyEntry(prvName, "s_A");

        for (size_t j = 0; j < services[i]["dependency"]["s_R"].size(); ++j) {
            const std::string reqName = services[i]["dependency"]["s_R"][j].asString();
            AddServiceStateDependencyEntry(prvName, reqName);
        }
    }

    //PrintServiceStateDependencyTable(std::cout);
}

bool GCM::AddServiceStateDependencyEntry(const std::string & providedInterfaceName,
                                         const std::string & name)
{
    // Update first table
    ServiceStateDependencyInfoType::const_iterator it = ServiceStateDependencyInfo.find(name);
    if (it == ServiceStateDependencyInfo.end()) {
        SFLOG_ERROR << "GCM::AddServiceStateDependencyEntry: No required interface \""
                    << name << "\" found" << std::endl;
        return false;
    }

    StrVecType * vec = it->second;
    SFASSERT(vec);
    //if (!vec) {
        //SFLOG_ERROR << "GCM::AddServiceStateDependencyEntry: No entry \""
                    //<< name << "\" found in service state dependency table"
                    //<< std::endl;
        //return false;
    //}

    for (size_t i = 0; i < vec->size(); ++i) {
        // check if provided interface is already in the table
        if (vec->at(i).compare(providedInterfaceName) == 0) {
            SFLOG_ERROR << "GCM::AddServiceStateDependency: Provided interface \"" 
                        << providedInterfaceName << "\" is already in service state dependency table"
                        << std::endl;
            return false;
        }
    }
    vec->push_back(providedInterfaceName);

    // Update second table
    it = ServiceStateDependencyInfo2.find(providedInterfaceName);
    if (it == ServiceStateDependencyInfo2.end()) {
        SFLOG_ERROR << "GCM::AddServiceStateDependencyEntry: No provided interface \""
                    << providedInterfaceName << "\" found" << std::endl;
        return false;
    }

    StrVecType * vec2 = it->second;
    SFASSERT(vec2);
    //if (!vec2) {
        //SFLOG_ERROR << "GCM::AddServiceStateDependencyEntry: No provided interface \""
                    //<< providedInterfaceName << "\" found in service state dependency table"
                    //<< std::endl;
        //return false;
    //}

    for (size_t i = 0; i < vec2->size(); ++i) {
        // check if entry is already in the table
        if (vec2->at(i).compare(name) == 0) {
            SFLOG_ERROR << "GCM::AddServiceStateDependency: Entry \"" 
                        << name << "\" is already in service state dependency table"
                        << std::endl;
            return false;
        }
    }
    vec2->push_back(name);

    return true;
}

void GCM::PrintServiceStateDependencyTable(std::ostream & out, const std::string & prefix)
{
    //out << "Component: \"" << ComponentName << "\"" << std::endl;
    out << prefix << "Look-up table using state:" << std::endl;
    ServiceStateDependencyInfoType::const_iterator it = ServiceStateDependencyInfo.begin();
    ServiceStateDependencyInfoType::const_iterator itEnd = ServiceStateDependencyInfo.end();
    for (; it != itEnd; ++it) {
        out << prefix << "\t" << it->first << std::endl;
        StrVecType * v = it->second;
        if (!v) {
            out << prefix << "\t\t(none)" << std::endl;
            continue;
        }
        for (size_t j = 0; j < v->size(); ++j) {
            out << prefix << "\t\t" << v->at(j) << std::endl;
        }
    }

    out << prefix << "Look-up table using provided interface:" << std::endl;
    it = ServiceStateDependencyInfo2.begin();
    itEnd = ServiceStateDependencyInfo2.end();
    for (; it != itEnd; ++it) {
        out << prefix << "\t" << it->first << std::endl;
        StrVecType * v = it->second;
        if (!v) {
            out << prefix << "\t\t(none)" << std::endl;
            continue;
        }
        for (size_t j = 0; j < v->size(); ++j) {
            out << prefix << "\t\t" << v->at(j) << std::endl;
        }
    }

}

State::StateType GCM::GetServiceState(const std::string & providedInterfaceName, const Event * & event,
                                      bool forErrorPropagation) const
{
    ServiceStateDependencyInfoType::const_iterator it = 
        ServiceStateDependencyInfo2.find(providedInterfaceName);
    if (it == ServiceStateDependencyInfo2.end()) {
        event = 0;
        SFLOG_WARNING << "GetServiceState: no provided interface found: \"" << providedInterfaceName << "\"" << std::endl;
        return State::INVALID;
    }

    StrVecType * v = it->second;
    SFASSERT(v);
    // If provided interface has no dependency on other state(s) for its service state
    if (v->size() == 0) {
        // Find provided interface instance
        InterfaceStateMachinesType::const_iterator it = States.ProvidedInterfaces.find(providedInterfaceName);
        SFASSERT(it != States.ProvidedInterfaces.end());
        event = it->second->GetPendingEvent();
        return GetInterfaceState(providedInterfaceName, PROVIDED_INTERFACE);
    }

    //const Event * e = 0; // Need to keep track of which event cuased the service state transition
    //State serviceState(State::NORMAL);
    
    // Initial state and pending event should be fetched from the provided interface
    const StateMachine * smProvided = GetStateMachineInterface(providedInterfaceName, PROVIDED_INTERFACE);
    const Event * e = smProvided->GetPendingEvent();
    State serviceState(smProvided->GetCurrentState());

    const StateMachine * sm;
    std::string name;
    for (size_t i = 0; i < v->size(); ++i) {
        name = v->at(i);
        if (name.compare("s_F") == 0) {
            sm = States.ComponentFramework;

            State oldState(serviceState);
            serviceState = serviceState * GetComponentState(FRAMEWORK_VIEW);

            // If service state changes due to framework state
            if (serviceState != oldState) {
                std::stringstream ss;
                ss << "GCM::GetServiceState (s_F): service state for \"" << providedInterfaceName << "\" changes - "
                   << State::GetStringState(oldState.GetState()) << " to "
                   << State::GetStringState(serviceState.GetState()) << ", "
                   << "Pending event changes from ";
#define GET_EVENT_STRING( _e)\
                if (_e)\
                    ss << *_e;\
                else\
                    ss << "null";
                GET_EVENT_STRING(e);
                ss << " to ";
                GET_EVENT_STRING(States.ComponentFramework->GetPendingEvent());
                ss << std::endl;

                SFLOG_DEBUG << ss.str();

                if (forErrorPropagation) {
                    e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                    SFASSERT(e);
                } else
                    e = States.ComponentFramework->GetPendingEvent();
#if 0
                if (!e) {
                    e = States.ComponentFramework->GetPendingEvent();
                } else {
                    // FIXME: clarify how to deal with this case
                    // If severity of pending event is higher than that of *this* event,
                    // should I use *this* event as the pending event? (if yes, this ignores severity semantics)
                    // (If no, this violates state change semantics; we lose info about who changed the state)
                    // Also, state change can be two types: getting worse vs. getting better
                    // ==> How should we determine which event remains the final pending event?
                    if (e->GetSeverity() > States.ComponentFramework->GetPendingEvent()->GetSeverity())
                        e = States.ComponentFramework->GetPendingEvent();
                }
#endif
            }
            // Even if service state remains the same, event of highest severity should be identified.
            else {
                if (e == 0) {
                    if (forErrorPropagation)
                        e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                    else
                        e = sm->GetPendingEvent();
                } else {
                    const Event * pending = States.ComponentFramework->GetPendingEvent();
                    if (pending) {
                        if (e->GetSeverity() < pending->GetSeverity()) {
                            SFLOG_DEBUG << "GCM::GetServiceState (s_F): service state for \"" << providedInterfaceName << "\" remains same - "
                                << State::GetStringState(oldState.GetState()) << ", "
                                << "Pending event changes from " << *e << " to " << *pending << std::endl;
                            if (forErrorPropagation) {
                                e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                                SFASSERT(e);
                            } else
                                e = sm->GetPendingEvent();
                        }
                    }
                }
            }
        }
        else if (name.compare("s_A") == 0) {
            sm = States.ComponentApplication;

            State oldState(serviceState);
            serviceState = serviceState * GetComponentState(APPLICATION_VIEW);

            // If service state changes due to application state
            if (serviceState != oldState) {
                std::stringstream ss;
                ss << "GCM::GetServiceState (s_A): service state for \"" << providedInterfaceName << "\" changes - "
                   << State::GetStringState(oldState.GetState()) << " to "
                   << State::GetStringState(serviceState.GetState()) << ", "
                   << "Pending event changes from ";
                GET_EVENT_STRING(e);
                ss << " to ";
                GET_EVENT_STRING(States.ComponentApplication->GetPendingEvent());
                ss << std::endl;

                SFLOG_DEBUG << ss.str();

                if (forErrorPropagation) {
                    e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                    SFASSERT(e);
                } else
                    e = sm->GetPendingEvent();
            }
            // Even if service state remains the same, event of highest severity should be identified.
            else {
                if (e == 0) {
                    if (forErrorPropagation)
                        e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                    else
                        e = sm->GetPendingEvent();
                } else {
                    const Event * pending = States.ComponentApplication->GetPendingEvent();
                    if (pending) {
                        if (e->GetSeverity() < pending->GetSeverity()) {
                            SFLOG_DEBUG << "GCM::GetServiceState (s_A): service state for \"" << providedInterfaceName << "\" remains same - "
                                << State::GetStringState(oldState.GetState()) << ", "
                                << "Pending event changes from " << *e << " to " << *pending
                                << std::endl;
                            if (forErrorPropagation) {
                                e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                                SFASSERT(e);
                            } else
                                e = States.ComponentApplication->GetPendingEvent();
                        }
                    }
                }
            }
        }
        else {
            State oldState(serviceState);
            const StateMachine * sm = GetStateMachineInterface(name, REQUIRED_INTERFACE);
            serviceState = serviceState * State(sm->GetCurrentState());
            if (serviceState != oldState) {
                std::stringstream ss;
                ss << "GCM::GetServiceState (s_R): service state for \"" << providedInterfaceName << "\" changes - "
                   << State::GetStringState(oldState.GetState()) << " to "
                   << State::GetStringState(serviceState.GetState()) << ", "
                   << "Pending event changes from ";
                GET_EVENT_STRING(e);
                ss << " to ";
                GET_EVENT_STRING(sm->GetPendingEvent());
                ss << std::endl;

                SFLOG_DEBUG << ss.str();

                if (forErrorPropagation) {
                    e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                    SFASSERT(e);
                } else
                    e = sm->GetPendingEvent();
            }
            // Even if service state remains the same, event of highest severity should be identified.
            else {
                if (e == 0) {
                    if (forErrorPropagation)
                        e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                    else
                        e = sm->GetPendingEvent();
                } else {
                    const Event * pending = sm->GetPendingEvent();
                    if (pending) {
                        if (e && (e->GetSeverity() < sm->GetPendingEvent()->GetSeverity())) {
                            SFLOG_DEBUG << "GCM::GetServiceState (s_R): service state for \"" << providedInterfaceName << "\" remains same - "
                                << State::GetStringState(oldState.GetState()) << ", "
                                << "Pending event changes from " << *e << " to " << *pending
                                << std::endl;
                            if (forErrorPropagation) {
                                e = (sm->IsLastTransitionToNormalState() ? sm->GetLastPendingEvent() : sm->GetPendingEvent());
                                SFASSERT(e);
                            } else
                                e = sm->GetPendingEvent();
                        }
                    }
                }
            }
        }

        // Should check all states that the provided interface depends on to get an event
        // of highest severy
        //if (serviceState.GetState() == State::ERROR)
            //break;
    }

    event = e;

    return (serviceState < State(State::ERROR) ? State::NORMAL : State::FAILURE);
}

const StateMachine * GCM::GetStateMachineComponent(GCM::ComponentStateViews view) const
{
    switch (view) {
    case GCM::FRAMEWORK_VIEW:   return States.ComponentFramework;
    case GCM::APPLICATION_VIEW: return States.ComponentApplication;
    case GCM::SYSTEM_VIEW:
    default:
        return 0;
    }
}

const StateMachine * GCM::GetStateMachineInterface(const std::string & name, GCM::InterfaceTypes type) const
{
    InterfaceStateMachinesType::const_iterator it;
    if (type == GCM::PROVIDED_INTERFACE) {
        it = States.ProvidedInterfaces.find(name);
        if (it == States.ProvidedInterfaces.end())
            return 0;
        else
            return it->second;
    } else {
        it = States.RequiredInterfaces.find(name);
        if (it == States.RequiredInterfaces.end())
            return 0;
        else
            return it->second;
    }
}

void GCM::AddConnection(const std::string & providedInterfaceName,
                        const std::string & safetyCoordinatorName,
                        const std::string & requiredComponentName,
                        const std::string & requiredInterfaceName)
{
    ConnectionsType::iterator it = Connections.find(providedInterfaceName);
    if (it == Connections.end()) {
        ConnectionListType * vec = new ConnectionListType;
        Connections[providedInterfaceName] = vec;
        it = Connections.find(providedInterfaceName);
    }

    ConnectionListType * connections = it->second;

    RequiredInterfaceInfoType req;
    req.SafetyCoordinatorName = safetyCoordinatorName;
    req.ComponentName         = requiredComponentName;
    req.RequiredInterfaceName = requiredInterfaceName;
    connections->push_back(req);

    // DEBUG
    //std::cout << "########### Connection updates: comp \"" << ComponentName << "\", "
    //          << "provided interface: \"" << providedInterfaceName << "\"" << std::endl;
    //PrintConnections(providedInterfaceName, std::cout);
}

void GCM::PrintConnections(const std::string providedInterfaceName, std::ostream & out,
                           const std::string & prefix)
{
    out << prefix << providedInterfaceName << ": ";
    ConnectionsType::const_iterator it = Connections.find(providedInterfaceName);
    if (it == Connections.end()) {
        out << std::endl;
        return;
    }
    out << std::endl;

    const ConnectionListType & vec = *(it->second);
    for (size_t i = 0; i < vec.size(); ++i) {
        out << prefix << "\t[" << (i + 1) << "] "
            << vec[i].SafetyCoordinatorName << " : "
            << vec[i].ComponentName << " : "
            << vec[i].RequiredInterfaceName
            << std::endl;
    }
}

void GCM::PrintConnections(std::ostream & out, const std::string & prefix)
{
    InterfaceStateMachinesType::const_iterator it = States.ProvidedInterfaces.begin();
    const InterfaceStateMachinesType::const_iterator itEnd = States.ProvidedInterfaces.end();
    for (; it != itEnd; ++it)
        PrintConnections(it->first, out, prefix);
}
