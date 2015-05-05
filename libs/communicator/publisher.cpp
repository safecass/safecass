//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "publisher.h"
#include "config.h"
#include "dict.h"

#define DEBUGLOG 0

using namespace SC;

// Publisher id (unique within a process)
unsigned int Publisher::Id = 0;

#define PUBLISHER_INFO "Publisher " << Id << " (\"" << this->TopicName << "\"): "

Publisher::Publisher(void): BaseIce(NONAME, NONAME)
{
    // Default constructor should not be used
    SCASSERT(false);
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

    SCLOG_INFO << PUBLISHER_INFO << "Created with config file: " << this->IcePropertyFileName << std::endl;
    SCLOG_INFO << PUBLISHER_INFO << "Created with topic name: " << this->TopicName << std::endl;
}

bool Publisher::Startup(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager;
    try {
        manager = IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    } catch (const Ice::ConnectionRefusedException & e) {
        SCLOG_ERROR << PUBLISHER_INFO << "Failed to initialize IceStorm.  Check if IceBox is running: " << e.what() << std::endl;
        return false;
    }

    if (!manager) {
        SCLOG_ERROR << PUBLISHER_INFO << "Invalid proxy" << std::endl;
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
            SCLOG_ERROR << PUBLISHER_INFO << "Topic not found. Try again." << std::endl;
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
        SCASSERT(false);
    }

    //BaseType::Startup();

    return true;
}

#define PUBLISH_BEGIN\
    try {
#define PUBLISH_END\
    }\
    catch(const Ice::CommunicatorDestroyedException & e) {\
        SCLOG_ERROR << PUBLISHER_INFO << "Error in publishing events: " << e.what() << std::endl;\
        return false;\
    }\
    catch (const IceUtil::NullHandleException & e) {\
        SCLOG_ERROR << PUBLISHER_INFO << "Error in publishing events: " << e.what() << std::endl;\
        return false;\
    }\
    catch (const std::exception & e) {\
        SCLOG_ERROR << PUBLISHER_INFO << "Error in publishing events: " << e.what() << std::endl;\
        return false;\
    }

bool Publisher::PublishData(const Topic::Data::CategoryType category, const std::string & json)
{
    //_PROBE << "json publish: " << json << std::endl;
    PUBLISH_BEGIN
    {
        switch (category) {
        case Topic::Data::MONITOR:  PublisherData->Monitor(json); break;
        case Topic::Data::EVENT:    PublisherData->Event(json);   break;
        case Topic::Data::READ_RES: PublisherData->ReadRes(json); break;
        default:
            SCLOG_ERROR << PUBLISHER_INFO << "Failed to publish message (invalid data category): " << json << std::endl;
            return false;
        }
        //PublisherData->Event(IceUtil::Time::now().toDateTime());
        //IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    }
    PUBLISH_END

    return true;
}

bool Publisher::PublishControl(const Topic::Control::CategoryType category, const std::string & json)
{
    PUBLISH_BEGIN
    {
        switch (category) {
        case Topic::Control::COMMAND:  PublisherControl->Command(json); break;
        case Topic::Control::READ_REQ: PublisherControl->ReadReq(json); break;
        case Topic::Control::STATE_UPDATE: PublisherControl->StateUpdate(json); break;
        default:
            SCLOG_ERROR << PUBLISHER_INFO << "Failed to publish message (invalid control category): " << json << std::endl;
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
    std::string path(SC_COMMUNICATOR_CONFIG_DIR);
    path += "/config.pub";

    return path;
}
