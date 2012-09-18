/*

  Safety Framework for Component-based Robotics

  Created on: September 17, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

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
#endif

namespace SF {

using namespace SF::Dict::Json;

JSONSerializer::JSONSerializer(void)
{
    Init();
}

void JSONSerializer::Init(void)
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
                cisstLocation->PopulateJSONValues(__root);
            }
#else
            Common.EventLocation->PopulateJSONValues(__root);
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
            // TODO
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
    }

    std::stringstream ss;
    ss << root;

    return ss.str();
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

};
