//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: Jul 1, 2014
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

void handler_state_help(void)
{
    std::cout << std::endl
              << "state [command]" << std::endl
              << "    help: show help for state command"  << std::endl
              << "    list: show all states in the system" << std::endl;
}

void handler_state_list(const std::string & safetyCoordinatorName,
                        const std::string & componentName)
{
    if (!casrosAccessor) {
        std::cerr << "ERROR: accessor is not initialized" << std::endl;
        return;
    }

    if (!casrosAccessor->RequestStateList(safetyCoordinatorName, componentName)) {
        std::cerr << "ERROR: failed to request state information" << std::endl;
        return;
    }
}


//------------------------------------------------------------ 
// state
//
// state help
// state list
//------------------------------------------------------------ 
typedef enum { HELP, LIST } StateOptionType;

void handler_state(const std::vector<std::string> &args)
{
    StateOptionType option;
    std::string safetyCoordinatorName, componentName;

    const size_t n = args.size();

    std::string cmd(args[0]);
    SF::to_lowercase(cmd);
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
    case HELP: handler_state_help(); break;
    case LIST: handler_state_list(safetyCoordinatorName, componentName); break;
    }
}
