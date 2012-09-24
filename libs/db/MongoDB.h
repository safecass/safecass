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

    /*! Convert JSON message from the Monitor topic into MongoDB record */
    static const std::string GetDBEntryFromMonitorTopic(JSONSerializer & jsonSerializer);

    /*! Convert JSON message from the Fault topic into MongoDB record */
    static const std::string GetDBEntryFromFaultTopic(JSONSerializer & jsonSerializer);
};

};

#endif // _MongoDB_h
