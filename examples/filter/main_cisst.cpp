/*

  Safety Framework for Component-based Robotics

  Created on: October 8, 2012

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
#include "trendVel.h"

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

using namespace SF;

// Example task that simulates sensor wrapper
class SensorReadingTask: public mtsTaskPeriodic {
protected:
    double Force;

public:
    SensorReadingTask(const std::string & name, double period) : mtsTaskPeriodic(name, period, false, 5000)
    {
        Force = 0.0;
        StateTable.AddData(Force, "Force");
    }
    ~SensorReadingTask() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
        
        // To test TrendVel
        double delta = double(rand() % 10) * 0.1;
        Force += delta;
        std::cout << delta << std::endl;
    }
    void Cleanup(void) {}
};


// Create periodic task
bool CreatePeriodicThread(const std::string & componentName, double period);
// Install filters
bool InstallFilter(const std::string & targetComponentName);

// Local component manager
mtsManagerLocal * ComponentManager = 0;
// Test periodic task
SensorReadingTask * task = 0;

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
    ComponentManager = mtsComponentManager::GetInstance();

    // Print information about middleware(s) available
    StrVecType info;
    GetMiddlewareInfo(info);
    std::cout << "Middleware(s) detected: ";
    if (info.size() == 0) {
        std::cout << "none" << std::endl;
    } else {
        std::cout << std::endl;
        for (size_t i = 0; i < info.size(); ++i) {
            std::cout << "[" << (i+1) << "] " << info[i] << std::endl;
        }
    }
    std::cout << std::endl;

    // Create simulated sensor reading component
    std::string componentName("sensorWrapper");

    // Create periodic task
    if (!CreatePeriodicThread(componentName, 100 * cmn_ms)) {
        SFLOG_ERROR << "Failed to add periodic component \"" << componentName << "\"" << std::endl;
        return 1;
    }

    // Install filters
    if (!InstallFilter(componentName)) {
        SFLOG_ERROR << "Failed to install monitor for periodic component \"" << componentName << "\"" << std::endl;
        return 1;
    }
    
    // Activate all the monitors and filters installed
    if (ComponentManager->GetCoordinator()) {
        if (!ComponentManager->GetCoordinator()->DeployMonitorsAndFDDs()) {
            SFLOG_ERROR << "Failed to deploy monitors and FDDs" << std::endl;
            return 1;
        }
    } else {
        SFLOG_ERROR  << "Failed to get coordinator in this process";
        return 1;
    }

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

// Create periodic thread
bool CreatePeriodicThread(const std::string & componentName, double period)
{
    // Create periodic thread
    task = new SensorReadingTask(componentName, period);
    if (!ComponentManager->AddComponent(task)) {
        SFLOG_ERROR << "Failed to add component \"" << componentName << "\"" << std::endl;
        return false;
    }

    return true;
}

bool InstallFilter(const std::string & targetComponentName)
{
    mtsSafetyCoordinator * coordinator = ComponentManager->GetCoordinator();
    if (!coordinator) {
        SFLOG_ERROR  << "Failed to get coordinator in this process";
        return false;
    }

    // Create trend velocity filter with active filtering
    SF::FilterTrendVel * filterTrendVel = 
        new FilterTrendVel(// Common arguments
                           SF::FilterBase::FEATURE, // filter category
                           targetComponentName,     // name of target component
                           SF::FilterBase::ACTIVE,  // monitoring type
                           // Arguments specific to this filter
                           "Force",                    // name of input signal: name of state vector
                           SF::SignalElement::SCALAR); // input signal type
    // Enable debug log
    filterTrendVel->EnableDebugLog(true);

    // Install filter to the target component
    if (!coordinator->AddFilter(filterTrendVel)) {
        SFLOG_ERROR << "Failed to add filter \"" << filterTrendVel->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SFLOG_INFO << "Successfully installed filter: \"" << filterTrendVel->GetFilterName() << "\"" << std::endl;
    std::cout << *filterTrendVel << std::endl;

    return true;
}
