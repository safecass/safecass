/*

  Safety Framework for Component-based Robotics

  Created on: July 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "publisher.h"

#include <monitorFDD.h>

namespace SF {

Publisher::Publisher(const std::string & propertyFileName)
    : BaseIce(propertyFileName)
{
}

Publisher::~Publisher()
{
}

void Publisher::Run(void)
{
    this->IceInitialize();

    IceStorm::TopicManagerPrx manager = 
        IceStorm::TopicManagerPrx::checkedCast(this->IceCommunicator->propertyToProxy("TopicManager.Proxy"));
    if(!manager)
    {
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
    //
    Ice::ObjectPrx publisher = topic->getPublisher();

    MonitorFDDPrx monitorFDD = MonitorFDDPrx::uncheckedCast(publisher);

    SFLOG_INFO << "publishing tick events. Press ^C to terminate the application." << std::endl;
    try
    {
        while(true)
        {
            monitorFDD->Event(IceUtil::Time::now().toDateTime());
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        }
    }
    catch(const Ice::CommunicatorDestroyedException&)
    {
        // Ignore
    }

    SFLOG_DEBUG << "FINISHED" << std::endl;
}

};
