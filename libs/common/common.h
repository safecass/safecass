//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 2, 2012
// Last revision: May 19, 2014
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
#include "SCDLLDefines.h"

#if SC_USE_G2LOG
  #include "g2logworker.h"
  #include "g2log.h"
  #include <iomanip>
#endif

#if SC_HAS_CISST
  #include <cisstCommon/cmnPortability.h>
  #include <cisstCommon/cmnLogger.h>
  #include <cisstCommon/cmnAssert.h>
#endif

namespace SC {

//--------------------------------------------------
//  Common type definitions
//--------------------------------------------------
typedef std::vector<int>          IntVecType;
typedef std::vector<unsigned int> UIntVecType;
typedef std::vector<float>        FloatVecType;
typedef std::vector<double>       DoubleVecType;
typedef std::vector<std::string>  StrVecType;

typedef unsigned int SamplingRateType;   // f (Hz)
typedef double       SamplingPeriodType; // T (=1/f)

//--------------------------------------------------
//  Utilities
//--------------------------------------------------
/*! Returns information about middlewares available */
StrVecType GetMiddlewareInfo(void);
void GetMiddlewareInfo(StrVecType & info);

/*! Get information about cisst detected */
#if SC_HAS_CISST
std::string GetCISSTInfo(void);
#endif

#define STR(faultType) std::string(#faultType)

//! Macro to suppress unused parameter warnings
/*
#if SC_ON_WINDOWS
  #define UNUSED(argument) MARKED_AS_UNUSED ## argument
#else // for gcc or clang compiler
  #define UNUSED(argument) MARKED_AS_UNUSED ## argument __attribute__((unused))
#endif
*/
#ifndef UNUSED
#if defined(__GNUC__) || defined(__clang__)
    #define UNUSED(arg) UNUSED_##arg __attribute__((unused))
#elif SC_ON_WINDOWS
    #define UNUSED(arg) UNUSED_##arg
#endif
#endif // UNUSED

//--------------------------------------------------
//  Common macro definitions
//--------------------------------------------------
// Logger macros
#if SC_USE_G2LOG
  #define SCLOG_INFO    LOG(INFO)
  #define SCLOG_DEBUG   LOG(DEBUG)
  #define SCLOG_WARNING LOG(WARNING)
  #define SCLOG_ERROR   LOG(WARNING)
  #define SCLOG_FATAL   LOG(FATAL)
#else
  #if SC_HAS_CISST
    #define SCLOG_INFO    CMN_LOG_RUN_VERBOSE
    #define SCLOG_DEBUG   CMN_LOG_RUN_DEBUG
    #define SCLOG_WARNING CMN_LOG_RUN_WARNING << __FILE__ << ":" << __LINE__ << " "
    #define SCLOG_ERROR   CMN_LOG_RUN_ERROR << __FILE__ << ":" << __LINE__ << " "
    #define SCLOG_FATAL   CMN_LOG_INIT_ERROR << __FILE__ << ":" << __LINE__ << " "
  #else
    #define SCLOG_INFO    std::cout << "INFO   : "
    #define SCLOG_DEBUG   std::cout << "DEBUG  : "
    #define SCLOG_WARNING std::cout << "WARNING: "
    #define SCLOG_ERROR   std::cout << "ERROR  : "
    #define SCLOG_FATAL   std::cout << "FATAL  : "
  #endif // SC_HAS_CISST
#endif

// Assertion macro
#if SC_USE_G2LOG
  /* G2LOG CHECK macro usage examples:
     (from http://www.codeproject.com/Articles/288827/g2log-An-efficient-asynchronous-logger-using-Cplus#TOC_design_by_contract)

     CHECK(1 != 2);
     CHECK(1 > 2) << "CHECK(false) will trigger a FATAL message, put it on log, then exit";

     const std::string arg = "CHECKF";
     CHECKF(1 > 2, "This is a test to see if %s works", arg.c_str());
  */
  #define SCASSERT CHECK
#else
  #if SC_HAS_CISST
    #define SCASSERT CMN_ASSERT
  #else
    #include <assert.h>
    #define SCASSERT assert
  #endif
#endif
};

// Throw macro
#define SCTHROW(_msg) throw(std::runtime_error(std::string(_msg)));

//--------------------------------------------------
//  Common macro definitions
//--------------------------------------------------
// String definitions
#define NONAME "NONAME"

#define _PROBE std::cout << __FILE__ << ":" << __LINE__ << "  "

#endif // _common_h

// Define which middleware is active
#if 0
#define SC_PACKAGE_CISST  0
#define SC_PACKAGE_ROS    1
#define SC_PACKAGE_OROCOS 2

#if SC_HAS_CISST
    #define SC_PACKAGE SC_PACKAGE_CISST
#endif
#ifdef SC_HAS_ROS
    #undef SC_PACKAGE
    #define SC_PACKAGE SC_PACKAGE_ROS
#endif
#ifdef SC_HAS_OROCOS
    #undef SC_PACKAGE
    #define SC_PACKAGE SC_PACKAGE_OROCOS
#endif
#endif
