//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: May 30, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// URL          : https://github.com/safecass/safecass
//
// This class implements the Generic Component Model (GCM).
//
// Chapter 4.
// Figures 4.5, 4.6
//
// TODO
// - Getter for every type of state machine
//
#ifndef _gcm_h
#define _gcm_h

#include "common/common.h"
#include "safecass/state.h"
#include "safecass/event.h"

#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>

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

    //! Typedef of graph export format
    typedef enum {
        EXPORT_HUMAN_READABLE,
        EXPORT_GRAPHVIZ
    } ExportFormatType;

protected:
    //
    // Graph containing information about state dependency and error propagation based on
    // Boost Graph Library (BGL).
    //
    // References:
    //
    // - BGL:
    //      http://www.boost.org/doc/libs/1_60_0/libs/graph/doc/table_of_contents.html
    // - Boost Property:
    //      http://www.boost.org/doc/libs/1_54_0/libs/graph/doc/property.html
    // - Bundled Properties:
    //      http://www.boost.org/doc/libs/1_40_0/libs/graph/doc/bundles.html
    // - Custom edge/vertex property:
    //      http://www.boost.org/doc/libs/1_60_0/libs/graph/doc/using_adjacency_list.html#sec:adjacency-list-properties
    // - Property example:
    //      build/external_packages/boost/src/libs/graph/example/edge_property.cpp
    //
    class VertexProperty {
    public:
        //! Name of vertex
        std::string Name;
        //! Type of state machine
        State::StateMachineType StateType;
        //! Instance of state machine
        StateMachine * SM;
        //! Constructor
        VertexProperty(void): Name(NONAME),
                              StateType(State::STATEMACHINE_INVALID),
                              SM(0) {}
    };

    class EdgeProperty {
    public:
        //! How many times error was propagated through this edge
        int Count;
        //! Constructor
        EdgeProperty(void): Count(0) {}
        // TODO boolean flag can be added to seletively enable and disable state dependency
    };

    // Choosing EdgeList and VertexList:
    // - http://www.boost.org/doc/libs/1_61_0/libs/graph/doc/using_adjacency_list.html#sec:choosing-graph-type
    /*!
        Although setS would have been a primary choice for vertex and edge containers in
        favor of the iterator and descriptor stability/invalidation, vecS has chosen
        because the same interface name can be used as the name of a required and provided
        interface.
    */
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                                  VertexProperty, EdgeProperty > GraphType;

    // BGL labeled_graph:
    // - http://stackoverflow.com/questions/2244580/find-boost-bgl-vertex-by-a-key
    // - http://www.boost.org/doc/libs/1_47_0/libs/graph/example/labeled_graph.cpp
    //typedef boost::labeled_graph<InternalGraphType, std::string> GraphType;

    //! Typedefs for convenience
    typedef boost::graph_traits<GraphType>::vertex_descriptor VertexDescriptor;
    typedef boost::graph_traits<GraphType>::edge_descriptor   EdgeDescriptor;

    typedef boost::graph_traits<GraphType>::vertex_iterator   VertexIter;
    typedef boost::graph_traits<GraphType>::out_edge_iterator OutEdgeIter;
    typedef boost::graph_traits<GraphType>::in_edge_iterator  InEdgeIter;

    //
    // BGL property writers for GraphViz export feature
    //
    // A property writer for vertices or edges is a functor which can be called with two
    // parameters: std::ostream and either a vertex or an edge descriptor.
    // A property writer for graphs is a functor which is called with one parameter of type
    // std::ostream and should print a series of graph properties.
    //
    // TO APPLY: cluster
    // http://www.graphviz.org/Gallery/directed/cluster.html
    //
    // References:
    //
    // GraphViz Node Shapes: http://www.graphviz.org/doc/info/shapes.html
    //

#if 0
    // Vertex property writer
    class VertexPropertyWriter {
    private:
        const GraphType & g;
    public:
        VertexPropertyWriter(const GraphType & graph): g(graph) {}
        template <class Vertex>
        void operator()(std::ostream & os, const Vertex & v) const {
            //os << "[label=\"" << g[v].Name << "\"]";
            os << "[label=<" << g[v].Name << ">]";
        }
    };

    // Edge property writer
    class EdgePropertyWriter {
    private:
        const GraphType & g;
    public:
        EdgePropertyWriter(const GraphType & graph): g(graph) {}
        template <class Edge>
        void operator()(std::ostream & os, const Edge & v) const {
            os << "[label=\"" << "FIXME" << "\"]";
        }
    };

    // Graph property writer
    class GraphPropertyWriter {
    public:
        void operator()(std::ostream & os) const {
            //os << "graph [bgcolor=lightgrey]" << std::endl;
            os << "node [shape=ellipse color=black]" << std::endl;
            os << "edge [style=dashed splines=curved]" << std::endl;
        }
    };
#endif

protected:
    //! Graph containing a set of GCM state machine objects, i.e., instances of StateMachine
    /*!
        Directed
    */
    GraphType Graph;

    //! Name of Coordinator instance that this GCM belongs to
    /*!
        Every GCM belongs to a Coordinator.  Because a Coordinator is typically defined
        for each process in the system, a process name can be used as a Coordinator name.
    */
    const std::string CoordinatorName;

    //! Name of component that this GCM represents
    const std::string ComponentName;

    //! Vertex descriptor of state machine for framework state
    VertexDescriptor VertexFramework;

    //! Vertex descriptor of state machine for application state
    VertexDescriptor VertexApplication;

    //! Initialize graph of state machines
    /*!
        The graph maintains two vertices by default: framework state and application state.
        This method removes all the existing vertices but those two.
    */
    void InitGraph(void);

    //! Dispatch event to specified state machine to process the event
    /*!
        \return true if specified state machine was found and thus event was dispatched to
        the state machine; false otherwise.

        Note that returning true does not necessarily indicate the event was processed
        successfully (e.g., event was ignored due to lower severity, no transition
        occurred due to invalid state transitions, etc).
    */
    bool DispatchEvent(Event & event,
                       State::StateMachineType type,
                       const std::string & target = "");

    //! Default constructor should not be used
    GCM(void);

public:
    //! Constructor
    GCM(const std::string & coordinatorName, const std::string & componentName);

    //! Destructor
    ~GCM();

    //
    // Interface Management
    //
    //! Add interface
    bool AddInterface(const std::string & name, InterfaceType type);

    //! Find interface
    bool FindInterface(const std::string & name, InterfaceType type) const;

    //! Remove interface
    bool RemoveInterface(const std::string & name, InterfaceType type);

    //
    // Graph Accessors
    //
    //! Find vertex
    bool FindVertex(const std::string & name, State::StateMachineType type) const;

    //! Find edge
    bool FindEdge(const std::string & vertexNameFrom, State::StateMachineType vertexTypeFrom,
                  const std::string & vertexNameTo, State::StateMachineType vertexTypeTo) const;

    //
    // Event Processing Modules
    //
    // FIXME
    //! Enqueue request to process event
    // 1. introduce MPSC queue
    // 2. determine threading model (GCM on its own? or rely on framework's model)
    // 3. process event

    //! Returns component state with given view
    /*!
        \param view argument specifying view (of type ViewType)
    */
    State::StateType GetComponentState(ViewType view) const;

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

    //! Human readable output of this class
    void ToStream(std::ostream & os = std::cout, ExportFormatType format = EXPORT_HUMAN_READABLE) const;

    //! Export graph to GraphViz format file (.dot)
    /*!
        \param fileName Output file name without extension.  Extension (.dot) is added to file name.
               If file already exists, it is overwritten.
        \return true if success; false otherwise
        \sa http://www.graphviz.org/doc/info/attrs.html
        \sa http://www.graphviz.org/doc/info/shapes.html#html
    */
    bool ExportToGraphViz(const std::string & fileName) const;

    //! Returns node color code for different state in RRGGBB hex format
    static std::string GetColorCode(State::StateType state);

    /*!
        \addtogroup Reserved names used by the GCM
        @{
    */
    //! Reserved name of state machine for framework state
    static std::string NameOfFrameworkStateMachine;
    //! Reserved name of state machine for application state
    static std::string NameOfApplicationStateMachine;
    //! Prefix of name of state machine for service state
    static std::string PrefixOfServiceStateMachine;
    /*! @} */

    //! Given provided interface name, returns name of state machine for service states
    static std::string NameOfServiceStateMachine(const std::string & nameOfProvidedInterface);
};

inline std::ostream & operator << (std::ostream & os, const GCM & gcm) {
    gcm.ToStream(os);
    return os;
}

};

#endif // _gcm_h
