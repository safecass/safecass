/*

  Safety Framework for Component-based Robotics

  Created on: July 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "subscriber.h"

#include <Ice/Application.h>
#include <monitorFDD.h>

namespace SF {

class MonitorFDDI: public MonitorFDD {
public:
    virtual void Event(const std::string & event, const Ice::Current&) {
        std::cout << "\nMonitorFDDI: " << event << std::endl;
    }
};

Subscriber::Subscriber(const std::string & propertyFileName)
    : BaseIce(propertyFileName)
{
}

Subscriber::~Subscriber()
{
}

void Subscriber::Run(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager = 
        IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    if(!manager)
    {
        SFLOG_ERROR << "SUBSCRIBER: invalid proxy" << std::endl;
        return;
    }

    const std::string topicName("event");

    // Retrieve the topic.
    IceStorm::TopicPrx topic;
    try 
    {   
        topic = manager->retrieve(topicName);
    }   
    catch(const IceStorm::NoSuchTopic&)
    {   
        try 
        {   
            topic = manager->create(topicName);
        }   
        catch(const IceStorm::TopicExists&)
        {   
            std::cout << "SUBSCRIBER: temporary failure. try again." << std::endl;
            return;
        }   
    }   

    //Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("MonitorFDD.Subscriber");
    Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("Clock.Subscriber");

    //
    // Add a servant for the Ice object. If --id is used the identity
    // comes from the command line, otherwise a UUID is used.
    //
    // id is not directly altered since it is used below to detect
    // whether subscribeAndGetPublisher can raise AlreadySubscribed.
    //
    std::string id, retryCount;
    Ice::Identity subId;
    subId.name = id;
    if(subId.name.empty())
    {
        subId.name = IceUtil::generateUUID();
    }
    Ice::ObjectPrx subscriber = adapter->add(new MonitorFDDI, subId);

    IceStorm::QoS qos;
    if(!retryCount.empty())
    {
        qos["retryCount"] = retryCount;
    }

    // set up the proxy
    subscriber = subscriber->ice_datagram();

    try
    {
        topic->subscribeAndGetPublisher(qos, subscriber);
    }
    catch(const IceStorm::AlreadySubscribed&)
    {
        // If we're manually setting the subscriber id ignore.
        if(id.empty())
        {
            throw;
        }
        std::cout << "reactivating persistent subscriber" << std::endl;
    }
    adapter->activate();

    // http://www.zeroc.com/forums/help-center/2062-how-send-last-message-before-server-terminates-ice-application-class.html
    Ice::Application::shutdownOnInterrupt();
    IceCommunicator->waitForShutdown();

    topic->unsubscribe(subscriber);


    SFLOG_DEBUG << "FINISHED" << std::endl;
}

};
