//---------------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//---------------------------------------------------------------------------------------
//
// Created on   : Jul 14, 2012
// Last revision: May 12, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// URL          : https://github.com/safecass/safecass
//
#ifndef _dict_h
#define _dict_h

#include <string>

namespace SC {

namespace Dict {

#define DECLARE_KEYWORD( _keyword )\
extern const std::string _keyword;

// Filters
DECLARE_KEYWORD(FILTERING_INTERNAL);
DECLARE_KEYWORD(FILTERING_EXTERNAL);
DECLARE_KEYWORD(STATE_INIT);
DECLARE_KEYWORD(STATE_DISABLED);
DECLARE_KEYWORD(STATE_ENABLED);
DECLARE_KEYWORD(STATE_DETECTED);

// Events
DECLARE_KEYWORD(PROCESS);
DECLARE_KEYWORD(COMPONENT);
DECLARE_KEYWORD(INTERFACE_REQUIRED);
DECLARE_KEYWORD(INTERFACE_PROVIDED);
DECLARE_KEYWORD(INTERFACE_PROVIDED);
DECLARE_KEYWORD(EVENT_DETECTION_LEVEL);
DECLARE_KEYWORD(EVENT_DETECTION_EDGE);
DECLARE_KEYWORD(EVENT_TRANSITION_N2W);
DECLARE_KEYWORD(EVENT_TRANSITION_W2E);
DECLARE_KEYWORD(EVENT_TRANSITION_N2E);
DECLARE_KEYWORD(EVENT_TRANSITION_NW2E);
DECLARE_KEYWORD(EVENT_TRANSITION_W2N);
DECLARE_KEYWORD(EVENT_TRANSITION_E2W);
DECLARE_KEYWORD(EVENT_TRANSITION_E2N);
DECLARE_KEYWORD(EVENT_TRANSITION_EW2N);
DECLARE_KEYWORD(EVENT_ATTR_NAME);
DECLARE_KEYWORD(EVENT_ATTR_SEVERITY);
DECLARE_KEYWORD(EVENT_ATTR_TRANSITION);
DECLARE_KEYWORD(EVENT_ATTR_TIMESTAMP);
DECLARE_KEYWORD(EVENT_ATTR_WHAT);
DECLARE_KEYWORD(EVENT_ATTR_ACTIVE);
DECLARE_KEYWORD(EVENT_ATTR_IGNORED);


// Miscellaneous
DECLARE_KEYWORD(INVALID);

#if 0
DEFINE_KEYWORD(DETECTED);
DEFINE_KEYWORD(DISABLED);
DEFINE_KEYWORD(ENABLED);
DEFINE_KEYWORD(EVENT);
DEFINE_KEYWORD(INVALID);
DEFINE_KEYWORD(OFF);
DEFINE_KEYWORD(ON);
DEFINE_KEYWORD(STREAM);

// Base filter - filtering mode
DEFINE_KEYWORD(ACTIVE);
DEFINE_KEYWORD(PASSIVE);

// Base filter - event detection type
DEFINE_KEYWORD(EDGE);
DEFINE_KEYWORD(LEVEL);

namespace Json {
    DEFINE_KEYWORD(category);
    DEFINE_KEYWORD(class_name);
    DEFINE_KEYWORD(component);
    DEFINE_KEYWORD(config);
    DEFINE_KEYWORD(command);
    DEFINE_KEYWORD(common);
    DEFINE_KEYWORD(custom);
    DEFINE_KEYWORD(data);
    DEFINE_KEYWORD(detector);
    DEFINE_KEYWORD(dutycycle_total);
    DEFINE_KEYWORD(dutycycle_total_ratio);
    DEFINE_KEYWORD(dutycycle_user);
    DEFINE_KEYWORD(dutycycle_user_ratio);
    DEFINE_KEYWORD(event);
    DEFINE_KEYWORD(event_generator);
    DEFINE_KEYWORD(event_handler);
    DEFINE_KEYWORD(fault);
    DEFINE_KEYWORD(fault_application);
    DEFINE_KEYWORD(filter);
    DEFINE_KEYWORD(filter_uid);
    DEFINE_KEYWORD(filter_event); // MJTEMP: am I using this?
    DEFINE_KEYWORD(function);
    DEFINE_KEYWORD(interface_provided);
    DEFINE_KEYWORD(interface_required);
    DEFINE_KEYWORD(invalid);
    DEFINE_KEYWORD(localization);
    DEFINE_KEYWORD(location);
    DEFINE_KEYWORD(MARGIN);
    DEFINE_KEYWORD(monitor);
    DEFINE_KEYWORD(name);
    DEFINE_KEYWORD(off);
    DEFINE_KEYWORD(on);
    DEFINE_KEYWORD(output);
    DEFINE_KEYWORD(output_type);
    DEFINE_KEYWORD(period);
    DEFINE_KEYWORD(period_actual);
    DEFINE_KEYWORD(period_nominal);
    DEFINE_KEYWORD(process);
    DEFINE_KEYWORD(PUBLISH);
    DEFINE_KEYWORD(sampling_rate);
    DEFINE_KEYWORD(severity);
    DEFINE_KEYWORD(state);
    DEFINE_KEYWORD(stream);
    DEFINE_KEYWORD(SUBSCRIBE);
    DEFINE_KEYWORD(target_component);
    DEFINE_KEYWORD(thread_period);
    DEFINE_KEYWORD(thread_dutycycle_user);
    DEFINE_KEYWORD(thread_dutycycle_total);
    DEFINE_KEYWORD(time);
    DEFINE_KEYWORD(timestamp);
    DEFINE_KEYWORD(topic);
    DEFINE_KEYWORD(type);
    DEFINE_KEYWORD(value);
    DEFINE_KEYWORD(values);
};

namespace TopicNames {
    // names for topics
    DEFINE_KEYWORD(CONTROL);
    DEFINE_KEYWORD(DATA);
    DEFINE_KEYWORD(INVALID);

    // names for categories
    namespace Control {
        DEFINE_KEYWORD(COMMAND);
        DEFINE_KEYWORD(READ_REQ);
        DEFINE_KEYWORD(STATE_UPDATE);
    }
    namespace Data {
        DEFINE_KEYWORD(MONITOR);
        DEFINE_KEYWORD(EVENT);
        DEFINE_KEYWORD(READ_RES);
    }
};

namespace MonitorNames {
    // Monitor can generate events; for event-type monitoring
    DEFINE_KEYWORD(MonitorEvent);
};

namespace FaultNames {
    // Used fault event generation and propagation
    DEFINE_KEYWORD(FaultEvent);
};

// Filters and filter-specific keywords
namespace Filter {
    DEFINE_KEYWORD(argument);
    DEFINE_KEYWORD(input_signal);
    DEFINE_KEYWORD(last_filter);
};

namespace FilterThreshold {
    DEFINE_KEYWORD(threshold);
    DEFINE_KEYWORD(margin);
    DEFINE_KEYWORD(output0);
    DEFINE_KEYWORD(output1);
}
#endif

}; // SC::Dict

}; // SC

#endif // _dict_h
