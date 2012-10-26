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
#include "statemachine.h"
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
// Simulated Force Sensor Component (could be generalized as generic sensor wrapper component)
//-------------------------------------------------- 
const std::string ForceSensorComponent::NameOfProvidedInterface = "ForceSensorProvided";

ForceSensorComponent::ForceSensorComponent(const std::string & name, double period)
    : mtsTaskPeriodic(name, period, false, 5000)
{
    ForceScalar = 0.0;
    ForceVector.SetSize(6); // x,y,z,tx,ty,tz
    ForceVector.SetAll(0.0);

    StateTable.AddData(ForceScalar, "ForceScalar");
    StateTable.AddData(ForceVector, "ForceVector");

    mtsInterfaceProvided * provided = AddInterfaceProvided(NameOfProvidedInterface);
    CMN_ASSERT(provided);
    provided->AddCommandReadState(StateTable, ForceScalar, "GetForceScalar");
    provided->AddCommandReadState(StateTable, ForceVector, "GetForceVector");
}

ForceSensorComponent::~ForceSensorComponent(void)
{
}

void ForceSensorComponent::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    ForceScalar += 0.01;
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
        if (count++ == 1000) {
            double forceScalar;
            GetForceScalar(forceScalar);
            std::cout << forceScalar << std::endl;
            count = 0;
        }
    }
    //this->FaultState.ProcessEvent(SF::State::FAULT_DETECTION);
    //std::cout << this->FaultState.GetStateString() << std::endl;
}

// MJ TODO: video component, application component, or robot control wrapper
// (e.g., AMC driver communicator) component could be added.
