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
    /*
    typedef enum { 
        LAYER_SYSTEM, 
        LAYER_PROCESS, 
        LAYER_COMPONENT, 
        LAYER_INTERFACE, 
        //LAYER_EXECUTION,
        LAYER_INVALID
    } LayerType;
    */

    // [SFUPDATE]
    typedef enum {
        FAULT_INVALID,
        FAULT_COMPONENT_PERIOD,   // Timing fault: OS thread scheduling latency
        FAULT_COMPONENT_OVERRUN   // Timing fault: thread overrun
    } FaultType;

public:
    Fault();
    virtual ~Fault();

    /*! Return string that corresponds to fault type */
    static const std::string GetFaultTypeString(FaultType faultType);
    /*! Return fault type from string */
    static FaultType GetFaultTypeFromString(const std::string & faultString);
};

};

#endif // _fault_h
