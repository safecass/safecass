//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: May 10, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "config.h"
#include "publisher.h"
#include "subscriber.h"
#include "dict.h"
#include "topic_def.h"
#include "accessor.h"
#include "utils.h"

// Ice communicator instances
extern Accessor * casrosAccessor;

void handler_filter_help(void)
{
    std::cout << std::endl
              << "filter [command]" << std::endl
              << "    help: show help for filter command"  << std::endl
              << "    list: list of all filters in the entire system" << std::endl;
}

void handler_filter_list(void)
{
    //if (!comm->Publisher->PublishData(SF::Topic::Data::MONITOR, "{ test json }"))
    //    std::cout << "Failed to publish" << std::endl;
    std::cout << "list" << std::endl;
}

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
typedef enum { HELP, LIST } FilterOptionType;

void handler_filter(const std::vector<std::string> & args)
{
#if 0
    std::cout << "==== filter\n";
    std::vector<std::string>::const_iterator it = args.begin();
    for (; it != args.end(); ++it)
        std::cout << *it << std::endl;
#endif

    FilterOptionType option;

    const size_t n = args.size();

    if (n == 1) {
        // filter command
        std::string cmd(args[0]);
        SF::to_lowercase(cmd);

        if (cmd.compare("list") == 0)
            option = LIST;
        else if (cmd.compare("help") == 0)
            option = HELP;
        else
            option = HELP;
    }
    else
        option = HELP;

    switch (option) {
    default:
    case HELP: handler_filter_help(); break;
    case LIST: handler_filter_list(); break;
    }
}
