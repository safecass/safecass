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

namespace SF {

class SFLIB_EXPORT MongoDB {
public:
    MongoDB();
    ~MongoDB();

    // MJ TODO: if multiple DBMSs are to be supported, this could be moved up to
    // the base class
    /*! Convert topic message into DB-specific entry format */
    static const std::string GetDBEntryFromMonitorTopic(const std::string & json);
};

};

#endif // _MongoDB_h
