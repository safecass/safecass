//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: Apr 19, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _publisher_h
#define _publisher_h

#include "baseIce.h"

#include <messages.h>

namespace SF {

class SFLIB_EXPORT Publisher: public BaseIce {
protected:
    /*! typedef for base class */
    typedef BaseIce BaseType;

    /*! Publisher id (unique within a process) */
    static unsigned int Id;

    /*! Topic name to which this publisher publishes */
    const std::string TopicName;

    /*! IceStorm publisher proxy */
    MonitorSamplesPrx MonitorSamples;
    SupervisorControlsPrx SupervisorControls;

    /*! Initialization */
    void Init(void);

public:
    Publisher(const std::string & topicName);
    Publisher(const std::string & topicName, const std::string & propertyFileName);
    virtual ~Publisher();

    bool Startup(void);
    void Run(void) {}
    void Publish(const std::string & json);
    void Stop(void);

    static const std::string GetDefaultConfigFilePath(void);
};

};

#endif // _publisher_h
