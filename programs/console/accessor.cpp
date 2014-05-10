//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 9, 2014
// Last revision: May 10, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "accessor.h"

//
// Subscriber callback
//
void ConsoleSubscriberCallback::Callback(const std::string & json)
{
    PROBE << "SUBSCRIBER: " << json << std::endl;
}

//
// ZeroC IceStorm Accessor
//
Accessor::Accessor(void)
{
    Publisher = new SF::Publisher(SF::Dict::TopicNames::control);
    if (!Publisher->Startup()) {
        std::stringstream ss;
        ss << "Accessor: Failed to initialize publisher for topic \""
            << SF::Dict::TopicNames::data << "\"";
        cmnThrow(ss.str());
    }

    SubscriberCallback = new ConsoleSubscriberCallback;
    Subscriber = new SF::Subscriber(SF::Dict::TopicNames::control, SubscriberCallback);
    //Subscriber = new SF::Subscriber(SF::Dict::TopicNames::data, SubscriberCallback);
    ThreadSubscriber.Thread.Create<Accessor, unsigned int>(this, &Accessor::RunSubscriber, 0);
    ThreadSubscriber.ThreadEventBegin.Wait();
}

Accessor::~Accessor()
{
    if (Subscriber && ThreadSubscriber.Running) {
        ThreadSubscriber.Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice Accessor
        Subscriber->Stop();
        ThreadSubscriber.ThreadEventEnd.Wait();

        delete Subscriber;
        // SubscriberCallback is deleted by Subscriber's destructor.
    }

    delete Publisher;
}


void * Accessor::RunSubscriber(unsigned int CMN_UNUSED(arg))
{
    ThreadSubscriber.Running = true;
    ThreadSubscriber.ThreadEventBegin.Raise();

    try {
        Subscriber->Startup();
        while (ThreadSubscriber.Running) {
            Subscriber->Run();
            std::cout << ".";
        }
    } catch (const Ice::InitializationException & e) {
        CMN_LOG_RUN_ERROR << "Accessor::RunSubscriber: ice init failed: " << e.what() << std::endl;
    } catch (const Ice::AlreadyRegisteredException & e) {
        CMN_LOG_RUN_ERROR << "Accessor::RunSubscriber: ice init failed: " << e.what() << std::endl;
    } catch (const std::exception & e) {
        CMN_LOG_RUN_ERROR << "Accessor::RunSubscriber: exception: " << e.what() << std::endl;
    }

    ThreadSubscriber.ThreadEventEnd.Raise();

    return 0;
}
