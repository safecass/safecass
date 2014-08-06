//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: Jul 29, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "accessor.h"

#include <iomanip>

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

    SF::JSON _json;
    if (_json.Read(json.c_str())) {
        std::string cmd(SF::JSON::GetSafeValueString(_json.GetRoot(), "cmd"));
        cmd = SF::rtrim(cmd);
        if (cmd.compare("state_list") != 0) {
            std::cout << "[ topic: " << TopicName << ", category: " << categoryName << " ] received " << std::endl;
            std::cout << json << std::endl;
        }
    }
}

//
// CASROS accessor for console
//
AccessorConsole::AccessorConsole(void)
    : SF::cisstAccessor(true,  // enablePublisherControl
                        false, // enablePublisherData
                        false, // enableSubscriberControl
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

    std::cout << "requested summary of filters" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestFilterInfo(const std::string & safetyCoordinatorName,
                                        const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"filter_info\" }";
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestFilterInfo: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of detailed information about filters" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestFilterFaultInject(const std::string & safetyCoordinatorName,
                                               const SF::FilterBase::FilterIDType fuid,
                                               const SF::DoubleVecType & inputs) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"fuid\": " << fuid << " }, "
          "\"request\": \"filter_inject\", "
          "\"input\": [ ";
    for (size_t i = 0; i < inputs.size(); ++i) {
        if (i != 0)
            ss << ", ";
        ss << std::setprecision(5) << inputs[i];
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


bool AccessorConsole::RequestEventList(const std::string & safetyCoordinatorName,
                                       const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"event_list\" }";
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestEventList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of events" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestConnectionList(const std::string & safetyCoordinatorName,
                                            const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"connection_list\" }";
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestConnectionList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of connections" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestServiceDependencyList(const std::string & safetyCoordinatorName,
                                                   const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"service_list\" }";
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestServiceDependencyList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of service dependency" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestEventGeneration(const std::string & eventName, const std::string & eventType, const std::string & safetyCoordinatorName,
                                             const std::string & componentName, const std::string & interfaceName) const
{
    SF::JSON _json;
    SF::JSON::JSONVALUE & json = _json.GetRoot();

    // command
    json["command"] = "event_generate";
    // target
    json["target"]["safety_coordinator"] = safetyCoordinatorName;
    json["target"]["component"] = componentName;
    if (interfaceName.size())
        json["target"]["interface"] = interfaceName;
    else
        json["target"]["interface"] = SF::JSON::JSONVALUE::null;
    // event
    json["event"]["name"] = eventName;
    json["event"]["type"] = eventType;

    if (!Publishers.Control->PublishControl(SF::Topic::Control::COMMAND, SF::JSON::GetJSONString(json))) {
        std::cerr << "RequestEventGeneration: Failed to publish message (Control, COMMAND): " 
                  << SF::JSON::GetJSONString(json) << std::endl;
        return false;
    }

    std::cout << "requested event generation" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestEventBroadcast(const std::string & eventName, const std::string & safetyCoordinatorName) const
{
    SF::JSON _json;
    SF::JSON::JSONVALUE & json = _json.GetRoot();

    // command
    json["command"] = "event_generate";
    // target
    json["target"]["safety_coordinator"] = safetyCoordinatorName;
    json["target"]["component"] = "*";
    //json["target"]["interface"] = SF::JSON::JSONVALUE::null;
    // event
    json["event"]["name"] = eventName;
    json["event"]["type"] = "application";

    if (!Publishers.Control->PublishControl(SF::Topic::Control::COMMAND, SF::JSON::GetJSONString(json))) {
        std::cerr << "RequestServiceDependencyList: Failed to publish message (Control, COMMAND): " 
                  << SF::JSON::GetJSONString(json) << std::endl;
        return false;
    }

    std::cout << "requested event broadcast" << std::endl;
    osaSleep(0.5);

    return true;
}
