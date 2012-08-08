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
#include "dict.h"

#include <Ice/Application.h>

namespace SF {

// Subscriber id (unique within a process)
unsigned int Subscriber::Id = 0;

#define SUBSCRIBER_INFO "Subscriber " << Id << " (\"" << TopicName << "\"): "

class MonitorSamplesI: public MonitorSamples {
public:
    virtual void CollectSample(const std::string & json, const Ice::Current &)
    {
        std::cout << "Subscriber MONITOR received: " << json << std::endl;
    }
};

class SupervisorControlsI: public SupervisorControls {
public:
    virtual void ControlCommand(const std::string & json, const Ice::Current &)
    {
        std::cout << "Subscriber SUPERVISOR received: " << json << std::endl;
    }
};

Subscriber::Subscriber(const std::string & topicName)
    : BaseIce(GetDefaultConfigFilePath()), TopicName(topicName)
{
    Init();
}

Subscriber::Subscriber(const std::string & topicName, const std::string & propertyFileName)
    : BaseIce(propertyFileName), TopicName(topicName)
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

    SFLOG_INFO << SUBSCRIBER_INFO << "Created with config file: " << this->IcePropertyFileName << std::endl;
    SFLOG_INFO << SUBSCRIBER_INFO << "Created with topic name: " << TopicName << std::endl;
}

void Subscriber::Startup(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager = 
        IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    if (!manager) {
        SFLOG_ERROR << SUBSCRIBER_INFO << "Invalid proxy" << std::endl;
        return;
    }

    // Retrieve the topic.
    try {   
        Topic = manager->retrieve(TopicName);
    }   
    catch(const IceStorm::NoSuchTopic&) {   
        try {   
            Topic = manager->create(TopicName);
        } catch(const IceStorm::TopicExists&) {   
            SFLOG_ERROR << SUBSCRIBER_INFO << "Temporary failure. try again." << std::endl;
            return;
        }   
    }   

    //Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("MonitorSamples.Subscriber");
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
    if(subId.name.empty()) {
        subId.name = IceUtil::generateUUID();
    }
    // [SFUPDATE]
    if (TopicName.compare(Dict::TopicNames::Monitor) == 0) {
        SubscriberObj = adapter->add(new MonitorSamplesI, subId);
    } else if (TopicName.compare(Dict::TopicNames::Supervisor) == 0) {
        SubscriberObj = adapter->add(new SupervisorControlsI, subId);
    }

    IceStorm::QoS qos;
    if(!retryCount.empty())
    {
        qos["retryCount"] = retryCount;
    }

    // set up the proxy
    SubscriberObj = SubscriberObj->ice_datagram();

    try {
        Topic->subscribeAndGetPublisher(qos, SubscriberObj);
    } catch(const IceStorm::AlreadySubscribed&) {
        // If we're manually setting the Subscriber id ignore.
        if(id.empty()) {
            throw;
        }
        SFLOG_ERROR << SUBSCRIBER_INFO << "Reactivating persistent subscriber" << std::endl;
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

    SFLOG_INFO << SUBSCRIBER_INFO << "Unsubscribed from topic" << std::endl;
    
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
