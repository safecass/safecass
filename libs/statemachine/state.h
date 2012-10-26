/*

  Safety Framework for Component-based Robotics

  Created on: October 26, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

  This file defines a set of states and events of the state machine of 
  Safety Framwork.  The definition and nomenclature follow Avizienis2004:
     
    A. Avizienis, J.-C. Laprie, B. Randell, and C. Landwehr., "Basic concepts and 
    taxonomy of dependable and secure computing," IEEE Trans. on Dependable and 
    Secure Computing, 1:11–33, Jan. 2004.

*/

#ifndef _state_h
#define _state_h

namespace SF {

    namespace State {

        /*! States */
        typedef enum {
            NORMAL,
            FAULT,
            ERROR,
            FAILURE,
            INVALID
        } StateType;

        /*! State entry and exit events */
        typedef enum {
            STATEMACHINE_ON_ENTRY,
            STATEMACHINE_ON_EXIT,
            NORMAL_ON_ENTRY,
            NORMAL_ON_EXIT,
            FAULT_ON_ENTRY,
            FAULT_ON_EXIT,
            ERROR_ON_ENTRY,
            ERROR_ON_EXIT,
            FAILURE_ON_ENTRY,
            FAILURE_ON_EXIT
        } StateEntryExitType;

        /*! State transition events */
        typedef enum {
            ON_ENTRY,
            ON_EXIT,
            FAULT_DETECTION,
            FAULT_REMOVAL,
            FAULT_ACTIVATION,
            ERROR_DETECTION,
            ERROR_REMOVAL,
            ERROR_PROPAGATION,
            FAILURE_DETECTION,
            FAILURE_REMOVAL,
            FAILURE_STOP
        } TransitionType;

    };

};

#endif // _state_h
