//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: May 8, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _subscriber_h
#define _subscriber_h

#include "baseIce.h"

#include <messages.h>

namespace SF {

// Adapter class: almost empty but pure virtual methods
class SFLIB_EXPORT Subscriber: public BaseIce {
private:
    /*! Default constructor should not be used */
    Subscriber(void);

protected:
    /*! typedef for base class */
    typedef BaseIce BaseType;

    /*! Subscriber id (unique within a process) */
    static unsigned int Id;

    /*! Initialization */
    void Init(void);

    IceStorm::TopicPrx Topic;
    Ice::ObjectPrx     SubscriberObj;

    SFCallback * CallbackInstance;

public:
    Subscriber(const std::string & topicName, SFCallback * callbackInstance);
    Subscriber(const std::string & topicName, const std::string & propertyFileName);
    virtual ~Subscriber();

    bool Startup(void);
    void Run(void);
    void Stop(void);

    static const std::string GetDefaultConfigFilePath(void);
};

};

#endif // _subscriber_h
