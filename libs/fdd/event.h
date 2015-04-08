//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 7, 2012
// Last revision: Apr 8, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _event_h
#define _event_h

#include "utils.h" // for TimestampType
#include "state.h"
#include "json.h"

namespace SF {

class SFLIB_EXPORT Event
{
    friend class StateMachine;

public:
    typedef std::vector<State::TransitionType> TransitionsType;
    
    // severity definition
    enum {
        SEVERITY_APP_MIN       = 1,
        SEVERITY_APP_MAX       = 200,
        SEVERITY_FRAMEWORK_MIN = 201,
        SEVERITY_FRAMEWORK_MAX = 250,
        SEVERITY_BROADCAST_MIN = 251,
        SEVERITY_BROADCAST_MAX = 255
    };

protected:
    //
    // Event Attributes
    //
    // Name of this event
    //const std::string Name;
    std::string Name;
    // Severity:1-255
    // 1: lowest priority, ..., 255: highest priority
    // 1-200: For application events
    // 201-255: Reserved for framework events
    //const unsigned int Severity;
    unsigned int Severity;
    // Possible transitions associated with this event
    //const TransitionsType Transitions;
    TransitionsType Transitions;
    // timestamp of the time when this event occurred.  zero otherwise.
    TimestampType Timestamp;
    // Description of event
    std::string What;

    // If this event is valid (used by StateMachine class to determine if outsanding 
    // event object is valid)
    bool Valid;

    // If event was ignored, i.e., if event did not result in state transition
    // (used for event visualization)
    bool Ignored;

    // 2D array containing boolean mask that defines possible transitions
    //
    //               Next state
    //              \  N  W  E
    // Current state \---------
    //             N | 0  0  0
    //             W | 0  0  0
    //             E | 0  0  0
    bool TransitionMask[TOTAL_NUMBER_OF_STATES][TOTAL_NUMBER_OF_STATES];

    // Set TransitionMask array based on transition information
    void SetTransitionMask(const TransitionsType & transitions);

    // Prevent use of default constructor by users
    Event(void);
public:
    // Default constructor
    Event(const std::string     & name,
          unsigned int            severity,
          const TransitionsType & transitions,
          const std::string     & what = "");
    // Copy constructor
    Event(const Event & event);
    // Destructor
    virtual ~Event() {}

    // Operator overloading
    Event & operator= (const Event & event);

    // Getters
    inline const std::string &   GetName(void) const        { return Name; }
    inline unsigned int          GetSeverity(void) const    { return Severity; }
    inline const TransitionsType GetTransitions(void) const { return Transitions; }
    inline TimestampType         GetTimestamp(void) const   { return Timestamp; }
    inline const std::string &   GetWhat(void) const        { return What; }

    // Returns possible transition from the given state
    State::TransitionType GetPossibleTransitions(State::StateType currentState) const;

    //
    // Setters for run-time attributes
    //
    // MJTEMP: Currently, Get/SetTimestamp is used only for the timeline viewer.
    // In other cases, event objects are handled as const pointer once the
    // object is created.
    void SetTimestamp(TimestampType timestamp) { Timestamp = timestamp; }
    void SetWhat(const std::string & what)     { What = what; }

    // Accessor for ignored property (used for timeline viewer)
    inline bool GetIgnored(void) const          { return Ignored; }
    inline void SetIgnored(bool ignored = true) { Ignored = ignored; }
    // Accessor for valid property (used by StateMachine class)
    inline bool GetValid(void) const        { return Valid; }
    inline void SetValid(bool valid = true) { Valid = valid; }

    // Copy attributes of other event
    void CopyFrom(const Event * event);

    // Returns human readable outputs
    virtual void ToStream(std::ostream & outputStream) const;
    // Serialize this object in json
    const JSON::JSONVALUE SerializeJSON(bool includeStateTransition = true) const;
    // Serialize this object in string
    const std::string SerializeString(bool includeStateTransition = true) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const Event & event)
{
    event.ToStream(outputStream);
    return outputStream;
}

};

#endif // _event_h
