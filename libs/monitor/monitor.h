/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "fault.h"
#include <string>

namespace SF {

class Monitor {
    // for unit-tests
    //friend class JSONTest;

public:
    typedef enum {
        OUTPUT_STREAM,
        OUTPUT_EVENT
    } OutputType;

    typedef enum {
        MONITOR_OFF,
        MONITOR_ON
    } StatusType;

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

#ifdef SF_HAS_CISST
public:
    class cisstTargetID {
    public:
        std::string ProcessName;
        std::string ComponentName;
        std::string InterfaceProvidedName;
        std::string InterfaceRequiredName;
        std::string CommandName;
        std::string FunctionName;
        std::string EventGeneratorName;
        std::string EventHandlerName;
    };

    typedef cisstTargetID TargetIDType;

protected:
    static const std::string GetMonitorJSON_cisst(const std::string &  name,
                                                  Fault::FaultType     faultType,
                                                  Monitor::OutputType  outputType,
                                                  Monitor::StatusType  initialStatus,
                                                  const TargetIDType & targetId);
#endif

public:
    Monitor();
    virtual ~Monitor();

    static const std::string GetMonitorJSON(const std::string &  name,
                                            Fault::FaultType     faultType,
                                            Monitor::OutputType  outputType,
                                            Monitor::StatusType  initialStatus,
                                            const TargetIDType & targetId);

    static std::string GetKeyString(KeyTypes keyType);
};

};
