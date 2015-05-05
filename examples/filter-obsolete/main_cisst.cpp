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

using namespace SC;

// Both work fine
#define USE_MTS_DOUBLE_VEC 0
//#define USE_MTS_DOUBLE_VEC 1

// Example task that simulates sensor wrapper
class SensorReadingTask: public mtsTaskPeriodic {
protected:
    // scalar type
    double Force;
    // vector type
#if USE_MTS_DOUBLE_VEC
    mtsDoubleVec ForceReadings;
#else
    std::vector<double> ForceReadings;
#endif

public:
    SensorReadingTask(const std::string & name, double period) : mtsTaskPeriodic(name, period, false, 5000)
    {
        Force = 0.0;
#if USE_MTS_DOUBLE_VEC
        ForceReadings.SetSize(3);
        ForceReadings.SetAll(0.0);
#else
        ForceReadings.resize(3);
#endif

        StateTable.AddData(Force, "Force");
        StateTable.AddData(ForceReadings, "ForceReadings");
    }
    ~SensorReadingTask() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
        
        // To test TrendVel filter for scalar type
        double delta = double(rand() % 10) * 0.1;
        Force += delta;
        // To test TrendVel filter for vector type
#if USE_MTS_DOUBLE_VEC
        ForceReadings(0) += double(rand() % 10) * 0.1;
        ForceReadings(1) += double(rand() % 10) * 0.1;
        ForceReadings(2) += double(rand() % 10) * 0.1;

        std::cout << Force << "\t | " << ForceReadings << std::endl;
#else
        ForceReadings[0] += double(rand() % 10) * 0.1;
        ForceReadings[1] += double(rand() % 10) * 0.1;
        ForceReadings[2] += double(rand() % 10) * 0.1;

        std::cout << Force << "\t | [ ";
        for (size_t i = 0; i < 3; ++i) {
            std::cout << ForceReadings[i] << " ";
        }
        std::cout << " ]" << std::endl;
#endif

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

#if SC_USE_G2LOG
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
    try {
        ComponentManager = mtsComponentManager::GetInstance();
    } catch (...) {
        SCLOG_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

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
        SCLOG_ERROR << "Failed to add periodic component \"" << componentName << "\"" << std::endl;
        return 1;
    }

    // Install filters
    if (!InstallFilter(componentName)) {
        SCLOG_ERROR << "Failed to install monitor for periodic component \"" << componentName << "\"" << std::endl;
        return 1;
    }
    
    // Activate all the monitors and filters installed
    if (ComponentManager->GetCoordinator()) {
        if (!ComponentManager->GetCoordinator()->DeployMonitorsAndFDDs()) {
            SCLOG_ERROR << "Failed to deploy monitors and FDDs" << std::endl;
            return 1;
        }
    } else {
        SCLOG_ERROR  << "Failed to get coordinator in this process";
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
    SCLOG_INFO << "Cleaning up..." << std::endl;

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
        SCLOG_ERROR << "Failed to add component \"" << componentName << "\"" << std::endl;
        return false;
    }

    return true;
}

bool InstallFilter(const std::string & targetComponentName)
{
    mtsSafetyCoordinator * coordinator = ComponentManager->GetCoordinator();
    if (!coordinator) {
        SCLOG_ERROR  << "Failed to get coordinator in this process";
        return false;
    }

    //-------------------------------------------------- 
    // Velocity filter tests
    //
    // Create trend velocity filter for scalar with active filtering
#if 1
    SC::FilterTrendVel * filterTrendVelScalar = 
        new FilterTrendVel(// Common arguments
                           SC::FilterBase::FEATURE, // filter category
                           targetComponentName,     // name of target component
                           SC::FilterBase::ACTIVE,  // monitoring type
                           // Arguments specific to this filter
                           "Force",                   // name of input signal: name of state vector
                           SC::SignalElement::SCALAR, // input signal type
                           false);                    // time scaling
    // Enable debug log
    filterTrendVelScalar->EnableDebugLog(true);

    // Install filter to the target component
    if (!coordinator->AddFilter(filterTrendVelScalar)) {
        SCLOG_ERROR << "Failed to add filter \"" << filterTrendVelScalar->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SCLOG_INFO << "Successfully installed filter: \"" << filterTrendVelScalar->GetFilterName() << "\"" << std::endl;
    std::cout << *filterTrendVelScalar << std::endl;
#endif

    // If two trend velocity filters are cascaded into a filter pipeline, the pipeline 
    // acts as a trend accelerometer filter.
#if 0
    SC::FilterTrendVel * filterTrendVelScalar2 = 
        new FilterTrendVel(// Common arguments
                           SC::FilterBase::FEATURE_VECTOR, // filter category
                           targetComponentName,     // name of target component
                           SC::FilterBase::ACTIVE,  // monitoring type
                           // Arguments specific to this filter
                           filterTrendVelScalar->GetOutputSignalName(0),
                           SC::SignalElement::SCALAR, // input signal type
                           false);                    // time scaling
    // Enable debug log
    filterTrendVelScalar2->EnableDebugLog(true);

    // Install filter to the target component
    if (!coordinator->AddFilter(filterTrendVelScalar2)) {
        SCLOG_ERROR << "Failed to add filter \"" << filterTrendVelScalar2->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SCLOG_INFO << "Successfully installed filter: \"" << filterTrendVelScalar2->GetFilterName() << "\"" << std::endl;
    std::cout << *filterTrendVelScalar2 << std::endl;
#endif

    // Create trend velocity filter for vector with active filtering
#if 0
    SC::FilterTrendVel * filterTrendVelVector = 
        new FilterTrendVel(// Common arguments
                           SC::FilterBase::FEATURE, // filter category
                           targetComponentName,     // name of target component
                           SC::FilterBase::ACTIVE,  // monitoring type
                           // Arguments specific to this filter
                           "ForceReadings",           // name of input signal: name of state vector
                           SC::SignalElement::VECTOR, // input signal type
                           false);                     // time scaling
    // Enable debug log
    filterTrendVelVector->EnableDebugLog(true);

    // Install filter to the target component
    if (!coordinator->AddFilter(filterTrendVelVector)) {
        SCLOG_ERROR << "Failed to add filter \"" << filterTrendVelVector->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SCLOG_INFO << "Successfully installed filter: \"" << filterTrendVelVector->GetFilterName() << "\"" << std::endl;
    std::cout << *filterTrendVelVector << std::endl;
#endif

    return true;
}
