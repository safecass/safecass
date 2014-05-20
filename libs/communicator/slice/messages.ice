//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : July 31, 2012
// Last revision: May 19, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// This defines the Slice interface for IceStorm.  For more details, refer to
// libs/communicator/topic_def.h file.
//
#ifndef _MONITOR_ICE
#define _MONITOR_ICE

//#include <communicator.ice>

module SF
{
    // interface for the "Control" topic
    interface Control {
        void Command(string arg);
        void ReadReq(string arg);
    };

    // interface for the "Data" topic. All string arguments are json-encoded.
    interface Data {
        void Monitor(string arg);
        void Event(string arg);
        void ReadRes(string arg);
    };
};

#endif // _MONITOR_ICE
