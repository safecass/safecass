/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "common.h"

namespace SF {

StrVecType GetMiddlewareInfo(void)
{
    StrVecType info;
#ifdef SF_HAS_CISST
    info.push_back(GetCISSTInfo());
#endif
    return info;
}

void GetMiddlewareInfo(StrVecType & info)
{
#ifdef SF_HAS_CISST
    info.push_back(GetCISSTInfo());
#endif
}


#ifdef SF_HAS_CISST
#include <cisstConfig.h>
#include <cisstRevision.h>
std::string GetCISSTInfo(void)
{
    std::stringstream ss;
    ss << "CISST: version: " << CISST_VERSION << ", svn revision: " << CISST_WC_REVISION;

    return ss.str();
}
#endif

};
