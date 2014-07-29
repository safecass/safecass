//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Oct 26, 2012
// Last revision: Jul 29, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _stateEventHandler_h
#define _stateEventHandler_h

#include "common.h"
#include "config.h" // to check if unit-test is enabled
#include "state.h"

namespace SF {

class SFLIB_EXPORT StateEventHandler {
protected:
    /*! Name of owner of this state machine */
    const std::string OwnerName;

    void Initialize(void);

public:
    StateEventHandler(void);
    StateEventHandler(const std::string & owner);
    virtual ~StateEventHandler(void);

    virtual void OnStateEntryOrExit(const State::StateEntryExitType stateEntryExit);
    virtual void OnStateTransition(const State::TransitionType transition);

    inline const std::string & GetOwnerName(void) const { return OwnerName; }

    // Define test fields for unit tests
#if ENABLE_UNIT_TEST
    std::vector<int> CountEntryExit;
    std::vector<int> CountTransition;
#endif
};
 
};

#endif // _stateEventHandler_h
