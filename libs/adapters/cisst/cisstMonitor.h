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
#include "cisstEventLocation.h"

namespace SF {

class SFLIB_EXPORT cisstMonitor: public Monitor {
public:
    cisstMonitor(const Monitor::TargetType target,
                 cisstEventLocation *      locationID,
                 const Monitor::StateType  state,
                 const Monitor::OutputType output,
                 // zero sampling rate for event-type monitoring
                 const SamplingRateType    samplingRate = 0);
    virtual ~cisstMonitor();

    const std::string GetMonitorJSON(void) const;

    /*! Returns cisstEventLocation instance */

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
