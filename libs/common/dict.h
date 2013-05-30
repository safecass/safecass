/*

  Safety Framework for Component-based Robotics

  Created on: July 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _dict_h
#define _dict_h

#include "common.h"

#define DEFINE_KEYWORD(_key) const std::string _key = #_key;

namespace SF {

    namespace Dict {

        DEFINE_KEYWORD(EVENT);
        DEFINE_KEYWORD(INVALID);
        DEFINE_KEYWORD(ON);
        DEFINE_KEYWORD(OFF);
        DEFINE_KEYWORD(STREAM);

        namespace Json {
            DEFINE_KEYWORD(component);
            DEFINE_KEYWORD(config);
            DEFINE_KEYWORD(command);
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
            DEFINE_KEYWORD(filter_event); // MJTEMP: am I using this?
            DEFINE_KEYWORD(function);
            DEFINE_KEYWORD(identity);
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
            DEFINE_KEYWORD(THRESHOLD);
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
            // For monitoring
            DEFINE_KEYWORD(Monitor);
            // For fault detection and diagnosis 
            DEFINE_KEYWORD(Fault);
            // For supervisory control and fault management
            DEFINE_KEYWORD(Supervisor);
        };

        namespace MonitorNames {
            // Monitor can generate events; for event-type monitoring
            DEFINE_KEYWORD(MonitorEvent);
        };

        namespace FaultNames {
            // Used fault event generation and propagation
            DEFINE_KEYWORD(FaultEvent);
        };

    }; // SF::Dict

}; // SF

#endif // _dict_h
