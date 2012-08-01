/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _common_h
#define _common_h

#include <vector>
#include <iostream>
#include <sstream>

#include "SFDLLDefines.h"

#if ENABLE_G2LOG
  #include "g2logworker.h"
  #include "g2log.h"
  #include <iomanip>
#else
  #ifdef SF_HAS_CISST
    #include <cisstCommon/cmnPortability.h>
    #include <cisstCommon/cmnLogger.h>
  #endif
#endif

namespace SF {

typedef std::vector<std::string> StrVecType;

typedef unsigned int SamplingRateType;   // f (Hz)
typedef double       SamplingPeriodType; // T (=1/f)

// Returns information about middlewares available
StrVecType GetMiddlewareInfo(void);

void GetMiddlewareInfo(StrVecType & info);

#ifdef SF_HAS_CISST
std::string GetCISSTInfo(void);
#endif

// Logger macro definitions
#if ENABLE_G2LOG
  #define SFLOG_INFO    LOG(INFO)
  #define SFLOG_DEBUG   LOG(DEBUG)
  #define SFLOG_WARNING LOG(WARNING)
  #define SFLOG_ERROR   LOG(WARNING)
  #define SFLOG_FATAL   LOG(FATAL)
#else
  #ifdef SF_HAS_CISST
    #define SFLOG_INFO    CMN_LOG_RUN_VERBOSE
    #define SFLOG_DEBUG   CMN_LOG_RUN_DEBUG
    #define SFLOG_WARNING CMN_LOG_RUN_WARNING
    #define SFLOG_ERROR   CMN_LOG_RUN_ERROR
    #define SFLOG_FATAL   CMN_LOG_INIT_ERROR
  #else
    #define SFLOG_INFO    std::cout << "INFO   : "
    #define SFLOG_DEBUG   std::cout << "DEBUG  : "
    #define SFLOG_WARNING std::cout << "WARNING: "
    #define SFLOG_ERROR   std::cout << "ERROR  : "
    #define SFLOG_FATAL   std::cout << "FATAL  : "
  #endif // SF_HAS_CISST
#endif
};

#endif // _common_h

// Define which middleware is active
#if 0
#define SF_PACKAGE_CISST  0
#define SF_PACKAGE_ROS    1
#define SF_PACKAGE_OROCOS 2

#ifdef SF_HAS_CISST
    #define SF_PACKAGE SF_PACKAGE_CISST
#endif
#ifdef SF_HAS_ROS
    #undef SF_PACKAGE
    #define SF_PACKAGE SF_PACKAGE_ROS
#endif
#ifdef SF_HAS_OROCOS
    #undef SF_PACKAGE
    #define SF_PACKAGE SF_PACKAGE_OROCOS
#endif
#endif
