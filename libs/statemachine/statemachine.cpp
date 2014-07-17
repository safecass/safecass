//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Jul 16, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "statemachine.h"
#include "event.h"

using namespace SF;

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
    PendingEvent = 0;
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

/*
void StateMachine::SetPendingEvent(Event * e)
{
    PendingEvent = e;

    SFLOG_INFO << "StateMachine::SetPendingEvent: set pending event: " << *e << std::endl;
}

void StateMachine::ClearPendingEvent(void)
{
    if (!PendingEvent)
        return;

    SFLOG_INFO << "StateMachine::SetPendingEvent: reset pending event: " << *PendingEvent << std::endl;

    PendingEvent = 0;
}
*/

bool StateMachine::ProcessEvent(const State::TransitionType transition, const Event * event)
{
    SFASSERT(event);

    // Check transition type: getting worse or getting better
    bool gettingWorse;

    // This is important because only one event can be active and initiate state transition, 
    // and thus a new event may be ignored if the severity of the new event (1-255) or the
    // criticality of it (N, W, E) is lower than those of the currently active event.
    // If the new event is ignored, this function returns false, which prevents further event
    // propagation.
    //
    // Note that different severity or criticality criteria are applied depending on the
    // transition type.

    State::StateType currentState, nextState;

    switch (transition) {
    case State::NORMAL_TO_WARNING:
        currentState = State::NORMAL;
        nextState = State::WARNING;
        gettingWorse = true;
        break;

    case State::NORMAL_TO_ERROR:
        currentState = State::NORMAL;
        nextState = State::ERROR;
        gettingWorse = true;
        break;

    case State::WARNING_TO_ERROR:
        currentState = State::WARNING;
        nextState = State::ERROR;
        gettingWorse = true;
        break;

    case State::WARNING_TO_NORMAL:
        currentState = State::WARNING;
        nextState = State::NORMAL;
        gettingWorse = false;
        break;

    case State::ERROR_TO_WARNING:
        currentState = State::ERROR;
        nextState = State::WARNING;
        gettingWorse = false;
        break;

    case State::ERROR_TO_NORMAL:
        currentState = State::ERROR;
        nextState = State::NORMAL;
        gettingWorse = false;
        break;
    default:
        return false;
    }

    // Determine if a new event should be processed or ignored. This implements the error
    // propagation strategy or policy for a single statemachine.  The error propagation
    // among multiple statemachines are handled by Coordinator.

    // Design rationale: state transition policy: severity vs. criticality
    // 1) Getting "worse" scenarios
    //    Criticality first -> Severity later
    //    : More sensitive to state transition, more emphasis on early detection and 
    //      prognostic state transition
    //
    // 2) Getting "better" scenarios
    //    Severity first -> Criticality later
    //    : More emphasis on pending event that originally caused state transition
    //      (e.g., FORCE_SAT caused state transition N2E, FORCE_NEAR_SAT restores
    //      the state from E2N?)
    bool ignore = true;
    if (PendingEvent == 0)
        ignore = false;
    else {
        SFLOG_DEBUG << "PENDING EVENT: " << *PendingEvent << std::endl;
        SFLOG_DEBUG << "NEW EVENT: " << *event << std::endl;
        // Getting worse case
        if (gettingWorse) {
            // Check criticality
            if (currentState < nextState)
                ignore = false;
            else if (currentState == nextState) {
                // Check severity
                if (PendingEvent->GetSeverity() < event->GetSeverity())
                    ignore = false;
            }
        }
        // Getting better case
        else {
#if 0 // Criticality first
            // Check criticality
            if (currentState > nextState)
                ignore = false;
            else if (currentState == nextState) {
                // Check severity
                if (PendingEvent->GetSeverity() > event->GetSeverity())
                    ignore = false;
            }
#endif
            // Check severity
            if (PendingEvent->GetSeverity() <= event->GetSeverity()) {
                // Check criticality
                if (currentState > nextState)
                    ignore = false;
            }
        }
    }

    if (ignore) {
        SFLOG_WARNING << "StateMachine::ProcessEvent: event \"" << *event << "\" is ignored" << std::endl;
        return false;
    }

    std::stringstream ss;
    ss << "StateMachine::ProcessEvent: event \"" << event->GetName() << "\" caused state transition "
       << "from " << State::GetStringState(currentState) << " to " << State::GetStringState(nextState);
    if (PendingEvent)
        ss << " and " << "active event was replaced from [ " << *PendingEvent << " ] to [ " << *event << " ].";
    else
        ss << " and new active event is installed as [ " << *event << " ].";
    SFLOG_DEBUG << ss.str() << std::endl;

    // Swap out currently active event with new event of higher criticality or equal
    // criticality but higher severity.
    if (nextState == State::NORMAL)
        PendingEvent = 0;
    else
        PendingEvent = event;

    switch (transition) {
    case State::NORMAL_TO_WARNING: State.process_event(evt_N2W()); break;
    case State::NORMAL_TO_ERROR:   State.process_event(evt_N2E()); break;
    case State::WARNING_TO_ERROR:  State.process_event(evt_W2E()); break;
    case State::WARNING_TO_NORMAL: State.process_event(evt_W2N()); break;
    case State::ERROR_TO_WARNING:  State.process_event(evt_E2W()); break;
    case State::ERROR_TO_NORMAL:   State.process_event(evt_E2N()); break;
    default:
        return false;
    }

    return true;
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
