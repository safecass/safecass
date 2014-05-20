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
#ifndef _accessor_console_h
#define _accessor_console_h

#include "cisstAccessor.h"

//
// Callback class for subscribers
//
class ConsoleSubscriberCallback : public SF::SFCallback {
protected:
    const std::string TopicName;

public:
    ConsoleSubscriberCallback(const std::string & topic) : TopicName(topic) {}
    ~ConsoleSubscriberCallback() {}

    void Callback(const std::string & json);
};

class AccessorConsole : public cisstAccessor {
public:
    AccessorConsole(void);
    ~AccessorConsole() {}

    // request list of all filters installed
    bool RequestFilterList(const std::string & processName = "",
                           const std::string & componentName = "") const;
};

#endif // _accessor_console_h
