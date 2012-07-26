/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _fault_h
#define _fault_h

#include "common.h"

namespace SF {

class Fault {
public:
    // [SFUPDATE]
    typedef enum {
        FAULT_INVALID,
        FAULT_COMPONENT_PERIOD,   // THREAD_SCHEDULING_LATENCY
    } FaultType;

public:
    Fault();
    virtual ~Fault();

    /*! Return string that corresponds to fault type */
    static const std::string GetFaultString(FaultType faultType);
    /*! Return fault type from string */
    static FaultType GetFaultFromString(const std::string & faultString);
};

};

#endif // _fault_h
