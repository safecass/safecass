//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
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

namespace SC {

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
    bool Enabled[SUBSCRIBER + 1][SC::Topic::TOTAL_TOPIC_COUNT];

    struct {
        SC::Publisher * Control;
        SC::Publisher * Data;
    } Publishers;

    struct {
        SC::Subscriber * Control;
        SC::Subscriber * Data;
    } Subscribers;

    struct {
        SC::SCCallback * Control;
        SC::SCCallback * Data;
    } SubscriberCallbacks;

    InternalThreadType ThreadSubscribers[2]; // 2: total # of topics to subscribe

public:
    cisstAccessor(bool enablePublisherControl,  bool enablePublisherData,
                  bool enableSubscriberControl, bool enableSubscriberData,
                  SC::SCCallback * cbSubscriberControl = 0,
                  SC::SCCallback * cbSubscriberData = 0);
    virtual ~cisstAccessor();

    // start and stop
    void * StartSubscriber(unsigned int CMN_UNUSED(arg));
    void StopSubscriber(void);

    //! Getters
    SC::Publisher * GetPublisher(SC::Topic::Type topicType) const;
    SC::Subscriber * GetSubscriber(SC::Topic::Type topicType) const;
    SC::SCCallback * GetSubscriberCallback(SC::Topic::Type topicType) const;
};

}; // SC

#endif // _cisst_ice_casros_accessor_h
