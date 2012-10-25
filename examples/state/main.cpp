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

using namespace SF;

// Example task that simulates sensor wrapper
class SensorReadingTask: public mtsTaskPeriodic {
public:
    static const double ThresholdForceX;
    static const std::string NameOfForceX;

protected:
    /*! Time when test began */
    double Tic;

public:
    SensorReadingTask(const std::string & name, double period) : mtsTaskPeriodic(name, period, false, 5000)
    {
        Tic = osaGetTime();
    }
    ~SensorReadingTask() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
        
        // TODO: state transition
        std::cout << ".";

        static int count = 0;
        count++;
        if (count == 20) {
            std::cout <<"\nto fault, back to normal\n";
            this->FaultState.ProcessEvent(SF::StateMachine::FAULT_DETECTION);
            this->FaultState.PrintState();
            this->FaultState.ProcessEvent(SF::StateMachine::FAULT_REMOVAL);
            this->FaultState.PrintState();
        } else if (count == 60) {
            std::cout <<"\nto error, back to normal\n";
            this->FaultState.ProcessEvent(SF::StateMachine::ERROR_DETECTION);
            this->FaultState.PrintState();
            this->FaultState.ProcessEvent(SF::StateMachine::ERROR_REMOVAL);
            this->FaultState.PrintState();
        } else if (count == 90) {
            std::cout <<"\nto failure, back to normal\n";
            this->FaultState.ProcessEvent(SF::StateMachine::FAILURE_DETECTION);
            this->FaultState.PrintState();
            this->FaultState.ProcessEvent(SF::StateMachine::FAILURE_REMOVAL);
            this->FaultState.PrintState();
        } else if (count == 120) {
            std::cout <<"\nto fault, error, failure, and back to normal\n";
            this->FaultState.ProcessEvent(SF::StateMachine::FAULT_DETECTION);
            this->FaultState.PrintState();
            this->FaultState.ProcessEvent(SF::StateMachine::FAULT_ACTIVATION);
            this->FaultState.PrintState();
            this->FaultState.ProcessEvent(SF::StateMachine::ERROR_PROPAGATION);
            this->FaultState.PrintState();
            this->FaultState.ProcessEvent(SF::StateMachine::FAILURE_REMOVAL);
            this->FaultState.PrintState();
        }
    }
    void Cleanup(void) {}
};

const double SensorReadingTask::ThresholdForceX = 10.0;
const std::string SensorReadingTask::NameOfForceX = "ForceX";

// Create periodic task
bool CreatePeriodicThread(const std::string & componentName, double period);
// Install monitors to monitor quantities in real-time
//bool InstallMonitor(const std::string & targetComponentName, unsigned int frequency);
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

#if 0
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
#endif

    // Create simulated sensor reading component
    std::string componentName("sensorWrapper");

    // Create periodic task
    if (!CreatePeriodicThread(componentName, 100 * cmn_ms)) {
        SFLOG_ERROR << "Failed to add periodic component \"" << componentName << "\"" << std::endl;
        return 1;
    }

#if 0 
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
#endif

    // Create and run all components
    ComponentManager->CreateAll();
    ComponentManager->WaitForStateAll(mtsComponentState::READY);

    ComponentManager->StartAll();
    ComponentManager->WaitForStateAll(mtsComponentState::ACTIVE);

#if 0
    std::cout << "Press 'q' to quit." << std::endl;
    std::cout << "Running periodic tasks ";

    // loop until 'q' is pressed
    int key = ' ';
    while (key != 'q') {
        key = cmnGetChar();
        osaSleep(100 * cmn_ms);
    }
    std::cout << std::endl;
#endif
    while(1) {
        std::cout << std::flush;
        osaSleep(0.1);
    }

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

    // Create active types of thresholding filters
    SF::FilterThreshold * filterThresholdActive = 
        new FilterThreshold(// Common arguments
                            SF::FilterBase::FEATURE, // filter category
                            targetComponentName,     // name of target component
                            SF::FilterBase::ACTIVE,  // monitoring type
                            // Arguments specific to this filter
                            SensorReadingTask::NameOfForceX,  // name of input signal
                            SensorReadingTask::ThresholdForceX, // threshold
                            0.5,      // margin
                            0.0,      // output 0 (input is below threshold)
                            1.0);     // output 1 (input exceeds threshold)

    // Declare the filter as the last filter of FDD pipeline.
    filterThresholdActive->DeclareLastFilterOfPipeline();

    // Install filter to the target component [active monitoring]
    if (!coordinator->AddFilter(filterThresholdActive)) {
        SFLOG_ERROR << "Failed to add ACTIVE filter \"" << filterThresholdActive->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SFLOG_INFO << "Successfully installed filter: \"" << filterThresholdActive->GetFilterName() << "\"" << std::endl;
    std::cout << *filterThresholdActive << std::endl;

    return true;
}
