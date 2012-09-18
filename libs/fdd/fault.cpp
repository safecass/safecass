/*

  Safety Framework for Component-based Robotics

  Created on: July 24, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "fault.h"

namespace SF {

Fault::Fault()
{
}

Fault::~Fault()
{
}

const std::string Fault::GetFaultTypeString(FaultType faultType)
{
    switch (faultType) {
        case FAULT_COMPONENT_PERIOD:  return STR(FAULT_COMPONENT_PERIOD);
        case FAULT_COMPONENT_OVERRUN: return STR(FAULT_COMPONENT_OVERRUN);
        case FAULT_APPLICATION: return STR(FAULT_APPLICATION);
        // [SFUPDATE]
        default: return STR(FAULT_INVALID);
    }
}

Fault::FaultType Fault::GetFaultTypeFromString(const std::string & faultString)
{
    if (faultString.compare(STR(FAULT_COMPONENT_PERIOD)) == 0) return FAULT_COMPONENT_PERIOD;
    if (faultString.compare(STR(FAULT_COMPONENT_OVERRUN)) == 0) return FAULT_COMPONENT_OVERRUN;
    if (faultString.compare(STR(FAULT_APPLICATION)) == 0) return FAULT_APPLICATION;
    // [SFUPDATE]

    return FAULT_INVALID;
}

};
