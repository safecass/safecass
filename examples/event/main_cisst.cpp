/*

  Safety Framework for Component-based Robotics

  Created on: August 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "config.h"
#include "common.h"
#include "json.h"
#include "monitor.h"
#include "cisstMonitor.h"

#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

using namespace SF;

class PeriodicTask: public mtsTaskPeriodic {
public:
    PeriodicTask(const std::string & name, double period) :
        mtsTaskPeriodic(name, period, false, 5000) {}
    ~PeriodicTask() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
        /*
        static double T = (1.0 / this->Period) * 60.0 * 60.0;
        static int i = 0;
        osaSleep(this->Period * (0.8 * sin(2 * cmnPI * ((double) ++i / T))));
        */
    }
    void Cleanup(void) {}
};


// Create periodic task
bool CreatePeriodicThread(const std::string & componentName, double period);
// to monitor values in real-time
bool InstallFilter(const std::string & targetComponentName, unsigned int T);

// Local component manager
mtsManagerLocal * ComponentManager = 0;
// Test periodic task
PeriodicTask * task = 0;

int main(int argc, char *argv[])
{
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
    //cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    
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

    // Create five test components with different T
    std::vector<unsigned int> f; // Hz
#if 0
    f.push_back(1);
    f.push_back(2);
    f.push_back(5);
    f.push_back(10);
    f.push_back(20);
    f.push_back(50);
    f.push_back(100);
    f.push_back(200);
    f.push_back(500);
#endif
    f.push_back(10);

    std::string componentName;
    std::stringstream ss;
    for (size_t i = 0; i < f.size(); ++i) {
        ss.str("");
        ss << "Component" << f[i];
        componentName = ss.str();

        // Create periodic task
        if (!CreatePeriodicThread(componentName, 1.0 / (double)f[i])) {
            SFLOG_ERROR << "Failed to add periodic component \"" << componentName << "\"" << std::endl;
            return 1;
        }
        // Install monitor 
        if (!InstallFilter(componentName, f[i])) {
            SFLOG_ERROR << "Failed to install monitor for periodic component \"" << componentName << "\"" << std::endl;
            return 1;
        }
    }
    
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
    task = new PeriodicTask(componentName, period);
    if (!ComponentManager->AddComponent(task)) {
        SFLOG_ERROR << "Failed to add component \"" << componentName << "\"" << std::endl;
        return false;
    }

    return true;
}

bool InstallFilter(const std::string & targetComponentName, unsigned int f)
{
    SF::Coordinator * Coordinator = ComponentManager->GetCoordinator();
    if (!Coordinator) {
        SFLOG_ERROR  << "Failed to get coordinator in this process";
        return false;
    }

    // Create two thresholding filters
    SF::FilterThreshold * thresholdFilter = new FilterThreshold(15.0, 0.0, 1.0);
    // Install filter to the target component [Active monitoring]
    Coordinator->AddFilter("targetComponentName", thresholdFilter, ACTIVE);
    // Install filter to the monitoring component [Passive monitoring]
    Coordinator->AddFilter("targetComponentName", thresholdFilter, PASSIVE);

    // Install the filter to 


    /*
    // Define target
    cisstTargetID * targetId = new cisstTargetID;
    targetId->ProcessName = ComponentManager->GetProcessName();
    targetId->ComponentName = targetComponentName;

    cisstMonitor * monitor;

    // Install monitor for timing fault - period
    {
        monitor = new cisstMonitor(Monitor::TARGET_THREAD_PERIOD,
                                   targetId,
                                   Monitor::STATE_ON,
                                   Monitor::OUTPUT_STREAM,
                                   f);
        // MJ TODO: Run system for a few minutes, collect experimental data,
        // and determine variance of period with upper/lower limits and thresholds.
        if (!ComponentManager->GetCoordinator()->AddMonitor(monitor)) {
            SFLOG_ERROR << "Failed to add new monitor target for component \"" << targetComponentName << "\"" << std::endl;
            SFLOG_ERROR << "JSON: " << monitor->GetMonitorJSON() << std::endl;
            return false;
        }
        SFLOG_INFO << "Successfully installed monitor [ " << monitor->GetMonitorJSON() 
                   << " ] to [ " << targetId->GetTargetID() << " ]" << std::endl;
    }
    */

    return true;
}