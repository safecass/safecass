/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsCollectorState.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

#include "common.h"

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


void StartUp(void);          // Start-up codes required by the middleware
void CreatePeriodicThread(const std::string & taskName, double period);
void InstallMonitor();       // to monitor values in real-time
void InstallFDD();           // to set up FDD pipeline for thread latency measurements
void InstallDataCollector(); // to collect data during experiment
void RunComponents(double second);
void CleanUp(void);


int main(int argc, char *argv[])
{
    StartUp();

    if (argc != 3) {
        std::cerr << "usage: latency period_in_msec duration_in_sec" << std::endl;
        return 1;
    }

    // Print information about middleware(s) available
    SF::StrVecType info;
    SF::GetMiddlewareInfo(info);
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

    // Create periodic thread
    CreatePeriodicThread(taskName, period);

    // Install monitor, FDD pipeline, and data collector 
    // (MJ TODO: possibly with data visualizer)
    InstallMonitor();       // to monitor values in real-time
    InstallFDD();           // to set up FDD pipeline for thread latency measurements
    InstallDataCollector(); // to collect data during experiment

    // Start experiment
    RunComponents(duration);

    // Post-processing for statistics
    // TODO: PostProcessing();

    // Cleanup local component manager
    CleanUp();

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

void InstallMonitor(void)
{
    // TODO
}

void InstallFDD(void)
{
    // TODO
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
