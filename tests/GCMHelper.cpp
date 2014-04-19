//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 19, 2012
// Last revision: Apr 19, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "GCMHelper.h"

using namespace SF;

ForceSensor   * GCMHelper::ForceSensorComp = 0;
Control  * GCMHelper::ControlComp = 0;
Workflow * GCMHelper::WorkflowComp = 0;
mtsManagerLocal * GCMHelper::ComponentManager = 0;

void GCMHelper::cisstInit(void)
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);

    // Initialize member variables specific to this test suite
    mtsManagerLocal::InstallSafetyCoordinator();
    ComponentManager = mtsManagerLocal::GetInstance();
    if (!ComponentManager) {
        throw std::runtime_error(std::string("Failed to initialize local component manager."));
    }

    // Create and add components to local component manager
#if 0
    ForceSensorComp   = new ForceSensor("ForceSensor", 5 * cmn_ms);
    ControlComp  = new Control("Control", 10 * cmn_ms);
    WorkflowComp = new Workflow("Workflow", 100 * cmn_ms);
#else
    forceSensor = new ForceSensor("ForceSensor", 0.5);
    control     = new Control("Control", 1);
    workflow    = new Workflow("Workflow", 1);
#endif
    ComponentManager->AddComponent(forceSensor);
    ComponentManager->AddComponent(control);
    ComponentManager->AddComponent(workflow);

    // create the tasks, i.e. find the commands
    ComponentManager->CreateAll();
    ComponentManager->WaitForStateAll(mtsComponentState::READY);

    // start the periodic Run
    ComponentManager->StartAll();
    ComponentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // TODO: define and install filters
    // C++ first
    // JSON next
    // refer to 'filter' example
}

void GCMHelper::cisstCleanup(void)
{
    if (ComponentManager) {
        ComponentManager->KillAll();
        ComponentManager->WaitForStateAll(mtsComponentState::FINISHED, 5.0 * cmn_s);
        ComponentManager->Cleanup();
    }
}

//-------------------------------------------------- 
// ForceSensor component
//
// Example task that simulates sensor wrapper
ForceSensor::ForceSensor(const std::string & name, double period) 
    : mtsTaskPeriodic(name, period, false, 5000)
{
    ForceX = 0.0;
    ForceXYZ.resize(3);

    StateTable.AddData(ForceX, "ForceX");
    StateTable.AddData(ForceXYZ, "ForceXYZ");

    mtsInterfaceRequired * required = AddInterfaceRequired("R1");
    if (required) {
    }
}

void ForceSensor::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    // To test TrendVel filter for scalar type
    double delta = double(rand() % 10) * 0.1;
    ForceX += delta;
    // To test TrendVel filter for vector type
    ForceXYZ[0] += double(rand() % 10) * 0.1;
    ForceXYZ[1] += double(rand() % 10) * 0.1;
    ForceXYZ[2] += double(rand() % 10) * 0.1;

    std::cout << ForceX << "\t | [ ";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << ForceXYZ[i] << " ";
    }
    std::cout << " ]" << std::endl;
}

//-------------------------------------------------- 
// Control component
//
Control::Control(const std::string & name, double period) 
    : mtsTaskPeriodic(name, period, false, 5000)
{
}

void Control::Run(void)
{
    std::cout << "C" << std::endl;
}

//-------------------------------------------------- 
// Workflow component
//
Workflow::Workflow(const std::string & name, double period) 
    : mtsTaskPeriodic(name, period, false, 5000)
{
}

void Workflow::Run(void)
{
    std::cout << "W" << std::endl;
}
