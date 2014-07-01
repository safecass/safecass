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
#include "accessor.h"

#include <cisstOSAbstraction/osaSleep.h>

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

    SFLOG_INFO << "[ topic: " << TopicName << ", category: " << categoryName << " ] received: " << json << std::endl;

    std::cout << json << std::endl;
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

bool AccessorConsole::RequestFilterList(const std::string & safetyCoordinatorName,
                                        const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"filter_list\" }";
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestFilterList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of filters" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestStateList(const std::string & safetyCoordinatorName,
                                       const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"state_list\" }";
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestStateList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of states" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestFilterFaultInject(const std::string & safetyCoordinatorName,
                                               const SF::FilterBase::FilterIDType fuid,
                                               const std::vector<double> inputs) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"fuid\": " << fuid << " }, "
          "\"request\": \"filter_inject\", "
          "\"input\": [ ";
    for (size_t i = 0; i < inputs.size(); ++i) {
        if (i != 0)
            ss << ", ";
        ss << inputs[i];
    }
    ss << " ] }";

    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestFilterFaultInject: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested input injection" << std::endl;
    osaSleep(0.5);

    return true;
}
