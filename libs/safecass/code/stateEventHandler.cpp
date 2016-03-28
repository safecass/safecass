//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Mar 15, 2016
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/safecass/safecass
//
#include "safecass/stateEventHandler.h"

namespace SC {

StateEventHandler::StateEventHandler(void) : OwnerName("NONAME")
{
    Initialize();
}

StateEventHandler::StateEventHandler(const std::string & owner) : OwnerName(owner)
{
    Initialize();
}

StateEventHandler::~StateEventHandler(void)
{
}

void StateEventHandler::Initialize(void)
{
#if SAFECASS_ENABLE_UNIT_TEST
    CountEntryExit.resize(State::NUMBER_OF_ENTRY_EXIT);
    CountTransition.resize(State::NUMBER_OF_TRANSITIONS);
#endif
}

void StateEventHandler::OnEntry(const State::StateEntryExitType entryType)
{
    SCLOG_DEBUG << OwnerName << " - Base state machine: OnEntry: " << State::GetStringEntryExit(entryType) << std::flush << std::endl;

#if SAFECASS_ENABLE_UNIT_TEST
    ++CountEntryExit[static_cast<size_t>(entryType)];
#endif
}

void StateEventHandler::OnExit(const State::StateEntryExitType exitType)
{
    SCLOG_DEBUG << OwnerName << " - Base state machine: OnExit: " << State::GetStringEntryExit(exitType) << std::flush << std::endl;

#if SAFECASS_ENABLE_UNIT_TEST
    ++CountEntryExit[static_cast<size_t>(exitType)];
#endif
}

void StateEventHandler::OnTransition(const State::TransitionType transition)
{
    SCLOG_DEBUG << OwnerName << " - Base state machine: OnTransition: " << State::GetStringTransition(transition) << std::flush << std::endl;

#if SAFECASS_ENABLE_UNIT_TEST
    ++CountTransition[static_cast<size_t>(transition)];
#endif
}

};
