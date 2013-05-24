/*

  Safety Framework for Component-based Robotics

  Created on: July 14, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "coordinator.h"

namespace SF {

Coordinator::Coordinator()
{}

Coordinator::~Coordinator()
{}

bool Coordinator::AddMonitoringTarget(const std::string & targetUID,
                                      const std::string & targetJSON)
{
    if (FindMonitoringTarget(targetUID)) {
        SFLOG_ERROR << "Already registered target: " << targetUID << std::endl;
        return false;
    }

    MonitorTargetMap[targetUID] = targetJSON;

    return true;
}

bool Coordinator::FindMonitoringTarget(const std::string & uid) const
{
    return (MonitorTargetMap.find(uid) != MonitorTargetMap.end());
}

bool Coordinator::RemoveMonitoringTarget(const std::string & targetUID)
{
    if (!FindMonitoringTarget(targetUID))
        return true;

    MonitorTargetMapType::iterator it = MonitorTargetMap.find(targetUID);
    SFASSERT(it != MonitorTargetMap.end());
    MonitorTargetMap.erase(it);

    return true;
}

void Coordinator::PrintMonitoringTargets(std::ostream & outputStream) const
{
    int i = 0;
    MonitorTargetMapType::const_iterator it = MonitorTargetMap.begin();

    for (; it != MonitorTargetMap.end(); ++it)
        outputStream << "[" << ++i << "] " << it->first << std::endl;
}

#if 0
void Coordinator::ToStream(std::ostream & outputStream) const
{
    PrintMonitoringTargets(outputStream);
}
#endif

};
