//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : July 31, 2012
// Last revision: May 7, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _MONITOR_ICE
#define _MONITOR_ICE

//#include <communicator.ice>

module SF
{

    interface Control
    {
        void Command(string json);
    };

    interface Data
    {
        // for monitoring
        void CollectSample(string json);

        // Response to FilterInfoRequest
        void Response(string json);
    };
/*
interface MonitorSamples
{
    void CollectSample(string json);
};

interface SupervisorControls
{
    void ControlCommand(string json);
};
*/

};

#endif // _MONITOR_ICE
