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

using namespace SC;

cisstAccessor::cisstAccessor(bool enablePublisherControl,  bool enablePublisherData,
                             bool enableSubscriberControl, bool enableSubscriberData,
                             SC::SCCallback * cbSubscriberControl,
                             SC::SCCallback * cbSubscriberData)

{
    Enabled[PUBLISHER][SC::Topic::CONTROL]  = enablePublisherControl;
    Enabled[PUBLISHER][SC::Topic::DATA]     = enablePublisherData;
    Enabled[SUBSCRIBER][SC::Topic::CONTROL] = enableSubscriberControl;
    Enabled[SUBSCRIBER][SC::Topic::DATA]    = enableSubscriberData;

    // sanity check
    if (enableSubscriberControl && !cbSubscriberControl)
        cmnThrow("cisstAccessor: callback for CONTROL subscriber is null.");
    if (enableSubscriberData && !cbSubscriberData)
        cmnThrow("cisstAccessor: callback for DATA subscriber is null.");

#define INIT_PUBLISHER(_instance, _topic)\
    Publishers._instance = new SC::Publisher(_topic);\
    if (!Publishers._instance->Startup()) {\
        std::stringstream ss;\
        ss << "cisstAccessor: Failed to initialize publisher for topic \""\
            << _topic << "\"";\
        cmnThrow(ss.str());\
    }

    if (enablePublisherControl) {
        INIT_PUBLISHER(Control, SC::Dict::TopicNames::CONTROL);
    }
    if (enablePublisherData) {
        INIT_PUBLISHER(Data, SC::Dict::TopicNames::DATA);
    }
#undef INIT_PUBLISHER

#define INIT_SUBSCRIBER(_index, _instance, _topic, _callback)\
    Subscribers._instance = new SC::Subscriber(_topic, _callback);\
    ThreadSubscribers[_index].Thread.Create<cisstAccessor, unsigned int>(this, &cisstAccessor::StartSubscriber, _index);\
    ThreadSubscribers[_index].ThreadEventBegin.Wait();

    if (enableSubscriberControl) {
        SubscriberCallbacks.Control = cbSubscriberControl;
        INIT_SUBSCRIBER(SC::Topic::CONTROL, 
                        Control, 
                        SC::Dict::TopicNames::CONTROL, 
                        SubscriberCallbacks.Control);
    }
    if (enableSubscriberData) {
        SubscriberCallbacks.Data = cbSubscriberData;
        INIT_SUBSCRIBER(SC::Topic::DATA,
                        Data,
                        SC::Dict::TopicNames::DATA,
                        SubscriberCallbacks.Data);
    }
#undef INIT_SUBSCRIBER
}

cisstAccessor::~cisstAccessor()
{
    StopSubscriber();

#define DELETE_INSTANCE(_obj)\
    if (_obj) {\
        delete _obj;\
        _obj = 0;\
    }

    DELETE_INSTANCE(Publishers.Control);
    DELETE_INSTANCE(Publishers.Data);

    // SubscriberCallback is deleted by Subscriber's destructor.
    DELETE_INSTANCE(Subscribers.Control);
    DELETE_INSTANCE(Subscribers.Data);
}

void * cisstAccessor::StartSubscriber(unsigned int idx)
{
    SCASSERT(0 <= idx && idx < SC::Topic::TOTAL_TOPIC_COUNT);

    ThreadSubscribers[idx].Running = true;
    ThreadSubscribers[idx].ThreadEventBegin.Raise();

    SC::Subscriber * sub = ((idx == SC::Topic::CONTROL) ? Subscribers.Control : Subscribers.Data);
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
    if (Subscribers.Control && ThreadSubscribers[SC::Topic::CONTROL].Running) {
        ThreadSubscribers[SC::Topic::CONTROL].Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice cisstAccessor
        Subscribers.Control->Stop();
        ThreadSubscribers[SC::Topic::CONTROL].ThreadEventEnd.Wait();
    }

    if (Subscribers.Data && ThreadSubscribers[SC::Topic::DATA].Running) {
        ThreadSubscribers[SC::Topic::DATA].Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice cisstAccessor
        Subscribers.Data->Stop();
        ThreadSubscribers[SC::Topic::DATA].ThreadEventEnd.Wait();
    }
}

SC::Publisher * cisstAccessor::GetPublisher(SC::Topic::Type topicType) const
{
    switch (topicType) {
    case SC::Topic::CONTROL:
        return Publishers.Control;
    case SC::Topic::DATA:
        return Publishers.Data;
    default:
        return 0;
    }
}

SC::Subscriber * cisstAccessor::GetSubscriber(SC::Topic::Type topicType) const
{
    switch (topicType) {
    case SC::Topic::CONTROL:
        return Subscribers.Control;
    case SC::Topic::DATA:
        return Subscribers.Data;
    default:
        return 0;
    }
}

SC::SCCallback * cisstAccessor::GetSubscriberCallback(SC::Topic::Type topicType) const
{
    switch (topicType) {
    case SC::Topic::CONTROL:
        return SubscriberCallbacks.Control;
    case SC::Topic::DATA:
        return SubscriberCallbacks.Data;
    default:
        return 0;
    }
}
