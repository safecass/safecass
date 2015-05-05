//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 8, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#ifndef _cisstMonitor_h
#define _cisstMonitor_h

#include "monitor.h"
#include "cisstDic.h"
#include "cisstEventLocation.h"

namespace SF {

class SFLIB_EXPORT cisstMonitor: public Monitor {
protected:
    //! cisstMonitor should be created with explicit arguments
    cisstMonitor(void) {}

public:
    //! Create cisst monitor target with explicit arguments
    cisstMonitor(const Monitor::TargetType target,
                 cisstEventLocation *      locationID,
                 const Monitor::StateType  state,
                 const Monitor::OutputType output,
                 // zero sampling rate for event-type monitoring
                 const SamplingRateType    samplingRate = 0);
    //! Constructor using instance of JSON structure
    cisstMonitor(const JsonWrapper::JsonValue & jsonNode);
    //! Constructor using JSON string.
    /*! Internally calls constructor with JSON structure */
    //Monitor(const std::string & jsonMonitorSpecString);
    //! Destructor
    virtual ~cisstMonitor();

    //! Returns JSON representation of this monitor
    const std::string GetMonitorJSON(void) const;

    //! Returns JSON message to publish to the Safety Framework
    const std::string GetJsonForPublish(double sample, double currentTick) const;

    //! Returns instance of monitoring target
    cisstEventLocation * GetLocationID(void) const;

    //! Returns contents of this class in human readable format
    void ToStream(std::ostream & outputStream, bool includeLocation = true) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const cisstMonitor & monitor) {
    monitor.ToStream(outputStream, false);
    return outputStream;
}

};

#endif // _cisstMonitor_h
