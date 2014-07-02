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
#ifndef _accessor_console_h
#define _accessor_console_h

#include "cisstAccessor.h"
#include "filterBase.h"

//
// Callback class for subscribers
//
class ConsoleSubscriberCallback : public SF::SFCallback {
protected:
    const std::string TopicName;

public:
    ConsoleSubscriberCallback(const std::string & topic) : TopicName(topic) {}
    ~ConsoleSubscriberCallback() {}

    void CallbackControl(SF::Topic::Control::CategoryType category, const std::string & json);
    void CallbackData   (SF::Topic::Data::CategoryType    category, const std::string & json);
};

//
// CASROS accessors
//
class AccessorConsole : public SF::cisstAccessor {
public:
    AccessorConsole(void);
    ~AccessorConsole() {}

    // request list of all filters installed
    bool RequestFilterList(const std::string & safetyCoordinatorName = "*",
                           const std::string & componentName = "*") const;
    // request list of all states in the Safety Coordinator
    bool RequestStateList(const std::string & safetyCoordinatorName = "*",
                          const std::string & componentName = "*") const;
    // request fault injection
    bool RequestFilterFaultInject(const std::string & safetyCoordinatorName,
                                  const SF::FilterBase::FilterIDType fuid,
                                  const SF::DoubleVecType & inputs) const;
};

#endif // _accessor_console_h
