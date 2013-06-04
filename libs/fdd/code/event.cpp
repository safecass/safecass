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

const std::string Event::GetEventTypeString(EventType event)
{
    switch (event) {
        case EVENT_FAULT:   return STR(EVENT_FAULT);
        case EVENT_ERROR:   return STR(EVENT_ERROR);
        case EVENT_FAILURE: return STR(EVENT_FAILURE);
        default:            return STR(EVENT_INVALID);
    }
}

const std::string Event::GetFaultTypeString(FaultType fault)
{
    switch (fault) {
        case FAULT_COMPONENT_PERIOD:  return STR(FAULT_COMPONENT_PERIOD);
        case FAULT_COMPONENT_OVERRUN: return STR(FAULT_COMPONENT_OVERRUN);
        case FAULT_APPLICATION:       return STR(FAULT_APPLICATION);
        default:                      return STR(FAULT_INVALID);
    }
}

Event::EventType Event::GetEventTypeFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(STR(EVENT_FAULT)) == 0)   return EVENT_FAULT;
    if (_str.compare(STR(EVENT_ERROR)) == 0)   return EVENT_ERROR;
    if (_str.compare(STR(EVENT_FAILURE)) == 0) return EVENT_FAILURE;

    return EVENT_INVALID;
}

Event::FaultType Event::GetFaultTypeFromString(const std::string & str)
{
    std::string _str(str);
    to_uppercase(_str);

    if (_str.compare(STR(FAULT_COMPONENT_PERIOD)) == 0)  return FAULT_COMPONENT_PERIOD;
    if (_str.compare(STR(FAULT_COMPONENT_OVERRUN)) == 0) return FAULT_COMPONENT_OVERRUN;
    if (_str.compare(STR(FAULT_APPLICATION)) == 0)       return FAULT_APPLICATION;

    return FAULT_INVALID;
}

};
