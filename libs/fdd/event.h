/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _event_h
#define _event_h

#include "common.h"

namespace SF {

class SFLIB_EXPORT Event
{
public:
    typedef enum {
        EVENT_INVALID,
        EVENT_FAULT,
        EVENT_ERROR,
        EVENT_FAILURE
    } EventType;

    typedef enum {
        FAULT_INVALID,
        // Predefined faults
        FAULT_COMPONENT_PERIOD,   // Timing fault: OS thread scheduling latency
        FAULT_COMPONENT_OVERRUN,  // Timing fault: thread overrun
        // Fault defined by applications
        FAULT_APPLICATION
    } FaultType;

    static const std::string GetEventTypeString(EventType Type);
    static const std::string GetFaultTypeString(FaultType faultType);

    static EventType GetEventTypeFromString(const std::string & eventString);
    static FaultType GetFaultTypeFromString(const std::string & faultString);
};

};

#endif // _event_h
