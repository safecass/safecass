//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: May 7, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "publisher.h"
#include "config.h"
#include "dict.h"

namespace SF {

// Publisher id (unique within a process)
unsigned int Publisher::Id = 0;

#define PUBLISHER_INFO "Publisher " << Id << " (\"" << TopicName << "\"): "

Publisher::Publisher(const std::string & topicName)
    : BaseIce(GetDefaultConfigFilePath()), TopicName(topicName)
{
    Init();
}

Publisher::Publisher(const std::string & topicName, const std::string & propertyFileName)
    : BaseIce(propertyFileName), TopicName(topicName)
{
    Init();
}

Publisher::~Publisher()
{
}

void Publisher::Init(void)
{
    ++Id;
    BaseType::Init();

    SFLOG_INFO << PUBLISHER_INFO << "Created with config file: " << this->IcePropertyFileName << std::endl;
    SFLOG_INFO << PUBLISHER_INFO << "Created with topic name: " << TopicName << std::endl;
}

bool Publisher::Startup(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager;
    try {
        manager = IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    } catch (const Ice::ConnectionRefusedException & e) {
        SFLOG_ERROR << "Failed to initialize IceStorm.  Check if IceBox is running: " << e.what() << std::endl;
        return false;
    }

    if (!manager) {
        SFLOG_ERROR << PUBLISHER_INFO << "Invalid proxy" << std::endl;
        return false;
    }

    // Retrieve the topic.
    IceStorm::TopicPrx topic;
    const std::string topicName = TopicName;
    try {   
        topic = manager->retrieve(topicName);
    } catch(const IceStorm::NoSuchTopic&) {   
        try {   
            topic = manager->create(topicName);
        } 
        catch(const IceStorm::TopicExists&) {   
            SFLOG_ERROR << PUBLISHER_INFO << "Topic not found. Try again." << std::endl;
            return false;
        }   
    }   

    // Get the topic's publisher object, and create a Clock proxy with
    // the mode specified as an argument of this application.
    Ice::ObjectPrx Publisher = topic->getPublisher();
    if (TopicName.compare(Dict::TopicNames::Monitor) == 0) {
        PublisherData = DataPrx::uncheckedCast(Publisher);
    } else if (TopicName.compare(Dict::TopicNames::Supervisor) == 0) {
        PublisherControl = ControlPrx::uncheckedCast(Publisher);
    }

    BaseType::Startup();

    return true;
}

#define PUBLISH_BEGIN\
    BaseType::Run();\
    try {
#define PUBLISH_END\
    }\
    catch(const Ice::CommunicatorDestroyedException & e) {\
        SFLOG_ERROR << PUBLISHER_INFO << "Error in publishing events: " << e.what() << std::endl;\
    }\
    BaseType::Stop();

void Publisher::Publish(const std::string & json)
{
    PUBLISH_BEGIN;

    if (TopicName.compare(Dict::TopicNames::Monitor) == 0) {
        PublisherData->CollectSample(json);
    } else if (TopicName.compare(Dict::TopicNames::Supervisor) == 0) {
        PublisherControl->Command(json);
    }

    //PublisherData->Event(IceUtil::Time::now().toDateTime());
    //IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));

    PUBLISH_END;
}


void Publisher::Stop(void)
{
    BaseType::Stop();
}

const std::string Publisher::GetDefaultConfigFilePath(void)
{
    std::string path(SF_COMMUNICATOR_CONFIG_DIR);
    path += "/config.pub";

    return path;
}

};
