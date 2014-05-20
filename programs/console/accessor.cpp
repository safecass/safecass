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
#define INIT_PUBLISHER(_instance, _topic)\
    Publishers._instance = new SF::Publisher(SF::Dict::TopicNames::_topic);\
    if (!Publishers._instance->Startup()) {\
        std::stringstream ss;\
        ss << "Accessor: Failed to initialize publisher for topic \""\
            << SF::Dict::TopicNames::_topic << "\"";\
        cmnThrow(ss.str());\
    }

    INIT_PUBLISHER(Control, CONTROL);
    INIT_PUBLISHER(Data,    DATA);
#undef INIT_PUBLISHER

#define INIT_SUBSCRIBER(_index, _instance, _topic)\
    SubscriberCallback = new ConsoleSubscriberCallback;\
    Subscribers._instance = new SF::Subscriber(SF::Dict::TopicNames::_topic, SubscriberCallback);\
    ThreadSubscriber[_index].Thread.Create<Accessor, unsigned int>(this, &Accessor::StartSubscriber, _index);\
    ThreadSubscriber[_index].ThreadEventBegin.Wait();
    INIT_SUBSCRIBER(Accessor::CONTROL, Control, CONTROL);
    INIT_SUBSCRIBER(Accessor::DATA, Data, DATA);
#undef INIT_SUBSCRIBER
}

Accessor::~Accessor()
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

void * Accessor::StartSubscriber(unsigned int idx)
{
    SFASSERT(0 <= idx && idx < Accessor::NUM_SUBSCRIBERS);

    ThreadSubscriber[idx].Running = true;
    ThreadSubscriber[idx].ThreadEventBegin.Raise();

    SF::Subscriber * sub = ((idx == Accessor::CONTROL) ? Subscribers.Control : Subscribers.Data);
    try {
        sub->Startup();
        while (ThreadSubscriber[idx].Running)
            sub->Run();
    } catch (const Ice::InitializationException & e) {
        CMN_LOG_RUN_ERROR << "Accessor::StartSubscriber: ice init failed: " << e.what() << std::endl;
    } catch (const Ice::AlreadyRegisteredException & e) {
        CMN_LOG_RUN_ERROR << "Accessor::StartSubscriber: ice init failed: " << e.what() << std::endl;
    } catch (const std::exception & e) {
        CMN_LOG_RUN_ERROR << "Accessor::StartSubscriber: exception: " << e.what() << std::endl;
    }

    ThreadSubscriber[idx].ThreadEventEnd.Raise();

    return 0;
}

void Accessor::StopSubscriber(void)
{
    if (Subscribers.Control && ThreadSubscriber[Accessor::CONTROL].Running) {
        ThreadSubscriber[Accessor::CONTROL].Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice Accessor
        Subscribers.Control->Stop();
        ThreadSubscriber[Accessor::CONTROL].ThreadEventEnd.Wait();
    }

    if (Subscribers.Data && ThreadSubscriber[Accessor::DATA].Running) {
        ThreadSubscriber[Accessor::DATA].Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice Accessor
        Subscribers.Data->Stop();
        ThreadSubscriber[Accessor::DATA].ThreadEventEnd.Wait();
    }
}

bool Accessor::RequestFilterList(const std::string & UNUSED(processName), 
                                 const std::string & UNUSED(componentName)) const
{
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, "{ target: \"*\" }")) {
        std::cerr << "Accessor: Failed to publish message (Control::READ_REQ)" << std::endl;
        return false;
    }

    return true;
}
