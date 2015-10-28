//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 2, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#include "common.h"

namespace SC {

StrVecType GetMiddlewareInfo(void)
{
    StrVecType info;
#if SC_HAS_CISST
    info.push_back(GetCISSTInfo());
#endif
    return info;
}

void GetMiddlewareInfo(StrVecType & info)
{
#if SC_HAS_CISST
    info.push_back(GetCISSTInfo());
#endif
}

#if SC_HAS_CISST
#include <cisstConfig.h>
#include <cisstRevision.h>
std::string GetCISSTInfo(void)
{
    std::stringstream ss;
    ss << "CISST version: " << CISST_VERSION << " (" << CISST_FULL_REVISION << ")";

    return ss.str();
}
#endif

};
