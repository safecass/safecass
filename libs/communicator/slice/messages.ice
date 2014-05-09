//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : July 31, 2012
// Last revision: May 8, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _MONITOR_ICE
#define _MONITOR_ICE

//#include <communicator.ice>

module SF
{
    // interface for the "Control" topic
    interface Control
    {
        // for sending control command between Safety Coordinator and/or Safety Supervisor
        void Command(string arg);
    };

    // interface for the "Data" topic
    // all string arguments are json-encoded.
    interface Data
    {
        // for monitoring
        void Monitor(string arg);

        // for reporting events
        void Event(string arg);

        // for logging
        void Log(string arg);
    };
};

#endif // _MONITOR_ICE
