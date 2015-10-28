//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 7, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#include "event.h"
#include <iomanip> // std::setprecision

using namespace SC;


Event::Event(void)
    : Name("INVALID"),
      Severity(0),
      Transitions(TransitionsType()),
      Timestamp(0),
      What(""),
      Valid(false),
      Ignored(false)
{
}

Event::Event(const std::string     & name,
             unsigned int            severity,
             const TransitionsType & transitions,
             const std::string     & what)
    : Name(name),
      Severity(severity),
      Transitions(transitions),
      Timestamp(0),
      What(what),
      Valid(false),
      Ignored(false)
{
    memset(TransitionMask, 0, TOTAL_NUMBER_OF_STATES * TOTAL_NUMBER_OF_STATES);

    SetTransitionMask(Transitions);
}

Event::Event(const Event & event)
    : Name(event.GetName()),
      Severity(event.GetSeverity()),
      Transitions(event.GetTransitions()),
      Timestamp(event.GetTimestamp()),
      What(event.GetWhat()),
      Valid(event.GetValid()),
      Ignored(event.GetIgnored())
{
}

// Operator overloading
Event & Event::operator= (const Event & event)
{
    // check self-assignment
    if (this == &event)
        return *this;

    CopyFrom(&event);

    return *this;
}

void Event::SetTransitionMask(const TransitionsType & transitions)
{
#define N 0
#define W 1
#define E 2
    // Counters to check if this event has multiple next states for one current state
    int fromN = 0, fromW = 0, fromE = 0;
    for (size_t i = 0; i < transitions.size(); ++i) {
        switch (transitions[i]) {
        case State::NORMAL_TO_WARNING: TransitionMask[N][W] = true; ++fromN; break;
        case State::NORMAL_TO_ERROR  : TransitionMask[N][E] = true; ++fromN; break;
        case State::WARNING_TO_NORMAL: TransitionMask[W][N] = true; ++fromW; break;
        case State::WARNING_TO_ERROR : TransitionMask[W][E] = true; ++fromW; break;
        case State::ERROR_TO_WARNING : TransitionMask[E][W] = true; ++fromE; break;
        case State::ERROR_TO_NORMAL  : TransitionMask[E][N] = true; ++fromE; break;
        default: continue;
        }
    }
#if 0 // check this out later when working on sanity check on event spec file
    if (fromN >= 2 || fromW >= 2 || fromE >= 2) {
        SCLOG_ERROR << "Event transition error: event \"" << Name << "\" has multiple next states" << std::endl;
        SCLOG_ERROR << (int) TransitionMask[N][N] << " " << (int) TransitionMask[N][W] << " " << (int) TransitionMask[N][E] << std::endl;
        SCLOG_ERROR << (int) TransitionMask[W][N] << " " << (int) TransitionMask[W][W] << " " << (int) TransitionMask[W][E] << std::endl;
        SCLOG_ERROR << (int) TransitionMask[E][N] << " " << (int) TransitionMask[E][W] << " " << (int) TransitionMask[E][E] << std::endl;
        throw("Event error");
    }
#endif

#undef N
#undef W
#undef E
}

State::TransitionType Event::GetPossibleTransitions(State::StateType currentState) const
{
    switch (currentState) {
    case State::NORMAL:
        // Preference on more severe state
        if (TransitionMask[State::NORMAL][State::ERROR])
            return State::NORMAL_TO_ERROR;
        else if (TransitionMask[State::NORMAL][State::WARNING])
            return State::NORMAL_TO_WARNING;
        break;
    case State::WARNING:
        // TODO: W2N and W2E can't be in the same event transition spec
        if (TransitionMask[State::WARNING][State::NORMAL])
            return State::WARNING_TO_NORMAL;
        else if (TransitionMask[State::WARNING][State::ERROR])
            return State::WARNING_TO_ERROR;
        break;
    case State::ERROR:
        if (TransitionMask[State::ERROR][State::NORMAL])
            return State::ERROR_TO_NORMAL;
        else if (TransitionMask[State::ERROR][State::WARNING])
            return State::ERROR_TO_WARNING;
        break;
    default:
        return State::INVALID_TRANSITION;
    }

    // Check no-transition case
    switch (currentState) {
    case State::NORMAL:
        if (TransitionMask[State::WARNING][State::NORMAL] ||
            TransitionMask[State::ERROR][State::NORMAL])
            return State::NO_TRANSITION;
        break;
    case State::WARNING:
        if (TransitionMask[State::NORMAL][State::WARNING] ||
            TransitionMask[State::ERROR][State::WARNING])
            return State::NO_TRANSITION;
        break;
    case State::ERROR:
        if (TransitionMask[State::NORMAL][State::ERROR] ||
            TransitionMask[State::WARNING][State::ERROR])
            return State::NO_TRANSITION;
    default:
        return State::INVALID_TRANSITION;
    }

    return State::INVALID_TRANSITION;
}

void Event::ToStream(std::ostream & os) const
{
    os << Name << ": " << Severity << ", [ ";

    for (size_t i = 0; i < Transitions.size(); ++i) {
        if (i > 0)
            os << " ";
        switch (Transitions[i]) {
        case State::NORMAL_TO_ERROR  : os << "N2E"; break;
        case State::ERROR_TO_NORMAL  : os << "E2N"; break;
        case State::NORMAL_TO_WARNING: os << "N2W"; break;
        case State::WARNING_TO_NORMAL: os << "W2N"; break;
        case State::WARNING_TO_ERROR : os << "W2E"; break;
        case State::ERROR_TO_WARNING : os << "E2W"; break;
        default:                       os << "INVALID";
        }
    }
    os << " ], " << std::setprecision(13) << Timestamp;

    if (What.size())
        os << ", \"" << What << "\"";
}

const JsonWrapper::JsonValue Event::SerializeJSON(bool includeStateTransition) const
{
    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.GetRoot();
    json["name"] = Name;
    json["severity"] = Severity;
    json["timestamp"] = Timestamp;
    if (What.size())
        json["what"] = What;

    if (includeStateTransition)
        for (Json::ArrayIndex i = 0; i < Transitions.size(); ++i) {
            switch (Transitions[i]) {
            case State::NORMAL_TO_ERROR  : json["state_transition"][i] = "N2E"; break;
            case State::ERROR_TO_NORMAL  : json["state_transition"][i] = "E2N"; break;
            case State::NORMAL_TO_WARNING: json["state_transition"][i] = "N2W"; break;
            case State::WARNING_TO_NORMAL: json["state_transition"][i] = "W2N"; break;
            case State::WARNING_TO_ERROR : json["state_transition"][i] = "W2E"; break;
            case State::ERROR_TO_WARNING : json["state_transition"][i] = "E2W"; break;
            default:                       json["state_transition"][i] = "INVALID";
            }
        }

    return json;
}

const std::string Event::SerializeString(bool includeStateTransition) const
{
    return JsonWrapper::GetJSONString(SerializeString(includeStateTransition));
}

void Event::CopyFrom(const Event * event)
{
    if (!event)
        return;

    this->Name = event->GetName();
    this->Severity = event->GetSeverity();
    this->Transitions = event->GetTransitions();
    this->Timestamp = event->GetTimestamp();
    this->What = event->GetWhat();

    this->Valid = event->GetValid();
    this->Ignored = event->GetIgnored();

    SetTransitionMask(this->Transitions);
}
