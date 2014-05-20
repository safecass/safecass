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
#include "baseIce.h"
#include "dict.h"

using namespace SF;

BaseIce::BaseIce(void)
    : TopicName(NONAME),
      Topic(Topic::INVALID),
      IcePropertyFileName(NONAME),
      State(INVALID)
{
    // Default constructor should not be used
    SFASSERT(false);
}

BaseIce::BaseIce(const std::string & topicName, const std::string & propertyFileName)
    : TopicName(topicName),
      IcePropertyFileName(propertyFileName),
      State(INIT)
{
    // Determine type of subscriber depending on topic
    if (TopicName.compare(SF::Dict::TopicNames::CONTROL) == 0)
        Topic = Topic::CONTROL;
    else if (TopicName.compare(SF::Dict::TopicNames::DATA) == 0)
        Topic = Topic::DATA;
    else {
        std::stringstream ss;
        ss << "Invalid topic name: \"" << topicName << "\"" << std::endl;
        SFLOG_ERROR << ss.str() << std::endl;
        SFTHROW(ss.str());
    }
}

BaseIce::~BaseIce()
{
}

#if 0
void BaseIce::Init(void) {
    StateChange(INIT);
}

bool BaseIce::Startup(void) {
    StateChange(READY);
    return true;
}

void BaseIce::Start(void) {
    StateChange(RUNNING);
}

void BaseIce::Stop(void) {
    StateChange(READY);
}

void BaseIce::Cleanup(void) {
    StateChange(STOP);
}

bool BaseIce::IsInitialized(void) const {
    return (State == READY);
}

bool BaseIce::IsRunning(void) const {
    return (State == RUNNING);
}

bool BaseIce::IsStopped(void) const {
    return (State == READY);
}

bool BaseIce::IsCleanedUp(void) const {
    return (State == STOP);
}
#endif

void BaseIce::StateChange(CommunicatorStateType state)
{
    if (State == state) return;

#if 0
    std::cout << "STATE CHANGE: FROM ";
    switch (State) {
    case INIT: std::cout << "INIT"; break;
    case STARTUP: std::cout << "STARTUP"; break;
    case RUNNING: std::cout << "RUNNING"; break;
    case STOP: std::cout << "STOP"; break;
    }
    std::cout << " TO ";
    switch (state) {
    case INIT: std::cout << "INIT"; break;
    case STARTUP: std::cout << "STARTUP"; break;
    case RUNNING: std::cout << "RUNNING"; break;
    case STOP: std::cout << "STOP"; break;
    }
    std::cout << "\n";
#endif

    Mutex.lock();
    State = state;
    Mutex.unlock();

}

void BaseIce::IceInitialize(void)
{
    // Load configuration file to set the properties of ICE proxy
    IceInitData.properties = Ice::createProperties();
    IceInitData.properties->load(IcePropertyFileName);

    // Create logger
    IceInitData.logger = new SFLogger();

    // Initialize Ice communicator and Ice logger
    IceCommunicator = Ice::initialize(IceInitData);
    IceLogger = IceCommunicator->getLogger();
}
