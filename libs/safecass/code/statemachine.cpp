//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#include "statemachine.h"
#include "event.h"

using namespace SC;

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
        SCTHROW("StateMachine: null event handler");

    Initialize(NONAME, eventHandler);
}

StateMachine::StateMachine(const std::string & ownerName, StateEventHandler * eventHandler)
{
    if (!eventHandler)
        SCTHROW("StateMachine: null event handler");

    Initialize(ownerName, eventHandler);
}

void StateMachine::Initialize(void)
{
    OutstandingEvent = new Event();
    LastOutstandingEvent = new Event();

    State.start();
}

void StateMachine::Initialize(const std::string & ownerName, StateEventHandler * eventHandler)
{
    OwnerName = ownerName;
    State.EventHandlerInstance = eventHandler;

    Initialize();
}

StateMachine::~StateMachine(void)
{
    State.stop();

    if (State.EventHandlerInstance)
        delete State.EventHandlerInstance;

    delete OutstandingEvent;
    delete LastOutstandingEvent;
}

void StateMachine::SetStateEventHandler(StateEventHandler * instance)
{
    std::stringstream ss;
    ss << "StateMachine::SetStateEventHandler: event handler is being replaced: owner \""
       << State.EventHandlerInstance->GetOwnerName() << "\" to ";

    // Disable event handler temporarily
    StateEventHandler * currentHandler = State.EventHandlerInstance;
    State.EventHandlerInstance = 0;

    // MJ: This can be extended to handle multiple event handler instances, if necessary.
    if (currentHandler)
        delete currentHandler;

    State.EventHandlerInstance = instance;

    ss << "\"" << State.EventHandlerInstance->GetOwnerName() << "\"" << std::endl;

    SCLOG_DEBUG << ss.str();
}

bool StateMachine::ProcessEvent(const State::TransitionType transition, const Event * event)
{
    SCASSERT(event);

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
    if (!OutstandingEvent->GetValid())
        ignore = false;
    else {
        SCLOG_DEBUG << "OUTSTANDING EVENT: " << *OutstandingEvent << std::endl;
        SCLOG_DEBUG << "NEW EVENT: " << *event << std::endl;
        // Getting worse case
        if (gettingWorse) {
            // Check criticality
            if (currentState < nextState)
                ignore = false;
            else if (currentState == nextState) {
                // Check severity
                if (OutstandingEvent->GetSeverity() < event->GetSeverity())
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
                if (OutstandingEvent->GetSeverity() > event->GetSeverity())
                    ignore = false;
            }
#endif
            // Check severity
            if (OutstandingEvent->GetSeverity() <= event->GetSeverity()) {
                // Check criticality
                if (currentState > nextState)
                    ignore = false;
            }
        }
    }

    if (ignore) {
        SCLOG_WARNING << "StateMachine::ProcessEvent: event \"" << *event << "\" is ignored" << std::endl;

        StateTransitionEntry entry;
        entry.Evt = new Event(*event);
        entry.Evt->SetIgnored();
        entry.NewState = State::INVALID;
        StateHistory.push_back(entry);

        return false;
    }

    std::stringstream ss;
    ss << "StateMachine::ProcessEvent: event \"" << event->GetName() << "\" caused state transition "
       << "from " << State::GetStringState(currentState) << " to " << State::GetStringState(nextState);
    if (OutstandingEvent->GetValid()) {
        ss << " and " << "active event was replaced from [ " << *OutstandingEvent << " ] to [ " << *event << " ].";
    }
    else {
        ss << " and new active event is installed as [ " << *event << " ].";
    }
    SCLOG_DEBUG << ss.str() << std::endl;

    // Swap out currently active event with new event of higher criticality or equal
    // criticality but higher severity.
    //LastOutstandingEvent->CopyFrom(OutstandingEvent);
    *LastOutstandingEvent = *OutstandingEvent;
    if (nextState == State::NORMAL)
        OutstandingEvent->SetValid(false);
    else {
        //OutstandingEvent->CopyFrom(event);
        *OutstandingEvent = *event;
        OutstandingEvent->SetValid(true);
    }

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

    StateTransitionEntry entry;
    entry.Evt = new Event(*event);
    entry.NewState = nextState;
    StateHistory.push_back(entry);

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
 
void StateMachine::Reset(bool resetHistory)
{
    State.stop();

    Initialize();

    if (resetHistory) {
        StateHistoryType::const_iterator it = StateHistory.begin();
        const StateHistoryType::const_iterator itEnd = StateHistory.end();
        for (; it != itEnd; ++it)
            delete it->Evt;

        StateHistory.clear();
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
    for (size_t i = 0; i < State.EventHandlerInstance->CountTransition.size(); ++i)
        ss << State.EventHandlerInstance->CountTransition[i] << " | ";
    ss << std::endl;
    ss << "Entry/Exit: ";
    for (size_t i = 0; i < State.EventHandlerInstance->CountEntryExit.size(); ++i)
        ss << State.EventHandlerInstance->CountEntryExit[i] << " | ";

    return ss.str();
}
#endif

// time (in second) to represent standalone events
#define DEFAULT_WIDTH 0.1
//#define STATE_HISTORY_DEBUG // MJTEMP
void StateMachine::GetStateTransitionHistory(JsonWrapper::JsonValue & json, unsigned int stateMachineId)
{
    StateHistoryType::const_iterator it = StateHistory.begin();
    const StateHistoryType::const_iterator itEnd = StateHistory.end();

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
        currEvt = it->Evt;

#if STATE_HISTORY_DEBUG
        std::cout << __LINE__ << " ----------- currEvt: " << *currEvt << " ====> " << State::GetStringState(it->NewState) << std::endl;
        if (prevEvt)
            std::cout << __LINE__ << "             prevEvt: " << *prevEvt << std::endl;
        else
            std::cout << __LINE__ << "             prevEvt: NULL\n";
#endif

        JsonWrapper _entry;
        JsonWrapper::JsonValue & entry = _entry.GetRoot();

        // numeric id of state machine
        entry["state"] = stateMachineId;

        // Events ignored show up as events of 0.1 second's duration
        if (currEvt->GetIgnored()) {
            entry["name"] = currEvt->GetName();
            entry["desc"] = currEvt->GetWhat();
            entry["class"] = "ignored";
            entry["start"] = GetUTCTimeString(currEvt->GetTimestamp());
            entry["end"] = GetUTCTimeString(currEvt->GetTimestamp() + DEFAULT_WIDTH);
            json["events"].append(entry);
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": ignored\n";
#endif
        } else {
            nextState = it->NewState;
            // If no prior event (outstanding event) exists
            if (prevEvt == 0) {
#if STATE_HISTORY_DEBUG
            std::cout << __LINE__ << ": no prior event\n";
#endif
                if (nextState == State::NORMAL) {
                    // This should not happen
                    // MJTEMP: add debug event
#define ADD_DEBUG_EVENT \
                    entry["name"] = currEvt->GetName();\
                    entry["desc"] = currEvt->GetWhat();\
                    entry["class"] = "debug";\
                    entry["start"] = GetUTCTimeString(currEvt->GetTimestamp());\
                    entry["end"] = GetUTCTimeString(currEvt->GetTimestamp() + DEFAULT_WIDTH);\
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
                if (it != StateHistory.begin()) {
                    --it;
                    currState = it->NewState;
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
                            entry["end"] = GetUTCTimeString(currEvt->GetTimestamp() + DEFAULT_WIDTH);
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
                    entry["class"] = State::GetStringState(currState);
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
