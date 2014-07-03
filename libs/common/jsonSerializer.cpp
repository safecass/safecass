//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Sep 17, 2012
// Last revision: May 19, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "jsonSerializer.h"
#include "dict.h"

#if SF_HAS_CISST
#include "cisstDic.h"
#include "cisstEventLocation.h"
#else
#include "eventLocationBase.h"
#endif

using namespace SF;
//using namespace SF::Dict;

JSONSerializer::JSONSerializer(void)
{
    JSONBuffer.clear();
}

JSONSerializer::JSONSerializer(const std::string & json)
{
    if (!ParseJSON(json)) {
        SFLOG_ERROR << "JSONSerializer: failed to parse json: " << std::endl << json << std::endl;
        SFTHROW("JSONSerializer: failed to parse json");
    }
}

JSONSerializer::~JSONSerializer(void)
{
}

const std::string JSONSerializer::GetJSON(void) const
{
    std::stringstream ss;
    ss << JSONBuffer;

    return ss.str();
#if 0
    JSON::JSONVALUE json;

    // Common::common
    JSON::JSONVALUE jsonValue;
    jsonValue[topic]      = GetTopicTypeString(Common.Topic);
    jsonValue[filter_uid] = Common.FilterUID;
    // EventLocation and Timestamp are populated below as part of localization of event
    json[common] = jsonValue;
    
    // Common::localization
    JSON::JSONVALUE jsonLocation;
    if (Common.EventLocation) {
#if SF_HAS_CISST
        cisstEventLocation * cisstLocation = dynamic_cast<cisstEventLocation *>(Common.EventLocation);
        if (cisstLocation)
            cisstLocation->ExportToJSON(jsonLocation);
#else
        Common.EventLocation->ExportToJSON(jsonLocation);
#endif
    } else {
        jsonLocation[process]                = "";
        jsonLocation[component]              = "";
        jsonLocation[interface_provided]     = "";
        jsonLocation[interface_required]     = "";
#if SF_HAS_CISST
        jsonLocation[cisst::command]         = "";
        jsonLocation[cisst::function]        = "";
        jsonLocation[cisst::event_generator] = "";
        jsonLocation[cisst::event_handler]   = "";
#endif

    }

    JSON::JSONVALUE jsonLocalization;
    jsonLocalization[timestamp] = Common.Timestamp;
    jsonLocalization[location] = jsonLocation;
    json[localization] = jsonLocalization;

    // TODO: selectively combine strings

    // Common::Topic

    JSON::JSONVALUE jsonPayload;
    switch (Common.Topic) {
    case Topic::DATA:
        switch (
        jsonPayload = Monitor.Json;
        jsonPayload[category] = 

#if 0
                JSON::JSONVALUE _json(Monitor.Json);
                _json[type] = Monitor::GetTargetTypeString(Monitor.Type);

                json[monitor] = _root;
            }
            break;

        case EVENT:
            {
                JSON::JSONVALUE _json(Event.Json);
                _json[type] = Event::GetEventTypeString(Event.Type);
                //_json[detector] = Fault.DetectorName;

                json[event] = _root;
            }
            break;

        case SUPERVISOR:
            {
                // TODO
            }
            break;

        default:
            break;
    case Topic::CONTRO:
    }
#endif

    std::stringstream ss;
    ss << json;

    return ss.str();
#endif
}

bool JSONSerializer::ParseJSON(const std::string & message)
{
    JSON json;
    if (!json.Read(message.c_str())) {
        return false;
    }

    JSONBuffer = json.GetRoot();

#if 0
    Json::Value & values = json.GetRoot();

    // Populate common fields
    Common.FilterUID = values[common].get(filter_uid, 0).asInt();
    Common.Topic = GetTopicTypeFromString(values[common].get(topic, "").asString());
    Common.Timestamp = values[localization].get(timestamp, 0.0).asDouble();
#if SF_HAS_CISST
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
#endif

    return true;
}

//void JSONSerializer::SetEvent(const SF::Event & event)
//{
    //SetTopicType(Topic::DATA);
    //SetCategoryTypeData(Topic::Data::EVENT);

    //SetEventType(event.GetEventType());
    //SetEventName(event.GetName());
    //SetEventLocation(event.GetLocation());
    //SetTimestamp(event.GetTimestamp());
//}

//------------------------------------------------------------ 
// Accessors for common fields
//------------------------------------------------------------ 
void JSONSerializer::SetTopicType(const Topic::Type topicType)
{
    switch (topicType) {
    case Topic::DATA: 
        JSONBuffer[Dict::Json::topic] = Dict::TopicNames::DATA;
        break;
    case Topic::CONTROL:
        JSONBuffer[Dict::Json::topic] = Dict::TopicNames::CONTROL;
        break;
    default:
        JSONBuffer[Dict::Json::topic] = Dict::TopicNames::INVALID;
        break;
    }
}

void JSONSerializer::SetCategoryTypeControl(Topic::Control::CategoryType category)
{
    switch (category) {
    case Topic::Control::COMMAND:
        JSONBuffer[Dict::Json::category] = Dict::TopicNames::Control::COMMAND;
        break;
    default:
        JSONBuffer[Dict::Json::category] = Dict::TopicNames::INVALID;
    }
}

void JSONSerializer::SetFilterUID(const FilterBase::FilterIDType uid)
{
    JSONBuffer[Dict::Json::filter_uid] = uid;
}

void JSONSerializer::SetEventLocation(EventLocationBase * location)
{
    if (!location) {
        SFLOG_WARNING << "JSONSerializer::SetEventLocation: NULL location instance" << std::endl;
        return;
    }

    location->ImportFromJSON(JSONBuffer[Dict::Json::localization][Dict::Json::location]);
}

void JSONSerializer::SetTimestamp(TimestampType timestamp)
{
    JSONBuffer[Dict::Json::localization][Dict::Json::timestamp] = timestamp;
}

FilterBase::FilterIDType JSONSerializer::GetFilterUID(void) const
{
    return JSON::GetSafeValueUInt(JSONBuffer[Dict::Json::common], Dict::Json::filter_uid);
}

Topic::Type JSONSerializer::GetTopicType(void) const
{
    int value = JSON::GetSafeValueInt(JSONBuffer[Dict::Json::common], Dict::Json::topic);

    Topic::Type type = static_cast<Topic::Type>(value);

    return type;
}

#if SF_HAS_CISST
const cisstEventLocation & JSONSerializer::GetEventLocation(void) const
#else
const EventLocationBase & JSONSerializer::GetEventLocation(void) const
#endif
{
    return EventLocation;
}

TimestampType JSONSerializer::GetTimestamp(void) const
{
    return JSON::GetSafeValueDouble(JSONBuffer[Dict::Json::localization], Dict::Json::timestamp);
}

//------------------------------------------------------------ 
// Accessors for Data
//------------------------------------------------------------ 
void JSONSerializer::SetCategoryTypeData(Topic::Data::CategoryType category)
{
    switch (category) {
    case Topic::Data::MONITOR:
        JSONBuffer[Dict::Json::category] = Dict::TopicNames::Data::MONITOR;
        break;
    case Topic::Data::EVENT:
        JSONBuffer[Dict::Json::category] = Dict::TopicNames::Data::EVENT;
        break;
    case Topic::Data::READ_RES:
        JSONBuffer[Dict::Json::category] = Dict::TopicNames::Data::READ_RES;
        break;
    default:
        JSONBuffer[Dict::Json::category] = Dict::TopicNames::INVALID;
    }
}

//------------------------------------------------------------ 
// Accessors for Data/Event
//------------------------------------------------------------ 
#if 0
void JSONSerializer::SetEventType(Event::EventType eventType)
{
    // FIXME
    return;
    //JSONBuffer[Dict::Json::monitor][Dict::Json::type] = Event::EventType
}

void JSONSerializer::SetEventName(const std::string & name)
{
    // FIXME
    //Event.Name = name;
}

Event::EventType JSONSerializer::GetEventType(void) const 
{
    // FIXME
    return Event::INVALID;
}

JSON::JSONVALUE & JSONSerializer::GetEventSpecificJson(void)
{
    // FIXME
    return JSONBuffer;
}
#endif

//------------------------------------------------------------ 
// Accessors for Data/Monitor
//------------------------------------------------------------ 
void JSONSerializer::SetMonitorTargetType(Monitor::TargetType target)
{
    // FIXME
    //Monitor.Type = target;
}

Monitor::TargetType JSONSerializer::GetMonitorTargetType(void) const
{
    // FIXME
    return Monitor::TARGET_INVALID;
}

JSON::JSONVALUE & JSONSerializer::GetMonitorFields(void)
{
    // FIXME
    return JSONBuffer;
}

//------------------------------------------------------------ 
// Other Getters
//------------------------------------------------------------ 
#if 0
const std::string JSONSerializer::GetTopicTypeString(Topic::Type topicType)
{
    switch (topicType) {
    case Topic::DATA:    return Dict::TopicNames::Data;
    case Topic::CONTROL: return Dict::TopicNames::Control;
    default:             return Dict::TopicNames::Invalid;
    }
}

Topic::Type JSONSerializer::GetTopicTypeFromString(const std::string & topicTypeString)
{
    if (topicTypeString.compare(Dict::TopicNames::Control) == 0) return Topic::CONTROL;
    if (topicTypeString.compare(Dict::TopicNames::Data) == 0)    return Topic::DATA;

    return Dict::TopicNames::Invalid;
}
#endif
