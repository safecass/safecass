/*

  Safety Framework for Component-based Robotics

  Created on: August 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _curl_wrapper_h
#define _curl_wrapper_h

#include "common.h"

namespace SC {

class SCLIB_EXPORT Curl {
protected:
    std::string BaseURL;

public:
    Curl(const std::string & baseURL);
    ~Curl();

    const std::string & GetBaseURL(void);
    void SetBaseURL(const std::string & baseURL);

    char * CallAPI(const std::string & arg);
};

};

#endif //_curl_wrapper_h
