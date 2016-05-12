//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
// This class implements the Generic Component Model (GCM).
//
#ifndef _gcm_h
#define _gcm_h

#include "common/common.h"
#include "common/jsonwrapper.h"
#include "safecass/statemachine.h"
#include "safecass/state.h"
#include "safecass/event.h"

#include <map>

namespace SC {

class SCLIB_EXPORT GCM
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
        //InterfaceStateMachinesType Services; /*!< s_S: map of service states (projected states) */
    } StateMachinesType;

    /*! Typedef for specification of service state dependency information */
    typedef std::map<std::string, StrVecType *> ServiceStateDependencyInfoType;

    // typedef for connection information (to identify required interface)
    typedef struct {
        std::string SafetyCoordinatorName;
        std::string ComponentName;
        std::string RequiredInterfaceName;
    } RequiredInterfaceInfoType;
    typedef std::vector<RequiredInterfaceInfoType> ConnectionListType;
    typedef std::map<std::string, ConnectionListType *> ConnectionsType;

protected:
    // Name of coordinator instance that this GCM is associated with */
    const std::string CoordinatorName;
    /*! Name of component that this GCM is associated with */
    const std::string ComponentName;

    /*! State machines */
    StateMachinesType States;

    // Connection information
    // key: provided interface name, value: list of required interface information
    ConnectionsType Connections;

    /*! Service state dependency information */
    // key: name of [ s_A, s_F, required interfaces ]
    // value: a list of names of provided interfaces that are dependent on elements of name "key"
    // Used to decide if service state should change when a state transition occurs.
    ServiceStateDependencyInfoType ServiceStateDependencyInfo;
    // key: name of provided interfaces
    // value: a list of elements names on which a provided interface state depends
    // Used to calculate service state of a provided interface
    ServiceStateDependencyInfoType ServiceStateDependencyInfo2;

    // Get json representation of service state change
    void GetJSONForServiceStateChange(const std::string & providedInterfaceName, Json::Value & json);
    // Get statemachine instance
    const StateMachine * GetStateMachineComponent(ComponentStateViews view) const;
    const StateMachine * GetStateMachineInterface(const std::string & name, InterfaceTypes type) const;

    void PopulateStateUpdateJSON(const std::string & providedInterfaceName, Json::Value & json) const;

private:
    /*! Component associated with GCM has to be declared */
    GCM(void);

public:
    /*! Constructor with owner.  GCM is initialized based on information about owner 
        component specified. */
    GCM(const std::string & coordinatorName, const std::string & componentName);

    /*! Destructor */
    virtual ~GCM(void);

    /*! Add interface */
    bool AddInterface(const std::string & name, InterfaceTypes type);
    /*! Find interface */
    bool FindInterface(const std::string & name, InterfaceTypes type) const;
    /*! Remove interface */
    bool RemoveInterface(const std::string & name, InterfaceTypes type);
    // Add service state dependency information
    void AddServiceStateDependency(const Json::Value & services);
    bool AddServiceStateDependencyEntry(const std::string & providedInterfaceName,
                                        const std::string & name);

    // Process state transition event and returns transition and json object that contains 
    // service state changes.
    State::TransitionType ProcessStateTransition(State::StateMachineType type,
                                                 const Event & event,
                                                 const std::string & interfaceName,
                                                 Json::Value & json);

    // Add connection information (about required interface) to provided interface
    // This information is used to build up json for error propagation.
    void AddConnection(const std::string & providedInterfaceName,
                       const std::string & safetyCoordinatorName,
                       const std::string & requiredComponentName,
                       const std::string & requiredInterfaceName);

    // Reset all states and events
    void ResetStatesAndEvents(void);
    // Reset states and events of particular type of state machine
    void ResetStatesAndEvents(State::StateMachineType type);

    // Extract state transition history from state machines and populate JSON object.
    // baseId is used as the id of the first entry (state machine).
    // Returns a number of state machines extracted.
    unsigned int GetStateHistory(Json::Value & json, unsigned int baseId = 0);

    //
    // Getters
    //
    //! Returns name of component that this GCM is associated with
    const std::string & GetComponentName(void) const { return ComponentName; }
    //! Returns names of interfaces
    StrVecType GetNamesOfInterfaces(InterfaceTypes type) const;
    void       GetNamesOfInterfaces(InterfaceTypes type, StrVecType & names) const;
    //! Returns component state
    State::StateType GetComponentState(ComponentStateViews view = SYSTEM_VIEW) const;
    //! Returns component state with outstanding event.  
    State::StateType GetComponentState(ComponentStateViews view, const Event* & e) const;
    //! Returns interface state
    State::StateType GetInterfaceState(const std::string & name, InterfaceTypes type) const;
    //! Returns interface state with outstanding event
    State::StateType GetInterfaceState(const std::string & name, InterfaceTypes type, const Event* & e) const;
    //! Returns consolidated interface state
    State::StateType GetInterfaceState(InterfaceTypes type) const;
    State::StateType GetInterfaceState(InterfaceTypes type, const Event* & e) const;
    //! Returns service state of provided interface and event information that caused service state transition
    State::StateType GetServiceState(const std::string & providedInterfaceName, const Event * & event,
                                     bool forErrorPropagation) const;
    //
    // Setters
    //
    // Install user-defined statemachine event handler
    bool SetEventHandlerForComponent(GCM::ComponentStateViews view, StateEventHandler * handler);
    bool SetEventHandlerForInterface(GCM::InterfaceTypes type, const std::string & interfaceName,
                                     StateEventHandler * handler);

    //
    // Misc.
    //
    //! Human readable output of this class
    void ToStream(std::ostream & outputStream) const;
    // Print service state dependency table in tabular format
    void PrintServiceStateDependencyTable(std::ostream & out, const std::string & prefix = "");
    // Given a provided interface, print connection information
    void PrintConnections(const std::string providedInterfaceName, std::ostream & out,
                          const std::string & prefix = "");
    // Print connection information for all provided interfaces
    void PrintConnections(std::ostream & out, const std::string & prefix = "");
};
 
inline std::ostream & operator << (std::ostream & outputStream, const GCM & gcm) {
    gcm.ToStream(outputStream);
    return outputStream;
}

};

#endif // _gcm_h
