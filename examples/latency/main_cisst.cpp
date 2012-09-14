/*

  Safety Framework for Component-based Robotics

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
#include <cisstMultiTask/mtsCollectorState.h>
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
    }
    void Cleanup(void) {}
};


// Start-up codes required by the middleware
void StartUp(int argc, char *argv[]);
// Create periodic task
void CreatePeriodicThread(const std::string & taskName, double period);
// to monitor values in real-time
bool InstallMonitor(const std::string & targetComponentName, unsigned int frequency);
// to set up FDD pipeline for thread latency measurements
void InstallFDD();
// to collect data during experiment
void InstallDataCollector();
// Start experiment by running components
void RunComponents(double second);
// Clean up
void CleanUp(void);


// Local component manager
mtsManagerLocal * ComponentManager = 0;
// Test periodic task
PeriodicTask * task = 0;

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "usage: latency period_in_msec duration_in_sec" << std::endl;
        return 1;
    }

    StartUp(argc, argv);

    double period = atof(argv[1]) * cmn_ms;
    double frequency = (1.0 / period);
    int duration = atoi(argv[2]);
    std::cout << "period   : " << period * 1000.0 << " msec\n";
    std::cout << "loop/sec : " << frequency << "\n";
    std::cout << "duration : " << duration << " sec\n";

    // Set thread name
    std::stringstream ss;
    ss << "thread" << (int) frequency;
    std::string taskName = ss.str();
    std::cout << taskName << std::endl;

    // Create two test components with periodic threads and install monitors and FDD
    // pipelines for them
    CreatePeriodicThread(taskName, period);

    // (MJ TODO: possibly with data visualizer)
    if (!InstallMonitor(taskName, (unsigned int) frequency)) {
        SFLOG_ERROR << "Failed to install monitor for task \"" << taskName << "\"" << std::endl;
        return 1;
    }
    //InstallFDD();
    //InstallDataCollector();
    
    if (ComponentManager->GetCoordinator()) {
        if (!ComponentManager->GetCoordinator()->DeployMonitorsAndFDDs()) {
            SFLOG_ERROR << "Failed to deploy monitors and FDDs" << std::endl;
            return 1;
        }
    } else {
        SFLOG_ERROR  << "Failed to get coordinator in this process";
        return 1;
    }

#if 0
    // add data collection
    mtsTask * monitor = dynamic_cast<mtsTask*>(ComponentManager->GetComponent("Monitor"));
    mtsCollectorState * collector = new mtsCollectorState(
        "Monitor", monitor->GetMonitoringStateTableName(), mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);

    std::string periodName = taskName;
    periodName += "Period";
    collector->AddSignal(periodName);
    collector->AddSignal("Arithmetic0");
    ComponentManager->AddComponent(collector);
    collector->Connect();
#endif

    // Start experiment
    RunComponents(duration);

    // Post-processing for statistics
    // TODO: PostProcessing();

    // Clean up resources
    CleanUp();

    return 0;
}


void StartUp(int argc, char *argv[])
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
}

// Create periodic thread
void CreatePeriodicThread(const std::string & taskName, double period)
{
    // Create periodic thread
    task = new PeriodicTask(taskName, period);
    if (!ComponentManager->AddComponent(task)) {
        SFLOG_ERROR << "Failed to add component \"" << taskName << "\"" << std::endl;
        return;
    }

#if 0
    // add data collection
    mtsTask * monitor = dynamic_cast<mtsTask*>(ComponentManager->GetComponent("Monitor"));
    mtsCollectorState * collector = new mtsCollectorState(
        "Monitor", monitor->GetMonitoringStateTableName(), mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);

    std::string periodName = taskName;
    periodName += "Period";
    collector->AddSignal(periodName);
    collector->AddSignal("Arithmetic0");
    ComponentManager->AddComponent(collector);
    collector->Connect();
#endif

    ComponentManager->CreateAll();
    ComponentManager->WaitForStateAll(mtsComponentState::READY);
}

bool InstallMonitor(const std::string & targetComponentName, unsigned int frequency)
{
    if (!ComponentManager->GetCoordinator()) {
        SFLOG_ERROR  << "No safety coordinator found in this process" << std::endl;
        return false;
    }

    cisstEventLocation * locationID = new cisstEventLocation;
    locationID->SetProcessName(ComponentManager->GetProcessName());
    locationID->SetComponentName(targetComponentName);

    cisstMonitor * monitor = new cisstMonitor(Monitor::TARGET_THREAD_PERIOD,
                                              locationID,
                                              Monitor::STATE_ON,
                                              Monitor::OUTPUT_STREAM,
                                              frequency);

    if (!ComponentManager->GetCoordinator()->AddMonitor(monitor)) {
        SFLOG_ERROR << "Failed to add new monitor target for component \"" << targetComponentName << "\"" << std::endl;
        SFLOG_ERROR << "JSON: " << monitor->GetMonitorJSON() << std::endl;
        return false;
    }

    return true;
}

void InstallFDD(void)
{
    // Any filter or FDD pipeline requires a monitor to be installed first.
    // This allows application developers to probe raw signal values for data collection or
    // visualization purpose or inputs to a filter or FDD pipeline.

    /* Sampe JSON file to specify filter and FDD pipeline in cisst
    {
        // Filter name
        "Name" : "
        // Monitor configuration (e.g., which filter to use, filter config params)
        "Config" : { "FilterName" : "Dual-threshold",
                     "FilterArgs" : {
                        "Threshold-low" : 0.01, // msec, (can be converted from % specification)
                        "Threshold-high" : 0.05 // msec, (can be converted from % specification)
                     }
                   }
    }
    */
}

void InstallDataCollector(void)
{
    // TODO
}

void RunComponents(double second)
{
    ComponentManager->StartAll();
    ComponentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    double duration = second;
    //collector->StartCollection(0.0);

    std::cout << std::endl;
    while (duration-- > 0) {
        osaSleep(1.0);
    }

    //collector->StopCollection(0.0);
}

void CleanUp(void)
{
    SFLOG_INFO << "Cleaning up ..." << std::endl;

#if (CISST_OS != CISST_LINUX_XENOMAI)
    ComponentManager->KillAll();
    ComponentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
#endif   

    ComponentManager->Cleanup();
}
