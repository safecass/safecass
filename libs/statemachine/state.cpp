//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Jul 16, 2014
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
const std::string State::GetStringState(const StateType type)
{
    switch (type) {
        STRINGFY(NORMAL);
        STRINGFY(WARNING);
        STRINGFY(ERROR);
        STRINGFY(FAILURE);
        default: return "INVALID";
    }
}

const std::string State::GetStringEntryExit(const StateEntryExitType type)
{
    switch (type) {
        STRINGFY(NORMAL_ON_ENTRY);
        STRINGFY(NORMAL_ON_EXIT);
        STRINGFY(WARNING_ON_ENTRY);
        STRINGFY(WARNING_ON_EXIT);
        STRINGFY(ERROR_ON_ENTRY);
        STRINGFY(ERROR_ON_EXIT);
        default: return "INVALID";
    }
}

const std::string State::GetStringTransition(const TransitionType type)
{
    switch (type) {
        STRINGFY(NORMAL_TO_ERROR);
        STRINGFY(ERROR_TO_NORMAL);
        STRINGFY(NORMAL_TO_WARNING);
        STRINGFY(WARNING_TO_NORMAL);
        STRINGFY(WARNING_TO_ERROR);
        STRINGFY(ERROR_TO_WARNING);
        default: return "INVALID";
    }
}
#undef STRINGFY

bool State::operator> (const State & rhs) const
{
    if (this->CurrentState == rhs.CurrentState) return false;
    if ((this->CurrentState == State::WARNING && rhs.CurrentState == NORMAL) || 
        (this->CurrentState == State::ERROR   && rhs.CurrentState == NORMAL) || 
        (this->CurrentState == State::ERROR   && rhs.CurrentState == WARNING))
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

