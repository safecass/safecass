//---------------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//---------------------------------------------------------------------------------------
//
// Created on   : Jul 14, 2012
// Last revision: Apr 28, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "common/dict.h"
#include "common/utils.h"

namespace SC {

namespace Dict {

#define DEFINE_KEYWORD( _keyword )\
const std::string _keyword = to_lowercase(#_keyword);

// Filters
DEFINE_KEYWORD(FILTERING_INTERNAL);
DEFINE_KEYWORD(FILTERING_EXTERNAL);
DEFINE_KEYWORD(STATE_INIT);
DEFINE_KEYWORD(STATE_DISABLED);
DEFINE_KEYWORD(STATE_ENABLED);
DEFINE_KEYWORD(STATE_DETECTED);

// Events
DEFINE_KEYWORD(PROCESS);
DEFINE_KEYWORD(COMPONENT);
DEFINE_KEYWORD(INTERFACE_REQUIRED);
DEFINE_KEYWORD(INTERFACE_PROVIDED);
DEFINE_KEYWORD(EVENT_DETECTION_LEVEL);
DEFINE_KEYWORD(EVENT_DETECTION_EDGE);

// Miscellaneous
DEFINE_KEYWORD(INVALID);

}; // Dict

}; // SC
