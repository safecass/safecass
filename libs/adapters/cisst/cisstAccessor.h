//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 20, 2014
// Last revision: May 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _cisst_ice_casros_accessor_h
#define _cisst_ice_casros_accessor_h

#include "config.h"
#include "publisher.h"
#include "subscriber.h"
#include "dict.h"
#include "topic_def.h"

#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

class cisstAccessor {
public:
    typedef struct {
        osaThread       Thread;
        osaThreadSignal ThreadEventBegin;
        osaThreadSignal ThreadEventEnd;
        bool            Running;
    } InternalThreadType;

    enum { PUBLISHER = 0, SUBSCRIBER };

protected:
    bool Enabled[SUBSCRIBER + 1][SF::Topic::TOTAL_TOPIC_COUNT];

    struct {
        SF::Publisher * Control;
        SF::Publisher * Data;
    } Publishers;

    struct {
        SF::Subscriber * Control;
        SF::Subscriber * Data;
    } Subscribers;

    struct {
        SF::SFCallback * Control;
        SF::SFCallback * Data;
    } SubscriberCallbacks;

    InternalThreadType ThreadSubscribers[2]; // 2: total # of topics to subscribe

public:
    cisstAccessor(bool enablePublisherControl,  bool enablePublisherData,
                  bool enableSubscriberControl, bool enableSubscriberData,
                  SF::SFCallback * cbSubscriberControl = 0,
                  SF::SFCallback * cbSubscriberData = 0);
    virtual ~cisstAccessor();

    // start and stop
    void * StartSubscriber(unsigned int CMN_UNUSED(arg));
    void StopSubscriber(void);
};

#endif // _cisst_ice_casros_accessor_h
