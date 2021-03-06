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
#ifndef _stateEventHandler_h
#define _stateEventHandler_h

#include "config.h" // to check if unit-test is enabled
#include "common/common.h"
#include "safecass/state.h"

namespace SC {

class SCLIB_EXPORT StateEventHandler
{
protected:
    //! Name of owner of this state machine
    const std::string OwnerName;

public:
    //! Constructor.  Owner name is set to NONAME by default.
    StateEventHandler(void);
    //! Constructor with owner name specified
    StateEventHandler(const std::string & owner);
    //! Destructor
    virtual ~StateEventHandler(void) {}

    //! Event handlers
    virtual void OnEntry(State::StateEntryExitType entryType);
    virtual void OnExit(State::StateEntryExitType exitType);
    virtual void OnTransition(State::TransitionType transition);

    //! Getter
    inline const std::string & GetOwnerName(void) const { return OwnerName; }
};

};

#endif // _stateEventHandler_h
