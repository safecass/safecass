/*

  Safety Framework for Component-based Robotics

  Created on: July 8, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _cisstMonitor_h
#define _cisstMonitor_h

#include "monitor.h"

namespace SF {

class cisstTargetID: public TargetIDBase {
public:
    std::string ProcessName;
    std::string ComponentName;
    std::string InterfaceProvidedName;
    std::string InterfaceRequiredName;
    std::string CommandName;
    std::string FunctionName;
    std::string EventGeneratorName;
    std::string EventHandlerName;

    const std::string GetTargetUID(void) const;
    const std::string GetTargetUID(Fault::FaultType faultType) const;
};

typedef cisstTargetID TargetIDType;

class cisstMonitor: public Monitor {
public:
    static std::string GetMonitorJSON(const std::string &        name,
                                      const Fault::FaultType     faultType,
                                      const Monitor::OutputType  outputType,
                                      const Monitor::StatusType  initialStatus,
                                      const TargetIDType &       targetID);
public:
    cisstMonitor();
    virtual ~cisstMonitor();
};

};

#endif // _cisstMonitor_h
