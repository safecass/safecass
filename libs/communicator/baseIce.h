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
#ifndef _baseIce_h
#define _baseIce_h

#include "common.h"
//#include "monitor.h"
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

namespace SF {

// Base class for Ice proxies
class SFLIB_EXPORT BaseIce
{
public:
    typedef enum { INIT, STARTUP, RUNNING, STOP } CommunicatorStateType;

protected:
    //
    // Resources and methods for Ice proxy
    //
    /*! Name of property file that configures proxy settings. */
	std::string IcePropertyFileName;

    /*! Ice initialization data */
    Ice::InitializationData IceInitData;

    /*! Ice proxy communicator -- the central object in Ice.
        See http://zeroc.com/doc/Ice-3.4.1-IceTouch/reference/Ice/Communicator.html
        for more details */
    Ice::CommunicatorPtr IceCommunicator;

    /*! Ice logger */
    Ice::LoggerPtr IceLogger;

    /*! Global unique id of this Ice object. */
    std::string IceGUID;

    /*! Initialize Ice module. */
    virtual void IceInitialize(void);

    /*! Ice module clean up. For Ice connection closure, see below:
        http://www.zeroc.com/doc/Ice-3.4.1-IceTouch/manual/Connections.37.6.html */
    //virtual void IceCleanup(void);

    /*! Return the global unique id of this object. Currently, IceUtil::generateUUID()
        is used to set ID which guarantees the uniqueness of it across networks
        It is basically a mixture of IP address and/or MAC address, process id,
        and object id (or a pointer to this object). */
    /*
    std::string GetIceGUID() {
        if (IceGUID.empty()) {
            IceGUID = IceUtil::generateUUID();
        }
        return IceGUID;
    }
    */

    //
    // Resources and methods for communicator
    //
private:
    /*! Current state */
    CommunicatorStateType State;

    /*! Mutex for state change */
    IceUtil::Mutex Mutex;

protected:
    /*! Change or update current state */
    void StateChange(CommunicatorStateType state);

    /*! Initialization */
    virtual void Init(void);
    
    bool IsInitialized(void) const;
    bool IsStartup(void) const;
    bool IsRunning(void) const;
    bool IsStopped(void) const;

public:
    BaseIce(const std::string & propertyFileName);
    virtual ~BaseIce();

    /*! Prepare publish/subscribe via Ice network */
    virtual bool Startup(void);

    /*! Start publish/subscribe topic */
    virtual void Run(void) = 0;

    /*! Stop publish/subscribe topic */
    virtual void Stop(void);
};


//-----------------------------------------------------
//  Auxiliary Class Definitions
//-----------------------------------------------------
/*! Logger class using the cisst's internal logging mechanism */
class SFLogger : public Ice::Logger
{
public:
    inline void print(const ::std::string & message) {
        SFLOG_INFO << "ICE: " << message << std::endl;
    }
    inline void trace(const ::std::string & category, const ::std::string & message) {
        SFLOG_DEBUG << "ICE: " << category << " :: " << message << std::endl;
    }
    inline void warning(const ::std::string & message) {
        SFLOG_WARNING << "ICE: " << message << std::endl;
    }
    inline void error(const ::std::string & message) {
        SFLOG_ERROR << "ICE: " << message << std::endl;
    }
    // Support for Ice 3.4 or above (used by IceBox)
    // MJ: This does not get called in the current implementation and thus
    // can return null.  However, to be on the safe side, it returns a new
    // instance of itself.
    ::Ice::LoggerPtr cloneWithPrefix(const ::std::string&) {
        return new SFLogger;
    }
};

/*! Callback class to pass data for subscriber.  When a subscriber receives an event
    notification, an event payload is delievered to the cisst framework via this 
    callback class.  The cisst plug-ins for the safety framework -- specifically
    mtsSafetySupervisor and mtsMonitorComponent -- should provide callback handler
    classes which derive from this class. */
class SFCallback {
public:
    SFCallback() {}

    virtual void Callback(const std::string & json) = 0;
};

};

#endif // _baseIce_h
