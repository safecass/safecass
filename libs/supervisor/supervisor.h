/*

  Safety Framework for Component-based Robotics

  Created on: August 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _supervisor_h
#define _supervisor_h

#include "common.h"
//#include "monitor.h"

namespace SF {

class SFLIB_EXPORT Supervisor {
public:
    Supervisor();
    virtual ~Supervisor();
};

};

#endif // _supervisor_h
