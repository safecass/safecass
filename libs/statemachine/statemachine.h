//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 23, 2012
// Last revision: Jul 15, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// CASROS uses the Boost Meta State Machine (MSM) library to implement the state 
// cmachine of the generic component model (GCM).  MSM enables quick and easy 
// cimplementation of state machines of high performance.  For more details, refer 
// to http://www.boost.org/doc/libs/1_55_0/libs/msm/doc/HTML/index.html 
//
#ifndef _statemachine_h
#define _statemachine_h

#include "common.h"
#include "config.h"
#include "stateEventHandler.h"
#include <iostream>

// boost msm
#include <boost/msm/back/state_machine.hpp> // back-end
#include <boost/msm/front/state_machine_def.hpp> // front-end

namespace SF {

namespace msm = ::boost::msm;
namespace mpl = ::boost::mpl;

class Event;

class SFLIB_EXPORT StateMachine 
{
protected:
    // Macros to define msm event
#define MSM_EVENT(_eventName) struct _eventName {};
    MSM_EVENT(evt_N2W);
    MSM_EVENT(evt_N2E);
    MSM_EVENT(evt_W2N);
    MSM_EVENT(evt_W2E);
    MSM_EVENT(evt_E2N);
    MSM_EVENT(evt_E2W);
#if 0
    // A "complicated" event type that carries some data.
	enum DiskTypeEnum
    {
        DISK_CD=0,
        DISK_DVD=1
    };
    struct cd_detected
    {
        cd_detected(std::string name, DiskTypeEnum diskType)
            : name(name),
            disc_type(diskType)
        {}

        std::string name;
        DiskTypeEnum disc_type;
    };
#endif

    // Define msm front-end (the FSM structure)
    struct GCMStateMachine_: public msm::front::state_machine_def<GCMStateMachine_> {
        /*! State machine event handler */
        StateEventHandler * EventHandlerInstance;

        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {
            if (EventHandlerInstance)
                EventHandlerInstance->OnStateEntryOrExit(State::STATEMACHINE_ON_ENTRY);
        }
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {
            if (EventHandlerInstance)
                EventHandlerInstance->OnStateEntryOrExit(State::STATEMACHINE_ON_EXIT);
        }

#define ON_STATE_ENTRY_EXIT(_transition)\
        if (fsm.EventHandlerInstance)\
            fsm.EventHandlerInstance->OnStateEntryOrExit(_transition);
        // List of FSM states
        struct Normal: public msm::front::state<> 
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::NORMAL_ON_ENTRY); }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::NORMAL_ON_EXIT); }
        };
        struct Warning: public msm::front::state<> 
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::WARNING_ON_ENTRY); }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::WARNING_ON_EXIT); }
        };
        struct Error: public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::ERROR_ON_ENTRY); }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::ERROR_ON_EXIT); }
        };
#undef ON_STATE_ENTRY_EXIT

        // Initial state (must be defined)
        typedef Normal initial_state;

        // Transition actions
#define ON_STATE_TRANSITION_ACTION(_transition)\
        if (EventHandlerInstance)\
            EventHandlerInstance->OnStateTransition(_transition);
        void on_N2W(evt_N2W const&) { ON_STATE_TRANSITION_ACTION(State::NORMAL_TO_WARNING); }
        void on_N2E(evt_N2E const&) { ON_STATE_TRANSITION_ACTION(State::NORMAL_TO_ERROR); }
        void on_W2N(evt_W2N const&) { ON_STATE_TRANSITION_ACTION(State::WARNING_TO_NORMAL); }
        void on_W2E(evt_W2E const&) { ON_STATE_TRANSITION_ACTION(State::WARNING_TO_ERROR); }
        void on_E2N(evt_E2N const&) { ON_STATE_TRANSITION_ACTION(State::ERROR_TO_NORMAL); }
        void on_E2W(evt_E2W const&) { ON_STATE_TRANSITION_ACTION(State::ERROR_TO_WARNING); }
#undef ON_STATE_TRANSITION_ACTION

        // Transition table for GCMStateMachine
        typedef GCMStateMachine_ fs; // to make transition table cleaner

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

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            SFLOG_ERROR << "GCMStateMachine: no transition from state " << state
                        << " on event " << typeid(e).name() << std::endl;
        }
    };

    // Pick a back-end
    typedef msm::back::state_machine<GCMStateMachine_> GCMStateMachine;

    /*! State machine instance */
    GCMStateMachine State;

    // Active event that caused transition last time.  NULL in NORMAL state, non-NULL otherwise
    Event * PendingEvent;

    /*! Name of owner of this state machine */
    std::string OwnerName;

private:
    /*! Common initializer */
    void Initialize(const std::string & ownerName, StateEventHandler * eventHandler);

public:
    /*! Default constructor.  An instance of StateEventHandler is internally created and 
        is used as default event handler. */
    StateMachine(void);
    /*! Constructors */
    StateMachine(const std::string & ownerName);
    StateMachine(StateEventHandler * eventHandler);
    StateMachine(const std::string & ownerName, StateEventHandler * eventHandler);

    /*! Destructor */
    virtual ~StateMachine(void);

    /*! Process state change events */
    virtual void ProcessEvent(const State::TransitionType transition, const Event * event);

    ////
    //// Pending Event
    ////
    //// Get pending event object
    //const Event * GetPendingEvent(void) const { return PendingEvent; }
    //// Set pending event
    //void SetPendingEvent(Event * e);
    //// Reset or clear pending event
    //void ClearPendingEvent(void);

    //
    // Getters
    //
    /*! Return current state */
    State::StateType GetCurrentState(void) const;

    /*! Return onwer name */
    inline const std::string & GetOwnerName(void) const { return OwnerName; }

    //
    // Setters
    //
    /*! Replace default state event handler with user-defined event handler.  This
        provides event hooks for applications, which allow the application layer to 
        handle state change events.  Any existing state event handler is deleted. */
    void SetStateEventHandler(StateEventHandler * instance);

#if ENABLE_UNIT_TEST
    /*! State machine testing */
    //void Test(void);
    // TODO: If multiple event handlers are used, update this method as well.
    int GetCountEntryExit(const State::StateEntryExitType stateEntryExit) const;
    int GetCountTransition(const State::TransitionType transition) const;

    void        PrintCounters(void) const;
    std::string GetCounterStatus(void) const;
#endif
};
 
/*
inline std::ostream & operator << (std::ostream & outputStream, const StateMachine & stateMachine) {
    stateMachine.ToStream(outputStream);
    return outputStream;
}
*/

};

#endif // _statemachine_h
