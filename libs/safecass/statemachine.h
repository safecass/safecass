//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Oct 23, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// URL          : https://github.com/minyang/safecass
//
// SAFECASS uses the Boost Meta State Machine (MSM) library to implement the state 
// machine of the generic component model (GCM).  MSM enables quick and easy 
// cimplementation of state machines of high performance.  For more details, refer 
// to http://www.boost.org/doc/libs/1_55_0/libs/msm/doc/HTML/index.html 
//
#ifndef _statemachine_h
#define _statemachine_h

#include <iostream>
#include <list>

#include "common.h"
#include "config.h"
#include "stateEventHandler.h"
#include "jsonwrapper.h"

// boost msm
#include <boost/msm/back/state_machine.hpp> // back-end
#include <boost/msm/front/state_machine_def.hpp> // front-end

namespace SC {

namespace msm = ::boost::msm;
namespace mpl = ::boost::mpl;

class Event;

class SCLIB_EXPORT StateMachine 
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
                EventHandlerInstance->OnEntry(State::STATEMACHINE_ON_ENTRY);
        }
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {
            if (EventHandlerInstance)
                EventHandlerInstance->OnExit(State::STATEMACHINE_ON_EXIT);
        }

#define ON_ENTRY(_transition)\
        if (fsm.EventHandlerInstance)\
            fsm.EventHandlerInstance->OnEntry(_transition);
#define ON_EXIT(_transition)\
        if (fsm.EventHandlerInstance)\
            fsm.EventHandlerInstance->OnExit(_transition);
        // List of FSM states
        struct Normal: public msm::front::state<> 
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_ENTRY(State::NORMAL_ON_ENTRY); }
            template <class Event,class FSM>
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

        // Initial state (must be defined)
        typedef Normal initial_state;

        // Transition actions
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
            SCLOG_ERROR << "GCMStateMachine: no transition from state " << state
                        << " on event " << typeid(e).name() << std::endl;
        }
    };

    // Pick a back-end
    typedef msm::back::state_machine<GCMStateMachine_> GCMStateMachine;

    /*! State machine instance */
    GCMStateMachine State;

    // Pending event that caused transition last time.  NULL in NORMAL state, non-NULL otherwise
    //const Event * OutstandingEvent;
    Event * OutstandingEvent;
    // Cache of the very last pending event.  To keep this cache is necessary for error propagation
    // because ProcessEvent() resets OutstandingEvent when getting back to NORMAL state.
    //const Event * LastOutstandingEvent;
    Event * LastOutstandingEvent;

    /*! Name of owner of this state machine */
    std::string OwnerName;

    // MJTEMP: This will be replaced with DB in the future.
    // List of events occurred associated with this state machine.  This
    // information is used for the timeline tool.
    typedef struct {
        // event occurred
        // 3. UTC <> osaGetTime correctness test
        // 4. define and handle various cases of onset/offset event visualization 
        // on the timeline chart (e.g., N -> W -> E -> W -> N)
        SC::Event * Evt;
        // new state due to event (INVALID if e was ignored)
        SC::State::StateType NewState;
    } StateTransitionEntry;

    typedef std::list<StateTransitionEntry> StateHistoryType;
    StateHistoryType StateHistory;

private:
    /*! Common initializer */
    void Initialize(const std::string & ownerName, StateEventHandler * eventHandler);

    // Called only by constructors and Reset()
    void Initialize(void);

public:
    /*! Default constructor.  An instance of StateEventHandler is internally created and 
        is used as default event handler. */
    StateMachine(void);
    //! Constructors
    // It is recommended to combine component name and interface name as owner name.
    StateMachine(const std::string & ownerName);
    StateMachine(StateEventHandler * eventHandler);
    StateMachine(const std::string & ownerName, StateEventHandler * eventHandler);

    /*! Destructor */
    virtual ~StateMachine(void);

    /*! Process state change events.  Internally, copy of event object is created and 
        pushed to the list of event history. */
    virtual bool ProcessEvent(const State::TransitionType transition, const Event * event);

    // Reset state machine.  StateHistory is reset if resetHistory is true.
    void Reset(bool resetHistory = false);

    // Get history of state transitions (used for event viewer)
    void GetStateTransitionHistory(JsonWrapper::JsonValue & json, unsigned int stateMachineId);

    //
    // Getters
    //
    //! Return current state
    State::StateType GetCurrentState(void) const;
    //! Return onwer name
    inline const std::string & GetOwnerName(void) const { return OwnerName; }
    //! Return pending event
    inline const Event * GetOutstandingEvent(void) const { return OutstandingEvent; }
    //! Return cached last pending event
    inline const Event * GetLastOutstandingEvent(void) const { return LastOutstandingEvent; }
    //! Check if last state transition was back to NORMAL state
    inline bool IsLastTransitionToNormalState(void) const { return (LastOutstandingEvent != 0); }

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
