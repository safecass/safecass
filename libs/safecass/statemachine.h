//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Oct 23, 2012
// Last revision: May 12, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// URL          : https://github.com/safecass/safecass
//
// SAFECASS uses the Boost Meta State Machine (MSM) library to implement the state
// machine of the generic component model (GCM).  MSM provides a structured and thorough
// implementation of state machine.
//
// For more details, refer to the Boost MSM documentation:
//   http://www.boost.org/doc/libs/1_60_0/libs/msm/doc/HTML/index.html
//
// TODO:
// 1. Add description of ProcessEvent() => important!
//    - Event severity policy
//    - Event handling policy (when to ignore), including how to handle event of the same
//      severity.  Currently, the latest event of which severity is equal to or greater
//      than that of the current outstanding event is processed.  The implication here is
//      that newer events of equal severity is being handled.
#ifndef _statemachine_h
#define _statemachine_h

#include "config.h"
#include "common/jsonwrapper.h"
#include "safecass/state.h"
#include "safecass/event.h"
#include "safecass/stateEventHandler.h"

// Boost msm
#include "boost/msm/back/state_machine.hpp" // back-end
#include "boost/msm/front/state_machine_def.hpp" // front-end

#include <list>

namespace SC {

namespace msm = ::boost::msm;
namespace mpl = ::boost::mpl;

// Forward class declaration
class Event;

class SCLIB_EXPORT StateMachine
{
protected:
    //! Name of owner of this state machine
    const std::string OwnerName;

    //! Outstanding event that caused last state transition
    /*!
        Event::Active property is false by default and in case of NORMAL state
        Event::Active property is true in case of WARNING or ERROR states
    */
    Event OutstandingEvent;

    // FIXME is this still the case?
    //! Cache of last outstanding event
    /*!
        For error propagation, it is necessary to maintain the cache of the last
        outstanding event because ProcessEvent() resets when transitioning to NORMAL.
    */
    Event LastOutstandingEvent;

    //! List of events occurred that this state machine has processed
    /*!
        This history of events is used for event history visualization

        \sa SAFECASS timeline tool
    */
    typedef struct {
        Event e;                /*!< Event object that initiated state transition */
        State::StateType state; /*!< New state due to event; INVALID if ignored */
    } StateTransitionEntry;

    typedef std::list<StateTransitionEntry> TransitionHistoryType;
    TransitionHistoryType TransitionHistory;

    //-----------------------------------------------------
    // GCM State Machine implementation using Boost MSM
    //-----------------------------------------------------
    // TODO: Event type that carries data can be also defined (maybe event object that
    // initiated state transition as argument..?)
    //! MSM events representing state transitions of the GCM state machine
    struct evt_N2W {};
    struct evt_N2E {};
    struct evt_W2N {};
    struct evt_W2E {};
    struct evt_E2N {};
    struct evt_E2W {};

    // MSM front-end
    struct GCMStateMachine_: public msm::front::state_machine_def<GCMStateMachine_>
    {
        //! State machine event handler
        /*!
            This event handler instance is called upon every state Transition if the
            instance is not null (null by default).
        */
        StateEventHandler * EventHandlerInstance;

        //! Constructor
        GCMStateMachine_(void): EventHandlerInstance(0) {}

        template <class Event, class FSM>
        void on_entry(Event const &, FSM &) {
            if (EventHandlerInstance)
                EventHandlerInstance->OnEntry(State::STATEMACHINE_ON_ENTRY);
        }
        template <class Event, class FSM>
        void on_exit(Event const &, FSM &) {
            if (EventHandlerInstance)
                EventHandlerInstance->OnExit(State::STATEMACHINE_ON_EXIT);
        }

#define ON_ENTRY(_transition)\
        if (fsm.EventHandlerInstance)\
            fsm.EventHandlerInstance->OnEntry(_transition);
#define ON_EXIT(_transition)\
        if (fsm.EventHandlerInstance)\
            fsm.EventHandlerInstance->OnExit(_transition);

        //! FSM states
        struct Normal: public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const &, FSM & fsm) { ON_ENTRY(State::NORMAL_ON_ENTRY); }
            template <class Event, class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_EXIT(State::NORMAL_ON_EXIT); }
        };

        struct Warning: public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_ENTRY(State::WARNING_ON_ENTRY); }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_EXIT(State::WARNING_ON_EXIT); }
        };

        struct Error: public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_ENTRY(State::ERROR_ON_ENTRY); }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_EXIT(State::ERROR_ON_EXIT); }
        };
#undef ON_ENTRY
#undef ON_EXIT

        //! Initial state (must be defined)
        typedef Normal initial_state;

        //! Transition actions
#define ON_TRANSITION_ACTION(_transition)\
        if (EventHandlerInstance)\
            EventHandlerInstance->OnTransition(_transition);
        void on_N2W(evt_N2W const&) { ON_TRANSITION_ACTION(State::NORMAL_TO_WARNING); }
        void on_N2E(evt_N2E const&) { ON_TRANSITION_ACTION(State::NORMAL_TO_ERROR); }
        void on_W2N(evt_W2N const&) { ON_TRANSITION_ACTION(State::WARNING_TO_NORMAL); }
        void on_W2E(evt_W2E const&) { ON_TRANSITION_ACTION(State::WARNING_TO_ERROR); }
        void on_E2N(evt_E2N const&) { ON_TRANSITION_ACTION(State::ERROR_TO_NORMAL); }
        void on_E2W(evt_E2W const&) { ON_TRANSITION_ACTION(State::ERROR_TO_WARNING); }
#undef ON_TRANSITION_ACTION

        //! Aliasing for cleaner transition table
        typedef GCMStateMachine_ fs;

        //! State transition table for GCM state machine
        struct transition_table : mpl::vector<
            //    Start     Event     Next      Action		   Guard
            //  +---------+---------+---------+--------------+--------+
          a_row < Normal  , evt_N2W , Warning , &fs::on_N2W>,
          a_row < Normal  , evt_N2E , Error   , &fs::on_N2E>,
            //  +---------+---------+---------+--------------+--------+
          a_row < Warning , evt_W2E , Error   , &fs::on_W2E>,
          a_row < Warning , evt_W2N , Normal  , &fs::on_W2N>,
            //  +---------+---------+---------+--------------+--------+
          a_row < Error   , evt_E2N , Normal  , &fs::on_E2N>,
          a_row < Error   , evt_E2W , Warning , &fs::on_E2W>
            //  +---------+---------+---------+--------------+--------+
        > {};

        //! Overrides default no-transition action
        template <class FSM, class Event>
        void no_transition(Event const & e, FSM &, int state)
        {
            SCLOG_WARNING << "GCM state machine: no transition from state " << state
                          << " on event " << typeid(e).name() << std::endl;
        }
    };

    //! FSM back-end
    typedef msm::back::state_machine<GCMStateMachine_> GCMStateMachine;

    //! GCM state machine as final state machine
    GCMStateMachine FSM;

    //! Push event to event history list
    void PushTransitionHistory(const Event & event, State::StateType newState);

public:
    //! Default constructor
    StateMachine(void);

    /*!
        Note that the ownership of the instance of StateEventHandler (second parameter) is
        taken by this state machine object.  The eventHandler instance is deleted by either
        the destructor of this state machine or SetStateEventHandler().
    */
    StateMachine(const std::string & ownerName, StateEventHandler * eventHandler = 0);

    //! Destructor
    virtual ~StateMachine(void);

    //! Process state transition event
    /*!
        Timestamp of event is updated to the time when the event was actually processed
        and caused state transition.

        \return true if event is successfully handled. false otherwise (e.g., event was
                ignored due to lower severity, invalid transition returned)
    */
    virtual bool ProcessEvent(Event & event);

    //! Reset state machine
    /*!
        \param resetHistory Event history is reset if true
    */
    void Reset(bool resetHistory = false);

    //! Get history of state transitions (required for the timeline tool)
    void GetStateTransitionHistory(Json::Value & json, unsigned int stateMachineId);

    //! State machine accessors
    /*!
        \addtogroup State machine accessors
        @{
    */
    //! Return current state
    State::StateType GetCurrentState(void) const;
    //! Return onwer name
    inline const std::string & GetOwnerName(void) const { return OwnerName; }
    //! Return state event handler instance
    inline StateEventHandler * GetStateEventHandler(void) const { return FSM.EventHandlerInstance; }
    //! Return outstanding event
    inline const Event & GetOutstandingEvent(void) const { return OutstandingEvent; }
    //! Return cached last pending event
    inline const Event & GetLastOutstandingEvent(void) const { return LastOutstandingEvent; }
    //! Check if last state transition was back to NORMAL state
    inline bool IsLastTransitionToNormalState(void) const { return LastOutstandingEvent.IsActive(); }

    /*! Replace default state event handler with user-defined event handler.  This
        provides event hooks for applications, which allow the application layer to
        handle state change events.  Any existing state event handler is deleted. */
    void SetStateEventHandler(StateEventHandler * instance);
    /*! @} */

    //! Print state machine object
    virtual void ToStream(std::ostream & os) const;
};

inline std::ostream & operator << (std::ostream & os, const StateMachine & sm) {
    sm.ToStream(os);
    return os;
}

};

#endif // _statemachine_h
