//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: Apr 19, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "baseIce.h"

namespace SF {

BaseIce::BaseIce(const std::string & propertyFileName)
    : IcePropertyFileName(propertyFileName),
      State(INIT)
{
    /* MJ TODO: exception throwing and handling (later)
    ifstream fp(propertyFileName.c_str());
    if (!fp.good())
        throw 
    */
        
    SFLOG_DEBUG << "BaseIce: Ice property file name: " << IcePropertyFileName << std::endl;
}

BaseIce::~BaseIce()
{
}

void BaseIce::Init(void) {
    StateChange(INIT);
}

bool BaseIce::Startup(void) {
    StateChange(STARTUP);
    return true;
}

void BaseIce::Run(void) {
    StateChange(RUNNING);
}

void BaseIce::Stop(void) {
    StateChange(STOP);
}

bool BaseIce::IsInitialized(void) const {
    return (State == INIT);
}

bool BaseIce::IsStartup(void) const {
    return (State == STARTUP);
}

bool BaseIce::IsRunning(void) const {
    return (State == RUNNING);
}

bool BaseIce::IsStopped(void) const {
    return (State == STOP);
}

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

};
