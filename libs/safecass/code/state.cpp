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
#include "safecass/state.h"

using namespace SC;

State::State(void) : CurrentState(State::NORMAL)
{}

State::State(State::StateType state) : CurrentState(state)
{}

#define STRINGFY(_state) case _state: return #_state;
const std::string State::GetStringState(const StateType state)
{
    switch (state) {
        STRINGFY(NORMAL);
        STRINGFY(WARNING);
        STRINGFY(ERROR);
        STRINGFY(FAILURE);
        default: return "INVALID";
    }
}

const std::string State::GetStringEntryExit(const StateEntryExitType entryExit)
{
    switch (entryExit) {
        STRINGFY(NORMAL_ON_ENTRY);
        STRINGFY(NORMAL_ON_EXIT);
        STRINGFY(WARNING_ON_ENTRY);
        STRINGFY(WARNING_ON_EXIT);
        STRINGFY(ERROR_ON_ENTRY);
        STRINGFY(ERROR_ON_EXIT);
        default: return "INVALID";
    }
}

const std::string State::GetStringTransition(const TransitionType transition)
{
    switch (transition) {
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

#if 0
State::StateType State::GetNextState(TransitionType transition)
{
    switch (transition) {
    case State::WARNING_TO_NORMAL:
    case State::ERROR_TO_NORMAL:
        return State::NORMAL;
    case State::NORMAL_TO_WARNING:
    case State::ERROR_TO_WARNING:
        return State::WARNING;
    case State::NORMAL_TO_ERROR:
    case State::WARNING_TO_ERROR:
        return State::ERROR;
    default:
        return State::INVALID;
    }
}
#endif

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

void State::ToStream(std::ostream & os) const
{
    os << GetStringState(CurrentState);
}
