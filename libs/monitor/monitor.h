/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _monitor_h
#define _monitor_h

#include "fault.h"

#include <string>
#include <map>

namespace SF {

class TargetIDBase {
public:
    virtual const std::string GetTargetUID(void) const = 0;
    virtual const std::string GetTargetUID(Fault::FaultType faultType) const = 0;
};

class Monitor {
public:
    typedef enum { OUTPUT_STREAM, OUTPUT_EVENT } OutputType;
    typedef enum { MONITOR_OFF, MONITOR_ON } StatusType;

protected:


public:
    Monitor();
    virtual ~Monitor();
};

};

#endif // _monitor_h
