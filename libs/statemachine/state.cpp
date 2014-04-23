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
#include "state.h"

namespace SF {

State::State(void) : CurrentState(State::NORMAL)
{}

State::State(State::StateType state) : CurrentState(state)
{}

#define STRINGFY(_state) case _state: return #_state;
const std::string State::GetString(const StateType type)
{
    switch (type) {
        STRINGFY(NORMAL);
        STRINGFY(FAULT);
        STRINGFY(ERROR);
        STRINGFY(FAILURE);
        default: return "INVALID";
    }
}

const std::string State::GetString(const StateEntryExitType type)
{
    switch (type) {
        STRINGFY(STATEMACHINE_ON_ENTRY);
        STRINGFY(STATEMACHINE_ON_EXIT);
        STRINGFY(NORMAL_ON_ENTRY);
        STRINGFY(NORMAL_ON_EXIT);
        STRINGFY(FAULT_ON_ENTRY);
        STRINGFY(FAULT_ON_EXIT);
        STRINGFY(ERROR_ON_ENTRY);
        STRINGFY(ERROR_ON_EXIT);
        STRINGFY(FAILURE_ON_ENTRY);
        STRINGFY(FAILURE_ON_EXIT);
        default: return "INVALID";
    }
}

const std::string State::GetString(const TransitionType type)
{
    switch (type) {
        STRINGFY(ON_ENTRY);
        STRINGFY(ON_EXIT);
        STRINGFY(FAULT_DETECTION);
        STRINGFY(FAULT_REMOVAL);
        STRINGFY(FAULT_ACTIVATION);
        STRINGFY(ERROR_DETECTION);
        STRINGFY(ERROR_REMOVAL);
        STRINGFY(ERROR_PROPAGATION);
        STRINGFY(FAILURE_DETECTION);
        STRINGFY(FAILURE_REMOVAL);
        STRINGFY(FAILURE_STOP);
        default: return "INVALID";
    }
}
#undef STRINGFY

bool State::operator> (const State & rhs) const
{
    if (this->CurrentState == rhs.CurrentState) return false;
    if ((this->CurrentState == State::FAULT   && rhs.CurrentState == NORMAL) || 
        (this->CurrentState == State::ERROR   && rhs.CurrentState == NORMAL) || 
        (this->CurrentState == State::FAILURE && rhs.CurrentState == NORMAL) || 
        (this->CurrentState == State::ERROR   && rhs.CurrentState == FAULT) || 
        (this->CurrentState == State::FAILURE && rhs.CurrentState == FAULT) || 
        (this->CurrentState == State::FAILURE && rhs.CurrentState == ERROR))
        return true;
    return false;
}

bool State::operator< (const State & rhs) const
{
    if (this->CurrentState == rhs.CurrentState) return false;

    return !(*this > rhs);
}

State State::operator* (const State & rhs) const
{
    return ((*this > rhs) ? *this : rhs);
}


State & State::operator= (const State & rhs)
{
    this->CurrentState = rhs.CurrentState;

    return *this;
}

}; // SF

