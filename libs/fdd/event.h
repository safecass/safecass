//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 7, 2012
// Last revision: Jul 9, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _event_h
#define _event_h

#include "state.h"

namespace SF {

class SFLIB_EXPORT Event
{
public:
    typedef std::vector<State::TransitionType> TransitionsType;

protected:
    const std::string     Name;
    const unsigned int    Severity;
    const TransitionsType Transitions;

    // 2D array containing boolean mask that defines possible transitions
    //
    //               Next state
    //              \  N  W  E
    // Current state \---------
    //             N | 0  0  0
    //             W | 0  0  0
    //             E | 0  0  0
    bool TransitionMask[TOTAL_NUMBER_OF_STATES][TOTAL_NUMBER_OF_STATES];

public:
    Event(const std::string     & name,
          unsigned int            severity,
          const TransitionsType & transitions);

    virtual ~Event() {}

    State::TransitionType GetTransition(State::StateType currentState) const;

    inline const std::string       GetName(void) const        { return Name; }
    inline unsigned int            GetSeverity(void) const    { return Severity; }
    //inline const TransitionsType & GetTransitions(void) const { return Transitions; }

    // Returns human readable outputs
    virtual void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const Event & event)
{
    event.ToStream(outputStream);
    return outputStream;
}

};

#endif // _event_h
