//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 7, 2012
// Last revision: Aug 6, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "event.h"

using namespace SF;

Event::Event(const std::string     & name,
             unsigned int            severity,
             const TransitionsType & transitions,
             const std::string     & what)
    : Name(name), Severity(severity), Transitions(transitions), Timestamp(0), What(what)
{
    memset(TransitionMask, 0, TOTAL_NUMBER_OF_STATES * TOTAL_NUMBER_OF_STATES);
#define N 0
#define W 1
#define E 2
    // Counters to check if this event has multiple next states for one current state
    int fromN = 0, fromW = 0, fromE = 0;
    for (size_t i = 0; i < Transitions.size(); ++i) {
        switch (Transitions[i]) {
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
        SFLOG_ERROR << "Event transition error: event \"" << Name << "\" has multiple next states" << std::endl;
        SFLOG_ERROR << (int) TransitionMask[N][N] << " " << (int) TransitionMask[N][W] << " " << (int) TransitionMask[N][E] << std::endl;
        SFLOG_ERROR << (int) TransitionMask[W][N] << " " << (int) TransitionMask[W][W] << " " << (int) TransitionMask[W][E] << std::endl;
        SFLOG_ERROR << (int) TransitionMask[E][N] << " " << (int) TransitionMask[E][W] << " " << (int) TransitionMask[E][E] << std::endl;
        throw("Event error");
    }
#endif

#undef N
#undef W
#undef E
}

State::TransitionType Event::GetTransition(State::StateType currentState) const
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
    os << " ]";

    if (What.size())
        os << ", " << What;
}

const JSON::JSONVALUE Event::SerializeJSON(bool includeStateTransition) const
{
    JSON _json;
    JSON::JSONVALUE & json = _json.GetRoot();
    json["name"] = Name;
    json["severity"] = Severity;
    json["timestamp"] = Timestamp;
    if (What.size())
        json["what"] = What;

    if (includeStateTransition)
        for (size_t i = 0; i < Transitions.size(); ++i) {
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
    return JSON::GetJSONString(SerializeString(includeStateTransition));
}

