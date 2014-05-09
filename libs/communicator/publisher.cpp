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
#include "publisher.h"
#include "config.h"
#include "dict.h"

namespace SF {

// Publisher id (unique within a process)
unsigned int Publisher::Id = 0;

#define PUBLISHER_INFO "Publisher " << Id << " (\"" << this->TopicName << "\"): "

Publisher::Publisher(void): BaseIce(NONAME, NONAME)
{
    // Default constructor should not be used
    SFASSERT(false);
}

Publisher::Publisher(const std::string & topicName)
    : BaseIce(topicName, GetDefaultConfigFilePath())
{
    Init();
}

Publisher::Publisher(const std::string & topicName, const std::string & propertyFileName)
    : BaseIce(topicName, propertyFileName)
{
    Init();
}

Publisher::~Publisher()
{
}

void Publisher::Init(void)
{
    ++Id;

    SFLOG_INFO << PUBLISHER_INFO << "Created with config file: " << this->IcePropertyFileName << std::endl;
    SFLOG_INFO << PUBLISHER_INFO << "Created with topic name: " << this->TopicName << std::endl;
}

bool Publisher::Startup(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager;
    try {
        manager = IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    } catch (const Ice::ConnectionRefusedException & e) {
        SFLOG_ERROR << PUBLISHER_INFO << "Failed to initialize IceStorm.  Check if IceBox is running: " << e.what() << std::endl;
        return false;
    }

    if (!manager) {
        SFLOG_ERROR << PUBLISHER_INFO << "Invalid proxy" << std::endl;
        return false;
    }

    // Retrieve the topic.
    IceStorm::TopicPrx topic;
    try {   
        topic = manager->retrieve(this->TopicName);
    } catch(const IceStorm::NoSuchTopic&) {   
        try {   
            topic = manager->create(this->TopicName);
        } 
        catch(const IceStorm::TopicExists&) {   
            SFLOG_ERROR << PUBLISHER_INFO << "Topic not found. Try again." << std::endl;
            return false;
        }   
    }   

    // Get the topic's publisher object, and create topic proxy
    Ice::ObjectPrx publisher = topic->getPublisher();
    
    // Get the topic's publisher object, and create a proper proxy depending on
    // the topic.
    switch (this->Topic) {
    case Topic::CONTROL:
        PublisherControl = ControlPrx::uncheckedCast(publisher);
        break;
    case Topic::DATA:
        PublisherData = DataPrx::uncheckedCast(publisher);
        break;
    default:
        SFASSERT(false);
    }

    //BaseType::Startup();

    return true;
}

#define PUBLISH_BEGIN\
    try {
#define PUBLISH_END\
    }\
    catch(const Ice::CommunicatorDestroyedException & e) {\
        SFLOG_ERROR << PUBLISHER_INFO << "Error in publishing events: " << e.what() << std::endl;\
        return false;\
    }\

bool Publisher::PublishData(const Topic::Data::CategoryType category, const std::string & json)
{
    PUBLISH_BEGIN
    {
        switch (category) {
        case Topic::Data::MONITOR: PublisherData->Monitor(json); break;
        case Topic::Data::EVENT:   PublisherData->Event(json);   break;
        case Topic::Data::LOG:     PublisherData->Log(json);     break;
        default:
            SFLOG_ERROR << PUBLISHER_INFO << "Failed to publish message (invalid data category): " << json << std::endl;
            return false;
        }
        //PublisherData->Event(IceUtil::Time::now().toDateTime());
        //IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    }
    PUBLISH_END

    return true;
}

bool Publisher::PublishControl(const Topic::Control::CategoryType  category, const std::string & json)
{
    PUBLISH_BEGIN
    {
        switch (category) {
        case Topic::Control::COMMAND: PublisherControl->Command(json); break;
        default:
            SFLOG_ERROR << PUBLISHER_INFO << "Failed to publish message (invalid control category): " << json << std::endl;
            return false;
        }
    }
    PUBLISH_END

    return true;
}

void Publisher::Stop(void)
{
    //BaseType::Stop();
}

const std::string Publisher::GetDefaultConfigFilePath(void)
{
    std::string path(SF_COMMUNICATOR_CONFIG_DIR);
    path += "/config.pub";

    return path;
}

};
