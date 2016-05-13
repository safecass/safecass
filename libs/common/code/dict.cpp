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
// Github       : https://github.com/safecass/safecass
//
#include "common/dict.h"
#include "common/utils.h"

namespace SC {

namespace Dict {

#define DEFINE_KEYWORD( _keyword )\
    const std::string _keyword = to_lowercase(#_keyword);

#define DEFINE_KEYWORD_PAIR( _keyword, _value )\
    const std::string _keyword = _value;

// Filters
DEFINE_KEYWORD_PAIR(FILTERING_INTERNAL, "internal");
DEFINE_KEYWORD_PAIR(FILTERING_EXTERNAL, "external");
DEFINE_KEYWORD_PAIR(STATE_INIT, "init");
DEFINE_KEYWORD_PAIR(STATE_DISABLED, "disabled");
DEFINE_KEYWORD_PAIR(STATE_ENABLED, "enabled");
DEFINE_KEYWORD_PAIR(STATE_DETECTED, "detected");

// Events
DEFINE_KEYWORD(PROCESS);
DEFINE_KEYWORD(COMPONENT);
DEFINE_KEYWORD_PAIR(INTERFACE_REQUIRED, "required interface");
DEFINE_KEYWORD_PAIR(INTERFACE_PROVIDED, "provided interface");
DEFINE_KEYWORD_PAIR(EVENT_DETECTION_LEVEL, "level trigger");
DEFINE_KEYWORD_PAIR(EVENT_DETECTION_EDGE, "edge trigger");

DEFINE_KEYWORD_PAIR(EVENT_TRANSITION_N2W, "N2W");
DEFINE_KEYWORD_PAIR(EVENT_TRANSITION_W2E, "W2E");
DEFINE_KEYWORD_PAIR(EVENT_TRANSITION_NW2E, "NW2E");
DEFINE_KEYWORD_PAIR(EVENT_TRANSITION_W2N, "W2N");
DEFINE_KEYWORD_PAIR(EVENT_TRANSITION_E2N, "E2N");
DEFINE_KEYWORD_PAIR(EVENT_TRANSITION_EW2N, "EW2N");

DEFINE_KEYWORD_PAIR(EVENT_ATTR_NAME, "name");
DEFINE_KEYWORD_PAIR(EVENT_ATTR_SEVERITY, "severity");
DEFINE_KEYWORD_PAIR(EVENT_ATTR_TRANSITION, "transition");
DEFINE_KEYWORD_PAIR(EVENT_ATTR_TIMESTAMP, "timestamp");
DEFINE_KEYWORD_PAIR(EVENT_ATTR_WHAT, "what");
DEFINE_KEYWORD_PAIR(EVENT_ATTR_ACTIVE, "active");
DEFINE_KEYWORD_PAIR(EVENT_ATTR_IGNORED, "ignored");

// Miscellaneous
DEFINE_KEYWORD(INVALID);

}; // Dict

}; // SC
