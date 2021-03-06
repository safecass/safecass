//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: Apr 8, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _accessor_console_h
#define _accessor_console_h

#include "cisstAccessor.h"
#include "filterBase.h"

//
// Callback class for subscribers
//
class ConsoleSubscriberCallback : public SC::SCCallback {
protected:
    const std::string TopicName;

public:
    ConsoleSubscriberCallback(const std::string & topic) : TopicName(topic) {}
    ~ConsoleSubscriberCallback() {}

    void CallbackControl(SC::Topic::Control::CategoryType category, const std::string & json);
    void CallbackData   (SC::Topic::Data::CategoryType    category, const std::string & json);
};

//
// SAFECASS accessors
//
class AccessorConsole : public SC::cisstAccessor {
public:
    AccessorConsole(void);
    ~AccessorConsole() {}

    // request list of detailed information for all filters
    bool RequestFilterInfo(const std::string & safetyCoordinatorName = "*",
                           const std::string & componentName = "*") const;
    // request summary of all filters
    bool RequestFilterList(const std::string & safetyCoordinatorName = "*",
                           const std::string & componentName = "*") const;
    // request list of all states in the system
    bool RequestStateList(const std::string & safetyCoordinatorName = "*",
                          const std::string & componentName = "*") const;
    // request history of states with event information
    bool RequestStateHistory(const std::string & safetyCoordinatorName = "*",
                             const std::string & componentName = "*") const;
    // request list of all events in the system
    bool RequestEventList(const std::string & safetyCoordinatorName = "*",
                          const std::string & componentName = "*") const;
    // request history of all events occurred in the system
    bool RequestEventHistory(const std::string & safetyCoordinatorName = "*",
                             const std::string & componentName = "*") const;
    // request list of all connections in the system
    bool RequestConnectionList(const std::string & safetyCoordinatorName = "*",
                               const std::string & componentName = "*") const;
    // request list of service dependency information in the system
    bool RequestServiceDependencyList(const std::string & safetyCoordinatorName = "*",
                                      const std::string & componentName = "*") const;
    // request fault injection
    bool RequestFilterFaultInject(const std::string & safetyCoordinatorName,
                                  const SC::FilterBase::FilterIDType fuid,
                                  const SC::DoubleVecType & inputs,
                                  bool deepInjection = false) const;
    // load input file and request fault injection (deep injection only)
    bool RequestFilterFaultInjectLoad(const std::string & safetyCoordinatorName,
                                      const SC::FilterBase::FilterIDType fuid,
                                      const std::string & fileName) const;
    // request event generation
    bool RequestEventGeneration(const std::string & eventName, const std::string & eventType, const std::string & safetyCoordinatorName,
                                const std::string & componentName, const std::string & interfaceName) const;
    // request event broadcast 
    bool RequestEventBroadcast(const std::string & eventName, const std::string & safetyCoordinatorName) const;
};

// Aux macro
#define SAFECASS_ACCESSOR_CHECK\
    if (!casrosAccessor) {\
        std::cerr << "ERROR: accessor is not initialized" << std::endl;\
        return;\
    }

#endif // _accessor_console_h
