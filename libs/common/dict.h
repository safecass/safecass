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

namespace SF {

namespace Dict {

const std::string CONFIG = "Config";
const std::string EVENT = "Event";
const std::string IDENTIFIER = "Identifier";
const std::string INVALID = "INVALID";
const std::string MARGIN = "Margin";
const std::string NAME = "Name";
const std::string NAME_PROCESS = "Process";
const std::string NAME_COMPONENT = "Component";
const std::string NAME_INTERFACE_PROVIDED = "InterfaceProvided";
const std::string NAME_INTERFACE_REQUIRED = "InterfaceRequired";
const std::string NAME_EVENT_GENERATOR = "EventGenerator";
const std::string NAME_EVENT_HANDLER = "EventHandler";
const std::string ON = "On";
const std::string OFF = "Off";
const std::string OUTPUT = "Output";
const std::string PUBLISH = "Publish";
const std::string SAMPLE = "Sample";
const std::string SAMPLING_RATE = "SamplingRate";
const std::string STATE = "State";
const std::string STREAM = "Stream";
const std::string SUBSCRIBE = "Subscribe";
const std::string TARGET = "Target";
const std::string THRESHOLD = "Threshold";
const std::string TYPE = "Type";

namespace TopicNames {
    // For monitoring
    const std::string Monitor = "Monitor";

    // For supervisory control and fault management
    const std::string Supervisor = "Supervisor";
};

};

};

#endif // _dict_h
