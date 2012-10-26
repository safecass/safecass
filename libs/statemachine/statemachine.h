/*

  Safety Framework for Component-based Robotics

  Created on: October 23, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

  The implementation of state machine in the Safety Framework uses
  the Boost Meta State Machine (MSM) which allows quick and easy implementation 
  of state machines of very high performance and is available at:

  http://www.boost.org/doc/libs/1_45_0/libs/msm/doc/HTML/index.html 

  (Boost MSM author: Christophe Henry <christophe.j.henry@googlemail.com>)

*/

#ifndef _statemachine_h
#define _statemachine_h

#include "common.h"
#include "StateEventHandler.h"
#include <iostream>
// boost msm
#include <boost/msm/back/state_machine.hpp> // back-end
#include <boost/msm/front/state_machine_def.hpp> // front-end

namespace SF {

namespace msm = ::boost::msm;
namespace mpl = ::boost::mpl;

// Forward declaration of state event handler
class StateEventHandler;

class SFLIB_EXPORT StateMachine {
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

        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {
            if (EventHandlerInstance)
                EventHandlerInstance->OnStateEntryExit(State::STATEMACHINE_ON_ENTRY);
        }
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {
            if (EventHandlerInstance)
                EventHandlerInstance->OnStateEntryExit(State::STATEMACHINE_ON_EXIT);
        }

#define ON_STATE_ENTRY_EXIT(_transition)\
        if (fsm.EventHandlerInstance)\
            fsm.EventHandlerInstance->OnStateEntryExit(_transition);
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

public:
    StateMachine(void);
    virtual ~StateMachine(void);

    /*! Getters */
    virtual void ProcessEvent(const State::TransitionType transition);

    const State::StateType GetState(void) const;
    const std::string GetStateString(void) const;

#if 1
    /*! State machine testing */
    void Test(void);
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
