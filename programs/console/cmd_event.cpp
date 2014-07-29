//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 3, 2014
// Last revision: Jul 29, 2014
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

void handler_event_help(void)
{
    std::cout << std::endl
              << "event [command]" << std::endl
              << "    help: show help for event command"  << std::endl
              << "    list: show all events in the system" << std::endl
              << "    generate: inject event into the system" << std::endl;
}

void handler_event_list(const std::string & safetyCoordinatorName, const std::string & componentName)
{
#define CASROS_ACCESSOR_CHECK\
    if (!casrosAccessor) {\
        std::cerr << "ERROR: accessor is not initialized" << std::endl;\
        return;\
    }
    CASROS_ACCESSOR_CHECK;

    if (!casrosAccessor->RequestEventList(safetyCoordinatorName, componentName)) {
        std::cerr << "ERROR: failed to request event information" << std::endl;
        return;
    }
}

void handler_event_generate(const std::string & eventName, const std::string & eventType, const std::string & safetyCoordinatorName,
                            const std::string & componentName, const std::string & interfaceName)
{
    CASROS_ACCESSOR_CHECK;

    if (!casrosAccessor->RequestEventGeneration(eventName, eventType, safetyCoordinatorName, componentName, interfaceName)) {
        std::cerr << "ERROR: failed to request event generation" << std::endl;
        return;
    }
}
#undef CASROS_ACCESSOR_CHECK

//------------------------------------------------------------ 
// event
//
// event help
// event list
//------------------------------------------------------------ 
typedef enum {
    HELP,  // show help
    LIST,  // show event list
    GENERATE // generate event
} EventOptionType;

void handler_event(const std::vector<std::string> & args)
{
    EventOptionType option;
    std::string safetyCoordinatorName, componentName;
    std::string eventName, eventType, interfaceName;

    const size_t n = args.size();

    std::string cmd(args[0]);
    SF::to_lowercase(cmd);

    if (cmd.compare("list") == 0)
        option = LIST;
    else if (cmd.compare("generate") == 0)
        option = GENERATE;
    else
        option = HELP;

    switch (option) {
    default:
    case HELP:
        handler_event_help();
        break;

    case LIST:
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
        handler_event_list(safetyCoordinatorName, componentName);
        break;

    case GENERATE:
        if (n < 5) {
            std::cout << "usage: event generate eName tName scName cName [iName]" << std::endl
                      << "   eName : event name (should be registered one)" << std::endl
                      << "   tName : event type. One of [\"framework\"|\"application\"|\"provided\"|\"required\"]" << std::endl
                      << "   scName: name of safety coordinator" << std::endl
                      << "   cName : name of target component" << std::endl
                      << "   iName : name of target interface (optional for \"framework\" and \"application\")" << std::endl;
            return;
        }
        eventName = args[1];
        eventType = args[2];
        safetyCoordinatorName = args[3];
        componentName = args[4];
        if (n == 6)
            interfaceName = args[5];
        else
            interfaceName = "";

        handler_event_generate(eventName, eventType, safetyCoordinatorName, componentName, interfaceName);
        break;
    }
}
