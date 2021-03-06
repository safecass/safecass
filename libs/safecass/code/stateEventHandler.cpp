//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: May 28, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "safecass/stateEventHandler.h"

using namespace SC;

StateEventHandler::StateEventHandler(void) : OwnerName("NONAME")
{}

StateEventHandler::StateEventHandler(const std::string & owner) : OwnerName(owner)
{}

void StateEventHandler::OnEntry(State::StateEntryExitType entryType)
{
    SCLOG_DEBUG << "StateEventHandler (owner: \"" << OwnerName << "\") OnEntry: "
                << State::GetString(entryType) << std::endl;
}

void StateEventHandler::OnExit(State::StateEntryExitType exitType)
{
    SCLOG_DEBUG << "StateEventHandler (owner: \"" << OwnerName << "\") OnExit: "
                << State::GetString(exitType) << std::endl;
}

void StateEventHandler::OnTransition(State::TransitionType transition)
{
    SCLOG_DEBUG << "StateEventHandler (owner: \"" << OwnerName << "\") OnTransition: "
                << State::GetString(transition) << std::endl;
}
