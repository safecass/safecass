/*

  Safety Framework for Component-based Robotics

  Created on: July 24, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

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
