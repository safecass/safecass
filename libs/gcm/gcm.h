//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: Jul 17, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// This class implements the Generic Component Model (GCM).
//
#ifndef _gcm_h
#define _gcm_h

#include "common.h"
#include "statemachine.h"
#include "state.h"
#include "event.h"

#include <map>

namespace SF {

class SFLIB_EXPORT GCM
{
    friend class Coordinator;

public:
    /*! Typedef for component state views */
    typedef enum { SYSTEM_VIEW, FRAMEWORK_VIEW, APPLICATION_VIEW } ComponentStateViews;
    /*! Typedef for interface types */
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
        InterfaceStateMachinesType Services; /*!< s_S: map of service states (projected states) */
    } StateMachinesType;

    /*! Typedef for specification of service state dependency information */
    // list of statemachines
    typedef std::vector<StateMachine *> StateMachineVectorType;
    // key: name of [ s_A, s_F, required interfaces ]
    typedef std::map<std::string, StateMachineVectorType> ServiceStateDependencyInfoType;

protected:
    /*! Name of component that GCM is associated with */
    const std::string ComponentName;

    /*! State machines */
    StateMachinesType States;

    /*! Service state dependency information */
    ServiceStateDependencyInfoType ServiceStateDependencyInfo;

private:
    /*! Component associated with GCM has to be declared */
    GCM(void);

public:
    /*! Constructor with owner.  GCM is initialized based on information about owner 
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

    // Process state transition event and returns transition
    State::TransitionType ProcessStateTransition(State::StateMachineType type,
                                                 const Event *           event,
                                                 const std::string &     interfaceName = "");

    //
    // Getters
    //
    /*! Returns name of component that this GCM is associated with */
    const std::string & GetComponentName(void) const { return ComponentName; }
    /*! Returns names of interfaces */
    StrVecType GetNamesOfInterfaces(InterfaceTypes type) const;
    void       GetNamesOfInterfaces(InterfaceTypes type, StrVecType & names) const;
    /*! Returns component state */
    State::StateType GetComponentState(const ComponentStateViews view = SYSTEM_VIEW) const;
    /*! Returns interface state */
    State::StateType GetInterfaceState(const std::string & name, const GCM::InterfaceTypes type) const;
    /*! Returns projected interface state */
    State::StateType GetInterfaceState(const GCM::InterfaceTypes type) const;

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
