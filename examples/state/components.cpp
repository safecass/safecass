/*

  Safety Framework for Component-based Robotics

  Created on: October 26, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

/*
#include <time.h> // for srand(time(NULL))

#include "config.h"
#include "common.h"
#include "json.h"
#include "monitor.h"
#include "cisstMonitor.h"
#include "threshold.h"
*/
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include "components.h"
/*
#include <cisstCommon/cmnGetChar.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif
*/

//-------------------------------------------------- 
// State event handler for ForceSensorComponent
//-------------------------------------------------- 
ForceSensorComponent::ForceSensorEventHandler::ForceSensorEventHandler()
    : SC::StateEventHandler()
{
    std::cout << "ForceSensorComponent state transition event handler: created\n";
}

ForceSensorComponent::ForceSensorEventHandler::~ForceSensorEventHandler()
{
    std::cout << "ForceSensorComponent state transition event handler: deleted\n";
}


void ForceSensorComponent::ForceSensorEventHandler::OnEntry(const SC::State::StateEntryExitType entryType)
{
    std::cout << "ForceSensorComponent onEntry: " << SC::State::GetString(entryType) << std::endl;
}

void ForceSensorComponent::ForceSensorEventHandler::OnExity(const SC::State::StateEntryExitType exitType)
{
    std::cout << "ForceSensorComponent onExit: " << SC::State::GetString(exitType) << std::endl;
}

void ForceSensorComponent::ForceSensorEventHandler::OnStateTransition(const SC::State::TransitionType transition)
{
    std::cout << "ForceSensorComponent onTransition: " << SC::State::GetString(transition) << std::endl;
}

//-------------------------------------------------- 
// Simulated Force Sensor Component (could be generalized as generic sensor wrapper component)
//-------------------------------------------------- 
const std::string ForceSensorComponent::NameOfProvidedInterface = "ForceSensorProvided";
const std::string ForceSensorComponent::NameOfScalarSignal = "ForceScalar";
const std::string ForceSensorComponent::NameOfVectorSignal = "ForceVector";

ForceSensorComponent::ForceSensorComponent(const std::string & name, double period)
    : mtsTaskPeriodic(name, period, false, 5000)
{
    ForceScalar = 0.0;
    ForceVector.SetSize(6); // x,y,z,tx,ty,tz
    ForceVector.SetAll(0.0);

    StateTable.AddData(ForceScalar, NameOfScalarSignal);
    StateTable.AddData(ForceVector, NameOfVectorSignal);

    mtsInterfaceProvided * provided = AddInterfaceProvided(NameOfProvidedInterface);
    CMN_ASSERT(provided);
    provided->AddCommandReadState(StateTable, ForceScalar, "GetForceScalar");
    provided->AddCommandReadState(StateTable, ForceVector, "GetForceVector");

    // If an application wants to handle state transition events, create a new event
    // handler class which is derived from SC::StateEventHandler and replace the default
    // state event handler with it by calling mtsComponent::ReplaceStateEventHandler().
    ForceSensorComponent::ForceSensorEventHandler * newEventHandler
        = new ForceSensorComponent::ForceSensorEventHandler;
    this->SetStateEventHandler(newEventHandler);

    // MJTEST
    this->FaultState->Test();
}

ForceSensorComponent::~ForceSensorComponent(void)
{
}

void ForceSensorComponent::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

#if 0 // usual codes look like this
    int ErrorCode = CheckSensorStatus();
    if (ErrorCode == ERROR_OK) {
        ForceScalar = ReadForceScalar();
    } else {
        std::cout << "Force sensor status error: error code = " << ErrorCode << std::endl;
        //
        // MJ: what do people do usually here, in case of sensor error?
        //
    }
#endif
    switch (GetFaultState()) {
        case SC::State::NORMAL:  RunNormal(); break;
        case SC::State::FAULT:   RunFault(); break;
        case SC::State::ERROR:   RunError(); break;
        case SC::State::FAILURE: RunFailure(); break;
        default: break;                         
    }
}

void ForceSensorComponent::RunNormal(void)
{
    int ErrorCode = CheckSensorStatus();
    if (ErrorCode == ERROR_OK) {
        ForceScalar = ReadForceScalar();
    } else {
        std::cout << "Force sensor status error: error code = " << ErrorCode << std::endl;

        // Determine the severity of the fault and inform the SC state machine of 
        // the fault/error/failure event depending on its severity.
        switch (ErrorCode) {
            case ERROR_DATA_RECEPTION_ERROR:
            case ERROR_SATURATION:
            case ERROR_BAD_CRC:
                //this->FaultState->HandleAbnormalEvent(FAULT);
                break;
            case ERROR_WATCHDOG:
                //this->FaultState->HandleAbnormalEvent(ERROR);
                break;
            case ERROR_DSP_DEAD:
            case ERROR_MEMORY_ERROR:
                //this->FaultState->HandleAbnormalEvent(FAILURE);
                break;
        }

        // Print-out current SC state
        std::cout << "CURRENT SC STATE: " << SC::State::GetString(this->FaultState->GetState()) << std::endl;
    }
}

void ForceSensorComponent::RunFault(void)
{
    static int count = 0;

    std::cout << "[RunFault]: Trying to recover from fault: " << ++count << std::endl;
    // try to recover from fault

    // DO NOTHING because fault removal strategy in this particular example is to wait for 
    // some time expecting the fault to be resolved.
    // Should other types of fault removal trials be made, it can be done here.
}

void ForceSensorComponent::RunError(void)
{
    static int count = 0;

    std::cout << "[RunError]: Trying to recover from error: " << ++count << std::endl;
    // try to recover from error

    // DO NOTHING because fault removal strategy in this particular example is to wait for 
    // some time expecting the fault to be resolved.
    // Should other types of fault removal trials be made, it can be done here.
}

void ForceSensorComponent::RunFailure(void)
{
    static int count = 0;

    std::cout << "[RunFailure]: Trying to recover from failure: " << ++count << std::endl;
    // try to recover from failure

    // DO NOTHING because fault removal strategy in this particular example is to wait for 
    // some time expecting the fault to be resolved.
    // Should other types of fault removal trials be made, it can be done here.
}

int ForceSensorComponent::CheckSensorStatus(void)
{
    // MJ TODO: randomly choose error code

    static int a = 0;
    if (++a == 6) {
        a = 0;
        return ERROR_DSP_DEAD;
    }
    return ERROR_OK;
}

double ForceSensorComponent::ReadForceScalar(void)
{
    return (ForceScalar + 0.01);
}

//-------------------------------------------------- 
//  Simulated Control Component
//-------------------------------------------------- 
const std::string ControlComponent::NameOfRequiredInterface = "ControllerRequired";

ControlComponent::ControlComponent(const std::string & name, double period)
    : mtsTaskPeriodic(name, period, false, 5000)
{
    mtsInterfaceRequired * required = AddInterfaceRequired(NameOfRequiredInterface);
    CMN_ASSERT(required);

    required->AddFunction("GetForceScalar", GetForceScalar);
    required->AddFunction("GetForceVector", GetForceVector);
}

ControlComponent::~ControlComponent(void)
{
}

void ControlComponent::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    if (GetForceScalar.IsValid()) {
        static int count = 0;
        if (count++ == 1) {
            double forceScalar;
            GetForceScalar(forceScalar);
            std::cout << forceScalar << std::endl;
            count = 0;
        }
    }
    //this->FaultState.ProcessEvent(SC::State::FAULT_DETECTION);
    //std::cout << this->FaultState.GetStateString() << std::endl;
}

// MJ TODO: video component, application component, or robot control wrapper
// (e.g., AMC driver communicator) component could be added.
