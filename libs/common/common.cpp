//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 2, 2012
// Last revision: Apr 19, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "common.h"

namespace SF {

StrVecType GetMiddlewareInfo(void)
{
    StrVecType info;
#if SF_HAS_CISST
    info.push_back(GetCISSTInfo());
#endif
    return info;
}

void GetMiddlewareInfo(StrVecType & info)
{
#if SF_HAS_CISST
    info.push_back(GetCISSTInfo());
#endif
}

#if SF_HAS_CISST
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
