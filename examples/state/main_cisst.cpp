/*

  Safety Framework for Component-based Robotics

  Created on: October 12, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include <time.h> // for srand(time(NULL))

#include "config.h"
#include "common.h"
#include "json.h"
#include "monitor.h"
#include "cisstMonitor.h"
#include "threshold.h"
#include "statemachine.h"

#include <cisstCommon/cmnGetChar.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

// To use predefined components
#include "components.h"

using namespace SF;

int main(int argc, char *argv[])
{
    srand(time(NULL));

#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT|MCL_FUTURE);
#endif

#if ENABLE_G2LOG
    // Logger setup
    g2LogWorker logger(argv[0], "./");
    g2::initializeLogging(&logger);
    std::cout << "Log file: \"" << logger.logFileName() << "\"\n" << std::endl;
#endif

    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    
    // Get instance of the cisst Component Manager
    mtsComponentManager::InstallSafetyCoordinator();
    mtsComponentManager * ComponentManager = mtsComponentManager::GetInstance();

    // Create simulated components
    ForceSensorComponent * forceSensor = new ForceSensorComponent("ForceSensor", 10 * cmn_ms);
    CMN_ASSERT(ComponentManager->AddComponent(forceSensor));
    ControlComponent * controller = new ControlComponent("Controller", 1 * cmn_ms);
    CMN_ASSERT(ComponentManager->AddComponent(controller));

#if 0
    // Create thresholding filter for scalar with active filtering
    mtsSafetyCoordinator * coordinator = ComponentManager->GetCoordinator();
    if (!coordinator) {
        SFLOG_ERROR  << "Failed to get coordinator in this process";
        return false;
    }

    SF::FilterThreshold * filterTrendVelScalar = 
        new FilterTrendVel(// Common arguments
                           SF::FilterBase::FEATURE, // filter category
                           targetComponentName,     // name of target component
                           SF::FilterBase::ACTIVE,  // monitoring type
                           // Arguments specific to this filter
                           ForceSensorComponent::NameOfScalarSignal, // inputSignalName
                           SF::SignalElement::SCALAR, // input signal type
                           false);                    // time scaling
    // Enable debug log
    filterTrendVelScalar->EnableDebugLog(true);

    // Install the filter to the target component
    if (!coordinator->AddFilter(filterTrendVelScalar)) {
        SFLOG_ERROR << "Failed to add filter \"" << filterTrendVelScalar->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SFLOG_INFO << "Successfully installed filter: \"" << filterTrendVelScalar->GetFilterName() << "\"" << std::endl;
    std::cout << *filterTrendVelScalar << std::endl;
#endif

    // Connect components
    CONNECT_LOCAL(controller->GetName(), ControlComponent::NameOfRequiredInterface,
                  forceSensor->GetName(), ForceSensorComponent::NameOfProvidedInterface);

    // Create and run all components
    ComponentManager->CreateAll();
    ComponentManager->WaitForStateAll(mtsComponentState::READY);

    ComponentManager->StartAll();
    ComponentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    std::cout << "Press 'q' to quit." << std::endl;
    std::cout << "Running periodic tasks ";

    // loop until 'q' is pressed
    int key = ' ';
    while (key != 'q') {
        key = cmnGetChar();
        osaSleep(100 * cmn_ms);
    }
    std::cout << std::endl;

    // Clean up resources
    SFLOG_INFO << "Cleaning up..." << std::endl;

#if (CISST_OS != CISST_LINUX_XENOMAI)
    ComponentManager->KillAll();
    ComponentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
#endif   

    ComponentManager->Cleanup();

    return 0;
}
