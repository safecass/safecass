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
    DEFINE_KEYWORD(CONFIG);
    DEFINE_KEYWORD(IDENTIFIER);
    DEFINE_KEYWORD(MARGIN);
    DEFINE_KEYWORD(NAME);
    DEFINE_KEYWORD(NAME_PROCESS);
    DEFINE_KEYWORD(NAME_COMPONENT);
    DEFINE_KEYWORD(NAME_INTERFACE_PROVIDED);
    DEFINE_KEYWORD(NAME_INTERFACE_REQUIRED);
    DEFINE_KEYWORD(NAME_EVENT_GENERATOR);
    DEFINE_KEYWORD(NAME_EVENT_HANDLER);
    DEFINE_KEYWORD(OUTPUT);
    DEFINE_KEYWORD(PUBLISH);
    DEFINE_KEYWORD(SAMPLE);
    DEFINE_KEYWORD(SAMPLING_RATE);
    DEFINE_KEYWORD(STATE);
    DEFINE_KEYWORD(SUBSCRIBE);
    DEFINE_KEYWORD(TARGET);
    DEFINE_KEYWORD(THRESHOLD);
    DEFINE_KEYWORD(TYPE);
};

namespace TopicNames {
    // For monitoring
    DEFINE_KEYWORD(Monitor);
    // For supervisory control and fault management
    DEFINE_KEYWORD(Supervisor);
};

namespace FaultNames {
    // Used fault event generation and propagation across the system and SF.
    DEFINE_KEYWORD(FaultEvent);
};

}; // SF::Dict

}; // SF

#endif // _dict_h
