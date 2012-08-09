/*

  Safety Framework for Component-based Robotics

  Created on: July 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _subscriber_h
#define _subscriber_h

#include "baseIce.h"

#include <messages.h>

namespace SF {

// Adapter class: almost empty but pure virtual methods
class SFLIB_EXPORT Subscriber: public BaseIce {
protected:
    /*! typedef for base class */
    typedef BaseIce BaseType;

    /*! Subscriber id (unique within a process) */
    static unsigned int Id;

    /*! Topic name to which this subscriber subscribes */
    const std::string TopicName;

    /*! Initialization */
    void Init(void);

    IceStorm::TopicPrx Topic;
    Ice::ObjectPrx     SubscriberObj;

    SFCallback * CallbackInstance;

public:
    Subscriber(const std::string & topicName, SFCallback * callbackInstance);
    Subscriber(const std::string & topicName, const std::string & propertyFileName);
    virtual ~Subscriber();

    void Startup(void);
    void Run(void);
    void Stop(void);

    static const std::string GetDefaultConfigFilePath(void);
};

};

#endif // _subscriber_h
