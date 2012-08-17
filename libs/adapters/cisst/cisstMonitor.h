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
#include "cisstDic.h"

namespace SF {

class SFLIB_EXPORT cisstTargetID: public TargetIDBase {
public:
    std::string ProcessName;
    std::string ComponentName;
    std::string InterfaceProvidedName;
    std::string InterfaceRequiredName;
    std::string CommandName;
    std::string FunctionName;
    std::string EventGeneratorName;
    std::string EventHandlerName;

    const std::string GetTargetID(void) const;

    void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const cisstTargetID & targetID) {
    targetID.ToStream(outputStream);
    return outputStream;
}

class SFLIB_EXPORT cisstMonitor: public Monitor {
public:
    cisstMonitor(const Monitor::TargetType target,
                 cisstTargetID *           targetID,
                 const Monitor::StateType  state,
                 const Monitor::OutputType output,
                 const SamplingRateType    samplingRate);
    virtual ~cisstMonitor();

    const std::string GetMonitorJSON(void) const;

    /*! Returns cisstTargetID instance */

    //
    // Publish monitored data in Json
    //
    /*! Getters to publish collected (monitored) data via SF */
    const std::string GetJsonForPublishingPeriod(double sample) const;
    const std::string GetJsonForPublishingDutyCycleUser(double dutyCycle) const;
    const std::string GetJsonForPublishingDutyCycleTotal(double dutyCycle) const;

    void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const cisstMonitor & monitor) {
    monitor.ToStream(outputStream);
    return outputStream;
}

};

#endif // _cisstMonitor_h
