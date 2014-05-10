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

//------------------------------------------------------------ 
//  state
//------------------------------------------------------------ 
void handler_state(const std::vector<std::string> &args)
{
    std::cout << "==== state\n";
    std::vector<std::string>::const_iterator it = args.begin();
    for (; it != args.end(); ++it)
        std::cout << *it << std::endl;
}
