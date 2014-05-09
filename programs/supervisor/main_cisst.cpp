//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Aug 7, 2014
// Last revision: May 7, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "config.h"
#include "common.h"
#include "publisher.h"
#include "subscriber.h"

#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsSafetySupervisor.h>

using namespace SF;

/* TODO
 * 3. add subscriber to Supervisor to listen to a topic for monitor (i.e., period sample)
 * 4. integrate d3 backend for real-time visualization and data collection
 * 5. add publisher to Supervisor to publish supervisory control data
 * 6. add subscriber to monitor to listen to a topic for supervisory control data
 */
int main(int argc, char *argv[])
{
    // configuration files for publishers and subscribers
    std::string configPub, configSub;

    if (argc == 1) {
        configPub = Publisher::GetDefaultConfigFilePath();
        configSub = Subscriber::GetDefaultConfigFilePath();
    } else if (argc == 3) {
        configPub = std::string(argv[1]);
        configSub = std::string(argv[2]);
    } else {
        std::cerr << "USAGE: supervisor [publisher config file] [subscriber config file]" << std::endl;
        return 1;
    }

#if SF_USE_G2LOG
    // Logger setup
    g2LogWorker logger(argv[0], "./");
    g2::initializeLogging(&logger);
    std::cout << "Log file: \"" << logger.logFileName() << "\"\n" << std::endl;
#endif

    // cisst logger setup
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    //cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    
    // Casros installs the Safety Coordinator to the process where the Safety Supervisor runs
    // for symmetry.
    mtsManagerLocal::InstallSafetyCoordinator();

    // Get local component manager instance
    mtsManagerLocal * componentManager;
    try {
        componentManager = mtsManagerLocal::GetInstance();
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    SFLOG_INFO << "Publisher configuration file: " << configPub << std::endl;
    SFLOG_INFO << "Subscriber configuration file: " << configSub << std::endl;

    // Print information about middleware(s) available
    StrVecType info;
    GetMiddlewareInfo(info);

    std::stringstream ss;
    ss << "Middleware(s) detected: ";
    if (info.size() == 0) {
        ss << "none" << std::endl;
    } else {
        ss << std::endl;
        for (size_t i = 0; i < info.size(); ++i) {
            ss << "[" << (i+1) << "] " << info[i] << std::endl;
        }
    }
    ss << std::endl;
    SFLOG_INFO << ss.str();

    // Create supervisor instance
    mtsSafetySupervisor * supervisor = new mtsSafetySupervisor;
    componentManager->AddComponent(supervisor);

    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    int in;
    while (true) {
        in = cmnGetChar();
        if (in == 'q') break;
        osaSleep(100 * cmn_ms);
    }

    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();

    delete supervisor;

    return 0;
}

