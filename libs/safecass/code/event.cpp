//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 7, 2012
// Last revision: May 8, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include <iomanip> // std::setprecision

#include "common/utils.h"
#include "common/dict.h"
#include "safecass/event.h"

using namespace SC;

Event::Event(void)
    : Name(NONAME),
      Severity(0),
      Transition(TRANSITION_INVALID),
      Timestamp(0),
      What("INVALID"),
      Active(false), Ignored(false)
{}


Event::Event(const std::string & name,
             unsigned int        severity,
             TransitionType      transition)
    : Name(name),
      Severity(severity),
      Transition(transition),
      Timestamp(0),
      What(""),
      Active(false), Ignored(false)
{}

Event::Event(const Event & event)
    : Name(event.GetName()),
      Severity(event.GetSeverity()),
      Transition(event.GetTransition()),
      Timestamp(event.GetTimestamp()),
      What(event.GetWhat()),
      Active(event.IsActive()),
      Ignored(event.IsIgnored())
{}

bool Event::operator==(const Event & e) const
{
    return (Name.compare(e.GetName()) == 0 &&
            Severity == e.GetSeverity() &&
            Transition == e.GetTransition());
}

State::TransitionType Event::GetStateTransition(State::StateType currentState) const
{
    if (Transition == TRANSITION_INVALID)
        return State::INVALID_TRANSITION;

    switch (currentState) {
    case State::NORMAL:
        switch (Transition) {
        case TRANSITION_N2W:
            return State::NORMAL_TO_WARNING;
        case TRANSITION_NW2E:
            return State::NORMAL_TO_ERROR;
        default:
            return State::INVALID_TRANSITION;
        }
        break;
    case State::WARNING:
        switch (Transition) {
        case TRANSITION_W2E:
        case TRANSITION_NW2E:
            return State::WARNING_TO_ERROR;
        case TRANSITION_W2N:
        case TRANSITION_EW2N:
            return State::WARNING_TO_NORMAL;
        default:
            return State::INVALID_TRANSITION;
        }
        break;
    case State::ERROR:
    case State::FAILURE:
        switch (Transition) {
        case TRANSITION_E2N:
        case TRANSITION_EW2N:
            return State::ERROR_TO_NORMAL;
        default:
            return State::INVALID_TRANSITION;
        }
        break;
    case State::INVALID:
        return State::INVALID_TRANSITION;
    }
}

void Event::ToStream(std::ostream & os) const
{
    os << Name << ", severity: " << Severity << ", ";

    os << "transition: " << GetTransitionTypeString(Transition) << ", ";

    os << "time: ";
    PrintTime(Timestamp, os);
    os << ", ";

    os << "active: " << (Active ? "true" : "false") << ", ";
    os << "ignored: " << (Ignored ? "true" : "false");

    if (!What.empty())
        os << ", what: " << What;
}

const Json::Value Event::SerializeJSON(void) const
{
    JsonWrapper jsonWrapper;
    Json::Value & json = jsonWrapper.GetJsonRoot();

    json[Dict::EVENT_ATTR_NAME]       = Name;
    json[Dict::EVENT_ATTR_SEVERITY]   = Severity;
    json[Dict::EVENT_ATTR_TRANSITION] = GetTransitionTypeString(Transition);
    json[Dict::EVENT_ATTR_TIMESTAMP]  = Timestamp;
    json[Dict::EVENT_ATTR_WHAT]       = What;
    json[Dict::EVENT_ATTR_ACTIVE]     = Active;
    json[Dict::EVENT_ATTR_IGNORED]    = Ignored;

    return json;
}

const std::string Event::SerializeJSONString(void) const
{
    return JsonWrapper::GetJsonString(SerializeJSON());
}

const std::string Event::GetTransitionTypeString(TransitionType transition) const
{
    switch (transition) {
    case TRANSITION_N2W:     return Dict::EVENT_TRANSITION_N2W;
    case TRANSITION_W2E:     return Dict::EVENT_TRANSITION_W2E;
    case TRANSITION_NW2E:    return Dict::EVENT_TRANSITION_NW2E;
    case TRANSITION_W2N:     return Dict::EVENT_TRANSITION_W2N;
    case TRANSITION_E2N:     return Dict::EVENT_TRANSITION_E2N;
    case TRANSITION_EW2N:    return Dict::EVENT_TRANSITION_EW2N;
    case TRANSITION_INVALID: return Dict::INVALID;
    }
}
