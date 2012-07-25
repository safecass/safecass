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

    const std::string GetTargetUID(void) const;
    const std::string GetTargetUID(Fault::FaultType faultType) const;

    void ToStream(std::ostream & outputStream) const;
};

typedef cisstTargetID TargetIDType;

inline std::ostream & operator << (std::ostream & outputStream, const cisstTargetID & targetID) {
    targetID.ToStream(outputStream);
    return outputStream;
}

class SFLIB_EXPORT cisstMonitor: public Monitor {
protected:
    Fault::FaultType    FaultType;
    TargetIDType        TargetID;
    Monitor::StatusType Status;
    Monitor::OutputType OutputType;
    int                 SamplingRate;
    StrVecType          AddressesToPublish;

public:
    cisstMonitor();
    virtual ~cisstMonitor();

    static std::string GetMonitorJSON(const std::string &        name,
                                      const Fault::FaultType     faultType,
                                      const Monitor::OutputType  outputType,
                                      const Monitor::StatusType  initialStatus,
                                      const TargetIDType &       targetID);

    bool IsActive(void) const;
    bool IsStream(void) const;
    bool IsEvent(void) const;

    TargetIDType &   GetTargetID(void);
    Fault::FaultType GetFaultType(void) const;

    void SetFaultType(const Fault::FaultType faultType);
    void SetTargetId(const TargetIDType & targetID);
    void SetStatus(const Monitor::StatusType status);
    void SetOutputType(const Monitor::OutputType outputType);
    void SetSamplingRate(int samplingRate);
    void SetAddressesToPublish(const StrVecType & addresses);

    void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const cisstMonitor & monitor) {
    monitor.ToStream(outputStream);
    return outputStream;
}

};

#endif // _cisstMonitor_h
