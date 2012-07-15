/*

  Safety Framework for Component-based Robotics

  Created on: July 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstCoordinator.h"
#include "json.h"

namespace SF {

cisstCoordinator::cisstCoordinator()
{
    std::cout << "CISST SAFETY" << std::endl;
}

cisstCoordinator::~cisstCoordinator()
{
    std::cout << "CISST SAFETY end" << std::endl;
}

bool cisstCoordinator::AddMonitor(const std::string & targetUID, const std::string & monitorJsonSpec)
{
    JSON json;
    json.Read(monitorJsonSpec.c_str());

    // Check if same monitoring target is already registered
    MonitorMapType::const_iterator it = MonitorMap.find(targetUID);
    if (it != MonitorMap.end()) {
        std::cerr << "Duplicated target UID: " << targetUID << std::endl;
        return false;
    }

    // 결정1: monitor creation 기능 자체는 cisst 안에 구현 (plug-in 개념)

    // 결정2: monitor creation 을 위한 call 은 SF 안에 adaptor로 구현 (SF에서는 동일한
    //        API를 호출하되 adaptor instance (cisst/ros)에서는 개별 middleware에 맞는
    //        API를 내부적으로 호출

    // 결정3: SF에서는 "일단" 오직 한 종류의 middleware만을 허용

    // 결정4: adaptors will be implemented as cisst.h/.cpp under libs/monitor/adaptor folder

    // create new monitor with (uid, json)
    // insert new monitor to monitor list
    // embed new monitor to cisst system

    // 2. run-time management
    // Q> how to fetch monitor's output?
    // Q> should SF use separate network middleware? pros vs. cons?
    // Q> if not, pros vs. cons?

    std::cout << "SUCCESS: " << targetUID << "\n, JSON: " << json.GetJSON() << std::endl;

    MonitorMap[targetUID] = monitorJsonSpec;

    return true;
}

};
