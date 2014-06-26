//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: Jun 26, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "accessor.h"

#include <cisstMultiTask/mtsManagerLocal.h>

//
// Subscriber callback
//
void ConsoleSubscriberCallback::CallbackControl(SF::Topic::Control::CategoryType category, const std::string & json)
{
    /*
    std::string categoryName;
    switch (category) {
    case SF::Topic::Control::COMMAND:
        categoryName = "COMMAND";
        break;
    case SF::Topic::Control::READ_REQ:
        categoryName = "READ_REQ";
        break;
    default:
        categoryName = "INVALID";
        break;
    }

    _PROBE << "[ topic \"" << TopicName << "\", category \"" << categoryName << "\" ] received: " << json << std::endl;
    */
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

    _PROBE << "[ topic: " << TopicName << ", category: " << categoryName << " ] received: " << json << std::endl;
}

//
// CASROS accessor for console
//
AccessorConsole::AccessorConsole(void)
    : SF::cisstAccessor(true,  // enablePublisherControl
                        false, // enablePublisherData
                        false,  // enableSubscriberControl
                        true,  // enableSubscriberData
                        0,//new ConsoleSubscriberCallback(SF::Dict::TopicNames::CONTROL),
                        new ConsoleSubscriberCallback(SF::Dict::TopicNames::DATA))
{
}

bool AccessorConsole::RequestFilterList(const std::string & processName, 
                                        const std::string & UNUSED(componentName)) const
{
    // TODO
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, 
            "{ \"target\": \"*\", \"cmd\": \"filter_list\" }")) 
    {
        std::cerr << "AccessorConsole: Failed to publish message (Control::READ_REQ)" << std::endl;
        return false;
    }

    return true;
}

bool AccessorConsole::RequestStateList(const std::string & processName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"*\", \"component\": \"*\" }, "
          "\"request\": \"state_list\" }";
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "AccessorConsole: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "Requested list of states" << std::endl;
    return true;
}
