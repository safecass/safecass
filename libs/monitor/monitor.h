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
    typedef enum {
        CONFIG,
        EVENT,
        IDENTIFIER,
        MARGIN,
        NAME, 
        NAME_PROCESS,
        NAME_COMPONENT,
        NAME_INTERFACE_PROVIDED,
        NAME_INTERFACE_REQUIRED,
        NAME_EVENT_GENERATOR,
        NAME_EVENT_HANDLER,
        OUTPUT,
        SAMPLING_RATE,
        STATE,
        STREAM,
        TARGET, 
        THRESHOLD,
        TYPE,
    } KeyTypes;

    typedef std::map<std::string, std::string> MonitorMapType;

public:
    Monitor();
    virtual ~Monitor();

    static bool AddMonitor(const std::string & targetUID, const std::string & monitorJson);

    /*
    virtual std::string GetMonitorJSON(const std::string &  name,
                                       Fault::FaultType     faultType,
                                       Monitor::OutputType  outputType,
                                       Monitor::StatusType  initialStatus,
                                       TargetIDBase *       targetId) = 0;
                                       */

    static std::string GetKeyString(KeyTypes keyType);
};

};

#endif // _monitor_h
