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

using namespace SF;

/* TODO
 * 1. remove all cisst-dependencies
 * 2. implement "BRAIN" of SF => Supervisor class => add it to libs
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
    SFLOG_INFO << "Publisher  config file: " << configPub << std::endl;
    SFLOG_INFO << "Subscriber config file: " << configSub << std::endl;

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
    
    return 0;
}

