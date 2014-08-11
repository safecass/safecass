//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: Aug 11, 2014
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
#include "filterBase.h"

extern AccessorConsole * casrosAccessor;

void handler_filter_help(void)
{
    std::cout << std::endl
              << "filter [command]" << std::endl
              << "    help   : show help for filter command"  << std::endl
              << "    list   : show all filters in the system" << std::endl
              << "    info   : show all filters in the system" << std::endl
              << "    inject : shallow fault injection (modifies filter input; no actual value changes)" << std::endl
              << "    dinject: deep fault injection (modifies actual values)" << std::endl;
}

void handler_filter_list(const std::string & safetyCoordinatorName,
                         const std::string & componentName)
{
#define CASROS_ACCESSOR_CHECK\
    if (!casrosAccessor) {\
        std::cerr << "ERROR: accessor is not initialized" << std::endl;\
        return;\
    }
    CASROS_ACCESSOR_CHECK;

    if (!casrosAccessor->RequestFilterList(safetyCoordinatorName, componentName)) {
        std::cerr << "ERROR: failed to request filter list" << std::endl;
        return;
    }
}

void handler_filter_info(const std::string & safetyCoordinatorName,
                         const std::string & componentName)
{
    CASROS_ACCESSOR_CHECK;

    if (!casrosAccessor->RequestFilterInfo(safetyCoordinatorName, componentName)) {
        std::cerr << "ERROR: failed to request filter info" << std::endl;
        return;
    }
}

void handler_filter_inject(const std::string & safetyCoordinatorName,
                           const SF::FilterBase::FilterIDType fuid,
                           const SF::DoubleVecType & inputs,
                           bool deepInjection)
{
    CASROS_ACCESSOR_CHECK;

    if (!casrosAccessor->RequestFilterFaultInject(safetyCoordinatorName, fuid, inputs, deepInjection)) {
        std::cerr << "ERROR: failed to request filter list" << std::endl;
        return;
    }
}
#undef CASROS_ACCESSOR_CHECK

//------------------------------------------------------------ 
//  filter
//
//  filter help
//  filter list
//  filter info
//  filter inject
//  filter dinject
//------------------------------------------------------------ 
typedef enum {
    HELP,   // show help
    LIST,   // show summary of filters installed on the system
    INFO,   // show list of detailed inforamtion for all filters
    INJECT, // shallow fault injection
    DINJECT // deep fault injection
} FilterOptionType;

void handler_filter(const std::vector<std::string> & args)
{
    FilterOptionType option;
    std::string safetyCoordinatorName, componentName;

    const size_t n = args.size();

    std::string cmd(args[0]);
    SF::to_lowercase(cmd);
    if (cmd.compare("help") == 0)
        option = HELP;
    else if (cmd.compare("list") == 0)
        option = LIST;
    else if (cmd.compare("info") == 0)
        option = INFO;
    else if (cmd.compare("inject") == 0)
        option = INJECT;
    else if (cmd.compare("dinject") == 0)
        option = DINJECT;
    else
        option = HELP;

    unsigned int filterUID = 0;
    bool deepInjection = false;

    switch (option) {
    default:
    case HELP:
        handler_filter_help();
        break;

    case LIST:
        if (n == 1) {
            safetyCoordinatorName = "*";
            componentName = "*";
        } else if (n == 2) {
            safetyCoordinatorName = args[1];
            componentName = "*";
        } else if (n == 3) {
            safetyCoordinatorName = args[1];
            componentName = args[2];
        }
        handler_filter_list(safetyCoordinatorName, componentName);
        break;

    case INFO:
        if (n == 1) {
            safetyCoordinatorName = "*";
            componentName = "*";
        } else if (n == 2) {
            safetyCoordinatorName = args[1];
            componentName = "*";
        } else if (n == 3) {
            safetyCoordinatorName = args[1];
            componentName = args[2];
        }
        handler_filter_info(safetyCoordinatorName, componentName);
        break;

    case DINJECT:
        deepInjection = true;
        if (n < 4) {
            std::cout << "usage: filter dinject [safety_coordinator_name] [filter_uid] [input(s)]" << std::endl;
            return;
        }
    case INJECT:
        if (n < 4) {
            std::cout << "usage: filter inject [safety_coordinator_name] [filter_uid] [input(s)]" << std::endl;
            return;
        }
        safetyCoordinatorName = args[1];
        filterUID = atoi(args[2].c_str());

        size_t nInputs = n - 3;
        std::vector<double> inputs;
        for (size_t i = 0; i < nInputs; ++i)
            inputs.push_back(atof(args[i + 3].c_str()));
        
        handler_filter_inject(safetyCoordinatorName, filterUID, inputs, deepInjection);
        break;
    }
}
