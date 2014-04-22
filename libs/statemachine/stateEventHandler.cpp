//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Apr 21, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "stateEventHandler.h"

namespace SF {

StateEventHandler::StateEventHandler(void)
{
#if ENABLE_UNIT_TEST
    CountEntryExit.resize(State::NUMBER_OF_ENTRY_EXIT);
    CountTransition.resize(State::NUMBER_OF_TRANSITIONS);
#endif
}

StateEventHandler::~StateEventHandler(void)
{
}

void StateEventHandler::OnStateEntryOrExit(const State::StateEntryExitType stateEntryExit)
{
    SFLOG_DEBUG << "Base state machine: OnStateEntryOrExit: " << State::GetString(stateEntryExit) << std::flush << std::endl;
    // TODO: remove std debug log later
    std::cout << "Base state machine: OnStateEntryOrExit: " << State::GetString(stateEntryExit) << std::flush << std::endl;

#if ENABLE_UNIT_TEST
    ++CountEntryExit[static_cast<size_t>(stateEntryExit)];
#endif
}

void StateEventHandler::OnStateTransition(const State::TransitionType transition)
{
    SFLOG_DEBUG << "Base state machine: OnStateTransition: " << State::GetString(transition) << std::flush << std::endl;
    // TODO: remove std debug log later
    std::cout << "Base state machine: OnStateTransition: " << State::GetString(transition) << std::flush << std::endl;

#if ENABLE_UNIT_TEST
    ++CountTransition[static_cast<size_t>(transition)];
#endif
}

};
