//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Aug 6, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// This file defines the state-based semantics for SAFECASS, which consists of states, state
// entry and exist, and state transitions.
//
// References:
// 
// A. Avizienis, J.-C. Laprie, B. Randell, and C. Landwehr. Basic concepts and taxonomy of 
// dependable and secure computing. IEEE Trans. on Dependable and Secure Computing, 1:11–33, 
// Jan. 2004.
//
#ifndef _state_h
#define _state_h

#include "common.h"

#define TOTAL_NUMBER_OF_STATES 3

namespace SC {

class SCLIB_EXPORT State
{
public:
    /*! States */
    typedef enum {
        // actual states
        NORMAL = 0,
        WARNING,
        ERROR,
        // propagated state
        FAILURE,
        // invalid state
        INVALID
    } StateType;

    /*! State entry and exit events */
    typedef enum {
        // state machine
        STATEMACHINE_ON_ENTRY,
        STATEMACHINE_ON_EXIT,
        // normal state
        NORMAL_ON_ENTRY,
        NORMAL_ON_EXIT,
        // warning state
        WARNING_ON_ENTRY,
        WARNING_ON_EXIT,
        // error state
        ERROR_ON_ENTRY,
        ERROR_ON_EXIT,
        // failure state
        //FAILURE_ON_ENTRY,
        //FAILURE_ON_EXIT,
        // total number of state entry/exit types
        NUMBER_OF_ENTRY_EXIT
    } StateEntryExitType;

    /*! State transition events */
    typedef enum {
        // When component is enabled or disabled
        // MJ: Should the last state be stored so that it can be restored later when
        // this statemachine comes back, i.e., ON_ENTRY occurs?
        //ON_ENTRY,
        //ON_EXIT,
        //
        NORMAL_TO_ERROR,
        ERROR_TO_NORMAL,
        //
        NORMAL_TO_WARNING,
        WARNING_TO_NORMAL,
        //
        WARNING_TO_ERROR,
        ERROR_TO_WARNING,
        //
        NO_TRANSITION,
        // total number of state transitions
        NUMBER_OF_TRANSITIONS,
        // invalid transition
        INVALID_TRANSITION
    } TransitionType;

    // State machine types
    typedef enum {
        STATEMACHINE_FRAMEWORK,
        STATEMACHINE_APP,
        STATEMACHINE_PROVIDED,
        STATEMACHINE_REQUIRED,
        STATEMACHINE_INVALID
    } StateMachineType;
    
protected:
    StateType CurrentState;

public:
    State(void);
    State(State::StateType);

    /*! State Accessor */
    inline StateType GetState(void) const    { return CurrentState; }
    inline void SetState(StateType newState) { CurrentState = newState; }

    /*! Operator overloading */
    inline bool operator==(const State & rhs) const {
        return (this->CurrentState == rhs.CurrentState);
    }
    inline bool operator!=(const State & rhs) const {
        return !(this->CurrentState == rhs.CurrentState);
    }

    /*! State inequality: NORMAL < WARNING < ERROR */
    bool operator> (const State & rhs) const;
    bool operator< (const State & rhs) const;

    /*! State Product Operation (N: Normal, W: Warning, E: Error)
     
        s1\s2 | N  W  E
        ------|---------
          N   | N  W  E 
          W   | W  W  E 
          E   | E  E  E 
     */
    State operator* (const State & rhs) const;
    State operator*= (const State & rhs) const;
    State & operator= (const State & rhs);

    /*! String representation of states, entry/exit events, and transitions */
    static const std::string GetStringState(StateType state);
    static const std::string GetStringEntryExit(StateEntryExitType entryExit);
    static const std::string GetStringTransition(TransitionType transition);

    // Given transition, return new state 
    static StateType GetNextState(TransitionType transition);
};


}; // SC

#endif // _state_h