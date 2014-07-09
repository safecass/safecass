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
#include "event.h"

using namespace SF;

Event::Event(const std::string     & name,
             unsigned int            severity,
             const TransitionsType & transitions)
             //State::StateMachineType smType,
             //const std::string     & smTypeId)
    : Name(name), Severity(severity), Transitions(transitions)
      //SMType(smType), SMTypeId(smTypeId)
{
}

void Event::ToStream(std::ostream & os) const
{
    os << Name << ": " << Severity << ", [ ";

    for (size_t i = 0; i < Transitions.size(); ++i) {
        switch (Transitions[i]) {
        case State::NORMAL_TO_ERROR  : os << "N2E "; break;
        case State::ERROR_TO_NORMAL  : os << "E2N "; break;
        case State::NORMAL_TO_WARNING: os << "N2W "; break;
        case State::WARNING_TO_NORMAL: os << "W2N "; break;
        case State::WARNING_TO_ERROR : os << "W2E "; break;
        case State::ERROR_TO_WARNING : os << "E2W "; break;
        default:                       os << "INVALID ";
        }
    }
    os << "], ";

    /*
    switch (SMType) {
    case State::STATEMACHINE_FRAMEWORK: os << "s_F"; break;
    case State::STATEMACHINE_APP:       os << "s_A"; break;
    case State::STATEMACHINE_PROVIDED:  os << "s_P"; break;
    case State::STATEMACHINE_REQUIRED:  os << "s_R"; break;
    default:                            os << "INVALID"; break;
    }

    if (SMTypeId.size())
        os << ", Id: \"" << SMTypeId << "\"";
    */
}

#if 0 // obsolete implementation
#include "event.h"
#include "utils.h"

namespace SF {

Event::Event(void)
    : Type(Event::INVALID), 
      TypeFault(Event::FAULT_INVALID), 
      TypeError(Event::ERROR_INVALID), 
      TypeFailure(Event::FAILURE_INVALID), 
      Name("undefined"), Location(0), Timestamp(0)
{}

Event::Event(EventType           type, 
             FaultType           typeFault,
             const std::string & name,
             EventLocationBase * location,
             double              timestamp)
    : Type(type),
      TypeError(Event::ERROR_INVALID), 
      TypeFault(typeFault), 
      TypeFailure(Event::FAILURE_INVALID), 
      Name(name), Location(location), Timestamp(timestamp)
{}

Event::Event(EventType           type, 
             ErrorType           typeError,
             const std::string & name,
             EventLocationBase * location,
             double              timestamp)
    : Type(type),
      TypeError(typeError), 
      TypeFault(Event::FAULT_INVALID), 
      TypeFailure(Event::FAILURE_INVALID), 
      Name(name), Location(location), Timestamp(timestamp)
{}

Event::Event(EventType           type, 
             FailureType         typeFailure,
             const std::string & name,
             EventLocationBase * location,
             double              timestamp)
    : Type(type), 
      TypeFault(Event::FAULT_INVALID), 
      TypeError(Event::ERROR_INVALID), 
      TypeFailure(typeFailure), 
      Name(name), Location(location), Timestamp(timestamp)
{}

const std::string Event::GetEventTypeString(EventType event)
{
    switch (event) {
        case FAULT:   return STR(FAULT);
        case ERROR:   return STR(ERROR);
        case FAILURE: return STR(FAILURE);
        default:      return STR(INVALID);
    }
}

const std::string Event::GetFaultTypeString(FaultType fault)
{
    switch (fault) {
        case FAULT_COMPONENT_PERIOD:  return STR(COMPONENT_PERIOD);
        case FAULT_COMPONENT_OVERRUN: return STR(COMPONENT_OVERRUN);
        case FAULT_APPLICATION:       return STR(APPLICATION);
        default:                      return STR(INVALID);
    }
}

Event::EventType Event::GetEventTypeFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(STR(FAULT)) == 0)   return FAULT;
    if (_str.compare(STR(ERROR)) == 0)   return ERROR;
    if (_str.compare(STR(FAILURE)) == 0) return FAILURE;

    return INVALID;
}

Event::FaultType Event::GetFaultTypeFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(STR(COMPONENT_PERIOD)) == 0)  return FAULT_COMPONENT_PERIOD;
    if (_str.compare(STR(COMPONENT_OVERRUN)) == 0) return FAULT_COMPONENT_OVERRUN;
    if (_str.compare(STR(APPLICATION)) == 0)       return FAULT_APPLICATION;

    return FAULT_INVALID;
}

};
#endif
