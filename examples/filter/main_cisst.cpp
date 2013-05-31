/*

  Safety Framework for Component-based Robotics

  Created on: August 31, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

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

bool InstallFilter(const std::string & targetComponentName);

mtsManagerLocal      * ComponentManager = 0;
mtsSafetyCoordinator * SafetyCoordinator = 0;

// Example task that simulates sensor wrapper
class SensorReadingTask: public mtsTaskPeriodic {
public:
    // Typedef for sensor readings
    typedef vctFixedSizeVector<double, 6> SensorReadingType;

protected:
    double Threshold;
    double Tic;
    SensorReadingType SensorReading;
    double ForceY;

public:
    SensorReadingTask(const std::string & name, double period) : mtsTaskPeriodic(name, period, false, 5000)
    {
        StateTable.AddData(SensorReading, "SensorReadings");
        StateTable.AddData(ForceY, "ForceY");

        Tic = osaGetTime();
        Threshold = 10.0;
    }
    ~SensorReadingTask() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
        
        // Update sensor readings
        SensorReading(1) = Threshold;

        // MJ TEMP: Inject fault event at 10% probability
        if (rand() % 100 > 90) {
            SensorReading(1) += SensorReading(1) * 0.1 + (double)(rand() % 10) * 0.1;
        }
        ForceY = SensorReading(1);

        // MJ TEMP: Manual check to confirm if fault events from FDD are correct.
        if (ForceY - Threshold > 0.0) {
            std::cout << "[" << osaGetTime() - Tic << "] Sensor reading " << SensorReading(1)
                << " exceeds threshold " << Threshold << " by " << ForceY - Threshold << std::endl;
        }

    }
    void Cleanup(void) {}
};

int main(int argc, char *argv[])
{
    srand(time(NULL));

#if SF_USE_G2LOG
    // Logger setup
    g2LogWorker logger(argv[0], "./");
    g2::initializeLogging(&logger);
    std::cout << "Log file: \"" << logger.logFileName() << "\"\n" << std::endl;
#endif

    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::SetMaskClassMatching("mtsSafetyCoordinator", CMN_LOG_ALLOW_ALL);
    //cmnLogger::SetMaskClassMatching("mtsMonitorComponent", CMN_LOG_ALLOW_ALL);
    
    // Get instance of the cisst Component Manager
    mtsComponentManager::InstallSafetyCoordinator();
    try {
        ComponentManager = mtsComponentManager::GetInstance();
    } catch (...) {
        SFLOG_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // Get safety coordinator instance
    SafetyCoordinator = ComponentManager->GetCoordinator();

    // Create simulated sensor reading component
    const std::string componentName("sensorWrapper");
    SensorReadingTask * task = new SensorReadingTask(componentName, 100 * cmn_ms);
    if (!ComponentManager->AddComponent(task)) {
        SFLOG_ERROR << "Failed to add component \"" << componentName << "\"" << std::endl;
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

    // loop until 'q' is pressed
    int key = ' ';
    while (key != 'q') {
        key = cmnGetChar();
        osaSleep(100 * cmn_ms);
    }
    std::cout << std::endl;

    // Clean up resources
    SFLOG_INFO << "Cleaning up..." << std::endl;

    ComponentManager->KillAll();
    ComponentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    ComponentManager->Cleanup();

    return 0;
}

// Create two different types of thresholding filters: active vs. passive filters
bool InstallFilter(const std::string & targetComponentName)
{
    // Active filter
#if 0
    SF::FilterThreshold * filterThresholdActive = 
        new FilterThreshold(// Common arguments
                            SF::FilterBase::FEATURE, // filter category
                            targetComponentName,     // name of target component
                            SF::FilterBase::ACTIVE,  // monitoring type
                            // Arguments specific to this filter
                            "ForceY", // name of input signal: name of state vector
                            10.0,     // threshold
                            0.5,      // margin
                            0.0,      // output 0 (input is below threshold)
                            1.0);     // output 1 (input exceeds threshold)

    // Declare the filter as the last filter of FDD pipeline.
    filterThresholdActive->DeclareLastFilterOfPipeline();

    // Install filter to the target component [active monitoring]
    if (!SafetyCoordinator->AddFilter(filterThresholdActive)) {
        SFLOG_ERROR << "Failed to add ACTIVE filter \"" << filterThresholdActive->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SFLOG_INFO << "Successfully installed filter: \"" << filterThresholdActive->GetFilterName() << "\"" << std::endl;
    std::cout << *filterThresholdActive << std::endl;
#endif

    // Passive filter
#if 0
    SF::FilterThreshold * filterThresholdPassive = 
        new FilterThreshold(// Common arguments
                            SF::FilterBase::FEATURE, // filter category
                            targetComponentName,     // name of target component
                            SF::FilterBase::PASSIVE, // monitoring type
                            // Arguments specific to this filter
                            /*
                            filterThresholdActive->GetNameOfInputSignal(),
                            filterThresholdActive->GetThreshold(),
                            filterThresholdActive->GetMargin(),
                            filterThresholdActive->GetOutput0(),
                            filterThresholdActive->GetOutput1());
                            */
                            "ForceY",
                            10.0,     // threshold
                            0.5,      // margin
                            0.0,      // output 0 (input is below threshold)
                            1.0);     // output 1 (input exceeds threshold)
    filterThresholdPassive->DeclareLastFilterOfPipeline();

    // Install filter to the target component [passive monitoring]
    if (!SafetyCoordinator->AddFilter(filterThresholdPassive)) {
        SFLOG_ERROR << "Failed to add PASSIVE filter \"" << filterThresholdPassive->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SFLOG_INFO << "Successfully installed filter: \"" << filterThresholdPassive->GetFilterName() << "\"" << std::endl;
    std::cout << *filterThresholdPassive << std::endl;
#endif
    
    const std::string jsonFileName(SF_SOURCE_ROOT_DIR"/examples/filter/filter.json");
    if (!SafetyCoordinator->AddFilterFromJSONFile(jsonFileName)) {
        SFLOG_ERROR << "Failed to add filter(s) from file: \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    return true;
}
