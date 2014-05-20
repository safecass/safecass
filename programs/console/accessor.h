//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: May 19, 2014
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
// Casros Network Accessor
//
class Accessor {
    enum { CONTROL, DATA, NUM_SUBSCRIBERS };

public:
    typedef struct {
        osaThread       Thread;
        osaThreadSignal ThreadEventBegin;
        osaThreadSignal ThreadEventEnd;
        bool            Running;
    } InternalThreadType;
    
protected:
    /*! SF publishers and subscribers (currently, casros uses ZeroC Ice)*/
    struct {
        SF::Publisher * Control;
        SF::Publisher * Data;
    } Publishers;

    struct {
        SF::Subscriber * Control;
        SF::Subscriber * Data;
    } Subscribers;

    /*! Callback for subscriber */
    ConsoleSubscriberCallback * SubscriberCallback;

    InternalThreadType ThreadSubscriber[NUM_SUBSCRIBERS];

public:
    Accessor(void);
    virtual ~Accessor();

    // start and stop
    void * StartSubscriber(unsigned int CMN_UNUSED(arg));
    void StopSubscriber(void);

    // request list of all filters installed
    bool RequestFilterList(const std::string & processName = "",
                           const std::string & componentName = "") const;
};

#endif // _communicator_ice_h
