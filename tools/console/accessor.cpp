//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#include "accessor.h"

#include <iomanip>

#include <cisstOSAbstraction/osaSleep.h>

using namespace SC;

//
// Subscriber callback
//
void ConsoleSubscriberCallback::CallbackControl(Topic::Control::CategoryType UNUSED(category),
                                                const std::string & UNUSED(json))
{
    /*
    std::string categoryName;
    switch (category) {
    case Topic::Control::COMMAND:
        categoryName = "COMMAND";
        break;
    case Topic::Control::READ_REQ:
        categoryName = "READ_REQ";
        break;
    default:
        categoryName = "INVALID";
        break;
    }

    _PROBE << "[ topic \"" << TopicName << "\", category \"" << categoryName << "\" ] received: " << json << std::endl;
    */
    SCASSERT(false);
}

void ConsoleSubscriberCallback::CallbackData(Topic::Data::CategoryType category, const std::string & json)
{
    std::string categoryName;
    switch (category) {
    case Topic::Data::MONITOR:
        categoryName = "MONITOR";
        break;
    case Topic::Data::EVENT:
        categoryName = "EVENT";
        break;
    case Topic::Data::READ_RES:
        categoryName = "READ_RES";
        break;
    default:
        categoryName = "INVALID";
        break;
    }

    JsonWrapper _json;
    std::cout << "[ topic: " << TopicName << ", category: " << categoryName << " ] received " << std::endl;
    if (_json.Read(json.c_str())) {
        if (_json.GetRoot().isNull()) {
            std::cout << "(null)" << std::endl;
            return;
        }

        // TEMP
        std::cout << json << std::endl;
#if 0
        std::string cmd("");
        cmd = JsonWrapper::GetSafeValueString(_json.GetRoot(), "cmd");
        cmd = rtrim(cmd);
        if (cmd.compare("message") == 0) {
            std::cout << JsonWrapper::GetSafeValueString(json, "msg") << std::endl;
        } else {
            // TEMP: std::cout for now, but could handle messages differently
            if (cmd.compare("state_list") != 0)
                std::cout << json << std::endl;
            else
                std::cout << json << std::endl;
        }
#endif
    } else {
        std::cout << "Failed to parse JSON.  Printing out raw string instead:" << std::endl;
        std::cout << json << std::endl;
    }
}

//
// SAFECASS accessor for console
//
AccessorConsole::AccessorConsole(void)
    : cisstAccessor(true,  // enablePublisherControl
                        false, // enablePublisherData
                        false, // enableSubscriberControl
                        true,  // enableSubscriberData
                        0,//new ConsoleSubscriberCallback(Dict::TopicNames::CONTROL),
                        new ConsoleSubscriberCallback(Dict::TopicNames::DATA))
{
}

bool AccessorConsole::RequestFilterList(const std::string & safetyCoordinatorName,
                                        const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"filter_list\" }";
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
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
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestFilterInfo: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of detailed information about filters" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestFilterFaultInject(const std::string & safetyCoordinatorName,
                                               const FilterBase::FilterIDType fuid,
                                               const DoubleVecType & inputs,
                                               bool deepInjection) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"fuid\": " << fuid << " }, "
          "\"request\": \"filter_inject\", "
          "\"deep\": " << (deepInjection ? "true" : "false") << ", "
          "\"input\": [ ";
    for (size_t i = 0; i < inputs.size(); ++i) {
        if (i != 0)
            ss << ", ";
        ss << std::setprecision(5) << inputs[i];
    }
    ss << " ] }";

    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
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
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestStateList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of states" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestStateHistory(const std::string & safetyCoordinatorName,
                                       const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"state_history\" }";
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestStateHistory: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested state history" << std::endl;
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
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestEventList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of events" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestEventHistory(const std::string & safetyCoordinatorName,
                                          const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"event_history\" }";
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestEventHistory: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested history of events" << std::endl;
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
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
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
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
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
    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.GetRoot();

    // command
    json["command"] = "event_generate";
    // target
    json["target"]["safety_coordinator"] = safetyCoordinatorName;
    json["target"]["component"] = componentName;
    if (interfaceName.size())
        json["target"]["interface"] = interfaceName;
    else
        json["target"]["interface"] = JsonWrapper::JsonValue::null;
    // event
    json["event"]["name"] = eventName;
    json["event"]["type"] = eventType;

    if (!Publishers.Control->PublishControl(Topic::Control::COMMAND, JsonWrapper::GetJSONString(json))) {
        std::cerr << "RequestEventGeneration: Failed to publish message (Control, COMMAND): " 
                  << JsonWrapper::GetJSONString(json) << std::endl;
        return false;
    }

    std::cout << "requested event generation" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestEventBroadcast(const std::string & eventName, const std::string & safetyCoordinatorName) const
{
    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.GetRoot();

    // command
    json["command"] = "event_generate";
    // target
    json["target"]["safety_coordinator"] = safetyCoordinatorName;
    json["target"]["component"] = "*";
    //json["target"]["interface"] = JsonWrapper::JsonValue::null;
    // event
    json["event"]["name"] = eventName;
    json["event"]["type"] = "application";

    if (!Publishers.Control->PublishControl(Topic::Control::COMMAND, JsonWrapper::GetJSONString(json))) {
        std::cerr << "RequestServiceDependencyList: Failed to publish message (Control, COMMAND): " 
                  << JsonWrapper::GetJSONString(json) << std::endl;
        return false;
    }

    std::cout << "requested event broadcast" << std::endl;
    osaSleep(0.5);

    return true;
}

bool AccessorConsole::RequestFilterFaultInjectLoad(const std::string & safetyCoordinatorName,
                                                   const FilterBase::FilterIDType fuid,
                                                   const std::string & fileName) const
{
    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.GetRoot();

    json["command"] = "event_generate";
    json["target"]["safety_coordinator"] = safetyCoordinatorName;
    json["target"]["fuid"] = fuid;
    json["request"] = "filter_inject";
    json["deep"] = true;

    // read file (delimiter: comma, tab, space)
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "RequestFilterFaultInjectLoad: failed loading data from file: " << fileName << std::endl;
        return false;
    }

    Json::ArrayIndex n = 0;
    std::string line;
    while (getline(file, line)) {
        char buf[256] = "";
        memcpy(buf, line.c_str(), line.size());
#define DELIMITERS " ,\t"
        char * p = strtok(buf, DELIMITERS);
        JsonWrapper _row;
        JsonWrapper::JsonValue & row = _row.GetRoot();
        Json::ArrayIndex i = 0;
        while (p) {
            row[i++] = atof(p);
            p = strtok(0, DELIMITERS);
#undef DELIMITERS
        }
        json["input_vector"][n++] = row;
    }
    file.close();

    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, JsonWrapper::GetJSONString(json))) {
        std::cerr << "RequestFilterFaultInject: Failed to publish message (Control, READ_REQ): " 
                  << JsonWrapper::GetJSONString(json) << std::endl;
        return false;
    }

    std::cout << "requested input injection using input file" << std::endl;
    osaSleep(0.5);

    return true;
}
