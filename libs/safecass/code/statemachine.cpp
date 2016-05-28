//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: May 21, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "safecass/statemachine.h"

#include "common/common.h"
#include "common/utils.h"

using namespace SC;

StateMachine::StateMachine(void): OwnerName(NONAME)
{
    SetStateEventHandler(0);

    // Explicitly start FSM
    // (ref: http://www.boost.org/doc/libs/1_60_0/libs/msm/doc/HTML/ch03s05.html#backend-start)
    FSM.start();
}

StateMachine::StateMachine(const std::string & ownerName, StateEventHandler * eventHandler)
    : OwnerName(ownerName)
{
    SetStateEventHandler(eventHandler);

    // Explicitly start FSM
    FSM.start();
}

StateMachine::~StateMachine(void)
{
    // Make sure last exit action be called
    FSM.stop();

    delete FSM.EventHandlerInstance;
}

void StateMachine::SetStateEventHandler(StateEventHandler * instance)
{
    // Disable event handler temporarily
    StateEventHandler * currentHandler = FSM.EventHandlerInstance;
    FSM.EventHandlerInstance = 0;

    if (!currentHandler) {
        if (!instance)
            return;
        else {
            FSM.EventHandlerInstance = instance;
            SCLOG_DEBUG << "Setting event handler (owner: \"" << instance->GetOwnerName() << "\")" << std::endl;
            return;
        }
    } else {
        if (!instance) {
            FSM.EventHandlerInstance = 0;
            SCLOG_DEBUG << "Disabled event handler (owner: \"" << currentHandler->GetOwnerName() << "\")" << std::endl;
            delete currentHandler;
            return;
        } else {
            FSM.EventHandlerInstance = instance;
            SCLOG_DEBUG << "Replaced event handler: owner from \"" << currentHandler->GetOwnerName() << "\" to \""
                        << instance->GetOwnerName() << "\"" << std::endl;
            delete currentHandler;
            return;
        }
    }
}

bool StateMachine::ProcessEvent(Event & event)
{
    SCLOG_DEBUG << "ProcessEvent: " << event << std::endl;

    // Should this event be ignored due to lower severity?
    bool ignore = true;

    // Handle event if no oustanding event exists
    if (!OutstandingEvent.IsActive()) {
        ignore = false;
        SCLOG_DEBUG << "ProcessEvent: No outstanding event, process event" << std::endl;
    } else {
        // Handle event if severity of the latest event is equal to or greater than that
        // of current outstanding event.
        if (event.GetSeverity() >= OutstandingEvent.GetSeverity()) {
            ignore = false;
            SCLOG_DEBUG << "ProcessEvent: New event with equal or higher severity (" << event.GetSeverity()
                        << " >= " << OutstandingEvent.GetSeverity() << ")" << std::endl;
        }
    }

    if (ignore) {
        SCLOG_WARNING << "ProcessEvent: event is ignored" << std::endl;

        // Keep history of all events even if event is ignored
        Event e = event;
        e.SetIgnored(true);
        PushTransitionHistory(e, State::INVALID);

        return false;
    }

    // Remember last outstanding event
    LastOutstandingEvent = OutstandingEvent;

    State::StateType currentState = GetCurrentState();

    State::TransitionType transition = event.GetStateTransition(currentState);
    if (transition == State::INVALID_TRANSITION) {
        SCLOG_ERROR << "ProcessEvent: Invalid transition, current state: " << currentState << std::endl;
        return false;
    }

    // Update timestamp of the event
    event.SetTimestamp(GetCurrentTimestamp());

    // Actual state transition
    switch (transition) {
    case State::NORMAL_TO_WARNING: FSM.process_event(evt_N2W()); break;
    case State::NORMAL_TO_ERROR:   FSM.process_event(evt_N2E()); break;
    case State::WARNING_TO_ERROR:  FSM.process_event(evt_W2E()); break;
    case State::WARNING_TO_NORMAL: FSM.process_event(evt_W2N()); break;
    case State::ERROR_TO_WARNING:  FSM.process_event(evt_E2W()); break;
    case State::ERROR_TO_NORMAL:   FSM.process_event(evt_E2N()); break;
    default:
        SCLOG_ERROR << "ProcessEvent: Invalid transition, current state: " << currentState
                    << ", transition: " << State::GetString(transition) << std::endl;
        return false;
    }

    if (State::NORMAL == GetCurrentState())
        OutstandingEvent.SetActive(false);
    else {
        OutstandingEvent = event;
        OutstandingEvent.SetActive(true);
        OutstandingEvent.SetIgnored(false);
    }

    PushTransitionHistory(OutstandingEvent, GetCurrentState());

    return true;
}

State::StateType StateMachine::GetCurrentState(void) const
{
    switch (FSM.current_state()[0]) {
    case 0: return State::NORMAL;
    case 1: return State::WARNING;
    case 2: return State::ERROR;
    default: return State::INVALID;
    }
}

void StateMachine::Reset(bool resetHistory)
{
    // Make sure last exit action be called
    FSM.stop();

    // FIXME How to reset state machine??
    //Initialize();

    if (resetHistory)
        TransitionHistory.clear();
}

// time (in second) to represent standalone events
#define DEFAULT_WIDTH 0.1
//#define STATE_HISTORY_DEBUG // MJTEMP
void StateMachine::GetStateTransitionHistory(Json::Value & json, unsigned int stateMachineId)
{
    TransitionHistoryType::const_iterator it = TransitionHistory.begin();
    const TransitionHistoryType::const_iterator itEnd = TransitionHistory.end();

    /*
                                                  Now 
        Time:  -----------------------------------|--------------

                              prevEvt             currEvt 
        Event: ---------------|-------------------|--------------

                                    currState        nextState
        State: ---------------|-------------------|--------------
    */
    const Event *currEvt = 0, *prevEvt = 0;
    State::StateType currState = State::NORMAL, nextState;
    for (; it != itEnd; ++it) {
        currEvt = &(it->e);

#if STATE_HISTORY_DEBUG
        std::cout << __LINE__ << " ----------- currEvt: " << *currEvt << " ====> " << State::GetStringState(it->state) << std::endl;
        if (prevEvt)
            std::cout << __LINE__ << "             prevEvt: " << *prevEvt << std::endl;
        else
            std::cout << __LINE__ << "             prevEvt: NULL\n";
#endif

        JsonWrapper _entry;
        Json::Value & entry = _entry.GetJsonRoot();

        // numeric id of state machine
        entry["state"] = stateMachineId;

        // Events ignored show up as events of 0.1 second's duration
        if (currEvt->IsIgnored()) {
            entry["name"] = currEvt->GetName();
            entry["desc"] = currEvt->GetWhat();
            entry["class"] = "ignored";
            entry["start"] = GetUTCTimeString(currEvt->GetTimestamp());
            // FIXME
            entry["end"] = "FIXME"; //GetUTCTimeString(currEvt->GetTimestamp() + DEFAULT_WIDTH);
            json["events"].append(entry);
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": ignored\n";
#endif
        } else {
            nextState = it->state;
            // If no prior event (outstanding event) exists
            if (prevEvt == 0) {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": no prior event\n";
#endif
                if (nextState == State::NORMAL) {
                    // This should not happen
                    // MJTEMP: add debug event
                    // FIXME entry["end"] = GetUTCTimeString(currEvt->GetTimestamp() + DEFAULT_WIDTH);
#define ADD_DEBUG_EVENT \
                    entry["name"] = currEvt->GetName();\
                    entry["desc"] = currEvt->GetWhat();\
                    entry["class"] = "debug";\
                    entry["start"] = GetUTCTimeString(currEvt->GetTimestamp());\
                    entry["end"] = "FIXME";\
                    json["events"].append(entry);

                    ADD_DEBUG_EVENT;
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": should not happen\n";
#endif
                } else {
                    // The very first onset event.  At this moment, we don't
                    // know yet when the completion event occurs, and thus we do
                    // not add this event to the timeline (yet).
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": NOP\n";
#endif
                }
            }
            // If outstsanding event exists
            else {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": outstanding event exists: " << *prevEvt << "\n";
#endif
                // Update currState
                if (it != TransitionHistory.begin()) {
                    --it;
                    currState = it->state;
                    ++it;
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": previous state: " << State::GetStringState(currState) << "\n";
#endif
                }

                // Remains in the same state
                if (currState == nextState) {
                    // This should not happen
                    if (currState == State::NORMAL) {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": should not happen\n";
#endif
                        ADD_DEBUG_EVENT;
                    }
                    // Handles event prioritization
                    else {
                        // New event of equal or higher severity overrides current outstanding event
                        if (prevEvt->GetSeverity() <= currEvt->GetSeverity()) {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": oustanding event overridden\n";
#endif
                            // Add current outstanding event to timeline
                            entry["name"] = prevEvt->GetName();
                            entry["desc"] = prevEvt->GetWhat();
                            entry["class"] = "ERROR-OVERRIDE";//State::GetStringState(currState);
                            entry["start"] = GetUTCTimeString(prevEvt->GetTimestamp());
                            entry["end"] = GetUTCTimeString(currEvt->GetTimestamp());
                        }
                        // New event of lower severity is ignored
                        else {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": new event ignored\n";
#endif
                            entry["name"] = currEvt->GetName();
                            entry["desc"] = currEvt->GetWhat();
                            entry["class"] = "ignored";
                            entry["start"] = GetUTCTimeString(currEvt->GetTimestamp());
                            // FIXME
                            //entry["end"] = GetUTCTimeString(currEvt->GetTimestamp() + DEFAULT_WIDTH);
                            entry["end"] = "FIXME";
                        }
                        json["events"].append(entry);
                    }
                }
                // completion event ("getting better") or onset event ("getting worse")
                else {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": onset or completion event\n";
#endif
                    // Add previous event to timeline
                    entry["name"] = prevEvt->GetName();
                    entry["desc"] = prevEvt->GetWhat();
                    entry["class"] = State::GetString(currState);
                    entry["start"] = GetUTCTimeString(prevEvt->GetTimestamp());
                    entry["end"] = GetUTCTimeString(currEvt->GetTimestamp());

                    // completion event ("getting better")
                    if (currState > nextState) {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": getting better\n";
#endif
                        json["events"].append(entry);
                    }
                    // onset event ("getting worse")
                    else {
                        SCASSERT(currState < nextState);
                        if (currState == State::WARNING) {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": warning\n";
#endif
                            json["events"].append(entry);
                        } else {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": CHECK THIS OUT!!!\n";
#endif
                        }
                    }
                }
            }
        }

        prevEvt = currEvt;
    }
}


void StateMachine::ToStream(std::ostream & os) const
{
    os << "Owner: " << OwnerName << std::endl
       << "Current state: " << State::GetString(GetCurrentState()) << std::endl
       << "Event handler: " << GetStateEventHandler() << std::endl
       << "Outstanding event: " << OutstandingEvent << std::endl
       << "Last outstanding event: " << LastOutstandingEvent << std::endl;

    os << "Transition history: ";
    if (TransitionHistory.empty()) {
        os << "(empty)" << std::endl;
        return;
    } else {
        os << " total " << TransitionHistory.size() << " transition(s)" << std::endl;

        TransitionHistoryType::const_iterator it = TransitionHistory.begin();
        TransitionHistoryType::const_iterator itEnd = TransitionHistory.end();
        for (; it != itEnd; ++it)
            os << State::GetString(it->state) << " : " << it->e << std::endl;
    }
}

void StateMachine::PushTransitionHistory(const Event & event, State::StateType newState)
{
    StateTransitionEntry entry;
    entry.e     = event;
    entry.state = newState;

    TransitionHistory.push_back(entry);
}
