//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 2, 2012
// Last revision: Apr 20, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
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

// Google logging library
#include "glog/logging.h"

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
// StrVecType GetMiddlewareInfo(void);
// void GetMiddlewareInfo(StrVecType & info);

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
// Logger macro aliases
#define SCLOG_INFO      LOG(INFO)
#define SCLOG_DEBUG     VLOG(1)
#define SCLOG_DEBUGV    VLOG(2)
#define SCLOG_DEBUGVV   VLOG(3)
#define SCLOG_WARNING   LOG(WARNING)
#define SCLOG_ERROR     LOG(ERROR)
#define SCLOG_FATAL     LOG(FATAL)

#if SC_HAS_CISST
  #define SCLOG_INFO    CMN_LOG_RUN_VERBOSE
  #define SCLOG_DEBUG   CMN_LOG_RUN_DEBUG
  #define SCLOG_WARNING CMN_LOG_RUN_WARNING << __FILE__ << ":" << __LINE__ << " "
  #define SCLOG_ERROR   CMN_LOG_RUN_ERROR << __FILE__ << ":" << __LINE__ << " "
  #define SCLOG_FATAL   CMN_LOG_INIT_ERROR << __FILE__ << ":" << __LINE__ << " "
#endif

// Assertion macro aliasing
#if SC_HAS_CISST
  #define SCASSERT CMN_ASSERT
#else
  // Google logger also provides other variations of CHECK macros:
  // CHECK_EQ, CHECK_NE, CHECK_LE, CHECK_LT, CHECK_GE, CHECK_GT
  // For more details, see https://google-glog.googlecode.com/svn/trunk/doc/glog.html
  #define SCASSERT CHECK
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
