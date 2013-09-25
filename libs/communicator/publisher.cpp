/*

  Safety Framework for Component-based Robotics

  Created on: July 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

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

void Publisher::Startup(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager = 
        IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    if (!manager) {
        SFLOG_ERROR << PUBLISHER_INFO << "Invalid proxy" << std::endl;
        return;
    }

    // Retrieve the topic.
    IceStorm::TopicPrx topic;
    const std::string topicName = TopicName;
    try {   
        topic = manager->retrieve(topicName);
    } 
    catch(const IceStorm::NoSuchTopic&) {   
        try {   
            topic = manager->create(topicName);
        } 
        catch(const IceStorm::TopicExists&) {   
            SFLOG_ERROR << PUBLISHER_INFO << "Temporary failure. try again." << std::endl;
            return;
        }   
    }   

    // Get the topic's publisher object, and create a SF proxy with
    // the mode specified as an argument of this application.
    Ice::ObjectPrx Publisher = topic->getPublisher();
    if (TopicName.compare(Dict::TopicNames::Monitor) == 0) {
        MonitorSamples = MonitorSamplesPrx::uncheckedCast(Publisher);
    } else if (TopicName.compare(Dict::TopicNames::Supervisor) == 0) {
        SupervisorControls = SupervisorControlsPrx::uncheckedCast(Publisher);
    }

    BaseType::Startup();
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
        MonitorSamples->CollectSample(json);
    } else if (TopicName.compare(Dict::TopicNames::Supervisor) == 0) {
        SupervisorControls->ControlCommand(json);
    }

    //MonitorSamples->Event(IceUtil::Time::now().toDateTime());
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
