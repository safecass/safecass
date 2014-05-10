//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: May 9, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _communicator_ice_h
#define _communicator_ice_h

#include "config.h"
#include "publisher.h"
#include "subscriber.h"
#include "dict.h"
#include "topic_def.h"
// Header from the Clipo project; See CMakeLists.txt for more details.
#include "command_line_interpreter.hpp"

#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

//
// Subscriber callback
//
class ConsoleSubscriberCallback : public SF::SFCallback {
public:
    ConsoleSubscriberCallback() {}
    ~ConsoleSubscriberCallback() {}

    void Callback(const std::string & json);
};

//
// ZeroC IceStorm Communicator
//
class Communicator {
public:
    typedef struct {
        osaThread       Thread;
        osaThreadSignal ThreadEventBegin;
        osaThreadSignal ThreadEventEnd;
        bool            Running;
    } InternalThreadType;
    
protected:
    /*! Ice publisher and subscriber */
    SF::Publisher *  Publisher;
    SF::Subscriber * Subscriber;

    /*! Callback for subscriber */
    ConsoleSubscriberCallback * SubscriberCallback;

    InternalThreadType ThreadSubscriber;

public:
    Communicator(void);
    virtual ~Communicator();

    void * RunSubscriber(unsigned int CMN_UNUSED(arg));
};

#endif // _communicator_ice_h
