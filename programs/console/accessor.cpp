//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: May 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "accessor.h"

//
// Subscriber callback
//
void ConsoleSubscriberCallback::CallbackControl(SF::Topic::Control::CategoryType category, const std::string & json)
{
    SFASSERT(false);
}

void ConsoleSubscriberCallback::CallbackData(SF::Topic::Data::CategoryType category, const std::string & json)
{
    std::string categoryName;
    switch (category) {
    case SF::Topic::Data::MONITOR:
        categoryName = "MONITOR";
        break;
    case SF::Topic::Data::EVENT:
        categoryName = "EVENT";
        break;
    case SF::Topic::Data::READ_RES:
        categoryName = "READ_RES";
        break;
    default:
        categoryName = "INVALID";
        break;
    }

    _PROBE << "[ " << TopicName << " | " << categoryName << " ] received: " << json << std::endl;
}

//
// CASROS accessor for console
//
AccessorConsole::AccessorConsole(void)
    : SF::cisstAccessor(true, false, false, true, 
                        0,//new ConsoleSubscriberCallback(SF::Dict::TopicNames::CONTROL),
                        new ConsoleSubscriberCallback(SF::Dict::TopicNames::DATA))
{
}

bool AccessorConsole::RequestFilterList(const std::string & UNUSED(processName), 
                                        const std::string & UNUSED(componentName)) const
{
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, 
            "{ \"target\": \"*\", \"cmd\": \"filter_list\" }")) 
    {
        std::cerr << "AccessorConsole: Failed to publish message (Control::READ_REQ)" << std::endl;
        return false;
    }

    return true;
}
