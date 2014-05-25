//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 31, 2012
// Last revision: May 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _baseIce_h
#define _baseIce_h

#include "common.h"
#include "topic_def.h"

#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

namespace SF {

// Base class for Ice proxies
class SFLIB_EXPORT BaseIce
{
public:
    //! Typedef for Ice comminucators
    typedef enum { INVALID, INIT, READY, RUNNING, STOP } CommunicatorStateType;

private:
    /*! Current state */
    CommunicatorStateType State;

    /*! Mutex for state change */
    IceUtil::Mutex Mutex;

    /*! Default constructor should not be used */
    BaseIce(void);

protected:
    //! Topic name for publish/subscribe
    const std::string TopicName;

    //! Topic associated with this Ice entity
    Topic::Type Topic;

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

    /*! Change or update current state */
    void StateChange(CommunicatorStateType state);

    /*! Initialization */
#if 0
    virtual void Init(void);
    
    bool IsInitialized(void) const;
    bool IsStartup(void) const;
    bool IsRunning(void) const;
    bool IsStopped(void) const;
    bool IsCleanedUp(void) const;
#endif

public:
    //! Constructor
    BaseIce(const std::string & topicName, const std::string & propertyFileName);

    //! Destructor
    virtual ~BaseIce();

    /*! Begin publishing to or subscribing from topic */
#if 0
    virtual bool Startup(void);

    // TODO: need to refactor states
    /*! Begin publishing/subscribing topic */
    virtual void Start(void);

    // TODO: need to refactor states
    /*! Begin publishing to or subscribing from topic */
    virtual void Run(void);

    // TODO: need to refactor states
    /*! Finish publishing/subscribing topic */
    virtual void Stop(void);

    virtual void Cleanup(void);
#endif
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

/*! Callback for subscriber.  When a subscriber receives messages from the topic
    that it has subscribed for, this callback is called by the communication layer 
    (IceStorm at this moment). */
class SFCallback {
public:
    SFCallback(void) {}
    virtual ~SFCallback() {}

    virtual void CallbackControl(Topic::Control::CategoryType category, 
                                 const std::string & json) = 0;
    virtual void CallbackData(Topic::Data::CategoryType category, 
                              const std::string & json) = 0;
};

};

#endif // _baseIce_h
