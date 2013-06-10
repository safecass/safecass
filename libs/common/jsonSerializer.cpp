/*

  Safety Framework for Component-based Robotics

  Created on: September 17, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

  References:

  - JsonCpp Documentation: http://jsoncpp.sourceforge.net

*/

#include "jsonSerializer.h"
#include "dict.h"

#ifdef SF_HAS_CISST
#include "cisstDic.h"
#include "cisstEventLocation.h"
#else
#include "eventLocationBase.h"
#endif

namespace SF {

using namespace SF::Dict::Json;

JSONSerializer::JSONSerializer(void)
{
    Initialize();
}

void JSONSerializer::Initialize(void)
{
    Common.FilterUID     = 0;
    Common.Topic         = JSONSerializer::INVALID;
    Common.EventLocation = 0;
    Common.Timestamp     = 0;

    Monitor.Type = Monitor::TARGET_INVALID;
    Monitor.Json.clear();

    Event.Type = Event::INVALID;
    Event.Json.clear();
}

JSONSerializer::~JSONSerializer(void)
{
    if (Common.EventLocation) {
        // MJFIXME: Why this crashes???
        // supervisor(18886,0x111c78000) malloc: *** error for object 0x7fff779c9570:
        // pointer being freed was not allocated
        // *** set a breakpoint in malloc_error_break to debug
        // Abort trap: 6
        //delete Common.EventLocation;
    }
}

const std::string JSONSerializer::GetTopicTypeString(TopicType topicType)
{
    switch (topicType) {
        case MONITOR:    return STR(monitor);
        case EVENT:      return STR(event);
        case SUPERVISOR: return STR(supervisor);
        default:         return STR(INVALID);
    }
}

JSONSerializer::TopicType JSONSerializer::GetTopicTypeFromString(const std::string & topicTypeString)
{
    if (topicTypeString.compare(STR(monitor)) == 0)    return MONITOR;
    if (topicTypeString.compare(STR(event)) == 0)      return EVENT;
    if (topicTypeString.compare(STR(supervisor)) == 0) return SUPERVISOR;

    return INVALID;
}

void JSONSerializer::SetEventLocation(EventLocationBase * location)
{
    if (Common.EventLocation) {
        delete Common.EventLocation;
        Common.EventLocation = 0;
    }

#ifdef SF_HAS_CISST
    cisstEventLocation * cisstLocation = dynamic_cast<cisstEventLocation *>(location);
    if (cisstLocation) {
        Common.EventLocation = new cisstEventLocation;
        *Common.EventLocation = *cisstLocation;
    } else {
        SFLOG_ERROR << "JSONSerializer::SetEventLocation: invalid cisstEventLocation instance" << std::endl;
        SFASSERT(false);
    }
#else
    // MJ TODO: implement this later
    SFASSERT(false);
#endif
}

const std::string JSONSerializer::GetJSON(void) const
{
    JSON::JSONVALUE root;

    // Common::common
    {   JSON::JSONVALUE _root;
        _root[filter_uid] = Common.FilterUID;
        _root[topic] = GetTopicTypeString(Common.Topic);

        root[common] = _root;
    }
    
    // Common::localization
    {   JSON::JSONVALUE _root;
        _root[timestamp] = Common.Timestamp;

        JSON::JSONVALUE __root;
        if (Common.EventLocation) {
#ifdef SF_HAS_CISST
            cisstEventLocation * cisstLocation = dynamic_cast<cisstEventLocation *>(Common.EventLocation);
            if (cisstLocation) {
                cisstLocation->ExportToJSON(__root);
            }
#else
            Common.EventLocation->ExportToJSON(__root);
#endif
        } else {
            __root[process]                = "";
            __root[component]              = "";
            __root[interface_provided]     = "";
            __root[interface_required]     = "";
#ifdef SF_HAS_CISST
            __root[cisst::command]         = "";
            __root[cisst::function]        = "";
            __root[cisst::event_generator] = "";
            __root[cisst::event_handler]   = "";
#endif

        }
        _root[location] = __root;
        root[localization] = _root;
    }

    // Common::Topic
    switch (Common.Topic) {
        case MONITOR:
            {
                JSON::JSONVALUE _root(Monitor.Json);
                _root[type] = Monitor::GetTargetTypeString(Monitor.Type);

                root[monitor] = _root;
            }
            break;

        case EVENT:
            {
                JSON::JSONVALUE _root(Event.Json);
                _root[type] = Event::GetEventTypeString(Event.Type);
                //_root[detector] = Fault.DetectorName;

                root[event] = _root;
            }
            break;

        case SUPERVISOR:
            {
                // TODO
            }
            break;

        default:
            break;
    }

    std::stringstream ss;
    ss << root;

    return ss.str();
}

bool JSONSerializer::ParseJSON(const std::string & message)
{
    JSON json;
    if (!json.Read(message.c_str())) {
        return false;
    }

    Json::Value & values = json.GetRoot();

    // Populate common fields
    Common.FilterUID = values[common].get(filter_uid, 0).asInt();
    Common.Topic = GetTopicTypeFromString(values[common].get(topic, "").asString());
    Common.Timestamp = values[localization].get(timestamp, 0.0).asDouble();
#ifdef SF_HAS_CISST
    Common.EventLocation = new cisstEventLocation;
    Common.EventLocation->ImportFromJSON(values[localization][location]);
#else
    //Common.EventLocation = 0; // MJ TODO: deal with this later
    SFASSERT(false);
#endif

    switch (Common.Topic) {
        case MONITOR:
            {
                Monitor.Type = Monitor::GetTargetTypeFromString(values[monitor].get(type, "").asString());

                Monitor.Json= values[monitor];
                Monitor.Json.removeMember(type);
            }
            break;

        case EVENT:
            {
                Event.Type = Event::GetEventTypeFromString(values[event].get(type, "").asString());
                //Fault.DetectorName = values[fault].get(detector, "").asString();

                Event.Json = values[event];
                Event.Json.removeMember(type);
                Event.Json.removeMember(detector);
            }
            break;

        case SUPERVISOR:
            {
                // TODO
            }
            break;

        default:
            break;
    }
    // Populate monitor fields

    // Populate fault fields

    return true;
}

void JSONSerializer::SetEvent(const SF::Event & event)
{
    SetTopicType(JSONSerializer::EVENT);

    SetEventType(event.GetEventType());
    SetEventName(event.GetName());
    SetEventLocation(event.GetLocation());
    SetTimestamp(event.GetTimestamp());
}

};
