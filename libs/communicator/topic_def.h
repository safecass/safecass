//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: May 8, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _topic_def_h
#define _topic_def_h

namespace SF {

namespace Topic {
    //! Typedef for topics in use
    typedef enum { INVALID, CONTROL, DATA } Type;

    namespace Control {
        //! Typedef for message categories of "Control" topic
        typedef enum { COMMAND } CategoryType;
    };

    namespace Data {
        // TODO: remove LOG
        //! Typedef for message categories of "Data" topic
        typedef enum { MONITOR, EVENT, LOG } CategoryType;
    }

};

};

#endif // _topic_def_h
