//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 20, 2014
// Last revision: May 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "cisstAccessor.h"

using namespace SF;

cisstAccessor::cisstAccessor(bool enablePublisherControl,  bool enablePublisherData,
                             bool enableSubscriberControl, bool enableSubscriberData,
                             SF::SFCallback * cbSubscriberControl,
                             SF::SFCallback * cbSubscriberData)

{
    Enabled[PUBLISHER][SF::Topic::CONTROL]  = enablePublisherControl;
    Enabled[PUBLISHER][SF::Topic::DATA]     = enablePublisherData;
    Enabled[SUBSCRIBER][SF::Topic::CONTROL] = enableSubscriberControl;
    Enabled[SUBSCRIBER][SF::Topic::DATA]    = enableSubscriberData;

    // sanity check
    if (enableSubscriberControl && !cbSubscriberControl)
        cmnThrow("cisstAccessor: callback for CONTROL subscriber is null.");
    if (enableSubscriberData && !cbSubscriberData)
        cmnThrow("cisstAccessor: callback for DATA subscriber is null.");

#define INIT_PUBLISHER(_instance, _topic)\
    Publishers._instance = new SF::Publisher(_topic);\
    if (!Publishers._instance->Startup()) {\
        std::stringstream ss;\
        ss << "cisstAccessor: Failed to initialize publisher for topic \""\
            << _topic << "\"";\
        cmnThrow(ss.str());\
    }

    if (enablePublisherControl) {
        INIT_PUBLISHER(Control, SF::Dict::TopicNames::CONTROL);
    }
    if (enablePublisherData) {
        INIT_PUBLISHER(Data, SF::Dict::TopicNames::DATA);
    }
#undef INIT_PUBLISHER

#define INIT_SUBSCRIBER(_index, _instance, _topic, _callback)\
    Subscribers._instance = new SF::Subscriber(_topic, _callback);\
    ThreadSubscribers[_index].Thread.Create<cisstAccessor, unsigned int>(this, &cisstAccessor::StartSubscriber, _index);\
    ThreadSubscribers[_index].ThreadEventBegin.Wait();

    if (enableSubscriberControl) {
        SubscriberCallbacks.Control = cbSubscriberControl;
        INIT_SUBSCRIBER(SF::Topic::CONTROL, 
                        Control, 
                        SF::Dict::TopicNames::CONTROL, 
                        SubscriberCallbacks.Control);
    }
    if (enableSubscriberData) {
        SubscriberCallbacks.Data = cbSubscriberData;
        INIT_SUBSCRIBER(SF::Topic::DATA,
                        Data,
                        SF::Dict::TopicNames::DATA,
                        SubscriberCallbacks.Data);
    }
#undef INIT_SUBSCRIBER
}

cisstAccessor::~cisstAccessor()
{
    StopSubscriber();

    if (Publishers.Control)
        delete Publishers.Control;
    if (Publishers.Data)
        delete Publishers.Data;

    // SubscriberCallback is deleted by Subscriber's destructor.
    if (Subscribers.Control)
        delete Subscribers.Control;
    if (Subscribers.Data)
        delete Subscribers.Data;
}

void * cisstAccessor::StartSubscriber(unsigned int idx)
{
    SFASSERT(0 <= idx && idx < SF::Topic::TOTAL_TOPIC_COUNT);

    ThreadSubscribers[idx].Running = true;
    ThreadSubscribers[idx].ThreadEventBegin.Raise();

    SF::Subscriber * sub = ((idx == SF::Topic::CONTROL) ? Subscribers.Control : Subscribers.Data);
    try {
        sub->Startup();
        while (ThreadSubscribers[idx].Running)
            sub->Run();
    } catch (const Ice::InitializationException & e) {
        CMN_LOG_RUN_ERROR << "cisstAccessor::StartSubscriber: ice init failed: " << e.what() << std::endl;
    } catch (const Ice::AlreadyRegisteredException & e) {
        CMN_LOG_RUN_ERROR << "cisstAccessor::StartSubscriber: ice init failed: " << e.what() << std::endl;
    } catch (const std::exception & e) {
        CMN_LOG_RUN_ERROR << "cisstAccessor::StartSubscriber: exception: " << e.what() << std::endl;
    }

    ThreadSubscribers[idx].ThreadEventEnd.Raise();

    return 0;
}

void cisstAccessor::StopSubscriber(void)
{
    if (Subscribers.Control && ThreadSubscribers[SF::Topic::CONTROL].Running) {
        ThreadSubscribers[SF::Topic::CONTROL].Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice cisstAccessor
        Subscribers.Control->Stop();
        ThreadSubscribers[SF::Topic::CONTROL].ThreadEventEnd.Wait();
    }

    if (Subscribers.Data && ThreadSubscribers[SF::Topic::DATA].Running) {
        ThreadSubscribers[SF::Topic::DATA].Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice cisstAccessor
        Subscribers.Data->Stop();
        ThreadSubscribers[SF::Topic::DATA].ThreadEventEnd.Wait();
    }
}

SF::SFCallback * cisstAccessor::GetSubscriberCallback(SF::Topic::Type topicType)
{
    switch (topicType) {
    case SF::Topic::CONTROL:
        return SubscriberCallbacks.Control;
    case SF::Topic::DATA:
        return SubscriberCallbacks.Data;
    default:
        return 0;
    }
}
