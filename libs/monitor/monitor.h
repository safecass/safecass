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

#include "common.h"
#include "fault.h"

#include <string>
#include <map>

namespace SF {

class TargetIDBase {
public:
    virtual const std::string GetTargetUID(void) const = 0;
    virtual const std::string GetTargetUID(Fault::FaultType faultType) const = 0;
};

class SFLIB_EXPORT Monitor {
public:
    typedef enum { OUTPUT_INVALID, OUTPUT_STREAM, OUTPUT_EVENT } OutputType;
    typedef enum { MONITOR_INVALID, MONITOR_OFF, MONITOR_ON } StatusType;

protected:


public:
    Monitor();
    virtual ~Monitor();

    static const std::string GetStatusString(StatusType type);
    static StatusType GetStatusFromString(const std::string & str);

    static const std::string GetOutputString(OutputType output);
    static OutputType GetOutputFromString(const std::string & str);
};

};

#endif // _monitor_h
