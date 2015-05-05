//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: Jul 17, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _topic_def_h
#define _topic_def_h

namespace SC {

namespace Topic {
    //! Typedef for topics in use
    typedef enum {
        CONTROL = 0,
        DATA,
        TOTAL_TOPIC_COUNT,
        INVALID
    } Type;

    namespace Control {
        //! Typedef for message categories of "Control" topic
        typedef enum {
            // Execute commands to change states or values of the system
            COMMAND,
            // Request to fetch current values of the system
            READ_REQ,
            // Inform other components of state change (error and state change event propagation)
            STATE_UPDATE
        } CategoryType;
    };

    namespace Data {
        //! Typedef for message categories of "Data" topic
        typedef enum {
            // Data stream from monitoring components
            MONITOR,
            // Event data from filters
            EVENT,
            // Response containing data requested (response to Control/READ message)
            READ_RES
        } CategoryType;
    }
};

};

#endif // _topic_def_h
