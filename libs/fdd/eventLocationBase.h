/*

  Safety Framework for Component-based Robotics

  Created on: September 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _EventLocationBase_h
#define _EventLocationBase_h

#include "common.h"

namespace SF {

class SFLIB_EXPORT EventLocationBase
{
protected:
    std::string ProcessName;
    std::string ComponentName;
    std::string InterfaceProvidedName;
    std::string InterfaceRequiredName;

public:
    EventLocationBase(void) {}
    virtual ~EventLocationBase() {}

    // Getters and Setters
#define DEFINE_ACCESSORS(_var)\
    inline const std::string Get##_var(void) const { return _var; }\
    inline void Set##_var(const std::string & _##_var) { _var = _##_var; }

    DEFINE_ACCESSORS(ProcessName);
    DEFINE_ACCESSORS(ComponentName);
    DEFINE_ACCESSORS(InterfaceProvidedName);
    DEFINE_ACCESSORS(InterfaceRequiredName);
#undef DEFINE_ACCESSORS
};

};

#endif // _EventLocationBase_h
