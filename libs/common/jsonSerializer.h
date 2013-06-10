/*

  Safety Framework for Component-based Robotics

  Created on: September 17, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _jsonSerializer_h
#define _jsonSerializer_h

#include "json.h"
#include "monitor.h"
#include "event.h"
#include "filterBase.h"

namespace SF {

//! JSON serializer and deserializer
/** 
    JSONSerializer implements serialization and deserialization of JSON messages
    for data exchange within Safety Framework.  This class also defines a list of
    topics avaiable.
*/
class SFLIB_EXPORT JSONSerializer {
public:
    typedef enum {
        INVALID,
        MONITOR,    /*!< monitoring (see SF::Monitor) */
        EVENT,      /*!< events (e.g., fault, error, failures; see SF::Event) */
        SUPERVISOR  /*!< supervisory control messages (TODO: SF::Supervisory) */
    } TopicType;

protected:
    //! Common Fields
    struct {
        //! Filter UID
        FilterBase::FilterIDType FilterUID;
        //! Type of topic (enum variable)
        TopicType Topic;
        //! Location of event (middleware specific)
        EventLocationBase * EventLocation;
        //! Event timestamp
        double Timestamp;
    } Common;

    //! Monitor topic
    struct {
        //! Common field
        Monitor::TargetType Type;
        //! Monitor-specific values
        JSON::JSONVALUE Json;
    } Monitor;

    //! Event topic
    struct {
        //! Common fields
        Event::EventType Type;
        std::string      Name;
        //std::string DetectorName;
        //! Event-specific values
        JSON::JSONVALUE Json;
    } Event;

    //! Initialization
    void Initialize(void);

public:
    JSONSerializer(void);
    ~JSONSerializer(void);

    //! Represent all information in JSON format and return JSON string
    const std::string GetJSON(void) const;

    //! Rebuild topic information based on JSON
    bool ParseJSON(const std::string & message);

    //! Populate event topic with SF::Event object
    void SetEvent(const SF::Event & event);

    /*! \addtogroup Accessors for common fields
        @{
     */
    //! Common::FilterUID
    inline FilterBase::FilterIDType GetFilterUID(void) const { return Common.FilterUID; }
    inline void SetFilterUID(const FilterBase::FilterIDType uid) { Common.FilterUID = uid; }
    //! Common::Topic
    inline TopicType GetTopicType(void) const                { return Common.Topic; }
    inline void      SetTopicType(const TopicType topicType) { Common.Topic = topicType; }
    //! Common::EventLocation
    inline EventLocationBase * GetEventLocation(void) const  { return Common.EventLocation; }
    void                       SetEventLocation(EventLocationBase * location);
    //! Common::Timestamp
    inline double GetTimestamp(void) const       { return Common.Timestamp; }
    inline void   SetTimestamp(double timestamp) { Common.Timestamp = timestamp; }
    /*! @} */

    /*! \addtogroup Accessors for Monitor messages
        @{
     */
    //! Monitor::Type
    inline Monitor::TargetType GetMonitorTargetType(void) const                 { return Monitor.Type; }
    inline void                SetMonitorTargetType(Monitor::TargetType target) { Monitor.Type = target; }
    //! Returns json container for monitor specific values
    inline JSON::JSONVALUE &   GetMonitorFields(void) { return Monitor.Json; }
    /*! @} */

    /*! \addtogroup Accessors for Event messages
        @{
     */
    //! Event::Type
    inline Event::EventType  GetEventType(void) const                 { return Event.Type; }
    inline void              SetEventType(Event::EventType eventType) { Event.Type = eventType; }
    inline void              SetEventName(const std::string & name)   { Event.Name = name; }
    //! Returns json container for event specific values
    inline JSON::JSONVALUE & GetEventSpecificJson(void) { return Event.Json; }

    /*! \addtogroup Getters
        @{
     */
    //! Get string representation of topic type
    static const std::string GetTopicTypeString(TopicType topicType);
    //! Get topic type enum from string
    static TopicType GetTopicTypeFromString(const std::string & topicTypeString);
    /*! @} */
};

};

#endif // _jsonSerializer_h
