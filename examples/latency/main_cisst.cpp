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
#include "cisstCoordinator.h"

#if ENABLE_G2LOG
#include "g2logworker.h"
#include "g2log.h"
#include <iomanip>
#endif

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
void StartUp(void);
// Create periodic task
void CreatePeriodicThread(const std::string & taskName, double period);
// to monitor values in real-time
bool InstallMonitor(const std::string & targetComponentName);
// to set up FDD pipeline for thread latency measurements
void InstallFDD();
// to collect data during experiment
void InstallDataCollector();
// Start experiment by running components
void RunComponents(double second);
// Clean up
void CleanUp(void);


// Safety coordinator for this process
Coordinator * SafetyCoordinator = 0;

int main(int argc, char *argv[])
{
#if ENABLE_G2LOG
    g2LogWorker logger(argv[0], "/tmp/");
    g2::initializeLogging(&logger);
    std::cout << "\n*** Log file: [" << logger.logFileName() << "]\n\n" << std::endl;

    LOGF(INFO, "Hi log %d", 123);
    LOG(INFO) << "Test SLOG INFO";
    LOG(DEBUG) << "Test SLOG DEBUG";
    LOG(INFO) << "one: " << 1;
    LOG(INFO) << "two: " << 2;
    LOG(INFO) << "one and two: " << 1 << " and ";

    return 0;
#endif

    // Initialize safety coordinator for this process
    SafetyCoordinator = new cisstCoordinator();

#if 0
    if (argc != 3) {
        std::cerr << "usage: latency period_in_msec duration_in_sec" << std::endl;
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
#endif

    // Create periodic thread
    //CreatePeriodicThread(taskName, period);

    // Install monitor, FDD pipeline, and data collector 
    // (MJ TODO: possibly with data visualizer)
    if (!InstallMonitor("aComponent")) {
        std::cerr << "Failed to install monitor for task \"";// << taskName << "\"" << std::endl;
        std::cerr << std::endl;
        return 1;
    }

    return 1;

#if 0
    InstallFDD();
    InstallDataCollector();

    // Start experiment
    RunComponents(duration);

    // Post-processing for statistics
    // TODO: PostProcessing();

    // Cleanup local component manager
    CleanUp();
#endif

    return 0;
}


void StartUp(void)
{
#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT|MCL_FUTURE);
#endif

    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    //cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
}

// Create periodic thread
mtsManagerLocal * componentManager = 0;
PeriodicTask * task = 0;
void CreatePeriodicThread(const std::string & taskName, double period)
{
    // Create periodic thread
    componentManager = mtsComponentManager::GetInstance();
    task = new PeriodicTask(taskName, period);
    componentManager->AddComponent(task);

#if 0
    // add data collection
    mtsTask * monitor = dynamic_cast<mtsTask*>(componentManager->GetComponent("Monitor"));
    mtsCollectorState * collector = new mtsCollectorState(
        "Monitor", monitor->GetMonitoringStateTableName(), mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);

    std::string periodName = taskName;
    periodName += "Period";
    collector->AddSignal(periodName);
    collector->AddSignal("Arithmetic0");
    componentManager->AddComponent(collector);
    collector->Connect();
#endif

    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);
}

bool InstallMonitor(const std::string & targetComponentName)
{
    TargetIDType targetId;
    targetId.ProcessName = mtsComponentManager::GetInstance()->GetProcessName();
    targetId.ComponentName = targetComponentName;

    // Generate JSON file to create new monitor instance
    const std::string newMonitorJSON = 
        cisstMonitor::GetMonitorJSON("Period Monitor",
                                     Fault::COMPONENT_PERIOD,
                                     Monitor::OUTPUT_STREAM,
                                     Monitor::MONITOR_ON,
                                     targetId);

#if 0
    // Check if the target object is already being monitored
    const std::string targetUID = targetId.GetTargetUID(Fault::COMPONENT_PERIOD);
    if (!Monitor::AddMonitor(targetUID, newMonitorJSON)) {
        std::cerr << "Failed to create new cisst monitor for component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
#endif
    if (!SafetyCoordinator->AddMonitor(newMonitorJSON)) {
        std::cerr << "Failed to create new cisst monitor for component \"" << targetComponentName << "\"" << std::endl;
        std::cerr << "JSON: " << newMonitorJSON << std::endl;
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
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    double duration = second;
    //collector->StartCollection(0.0);

    std::cout << std::endl;
    while (duration-- > 0) {
        osaSleep(1.0);
        //std::cout << "." << std::flush;
    }

    //collector->StopCollection(0.0);
}

void CleanUp(void)
{
    std::cout << "Quitting ..." << std::endl;
    osaSleep(3.0 * cmn_s);

#if (CISST_OS != CISST_LINUX_XENOMAI)
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
#endif   

    componentManager->Cleanup();

    delete task;
    //delete collector;
}
