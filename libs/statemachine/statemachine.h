//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 23, 2012
// Last revision: Apr 21, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// CAROS uses the Boost Meta State Machine (MSM) library to implement the state 
// cmachine of the generic component model (GCM).  MSM enables quick and easy 
// cimplementation of state machines of high performance.  For more details, refer 
// cto http://www.boost.org/doc/libs/1_55_0/libs/msm/doc/HTML/index.html 
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

// Forward declaration of state event handler
class StateEventHandler;

class SFLIB_EXPORT StateMachine 
{
protected:
    // Macros to define msm event
#define MSM_EVENT(_eventName) struct _eventName {};
    MSM_EVENT(fault_detection);
    MSM_EVENT(fault_removal);
    MSM_EVENT(fault_activation);
    MSM_EVENT(error_detection);
    MSM_EVENT(error_removal);
    MSM_EVENT(error_propagation);
    MSM_EVENT(failure_detection);
    MSM_EVENT(failure_removal);
    MSM_EVENT(failure_stop);
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
    struct FaultState_: public msm::front::state_machine_def<FaultState_> {
        /*! State machine event handler */
        StateEventHandler * EventHandlerInstance;

        // TODO: don't I need to use second argument (fsm instance?)
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {
            if (EventHandlerInstance)
                EventHandlerInstance->OnStateEntryOrExit(State::STATEMACHINE_ON_ENTRY);
        }
        // TODO: don't I need to use second argument (fsm instance?)
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
        struct Fault: public msm::front::state<> 
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::FAULT_ON_ENTRY); }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::FAULT_ON_EXIT); }
        };
        struct Error: public msm::front::state<> 
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::ERROR_ON_ENTRY); }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::ERROR_ON_EXIT); }
        };
        struct Failure: public msm::front::state<> 
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::FAILURE_ON_ENTRY); }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { ON_STATE_ENTRY_EXIT(State::FAILURE_ON_EXIT); }
        };
#undef ON_STATE_ENTRY_EXIT

        // Initial state (must be defined)
        typedef Normal initial_state;

        // Transition actions
#define ON_STATE_TRANSITION_ACTION(_transition)\
        if (EventHandlerInstance)\
            EventHandlerInstance->OnStateTransition(_transition);
        void fault_detected(fault_detection const&)     { ON_STATE_TRANSITION_ACTION(State::FAULT_DETECTION); }
        void back_to_normal(fault_removal const&)       { ON_STATE_TRANSITION_ACTION(State::FAULT_REMOVAL); }
        void fault_activated(fault_activation const&)   { ON_STATE_TRANSITION_ACTION(State::FAULT_ACTIVATION); }
        void error_detected(error_detection const&)     { ON_STATE_TRANSITION_ACTION(State::ERROR_DETECTION); }
        void back_to_normal(error_removal const&)       { ON_STATE_TRANSITION_ACTION(State::ERROR_REMOVAL); }
        void error_propagated(error_propagation const&) { ON_STATE_TRANSITION_ACTION(State::ERROR_PROPAGATION); }
        void failure_detected(failure_detection const&) { ON_STATE_TRANSITION_ACTION(State::FAILURE_DETECTION); }
        void back_to_normal(failure_removal const&)     { ON_STATE_TRANSITION_ACTION(State::FAILURE_REMOVAL); }
        // MJ TODO
        void terminated(failure_stop const&)            { ON_STATE_TRANSITION_ACTION(State::FAILURE_STOP); }
        // MJ TODO
        void deactivated(failure_stop const&)           { ON_STATE_TRANSITION_ACTION(State::FAILURE_STOP); }
#undef ON_STATE_TRANSITION_ACTION

        // Transition table for FaultState
        typedef FaultState_ fs; // to make transition table cleaner

        struct transition_table : mpl::vector<
            //    Start     Event               Next      Action				 Guard
            //  +---------+-------------------+---------+----------------------+---------------+
          a_row < Normal  , fault_detection   , Fault   , &fs::fault_detected                  >,
          a_row < Normal  , error_detection   , Error   , &fs::error_detected                  >,
          a_row < Normal  , failure_detection , Failure , &fs::failure_detected                >,
            //  +---------+-------------------+---------+----------------------+---------------+
          a_row < Fault   , fault_activation  , Error   , &fs::fault_activated                 >,
          a_row < Fault   , fault_removal     , Normal  , &fs::back_to_normal                  >,
            //  +---------+-------------------+---------+----------------------+---------------+
          a_row < Error   , error_propagation , Failure , &fs::error_propagated                >,
          a_row < Error   , error_removal     , Normal  , &fs::back_to_normal                  >,
            //  +---------+-------------------+---------+----------------------+---------------+
          a_row < Failure , failure_removal   , Normal  , &fs::back_to_normal                  >
          // TODO: Next state needs to be fixed
          //a_row < Failure , failure_stop      , Failure , &fs::terminated                      >, 
          // TODO: Next state needs to be fixed
          //a_row < Failure , failure_stop      , Failure , &fs::deactivated                     >
            //  +---------+-------------------+---------+----------------------+---------------+
        > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            std::cout << ">>> SM: no transition from state " << state
                << " on event " << typeid(e).name() << std::endl;
        }
    };

    // Pick a back-end
    typedef msm::back::state_machine<FaultState_> FaultState;

    /*! State machine instance */
    FaultState State;

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
    virtual void ProcessEvent(const State::TransitionType transition);

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
