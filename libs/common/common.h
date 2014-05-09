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
#ifndef _common_h
#define _common_h

#include <vector>
#include <iostream>
#include <sstream>

// import build tree configurations
#include "config.h"
// for dynamic library support
#include "SFDLLDefines.h"

#if SF_USE_G2LOG
  #include "g2logworker.h"
  #include "g2log.h"
  #include <iomanip>
#endif

#if SF_HAS_CISST
  #include <cisstCommon/cmnPortability.h>
  #include <cisstCommon/cmnLogger.h>
  #include <cisstCommon/cmnAssert.h>
#endif

namespace SF {

//--------------------------------------------------
//  Common type definitions
//--------------------------------------------------
typedef std::vector<std::string> StrVecType;

typedef unsigned int SamplingRateType;   // f (Hz)
typedef double       SamplingPeriodType; // T (=1/f)

//--------------------------------------------------
//  Utilities
//--------------------------------------------------
/*! Returns information about middlewares available */
StrVecType GetMiddlewareInfo(void);
void GetMiddlewareInfo(StrVecType & info);

/*! Get information about cisst detected */
#if SF_HAS_CISST
std::string GetCISSTInfo(void);
#endif

#define STR(faultType) std::string(#faultType)

//! Macro to suppress unused parameter warnings
#if SF_ON_WINDOWS
  #define UNUSED(argument) MARKED_AS_UNUSED ## argument
#else // for gcc or clang compiler
  #define UNUSED(argument) MARKED_AS_UNUSED ## argument __attribute__((unused))
#endif

//--------------------------------------------------
//  Common macro definitions
//--------------------------------------------------
// Logger macros
#if SF_USE_G2LOG
  #define SFLOG_INFO    LOG(INFO)
  #define SFLOG_DEBUG   LOG(DEBUG)
  #define SFLOG_WARNING LOG(WARNING)
  #define SFLOG_ERROR   LOG(WARNING)
  #define SFLOG_FATAL   LOG(FATAL)
#else
  #if SF_HAS_CISST
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

// Assertion macro
#if SF_USE_G2LOG
  /* G2LOG CHECK macro usage examples:
     (from http://www.codeproject.com/Articles/288827/g2log-An-efficient-asynchronous-logger-using-Cplus#TOC_design_by_contract)

     CHECK(1 != 2);
     CHECK(1 > 2) << "CHECK(false) will trigger a FATAL message, put it on log, then exit";

     const std::string arg = "CHECKF";
     CHECKF(1 > 2, "This is a test to see if %s works", arg.c_str());
  */
  #define SFASSERT CHECK
#else
  #if SF_HAS_CISST
    #define SFASSERT CMN_ASSERT
  #else
    #include <assert.h>
    #define SFASSERT assert
  #endif
#endif
};

// Throw macro
#define SFTHROW(_msg) throw(std::runtime_error(std::string(_msg)));

//--------------------------------------------------
//  Common macro definitions
//--------------------------------------------------
// String definitions
#define NONAME "NONAME"

#define PROBE std::cout << __FILE__ << ":" << __LINE__ << "  "

#endif // _common_h

// Define which middleware is active
#if 0
#define SF_PACKAGE_CISST  0
#define SF_PACKAGE_ROS    1
#define SF_PACKAGE_OROCOS 2

#if SF_HAS_CISST
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
