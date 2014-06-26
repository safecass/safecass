//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Jun 25, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "statemachine.h"

namespace SF {

StateMachine::StateMachine(void)
{
    StateEventHandler * defaultEventHandler = new StateEventHandler(NONAME);

    Initialize(NONAME, defaultEventHandler);
}


StateMachine::StateMachine(const std::string & ownerName)
{
    StateEventHandler * defaultEventHandler = new StateEventHandler(ownerName);

    Initialize(ownerName, defaultEventHandler);
}

StateMachine::StateMachine(StateEventHandler * eventHandler)
{
    if (!eventHandler)
        SFTHROW("StateMachine: null event handler");

    Initialize(NONAME, eventHandler);
}

StateMachine::StateMachine(const std::string & ownerName, StateEventHandler * eventHandler)
{
    if (!eventHandler)
        SFTHROW("StateMachine: null event handler");

    Initialize(ownerName, eventHandler);
}

void StateMachine::Initialize(const std::string & ownerName, StateEventHandler * eventHandler)
{
    OwnerName = ownerName;

    State.EventHandlerInstance = eventHandler;

    State.start();
}

StateMachine::~StateMachine(void)
{
    State.stop();

    if (State.EventHandlerInstance)
        delete State.EventHandlerInstance;
}

void StateMachine::SetStateEventHandler(StateEventHandler * instance)
{
    // Disable event handler temporarily
    StateEventHandler * currentHandler = State.EventHandlerInstance;
    State.EventHandlerInstance = 0;

    // MJ: This can be extended to handle multiple event handler instances, if necessary.
    if (currentHandler)
        delete currentHandler;

    State.EventHandlerInstance = instance;
}

void StateMachine::ProcessEvent(const State::TransitionType transition)
{
    switch (transition) {
    case State::NORMAL_TO_WARNING: State.process_event(evt_N2W()); break;
    case State::NORMAL_TO_ERROR:   State.process_event(evt_N2E()); break;
    case State::WARNING_TO_NORMAL: State.process_event(evt_W2N()); break;
    case State::WARNING_TO_ERROR:  State.process_event(evt_W2E()); break;
    case State::ERROR_TO_NORMAL:   State.process_event(evt_E2N()); break;
    case State::ERROR_TO_WARNING:  State.process_event(evt_E2W()); break;
    default:
        return;
    }
}

State::StateType StateMachine::GetCurrentState(void) const
{
    switch (State.current_state()[0]) {
    case 0: return State::NORMAL;
    case 1: return State::WARNING;
    case 2: return State::ERROR;
    default: return State::INVALID;
    }
}
 
#if ENABLE_UNIT_TEST
int StateMachine::GetCountEntryExit(const State::StateEntryExitType stateEntryExit) const
{
    if (State.EventHandlerInstance == 0)
        return -1;

    size_t index = static_cast<size_t>(stateEntryExit);

    return State.EventHandlerInstance->CountEntryExit[index];
}

int StateMachine::GetCountTransition(const State::TransitionType transition) const
{
    if (State.EventHandlerInstance == 0)
        return -1;

    size_t index = static_cast<size_t>(transition);

    return State.EventHandlerInstance->CountTransition[index];
}

void StateMachine::PrintCounters(void) const
{
    std::cout << GetCounterStatus() << std::endl;
}

std::string StateMachine::GetCounterStatus(void) const
{
    std::stringstream ss;
    ss << "Transition: ";
    for (int i = 0; i < State.EventHandlerInstance->CountTransition.size(); ++i)
        ss << State.EventHandlerInstance->CountTransition[i] << " | ";
    ss << std::endl;
    ss << "Entry/Exit: ";
    for (int i = 0; i < State.EventHandlerInstance->CountEntryExit.size(); ++i)
        ss << State.EventHandlerInstance->CountEntryExit[i] << " | ";

    return ss.str();
}

#endif

};
