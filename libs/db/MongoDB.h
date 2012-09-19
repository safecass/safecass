/*

  Safety Framework for Component-based Robotics

  Created on: August 14, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _MongoDB_h
#define _MongoDB_h

#include "common.h"
#include "jsonSerializer.h"

namespace SF {

class SFLIB_EXPORT MongoDB {
public:
    MongoDB() {}
    ~MongoDB() {}

    /*! Generate DB record from json */
    static const std::string GetDBEntryFromMonitorTopic(JSONSerializer & jsonSerializer);
};

};

#endif // _MongoDB_h
