//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Sep 17, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#ifndef _jsonSerializer_h
#define _jsonSerializer_h

#include "jsonwrapper.h"
#include "monitor.h"
#include "event.h"
#include "filterBase.h"
//#include "topic_def.h"
//#if SC_HAS_CISST
//#include "cisstEventLocation.h"
//#else
//#include "eventLocation.h"
//#endif

namespace SC {

//! JSON serializer and deserializer
/** 
    JSONSerializer implements serialization and deserialization of JSON messages
    for data exchange within SAFECASS.
*/
class EventLocationBase;
class SCLIB_EXPORT JSONSerializer {
protected:
    //! JSON value buffer
    JsonWrapper::JsonValue JSONBuffer;

//#if SC_HAS_CISST
//    cisstEventLocation EventLocation;
//#else
    EventLocationBase  * EventLocation;
//#endif

public:
    //! Constructor
    JSONSerializer(void);
    JSONSerializer(const std::string & json);

    //! Destructor
    ~JSONSerializer(void);

    //! Serialize current information into json format and return it as string
    const std::string GetJSON(void) const;

    //! Rebuild topic information based on JSON
    bool ParseJSON(const std::string & message);


    /** @name Accessors for common fields of all topics
     *  Getters and setters for common fields of all topics
     */
    /** @{ */
    FilterBase::FilterIDType  GetFilterUID(void) const;
    Topic::Type               GetTopicType(void) const;
#if SC_HAS_CISST
    const cisstEventLocation & GetEventLocation(void) const;
#else
    const EventLocationBase & GetEventLocation(void) const;
#endif
    double                    GetTimestamp(void) const;
    void SetFilterUID(const FilterBase::FilterIDType uid);
    void SetTopicType(const Topic::Type topicType);
    void SetEventLocation(EventLocationBase * location);
    void SetTimestamp(TimestampType timestamp);
    /** @} */

    /** @name Accessors for Control
     *  Getters and setters for Control messages
     */
    /** @{ */
    void SetCategoryTypeControl(Topic::Control::CategoryType category);
    /** @} */

    /** @name Accessors for Data messages
     *  Getters and setters for common fields of Data messages
     */
    /** @{ */
    void SetCategoryTypeData(Topic::Data::CategoryType category);
    /** @} */

    /** @name Accessors for Data/Monitor messages
     *  Getters and setters for Data/Monitor messages
     */
    /** @{ */
    Monitor::TargetType GetMonitorTargetType(void) const;
    JsonWrapper::JsonValue &   GetMonitorFields(void);
    void SetMonitorTargetType(Monitor::TargetType target);
    /** @} */

    /** @name Accessors for Data with Event
     *  Getters and setters for Data/Event messages
     */
    /** @{ */
    //! Event::Type
    //Event::EventType  GetEventType(void) const;
    //JsonWrapper::JsonValue & GetEventSpecificJson(void);
    //void SetEvent(const SC::Event & event);
    //void SetEventType(Event::EventType eventType);
    //void SetEventName(const std::string & name);
    /** @} */

    /** @name Accessors for Data with Log
     *  Getters and setters for Data/Log messages
     */
    /** @{ */
    /** @} */

    /** @name Getters
     *  Other getters
     */
    /** @{ */
    //! Get string representation of topic type
    //static const std::string GetTopicTypeString(Topic::Type topicType);
    //! Get topic type enum from string
    //static Topic::Type GetTopicTypeFromString(const std::string & topicTypeString);
    /** @} */
};

};

#endif // _jsonSerializer_h
