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
//
#include "baseIce.h"
#include "dict.h"

using namespace SC;

BaseIce::BaseIce(void)
    : State(INVALID),
      TopicName(NONAME),
      Topic(Topic::INVALID),
      IcePropertyFileName(NONAME)
{
    // Default constructor should not be used
    SCASSERT(false);
}

BaseIce::BaseIce(const std::string & topicName, const std::string & propertyFileName)
    : State(INIT),
      TopicName(topicName),
      IcePropertyFileName(propertyFileName)
{
    // Determine type of subscriber depending on topic
    if (TopicName.compare(SC::Dict::TopicNames::CONTROL) == 0)
        Topic = Topic::CONTROL;
    else if (TopicName.compare(SC::Dict::TopicNames::DATA) == 0)
        Topic = Topic::DATA;
    else {
        std::stringstream ss;
        ss << "Invalid topic name: \"" << topicName << "\"" << std::endl;
        SCLOG_ERROR << ss.str() << std::endl;
        SCTHROW(ss.str());
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
    IceInitData.logger = new SCLogger();

    // Initialize Ice communicator and Ice logger
    IceCommunicator = Ice::initialize(IceInitData);
    IceLogger = IceCommunicator->getLogger();
}
