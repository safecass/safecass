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
#include <iostream>
// boost msm
#include <boost/msm/back/state_machine.hpp> // back-end
#include <boost/msm/front/state_machine_def.hpp> // front-end

static char const* const state_names[] = { "Normal", "Fault", "Error", "Failure" };

namespace SF {

namespace msm = ::boost::msm;
namespace mpl = ::boost::mpl;

class SFLIB_EXPORT StateMachine {
public:
    typedef enum {
        FAULT_DETECTION,
        FAULT_REMOVAL,
        FAULT_ACTIVATION,
        ERROR_DETECTION,
        ERROR_REMOVAL,
        ERROR_PROPAGATION,
        FAILURE_DETECTION,
        FAILURE_REMOVAL,
        FAILURE_STOP
    } EventType;

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
    struct FaultState_: public msm::front::state_machine_def<FaultState_>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) 
        {
            std::cout << ">>> entering" << std::endl;
        }
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) 
        {
            std::cout << ">>> leaving" << std::endl;
        }

        // List of FSM states
        struct Normal: public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                std::cout << ">>> SM: entering: Normal" << std::endl;
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                std::cout << ">>> SM: leaving: Normal" << std::endl;
            }
        };
        struct Fault: public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                std::cout << ">>> SM: entering: Fault" << std::endl;
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                std::cout << ">>> SM: leaving: Fault" << std::endl;
            }
        };
        struct Error: public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                std::cout << ">>> SM: entering: Error" << std::endl;
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                std::cout << ">>> SM: leaving: Error" << std::endl;
            }
        };
        struct Failure: public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                std::cout << ">>> SM: entering: Failure" << std::endl;
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                std::cout << ">>> SM: leaving: Failure" << std::endl;
            }
        };

        // Initial state (must be defined)
        typedef Normal initial_state;

        // Transition actions
        void fault_detected(fault_detection const&) { std::cout << ">>> SM: fault detected\n"; }
        void back_to_normal(fault_removal const&) { std::cout << ">>> SM: fault removed\n"; }
        void fault_activated(fault_activation const&) { std::cout << ">>> SM: fault activated\n"; }
        void error_detected(error_detection const&) { std::cout << ">>> SM: error detected\n"; } 
        void back_to_normal(error_removal const&) { std::cout << ">>> SM: error removed\n"; }
        void error_propagated(error_propagation const&) { std::cout << ">>> SM: error propagated\n"; } 
        void failure_detected(failure_detection const&) { std::cout << ">>> SM: failure detected\n"; }
        void back_to_normal(failure_removal const&) { std::cout << ">>> SM: failure removed\n"; }
        void terminated(failure_stop const&) { std::cout << ">>> SM: failure stop and terminated\n"; }
        void deactivated(failure_stop const&) { std::cout << ">>> SM: failure stop and deactivated\n"; }

        // Transition table for FaultState
        typedef FaultState_ fs; // to make transition table cleaner

        struct transition_table : mpl::vector<
            //    Start     Event               Next      Action				 Guard
            //  +---------+-------------------+---------+----------------------+---------------+
          a_row < Normal  , fault_detection   , Fault   , &fs::fault_detected                  >,
          a_row < Normal  , error_detection   , Error   , &fs::error_detected                  >,
          a_row < Normal  , failure_detection , Failure , &fs::failure_detected                >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Fault   , fault_activation  , Error   , &fs::fault_activated                 >,
          a_row < Fault   , fault_removal     , Normal  , &fs::back_to_normal                  >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Error   , error_propagation , Failure , &fs::error_propagated                >,
          a_row < Error   , error_removal     , Normal  , &fs::back_to_normal                  >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Failure , failure_removal   , Normal  , &fs::back_to_normal                  >
          // TODO: Next state needs to be fixed
          //a_row < Failure , failure_stop      , Failure , &fs::terminated                      >, 
          // TODO: Next state needs to be fixed
          //a_row < Failure , failure_stop      , Failure , &fs::deactivated                     >
            //  +---------+-------------+---------+---------------------+----------------------+
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

    //
    // Testing utilities.
    //
    //static char const* const state_names[] = { "Normal", "Fault", "Error", "Failure" };
    void pstate(FaultState const& p)
    {
        std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
    }

    void test()
    {        
	    FaultState p;
        // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
        p.start(); 
        // to fault, back to normal
        std::cout <<"\nto fault, back to normal\n";
        p.process_event(fault_detection()); pstate(p);
        p.process_event(fault_removal()); pstate(p);
        // to error, back to Normal
        std::cout <<"\nto error, back to Normal\n";
        p.process_event(error_detection()); pstate(p);
        p.process_event(error_removal()); pstate(p);
        // to failure, back to Normal
        std::cout <<"\nto failure, back to Normal\n";
        p.process_event(failure_detection()); pstate(p);
        p.process_event(failure_removal()); pstate(p);
        // to fault and to error and to faillure and to normal
        std::cout <<"\nto fault and to error and to faillure and to normal\n";
        p.process_event(fault_detection()); pstate(p);
        p.process_event(fault_activation()); pstate(p);
        p.process_event(error_propagation()); pstate(p);
        p.process_event(failure_removal()); pstate(p);
        std::cout << "stop fsm" << std::endl;
        p.stop();
    }

    /*! State machine instance */
    FaultState State;

public:
    StateMachine(void)
    {
        //test();
        State.start();
    }

    ~StateMachine(void)
    {
        State.stop();
    }

    void ProcessEvent(const EventType event) {
        switch (event) {
        case FAULT_DETECTION:   State.process_event(fault_detection()); break;
        case FAULT_REMOVAL:     State.process_event(fault_removal()); break;
        case FAULT_ACTIVATION:  State.process_event(fault_activation()); break;
        case ERROR_DETECTION:   State.process_event(error_detection()); break;
        case ERROR_REMOVAL:     State.process_event(error_removal()); break;
        case ERROR_PROPAGATION: State.process_event(error_propagation()); break;
        case FAILURE_DETECTION: State.process_event(failure_detection()); break;
        case FAILURE_REMOVAL:   State.process_event(failure_removal()); break;
        case FAILURE_STOP:      State.process_event(failure_stop()); break;
        default:
            return;
        }
    }

    void PrintState(void)
    {
        std::cout << "\tCurrent state: " << state_names[State.current_state()[0]] << std::endl;
    }
};
 
/*
inline std::ostream & operator << (std::ostream & outputStream, const StateMachine & stateMachine) {
    stateMachine.ToStream(outputStream);
    return outputStream;
}
*/

};

#endif // _statemachine_h
