//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: May 9, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "config.h"
#include "publisher.h"
#include "subscriber.h"
#include "dict.h"
#include "topic_def.h"
#include "communicator_ice.h"

// Ice communicator instances
extern Communicator * comm;

//------------------------------------------------------------ 
//  filter
//
//  filter help
//  filter list
//
//  filter propedit fuid enable
//  filter propedit fuid disable
//  filter propedit fuid debug [on|off]
//  filter info fuid [filter_uid]
//  filter info process [process_name]
//  filter info process [process_name] component [component_name]
//------------------------------------------------------------ 
void handler_filter(const std::vector<std::string> & args)
{
    const size_t n = args.size();
    if (n == 1)
#if 0
    std::cout << "==== filter\n";
    std::vector<std::string>::const_iterator it = args.begin();
    for (; it != args.end(); ++it)
        std::cout << *it << std::endl;
#endif
    if (!comm->Publisher->PublishData(SF::Topic::Data::MONITOR, "{ test json }"))
        std::cout << "Failed to publish" << std::endl;
}
