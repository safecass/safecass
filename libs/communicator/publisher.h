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

#include <monitorSamples.h>

namespace SF {

class SFLIB_EXPORT Publisher: public BaseIce {
protected:
    /*! typedef for base class */
    typedef BaseIce BaseType;

    /*! Publisher id (unique within a process) */
    static unsigned int Id;

    /*! IceStorm publisher proxy */
    MonitorSamplesPrx MonitorSamples;

    /*! Initialization */
    void Init(void);

public:
    Publisher();
    Publisher(const std::string & propertyFileName);
    virtual ~Publisher();

    void Startup(void);
    void Run(void) {}
    void Publish(const std::string & processName, const std::string & componentName, double period);
    void Stop(void);

    static const std::string GetDefaultConfigFilePath(void);
};

};

#endif // _publisher_h
