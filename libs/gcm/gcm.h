//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: Apr 22, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// This class implements the Generic Component Model (GCM).
//
// Reference:
// TODO: add SafeComp14 paper
//
#ifndef _gcm_h
#define _gcm_h

#include "common.h"
#include "statemachine.h"
#include <map>

namespace SF {

class SFLIB_EXPORT GCM
{
public:
    typedef enum { PROVIDED_INTERFACE, REQUIRED_INTERFACE } InterfaceTypes;

    /*! Typedef for map of interfaces (key: interface name, value: statemachine 
        instance */
    typedef std::map<std::string, StateMachine *> InterfaceStateMachinesType;

    /*! Typedef for a set of state machines */
    typedef struct _StateMachines {
        StateMachine * ComponentFramework;   /*!< s_F: Component State, Framework View */
        StateMachine * ComponentApplication; /*!< s_A: Component State, Application View */
        InterfaceStateMachinesType RequiredInterfaces; /*!< s_R: map of required interface states */
        InterfaceStateMachinesType ProvidedInterfaces; /*!< s_P: map of provided interface states */
    } StateMachinesType;

protected:
    /*! Name of component that GCM is associated with */
    const std::string ComponentName;

    /*! State machines */
    StateMachinesType States;

private:
    /*! Component associated with GCM has to be declared */
    GCM(void);

public:
    /*! Constructors with owner.  GCM is initialized based on information about owner 
        component specified. */
    GCM(const std::string & componentName);

    /*! Destructor */
    virtual ~GCM(void);

    /*! Add interface */
    bool AddInterface(const std::string & name, const GCM::InterfaceTypes type);
    /*! Find interface */
    bool FindInterface(const std::string & name, const GCM::InterfaceTypes type) const;
    /*! Remove interface */
    bool RemoveInterface(const std::string & name, const GCM::InterfaceTypes type);

    /*! State change event handlers */
    void ProcessEvent_ComponentFramework(const SF::State::TransitionType transition);
    void ProcessEvent_ComponentApplication(const SF::State::TransitionType transition);
    void ProcessEvent_Interface(const SF::State::TransitionType transition);

    //
    // Getters
    //
    /*! Return name of component that this GCM is associated with */
    const std::string & GetComponentName(void) const { return ComponentName; }

    //
    // Misc.
    //
    /*! Human readable output of this class */
    void ToStream(std::ostream & outputStream) const;
};
 
inline std::ostream & operator << (std::ostream & outputStream, const GCM & gcm) {
    gcm.ToStream(outputStream);
    return outputStream;
}

};

#endif // _gcm_h
