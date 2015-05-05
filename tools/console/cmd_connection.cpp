//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 23, 2014
// Last revision: Jul 23, 2014
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

extern AccessorConsole * casrosAccessor;

void handler_connection_help(void)
{
    std::cout << std::endl
              << "connection [ help | list ]" << std::endl
              << "    help: show help for connection command"  << std::endl
              << "    list: show all connections in the system" << std::endl;
}

void handler_connection_list(const std::string & safetyCoordinatorName,
                             const std::string & componentName)
{
    if (!casrosAccessor) {
        std::cerr << "ERROR: accessor is not initialized" << std::endl;
        return;
    }

    if (!casrosAccessor->RequestConnectionList(safetyCoordinatorName, componentName)) {
        std::cerr << "ERROR: failed to request connection information" << std::endl;
        return;
    }
}

//------------------------------------------------------------ 
// connection
//
// connection help
// connection list
//------------------------------------------------------------ 
typedef enum { HELP, LIST } ConnectionOptionType;

void handler_connection(const std::vector<std::string> & args)
{
    ConnectionOptionType option;
    std::string safetyCoordinatorName, componentName;

    const size_t n = args.size();

    std::string cmd(args[0]);
    SC::to_lowercase(cmd);
    if (cmd.compare("list") == 0) {
        option = LIST;
        safetyCoordinatorName = "*";
        componentName = "*";
    }
    else if (cmd.compare("help") == 0)
        option = HELP;
    else
        option = HELP;

    if (n == 1) {
        safetyCoordinatorName = "*";
        componentName = "*";
    }
    else if (n == 2) {
        safetyCoordinatorName = args[1];
        componentName = "*";
    }
    else if (n == 3) {
        safetyCoordinatorName = args[1];
        componentName = args[2];
    }
    else
        option = HELP;

    switch (option) {
    default:
    case HELP: handler_connection_help(); break;
    case LIST: handler_connection_list(safetyCoordinatorName, componentName); break;
    }
}
