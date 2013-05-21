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

//--------------------------------------------------
//  Trimming Utils
//--------------------------------------------------
#include <algorithm> 
#include <functional> 
#include <cctype>
//#include <locale>

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

// to lowercase
static void to_lowercase(std::string & s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

// to uppercase 
static void to_uppercase(std::string & s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

};

#endif // _utils_h
