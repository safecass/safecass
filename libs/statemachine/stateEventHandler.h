/*

  Safety Framework for Component-based Robotics

  Created on: October 26, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _stateEventHandler_h
#define _stateEventHandler_h

#include "common.h"
#include "state.h"

namespace SF {

class SFLIB_EXPORT StateEventHandler {
protected:

public:
    StateEventHandler(void);
    virtual ~StateEventHandler(void);

    virtual void OnStateEntryOrExit(const State::StateEntryExitType stateEntryExit);
    virtual void OnStateTransition(const State::TransitionType transition);
};
 
};

#endif // _stateEventHandler_h
