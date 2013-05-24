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
    Common.Topic         = JSONSerializer::INVALID;
    Common.EventLocation = 0;
    Common.Timestamp     = 0;

    Monitor.Type = Monitor::TARGET_INVALID;
    Monitor.Fields.clear();

    Fault.Type = Fault::FAULT_INVALID;
    Fault.DetectorName = "";
    Fault.Fields.clear();
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
        case MONITOR: return STR(monitor);
        case FAULT: return STR(fault);
        case SUPERVISOR: return STR(supervisor);
        default: return STR(INVALID);
    }
}

JSONSerializer::TopicType JSONSerializer::GetTopicTypeFromString(const std::string & topicTypeString)
{
    if (topicTypeString.compare(STR(monitor)) == 0) return MONITOR;
    if (topicTypeString.compare(STR(fault)) == 0) return FAULT;
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
    ::Json::Value root;

    // Common::identity
    {   ::Json::Value _root;
        _root[topic] = GetTopicTypeString(Common.Topic);

        root[identity] = _root;
    }
    
    // Common::localization
    {   ::Json::Value _root;
        _root[timestamp] = Common.Timestamp;

        ::Json::Value __root;
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

    switch (Common.Topic) {
        case MONITOR:
            {
                ::Json::Value _root(Monitor.Fields);
                _root[type] = Monitor::GetTargetTypeString(Monitor.Type);

                root[monitor] = _root;
            }
            break;

        case FAULT:
            {
                ::Json::Value _root(Fault.Fields);
                _root[type] = Fault::GetFaultTypeString(Fault.Type);
                _root[detector] = Fault.DetectorName;

                root[fault] = _root;
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
    Common.Topic = GetTopicTypeFromString(values[identity].get(topic, "").asString());
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

                Monitor.Fields = values[monitor];
                Monitor.Fields.removeMember(type);
            }
            break;

        case FAULT:
            {
                Fault.Type = Fault::GetFaultTypeFromString(values[fault].get(type, "").asString());
                Fault.DetectorName = values[fault].get(detector, "").asString();

                Fault.Fields = values[fault];
                Fault.Fields.removeMember(type);
                Fault.Fields.removeMember(detector);
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

};
