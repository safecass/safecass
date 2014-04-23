//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Apr 22, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// This file defines the state-based semantics for CASROS, which consists of states, state
// entry and exist, and state transitions.
//
// References:
// 
// A. Avizienis, J.-C. Laprie, B. Randell, and C. Landwehr. Basic concepts and taxonomy of 
// dependable and secure computing. IEEE Trans. on Dependable and Secure Computing, 1:11–33, 
// Jan. 2004.
//
// TODO: SafeComp14 (if accepted)
//
#ifndef _state_h
#define _state_h

#include "common.h"

namespace SF {

//    namespace State {
class SFLIB_EXPORT State
{
public:
    /*! States */
    typedef enum {
        NORMAL,
        FAULT,
        ERROR,
        FAILURE,
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
        // fault state
        FAULT_ON_ENTRY,
        FAULT_ON_EXIT,
        // error state
        ERROR_ON_ENTRY,
        ERROR_ON_EXIT,
        // failure state
        FAILURE_ON_ENTRY,
        FAILURE_ON_EXIT,
        // total number of state entry/exit types
        NUMBER_OF_ENTRY_EXIT
    } StateEntryExitType;

    /*! State transition events */
    typedef enum {
        // When component is enabled or disabled
        // MJ: Should the last state be stored so that it can be restored later when
        // this statemachine comes back, i.e., ON_ENTRY occurs?
        ON_ENTRY,
        ON_EXIT,
        // From NORMAL
        FAULT_DETECTION,
        FAULT_REMOVAL,
        FAULT_ACTIVATION,
        ERROR_DETECTION,
        ERROR_REMOVAL,
        ERROR_PROPAGATION,
        FAILURE_DETECTION,
        FAILURE_REMOVAL,
        FAILURE_STOP,
        // total number of state transitions
        NUMBER_OF_TRANSITIONS
    } TransitionType;
    
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

    bool operator> (const State & rhs) const;
    bool operator< (const State & rhs) const;

    /*! State Product Operation (N: Normal, F: Fault, E: Error, Fa: Failure)
     
        s1\s2 | N  F  E  Fa
        ------|------------
          N   | N  F  E  Fa
          F   | F  F  E  Fa
          E   | E  E  E  Fa
          Fa  | Fa Fa Fa Fa
     */
    State operator* (const State & rhs) const;

    State operator*= (const State & rhs) const;

    State & operator= (const State & rhs);

    static const std::string GetString(StateType type);
    static const std::string GetString(StateEntryExitType type);
    static const std::string GetString(TransitionType type);
};

/*! Human readable output of state */

}; // SF

#endif // _state_h
