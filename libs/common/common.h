/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include <vector>
#include <sstream>

namespace SF {

typedef std::vector<std::string> StrVecType;

// Returns information about middlewares available
StrVecType GetMiddlewareInfo(void);
void GetMiddlewareInfo(StrVecType & info);

#ifdef SF_HAS_CISST
std::string GetCISSTInfo(void);
#endif

};
