//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: May 13, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// URL          : https://github.com/safecass/safecass
//
// This class implements the Generic Component Model (GCM).
//
// Chapter 4.
// Figures 4.5, 4.6
//
#ifndef _gcm_h
#define _gcm_h

#include "common/common.h"
//#include "safecass/statemachine.h"

#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace SC {

// Forward declaration
class StateMachine;

class SCLIB_EXPORT GCM
{
    friend class Coordinator;

public:
    //! Typedef of component state views
    /*!
        Two layers are defined within the GCM: component framework layer and application
        logic layer.  The component framework layer provides application-independent but
        framework-specific services, and the application logic layer uses these services
        to implement application-specific logic or algorithms.
    */
    typedef enum {
        FRAMEWORK_VIEW,   /*!< View on the component framework layer */
        APPLICATION_VIEW, /*!< View on the application logic layer */
        SYSTEM_VIEW       /*!< Combined view representing overall status of the system */
    } ViewType;

    //! Typedef of interface types
    typedef enum {
        PROVIDED_INTERFACE,
        REQUIRED_INTERFACE
    } InterfaceType;

    //! Typedef of container for state machines representing interface states
    /*!
        Key: interface name, value: statemachine instance
    */
    //typedef std::map<std::string, StateMachine *> InterfaceStateMachinesType;

    //! Typedef of complete set of state machines representing GCM component status
    // => Replaced by graph
#if 0
    /*!
        State machines for component states (Framework and Application states) always
        exist, whereas state machines for interface states are dynamically created only
        when interfaces are registered to SAFECASS.
    */
    typedef struct {
        StateMachine Framework;                      /*!< Component state in system view */
        StateMachine Application;                    /*!< Component state in application View */
        InterfaceStateMachinesType RequiredInterfaces; /*!< Collection of required interface states */
        InterfaceStateMachinesType ProvidedInterfaces; /*!< Collection of provided interface states */
    } StateMachinesType;
#endif

protected:
    //! Typedef of service state dependency specification
    /*!
        1) Front-end: JSON

        "service" : [
            {   // name of provided interface
                "name" : "Robot",
                // service state dependency information
                "dependency" : {
                    "s_R" : [ "ForceSensor", "AMCInterface", "PendantRequired" ],
                    "s_A" : true,
                    "s_F" : true
                }
            },
            {   "name" : "prmRobot",
                "dependency" : {
                    "s_R" : [ "ForceSensor", "AMCInterface", "PendantRequired" ],
                    "s_A" : true,
                    "s_F" : true
                }
            }
        ]

        2) Back-end: boost::adjacency_list of boost.graph

            http://www.boost.org/doc/libs/1_60_0/libs/graph/doc/using_adjacency_list.html
    */
    //typedef std::map<std::string, StrVecType *> ServiceStateDependencyInfoType;

    typedef enum {
        COLOR_FRAMEWORK,   // Framework state
        COLOR_APPLICATION, // Application state
        COLOR_REQUIRED,    // Required interface state
        COLOR_PROVIDED     // Provided interface state
    } VertexColorType;

    // - BGL:
    //      http://www.boost.org/doc/libs/1_60_0/libs/graph/doc/table_of_contents.html
    // - Boost Property:
    //      http://www.boost.org/doc/libs/1_54_0/libs/graph/doc/property.html
    // - Bundled Properties:
    //      http://www.boost.org/doc/libs/1_40_0/libs/graph/doc/bundles.html
    // - Custom edge/vertex property:
    //      http://www.boost.org/doc/libs/1_60_0/libs/graph/doc/using_adjacency_list.html#sec:adjacency-list-properties

    struct StateMachineProperty_t {
        typedef boost::vertex_property_tag kind;
    };

    typedef boost::property<boost::vertex_color_t, int,
            boost::property<boost::vertex_name_t, std::string,
            boost::property<StateMachineProperty_t, StateMachine* > > > VertexProperty;
    typedef boost::property<boost::edge_weight_t, int> EdgeProperty;

    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                  VertexProperty, EdgeProperty> GraphType;

    typedef boost::graph_traits<GraphType>::vertex_descriptor Vertex;

    //! Graph containing a set of GCM state machine objects, i.e., instances of StateMachine
    /*!
        Directed
    */
    GraphType Graph;

    // This may not be required either (replaced by graph)
#if 0
    // typedef for connection information (to identify required interface)
    typedef struct {
        std::string SafetyCoordinatorName;
        std::string ComponentName;
        std::string RequiredInterfaceName;
    } RequiredInterfaceInfoType;
    typedef std::vector<RequiredInterfaceInfoType> ConnectionListType;
    typedef std::map<std::string, ConnectionListType *> ConnectionsType;
#endif

    //! Name of Coordinator instance that this GCM belongs to
    /*!
        Every GCM belongs to a Coordinator.  Because a Coordinator is typically defined
        for each process in the system, a process name can be used as a Coordinator name.
    */
    const std::string CoordinatorName;

    //! Name of component that this GCM represents
    const std::string ComponentName;

    //! Default constructor should not be used
    GCM(void);

public:
    //! Constructor
    GCM(const std::string & coordinatorName, const std::string & componentName);

    //! Destructor
    ~GCM();

    //! GCM accessors
    /*!
        \addtogroup Generic Component Model (GCM) accessors
        @{
    */
    //! Returns name of Coordinator that this GCM belongs to
    inline const std::string & GetCoordinatorName(void) const { return CoordinatorName; }
    //! Returns name of component that this GCM represents
    inline const std::string & GetComponentName(void) const { return ComponentName; }
    /*! @} */

#if 0
protected:
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
    const StateMachine * GetStateMachineComponent(ViewType view) const;
    const StateMachine * GetStateMachineInterface(const std::string & name, InterfaceType type) const;

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
    bool AddInterface(const std::string & name, InterfaceType type);
    /*! Find interface */
    bool FindInterface(const std::string & name, InterfaceType type) const;
    /*! Remove interface */
    bool RemoveInterface(const std::string & name, InterfaceType type);
    // Add service state dependency information
    void AddServiceStateDependency(const Json::Value & services);
    bool AddServiceStateDependencyEntry(const std::string & providedInterfaceName,
                                        const std::string & name);

    // Process state transition event and returns transition and json object that contains 
    // service state changes.
    State::TransitionType ProcessStateTransition(State::StateMachineType type,
                                                 Event & event,
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
    //! Returns names of interfaces
    StrVecType GetNamesOfInterfaces(InterfaceType type) const;
    void       GetNamesOfInterfaces(InterfaceType type, StrVecType & names) const;
    //! Returns component state
    State::StateType GetComponentState(ViewType view = SYSTEM_VIEW) const;
    //! Returns component state with outstanding event.  
    State::StateType GetComponentState(ViewType view, const Event* & e) const;
    //! Returns interface state
    State::StateType GetInterfaceState(const std::string & name, InterfaceType type) const;
    //! Returns interface state with outstanding event
    State::StateType GetInterfaceState(const std::string & name, InterfaceType type, const Event* & e) const;
    //! Returns consolidated interface state
    State::StateType GetInterfaceState(InterfaceType type) const;
    State::StateType GetInterfaceState(InterfaceType type, const Event* & e) const;
    //! Returns service state of provided interface and event information that caused service state transition
    State::StateType GetServiceState(const std::string & providedInterfaceName, const Event * & event,
                                     bool forErrorPropagation) const;
    //
    // Setters
    //
    // Install user-defined statemachine event handler
    bool SetEventHandlerForComponent(GCM::ViewType view, StateEventHandler * handler);
    bool SetEventHandlerForInterface(GCM::InterfaceType type, const std::string & interfaceName,
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
#endif

    //! Human readable output of this class
    void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & os, const GCM & gcm) {
    gcm.ToStream(os);
    return os;
}

};

#endif // _gcm_h
