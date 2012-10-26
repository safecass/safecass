/*

  Safety Framework for Component-based Robotics

  Created on: October 26, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "stateEventHandler.h"

namespace SF {

StateEventHandler::StateEventHandler(void)
{
}

StateEventHandler::~StateEventHandler(void)
{
}

void StateEventHandler::OnStateEntryExit(const State::StateEntryExitType stateEntryExit)
{
    std::cout << "OnStateEntryExit: " << (int) stateEntryExit << std::endl;
}

void StateEventHandler::OnStateTransition(const State::TransitionType transition)
{
    std::cout << "OnStateTransition: " << (int) transition << std::endl;
}

};
