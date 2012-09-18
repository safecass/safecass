/*

  Safety Framework for Component-based Robotics

  Created on: September 17, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _jsonSerializer_h
#define _jsonSerializer_h

#include "json.h"
#include "eventLocationBase.h"
#include "monitor.h"
#include "fault.h"

namespace SF {

class SFLIB_EXPORT JSONSerializer {
public:
    typedef enum {
        INVALID,
        MONITOR,    // monitoring
        FAULT,      // fault detection and diagnosis
        SUPERVISOR  // fault tolerance and removal
    } TopicType;

protected:
    // Common Fields
    struct {
        /*! Type of topic */
        TopicType Topic;
        /*! Event localization */
        EventLocationBase * EventLocation;
        /*! Event timestamp */
        double Timestamp;
    } Common;

    // Monitor
    struct {
        /*! Common field */
        Monitor::TargetType Type;
        /*! Monitor-specific fields */
        ::Json::Value Fields;
    } Monitor;

    // Fault
    struct {
        /*! Common fields */
        Fault::FaultType Type;
        std::string DetectorName;
        /*! Fault-specific fields */
        ::Json::Value Fields;
    } Fault;

    /*! Initialization */
    void Init(void);

public:
    JSONSerializer(void);
    virtual ~JSONSerializer(void);

    /*! Represent all information in JSON format and return JSON string */
    const std::string GetJSON(void) const;

    /*! Rebuild information based on JSON string.  If successful, internal structures such
        as Common and either Monitor or Fault are populated and updated. */
    bool ParseJSON(const std::string & message);

    //---------------------------------------- 
    //  Accessors
    //---------------------------------------- 
    // Common::Topic
    inline TopicType GetTopicType(void) const { return Common.Topic; }
    inline void      SetTopicType(const TopicType topicType) { Common.Topic = topicType; }
    // Common::EventLocation
    inline EventLocationBase * GetEventLocation(void) const { return Common.EventLocation; }
    void                       SetEventLocation(EventLocationBase * location);
    // Common::Timestamp
    inline double GetTimestamp(void) const { return Common.Timestamp; }
    inline void   SetTimestamp(double timestamp) { Common.Timestamp = timestamp; }

    // Monitor::Type
    inline Monitor::TargetType GetMonitorTargetType(void) const { return Monitor.Type; }
    inline void                SetMonitorTargetType(Monitor::TargetType target) { Monitor.Type = target; }
    // Monitor::Fields
    inline ::Json::Value & GetMonitorFields(void) { return Monitor.Fields; }

    // Fault::Type
    inline Fault::FaultType GetFaultType(void) const { return Fault.Type; }
    inline void             SetFaultType(Fault::FaultType faultType) { Fault.Type = faultType; }
    // Fault::Detector
    inline const std::string GetFaultDetectorName(void) const { return Fault.DetectorName; }
    inline void              SetFaultDetectorName(const std::string & name) { Fault.DetectorName = name; }
    // Fault::Fields
    inline ::Json::Value & GetFaultFields(void) { return Fault.Fields; }

    /*! Return string that corresponds to topic type */
    static const std::string GetTopicTypeString(TopicType topicType);
    /*! Return topic type from string */
    static TopicType GetTopicTypeFromString(const std::string & topicTypeString);
};

};

#endif // _jsonSerializer_h
