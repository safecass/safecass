/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _utils_h
#define _utils_h

#include "common.h"

namespace SF {

// trim from start
std::string &ltrim(std::string &s);

// trim from end
std::string &rtrim(std::string &s);

// trim from both ends
std::string &trim(std::string &s);

// to lowercase
void to_lowercase(std::string & s);

// to uppercase 
void to_uppercase(std::string & s);

};

#endif // _utils_h
