
//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Apr 21, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "statemachine.h"

using namespace SF::State;

namespace SF {

StateMachine::StateMachine(void)
{
    StateEventHandler * defaultEventHandler = new StateEventHandler;

    Initialize(defaultEventHandler);
}

StateMachine::StateMachine(StateEventHandler * instance)
{
    if (!instance)
        SFTHROW("StateMachine: null event handler");

    Initialize(instance);
}

void StateMachine::Initialize(StateEventHandler * instance)
{
    State.EventHandlerInstance = instance;
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

void StateMachine::ProcessEvent(const TransitionType transition)
{
    switch (transition) {
    case FAULT_DETECTION:   State.process_event(fault_detection()); break;
    case FAULT_REMOVAL:     State.process_event(fault_removal()); break;
    case FAULT_ACTIVATION:  State.process_event(fault_activation()); break;
    case ERROR_DETECTION:   State.process_event(error_detection()); break;
    case ERROR_REMOVAL:     State.process_event(error_removal()); break;
    case ERROR_PROPAGATION: State.process_event(error_propagation()); break;
    case FAILURE_DETECTION: State.process_event(failure_detection()); break;
    case FAILURE_REMOVAL:   State.process_event(failure_removal()); break;
    case FAILURE_STOP:      State.process_event(failure_stop()); break;
    default:
        return;
    }
}

StateType StateMachine::GetState(void) const
{
    switch (State.current_state()[0]) {
        case 0: return NORMAL;
        case 1: return FAULT;
        case 2: return ERROR;
        case 3: return FAILURE;
        default: return INVALID;
    }
}
 
};
