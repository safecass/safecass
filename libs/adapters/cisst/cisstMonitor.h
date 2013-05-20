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
protected:
    // cisstMonitor should be created with explicit arguments
    cisstMonitor(void) {}

public:
    cisstMonitor(const Monitor::TargetType target,
                 cisstEventLocation *      locationID,
                 const Monitor::StateType  state,
                 const Monitor::OutputType output,
                 // zero sampling rate for event-type monitoring
                 const SamplingRateType    samplingRate = 0);
    virtual ~cisstMonitor();

    /*! Returns JSON representation of this monitor */
    const std::string GetMonitorJSON(void) const;

    /*! Returns JSON message to publish to the Safety Framework */
    const std::string GetJsonForPublish(double sample, double currentTick) const;

    void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const cisstMonitor & monitor) {
    monitor.ToStream(outputStream);
    return outputStream;
}

};

#endif // _cisstMonitor_h
