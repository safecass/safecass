/*

  Safety Framework for Component-based Robotics

  Created on: July 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _publisher_h
#define _publisher_h

#include "baseIce.h"

namespace SF {

// Adapter class: almost empty but pure virtual methods
class SFLIB_EXPORT Publisher: public BaseIce {
public:
    Publisher(const std::string & propertyFileName);
    virtual ~Publisher();

    void Run(void);
};

};

#endif // _publisher_h
