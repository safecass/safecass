//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 7, 2012
// Last revision: May 13, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
// TODO:
// 1. Add verifier/checker when adding (registering) events including event creation via
// JSON specification.
// 2. Add new type of event: no state transition, no completion event (=> general type of
// events; lowest severity because of no state transition)
//
#ifndef _event_h
#define _event_h

#include "common/common.h" // for TimestampType
#include "common/jsonwrapper.h"
#include "safecass/state.h"

namespace SC {

//! Event class
/*!

    The GCM event has five attributes: name, severity, transition, timestamp,
    and description.  The name is a unique identifier of an event and no
    duplicate name within a system is allowed.  The severity is the degree of
    criticality used for determining the relative priority of events.  The
    transition defines a set of possible state transitions that may occur due to
    an event, and each transition is represented as a pair of the current state
    and the next state.  The GCM state machine (refer to statemachine.h) defines
    six possible transitions: NORMAL to WARNING, NORMAL to ERROR, WARNING to
    ERROR and ERROR to WARNING, ERROR to NORMAL, WARNING to NORMAL.  The
    timestamp is the time when an event is generated, and the description can
    include additional information about an event (e.g., error description in
    human readable form).  The first three attributes -- name, severity, and
    transitions -- are specified by the system designer as part of the system
    development process, whereas the other two -- timestamp and description --
    are determined at run-time.

    The following table summarizes the five attributes of GCM events:

    Attribute   Definition                                          Determined at
    ---------------------------------------------------------------------------
    Name        Unique identifier                                   Design-time
    Severity    Relative degree of criticality                      Design-time
    Transitions State transitions that may occur due to this event  Design-time
    ---------------------------------------------------------------------------
    Timestamp   Time when an event was detected or generated        Run-time
    Description Extra information                                   Run-time
    ---------------------------------------------------------------------------

    The GCM event semantics defines two different types of events: onset and completion
    events.  The onset event refers to the occurrence of an event, and the completion
    event represents the resolution or completion of the event handling process for an
    associated onset event. The idea is to represent an event and related activities in
    terms of the two individual GCM events and handle them separately. This distinction
    enables the structured and systematic management of events.

    Valid input for transitions are defined as follows:
    - For onset events: "N2W" | "NW2E"
    - For completion events: "W2N" | "EW2N"

    \sa statemachine.h
*/
class SCLIB_EXPORT Event
{
    // For outstanding event object of StateMachine
    friend class StateMachine;

public:
    //! Typedef of transition types
    /*!
        Note that the state transition from ERROR to WARNING is not considered in the
        current design.

        \sa state.h
    */
    typedef enum {
        // Onset events
        TRANSITION_N2W,  /*!< NORMAL to WARNING */
        TRANSITION_W2E,  /*!< WARNING to ERROR */
        TRANSITION_NW2E, /*!< NORMAL/WARNING to ERROR */
        // Completion events
        // Note: The current design defines no state transition from ERROR to WARNING
        // because error recovery into WARNING state doesn't make much sense in practice.
        // That is, why does someone want to recover from error to be in WARNING state,
        // rather than NORMAL state?
        TRANSITION_W2N,  /*!< WARNING to NORMAL */
        TRANSITION_E2N,  /*!< ERROR to NORMAL */
        TRANSITION_EW2N, /*!< WARNING/ERROR to NORMAL */
        // Invalid transition
        TRANSITION_INVALID
    } TransitionType;

    //! Typedef of event severity
    /*!
        1: lowest priority, 255: highest priority
        1-200: For application events
        201-250: For framework events
        251-255: For broadcast events
    */
    // TODO Support for user-specified (custom) range
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
    //! Name
    std::string Name;

    //! Severity
    unsigned int Severity;

    //! State transitions
    TransitionType Transition;

    //! Time when an event was detected or generated (zero by default)
    TimestampType Timestamp;

    //! Description containing detailed information (human-readable)
    std::string What;

    //! If this event is active, i.e., if this event is outstanding event
    /*!
        True if this event is active, false otherwise (false by default)
    */
    bool Active;

    //! If this event was ignored when detected
    /*!
        Timeline utility uses this property.  False by default.
    */
    bool Ignored;

    //! Private constructor: Prevent use of default constructor
    Event(void);

    //! Get string representation of transition type
    const std::string GetTransitionTypeString(TransitionType transition) const;

public:
    //! Default constructor with design-time attributes
    Event(const std::string & name, unsigned int severity, TransitionType transition);

    //! Copy constructor
    Event(const Event & event);

    //! Destructor
    ~Event() {}

    //! Equality operator overloading
    /*!
        Two event objects are equal if design-time attributes are the same
    */
    bool operator==(const Event & rhs) const;

    inline bool operator!=(const Event & rhs) const {
        return !(*this == rhs);
    }

    //! Getters
    /*!
        Design-time attributes (name, severity, transition) are read-only, whereas
        run-time attributes (timestamp, what) are set at run-time.

        \addtogroup Event accessors (getters)
        @{
    */
    inline const std::string &  GetName(void) const       { return Name; }
    inline unsigned int         GetSeverity(void) const   { return Severity; }
    inline const TransitionType GetTransition(void) const { return Transition; }
    inline TimestampType        GetTimestamp(void) const  { return Timestamp; }
    inline const std::string &  GetWhat(void) const       { return What; }

    void SetTimestamp(TimestampType timestamp) { Timestamp = timestamp; }
    void SetWhat(const std::string & what)     { What = what; }
    /*! @} */

    //! Returns transition from given state
    State::TransitionType GetStateTransition(State::StateType currentState) const;

    //! Accessors
    inline bool IsActive(void) const  { return Active; }
    inline bool IsIgnored(void) const { return Ignored; }
    inline void SetActive(bool active = true)   { Active = active; }
    inline void SetIgnored(bool ignored = true) { Ignored = ignored; }

    //! Returns human readable outputs
    virtual void ToStream(std::ostream & os) const;

    //! Serialize this object into Json object
    const Json::Value SerializeJSON(void) const;

    //! Serialize this object into string
    const std::string SerializeJSONString(void) const;

    // FIXME Add DeserializeJSON() and DeserializeJSONString()
};

inline std::ostream & operator << (std::ostream & os, const Event & event)
{
    event.ToStream(os);
    return os;
}

};

#endif // _event_h
