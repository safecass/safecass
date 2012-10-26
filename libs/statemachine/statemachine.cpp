/*

  Safety Framework for Component-based Robotics

  Created on: October 26, 2012

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

#include "statemachine.h"

using namespace SF::State;

namespace SF {

StateMachine::StateMachine(void)
{
    State.start();
}

StateMachine::~StateMachine(void)
{
    State.stop();
}

void StateMachine::ProcessEvent(const TransitionType transition)
{
    switch (transition) {
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

const StateType StateMachine::GetState(void) const
{
    switch (*State.current_state()) {
        case 0: return NORMAL;
        case 1: return FAULT;
        case 2: return ERROR;
        case 3: return FAILURE;
        default: return INVALID;
    }
}
 
const std::string StateMachine::GetStateString(void) const
{
    switch (GetState()) {
        case NORMAL:  return "NORMAL";
        case FAULT:   return "FAULT";
        case ERROR:   return "ERROR";
        case FAILURE: return "FAILURE";
        default:      return "INVALID";
    }
}

void StateMachine::Test(void)
{        
    //static char const* const state_names[] = { "Normal", "Fault", "Error", "Failure" };
    //std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;

    FaultState p;
    // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
    p.start(); 
    // to fault, back to normal
    std::cout <<"\nto fault, back to normal\n";
    p.process_event(fault_detection()); std::cout << GetStateString() << std::endl;
    p.process_event(fault_removal()); std::cout << GetStateString() << std::endl;
    // to error, back to Normal
    std::cout <<"\nto error, back to Normal\n";
    p.process_event(error_detection()); std::cout << GetStateString() << std::endl;
    p.process_event(error_removal()); std::cout << GetStateString() << std::endl;
    // to failure, back to Normal
    std::cout <<"\nto failure, back to Normal\n";
    p.process_event(failure_detection()); std::cout << GetStateString() << std::endl;
    p.process_event(failure_removal()); std::cout << GetStateString() << std::endl;
    // to fault and to error and to faillure and to normal
    std::cout <<"\nto fault and to error and to faillure and to normal\n";
    p.process_event(fault_detection()); std::cout << GetStateString() << std::endl;
    p.process_event(fault_activation()); std::cout << GetStateString() << std::endl;
    p.process_event(error_propagation()); std::cout << GetStateString() << std::endl;
    p.process_event(failure_removal()); std::cout << GetStateString() << std::endl;
    std::cout << "stop fsm" << std::endl;
    p.stop();
}

};
