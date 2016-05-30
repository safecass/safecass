//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: May 28, 2016
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
const std::string State::GetString(StateType state)
{
    switch (state) {
        STRINGFY(NORMAL);
        STRINGFY(WARNING);
        STRINGFY(ERROR);
        STRINGFY(FAILURE);
        default: return "INVALID";
    }
}

const std::string State::GetString(StateEntryExitType entryExit)
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

const std::string State::GetString(TransitionType transition)
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

const std::string State::GetString(StateMachineType type)
{
    switch (type) {
        STRINGFY(STATEMACHINE_FRAMEWORK);
        STRINGFY(STATEMACHINE_APP);
        STRINGFY(STATEMACHINE_PROVIDED);
        STRINGFY(STATEMACHINE_REQUIRED);
        STRINGFY(STATEMACHINE_SERVICE);
        default: return "INVALID";
    }
}
#undef STRINGFY

bool State::operator< (const State & rhs) const
{
    if (this->CurrentState == rhs.CurrentState)
        return false;

    if ((this->CurrentState == State::NORMAL && rhs.CurrentState == WARNING) ||
        (this->CurrentState == State::NORMAL && rhs.CurrentState == ERROR) ||
        (this->CurrentState == State::WARNING && rhs.CurrentState == ERROR))
    {
        return true;
    }

    return false;
}

State State::operator* (const State & rhs) const
{
    return ((*this > rhs) ? *this : rhs);
}

State & State::operator*= (const State & rhs)
{
    if (*this < rhs)
        this->CurrentState = rhs.GetState();

    return *this;
}

State & State::operator= (const State & rhs)
{
    this->CurrentState = rhs.CurrentState;

    return *this;
}

void State::ToStream(std::ostream & os) const
{
    os << GetString(CurrentState);
}
