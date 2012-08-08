/*

   Safety Framework for Component-based Robotics

   Created on: July 31, 2012

   Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)

 */

#ifndef _MONITOR_ICE
#define _MONITOR_ICE

//#include <communicator.ice>

module SF
{

interface MonitorSamples
{
    void CollectSample(string json);
};

interface SupervisorControls 
{
    void ControlCommand(string json);
};

};

#endif // _MONITOR_ICE
