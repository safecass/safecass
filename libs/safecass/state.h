//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: May 12, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
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

#include "common/common.h"

namespace SC {

class SCLIB_EXPORT State
{
public:
    //! Typedef of states
    typedef enum {
        NORMAL = 0, /*!< Actual state: Normal */
        WARNING,    /*!< Actual state: Warning */
        ERROR,      /*!< Actual state: Error */
        FAILURE,    /*!< Propagated state */
        INVALID     /*!< Invalid state */
    } StateType;

    //! Typdef of state entry and exit events
    typedef enum {
        STATEMACHINE_ON_ENTRY,
        STATEMACHINE_ON_EXIT,
        NORMAL_ON_ENTRY,
        NORMAL_ON_EXIT,
        WARNING_ON_ENTRY,
        WARNING_ON_EXIT,
        ERROR_ON_ENTRY,
        ERROR_ON_EXIT,
        NUMBER_OF_ENTRY_EXIT /*!< number of state entry/exit types */
    } StateEntryExitType;

    //! Typedef of state transition events
    /*!
        Note that the state transition from ERROR to WARNING is not considered in the
        current design.

        FIXME Does ERROR_TO_WARNING make sense in practice???

        \sa event.h
    */
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
    //! Constructor (default state: NORMAL)
    State(void);
    //! Constructor with explicit state
    State(State::StateType);

    //! Accessors
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

    //! String representation of states, entry/exit events, and transitions
    static const std::string GetStringState(StateType state);
    static const std::string GetStringEntryExit(StateEntryExitType entryExit);
    static const std::string GetStringTransition(TransitionType transition);

    //! Given transition, return new state
    // FIXME This should be removed -- statemachine does this
    //static StateType GetNextState(TransitionType transition);

    // Returns human readable outputs
    virtual void ToStream(std::ostream & os) const;
};

inline std::ostream & operator << (std::ostream & os, const State & state)
{
    state.ToStream(os);
    return os;
}

}; // SC

#endif // _state_h
