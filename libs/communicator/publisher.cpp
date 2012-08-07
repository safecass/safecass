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

namespace SF {

// Publisher id (unique within a process)
unsigned int Publisher::Id = 0;

Publisher::Publisher()
    : BaseIce(GetDefaultConfigFilePath())
{
    Init();
}

Publisher::Publisher(const std::string & propertyFileName)
    : BaseIce(propertyFileName)
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

    SFLOG_INFO << "Publisher " << Id << " created with config file: " << this->IcePropertyFileName << std::endl;
}

void Publisher::Startup(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager = 
        IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    if (!manager) {
        SFLOG_ERROR << "PUBLISHER: invalid proxy" << std::endl;
        return;
    }

    // Retrieve the topic.
    IceStorm::TopicPrx topic;
    const std::string topicName = "event";
    try {   
        topic = manager->retrieve(topicName);
    } 
    catch(const IceStorm::NoSuchTopic&) {   
        try {   
            topic = manager->create(topicName);
        } 
        catch(const IceStorm::TopicExists&) {   
            SFLOG_ERROR << "PUBLISHER: temporary failure. try again." << std::endl;
            return;
        }   
    }   

    // Get the topic's publisher object, and create a Clock proxy with
    // the mode specified as an argument of this application.
    Ice::ObjectPrx Publisher = topic->getPublisher();
    MonitorSamples = MonitorSamplesPrx::uncheckedCast(Publisher);

    BaseType::Startup();
}

#define PUBLISH_BEGIN\
    BaseType::Run();\
    try {
#define PUBLISH_END\
    }\
    catch(const Ice::CommunicatorDestroyedException & e) {\
        SFLOG_ERROR << "Error in publishing events: " << e.what() << std::endl;\
    }\
    BaseType::Stop();

void Publisher::Publish(const std::string & processName, const std::string & componentName, double period)
{
    PUBLISH_BEGIN;

    ComponentIdType componentId;
    componentId.ProcessName = processName;
    componentId.ComponentName = componentName;

    MonitorSamples->PeriodSample(componentId, period);
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
