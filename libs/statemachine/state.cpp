/*

  Safety Framework for Component-based Robotics

  Created on: October 26, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "state.h"

using namespace SF::State;

namespace SF {

    namespace State {

#define STRINGFY(_state) case _state: return #_state;

        const std::string GetString(StateType type) {
            switch (type) {
                STRINGFY(NORMAL);
                STRINGFY(FAULT);
                STRINGFY(ERROR);
                STRINGFY(FAILURE);
            default: return "INVALID";
            }
        }

        const std::string GetString(StateEntryExitType type) {
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

        const std::string GetString(TransitionType type) {
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

    }; // SF::State

}; // SF
