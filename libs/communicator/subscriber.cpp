/*

  Safety Framework for Component-based Robotics

  Created on: July 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "subscriber.h"
#include "config.h"

#include <Ice/Application.h>

namespace SF {

// Subscriber id (unique within a process)
unsigned int Subscriber::Id = 0;

class MonitorFDDI: public MonitorFDD {
public:
    virtual void Event(const std::string & event, const Ice::Current&) {
        std::cout << "\nMonitorFDDI: " << event << std::endl;
    }
};

Subscriber::Subscriber()
    : BaseIce(GetDefaultConfigFilePath())
{
    Init();
}

Subscriber::Subscriber(const std::string & propertyFileName)
: BaseIce(propertyFileName)
{
    Init();
}

Subscriber::~Subscriber()
{
}

void Subscriber::Init(void)
{
    ++Id;
    BaseType::Init();

    SFLOG_INFO << "Subscriber " << Id << " created with config file: " << this->IcePropertyFileName << std::endl;
}

void Subscriber::Startup(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager = 
        IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    if (!manager) {
        SFLOG_ERROR << "SUBSCRIBER: invalid proxy" << std::endl;
        return;
    }

    const std::string topicName("event");

    // Retrieve the topic.
    try {   
        Topic = manager->retrieve(topicName);
    }   
    catch(const IceStorm::NoSuchTopic&) {   
        try {   
            Topic = manager->create(topicName);
        } catch(const IceStorm::TopicExists&) {   
            SFLOG_ERROR << "SUBSCRIBER: temporary failure. try again." << std::endl;
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
    SubscriberObj = adapter->add(new MonitorFDDI, subId);

    IceStorm::QoS qos;
    if(!retryCount.empty())
    {
        qos["retryCount"] = retryCount;
    }

    // set up the proxy
    SubscriberObj = SubscriberObj->ice_datagram();

    try
    {
        Topic->subscribeAndGetPublisher(qos, SubscriberObj);
    }
    catch(const IceStorm::AlreadySubscribed&)
    {
        // If we're manually setting the Subscriber id ignore.
        if(id.empty())
        {
            throw;
        }
        SFLOG_ERROR << "reactivating persistent subscriber" << std::endl;
    }
    adapter->activate();

    BaseType::Startup();
}

void Subscriber::Run(void)
{
    BaseType::Run();

    Ice::Application::shutdownOnInterrupt();
    IceCommunicator->waitForShutdown();

    Topic->unsubscribe(SubscriberObj);

    SFLOG_INFO << "Unsubscribed from topic" << std::endl;
    
    BaseType::Stop();
}

void Subscriber::Stop(void)
{
    // Terminating subscriber needs to call shutdown() on the Ice communicator
    IceCommunicator->shutdown();

    BaseType::Stop();
}

const std::string Subscriber::GetDefaultConfigFilePath(void)
{
    std::string path(SF_COMMUNICATOR_CONFIG_DIR);
    path += "/config.sub";

    return path;
}

};
