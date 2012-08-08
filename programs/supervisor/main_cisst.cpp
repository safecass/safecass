/*

  Safety Framework for Component-based Robotics

  Created on: August 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "config.h"
#include "common.h"
//#include "json.h"
//#include "monitor.h"
#include "publisher.h"
#include "subscriber.h"

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

#if ENABLE_G2LOG
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
    
    // Get instance of the cisst Component Manager
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();
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

    mtsSafetySupervisor * supervisor = new mtsSafetySupervisor;
    
    std::cout << "-------------------- subscriber start" << std::endl; 
    osaSleep(3.0);
    std::cout << "-------------------- subscriber end" << std::endl; 
    delete supervisor;

    return 0;
}

