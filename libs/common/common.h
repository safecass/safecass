/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _common_h
#define _common_h

#include <vector>
#include <sstream>

namespace SF {

typedef std::vector<std::string> StrVecType;

// Returns information about middlewares available
StrVecType GetMiddlewareInfo(void);
void GetMiddlewareInfo(StrVecType & info);

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

#ifdef SF_HAS_CISST
std::string GetCISSTInfo(void);
#endif

};

#endif // _common_h
