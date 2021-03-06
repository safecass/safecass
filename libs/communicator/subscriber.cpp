//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: May 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "subscriber.h"
#include "config.h"
#include "dict.h"

#include <Ice/Application.h>

using namespace SC;

// Subscriber id (unique within a process)
unsigned int Subscriber::Id = 0;

#define SUBSCRIBER_INFO "Subscriber " << Id << " (\"" << this->TopicName << "\"): "

class DataI: public Data {
protected:
    SCCallback * CallbackInstance;
public:
    DataI(SCCallback * callbackInstance): CallbackInstance(callbackInstance) {}
    void Monitor(const std::string & json, const Ice::Current &) {
        CallbackInstance->CallbackData(Topic::Data::MONITOR, json);
    }
    void Event(const std::string & json, const Ice::Current &) {
        CallbackInstance->CallbackData(Topic::Data::EVENT, json);
    }
    void ReadRes(const std::string & json, const Ice::Current &) {
        CallbackInstance->CallbackData(Topic::Data::READ_RES, json);
    }
};

class ControlI: public Control {
protected:
    SCCallback * CallbackInstance;
public:
    ControlI(SCCallback * callbackInstance): CallbackInstance(callbackInstance) {}
    void Command(const std::string & json, const Ice::Current &) {
        CallbackInstance->CallbackControl(Topic::Control::COMMAND, json);
    }
    void ReadReq(const std::string & json, const Ice::Current &) {
        CallbackInstance->CallbackControl(Topic::Control::READ_REQ, json);
    }
    void StateUpdate(const std::string & json, const Ice::Current &) {
        CallbackInstance->CallbackControl(Topic::Control::STATE_UPDATE, json);
    }
};

Subscriber::Subscriber(void): BaseIce(NONAME, NONAME), CallbackInstance(0)
{
    // Default constructor should not be used
    SCASSERT(false);
}

Subscriber::Subscriber(const std::string & topicName, SCCallback * callbackInstance)
    : BaseIce(topicName, GetDefaultConfigFilePath()), 
      CallbackInstance(callbackInstance)
{
    Init();
}

Subscriber::Subscriber(const std::string & topicName, const std::string & propertyFileName)
    : BaseIce(topicName, propertyFileName), 
      CallbackInstance(0)
{
    Init();
}

Subscriber::~Subscriber()
{
    if (CallbackInstance)
        delete CallbackInstance;
}

void Subscriber::Init(void)
{
    ++Id;

    SCLOG_INFO << SUBSCRIBER_INFO << "Created with config file: " << this->IcePropertyFileName << std::endl;
    SCLOG_INFO << SUBSCRIBER_INFO << "Created with topic name: " << this->TopicName << std::endl;
}

bool Subscriber::Startup(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager;
    try {
        manager = IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    } catch (const Ice::ConnectionRefusedException & e) {
        SCLOG_ERROR << SUBSCRIBER_INFO << "Failed to initialize IceStorm.  Check if IceBox is running." << std::endl;
        return false;
    }
 
    if (!manager) {
        SCLOG_ERROR << SUBSCRIBER_INFO << "Invalid proxy" << std::endl;
        return false;
    }

    // Retrieve the topic.
    try {   
        Topic = manager->retrieve(this->TopicName);
    } catch(const IceStorm::NoSuchTopic&) {   
        try {   
            Topic = manager->create(this->TopicName);
        } catch(const IceStorm::TopicExists&) {   
            SCLOG_ERROR << SUBSCRIBER_INFO << "Topic not found. Try again." << std::endl;
            return false;
        }   
    }   

    // Add a servant for the Ice object using a UUID.
    //
    // id is not directly altered since it is used below to detect
    // whether subscribeAndGetPublisher can raise AlreadySubscribed.
    //
    std::string id, retryCount;
    Ice::Identity subId;
    subId.name = id;
    if (subId.name.empty())
        subId.name = IceUtil::generateUUID();

    Ice::ObjectAdapterPtr adapter;
    switch (BaseType::Topic) {
    case Topic::CONTROL:
        adapter = IceCommunicator->createObjectAdapter("CONTROL.Subscriber");
        SubscriberObj = adapter->add(new ControlI(CallbackInstance), subId);
        break;
    case Topic::DATA:
        adapter = IceCommunicator->createObjectAdapter("DATA.Subscriber");
        SubscriberObj = adapter->add(new DataI(CallbackInstance), subId);
        break;
    default:
        SCASSERT(false);
    }

    IceStorm::QoS qos;
    if (!retryCount.empty())
        qos["retryCount"] = retryCount;

    // set up the proxy
    // TODO: confirm is ice_datagram() uses UDP instead of TCP..?
    SubscriberObj = SubscriberObj->ice_datagram();

    try {
        Topic->subscribeAndGetPublisher(qos, SubscriberObj);
    } catch(const IceStorm::AlreadySubscribed&) {
        // If we're manually setting the Subscriber id ignore.
        if (id.empty())
            throw;

        SCLOG_ERROR << SUBSCRIBER_INFO << "Reactivating persistent subscriber" << std::endl;
    }
    adapter->activate();

    //BaseType::Startup();

    return true;
}

void Subscriber::Run(void)
{
    //BaseType::Run();

    Ice::Application::shutdownOnInterrupt();
    IceCommunicator->waitForShutdown();

    Topic->unsubscribe(SubscriberObj);

    SCLOG_INFO << SUBSCRIBER_INFO << "Unsubscribed from topic" << std::endl;
    
    //BaseType::Stop();
}

void Subscriber::Stop(void)
{
    // Terminating subscriber needs to call shutdown() on the Ice communicator
    IceCommunicator->shutdown();

    //BaseType::Stop();
}

const std::string Subscriber::GetDefaultConfigFilePath(void)
{
    std::string path(SC_COMMUNICATOR_CONFIG_DIR);
    path += "/config.sub";

    return path;
}
