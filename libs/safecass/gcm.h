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

    //! Typedef of graph export format
    typedef enum {
        EXPORT_HUMAN_READABLE,
        EXPORT_GRAPHVIZ
    } ExportFormatType;

    //! Typedef of container for state machines representing interface states
    /*!
        Key: interface name, value: statemachine instance
    */
    //typedef std::map<std::string, StateMachine *> InterfaceStateMachinesType;

protected:
    typedef enum {
        STATE_INVALID,
        STATE_FRAMEWORK,   // Framework state
        STATE_APPLICATION, // Application state
        STATE_REQUIRED,    // Required interface state
        STATE_PROVIDED,    // Provided interface state
        STATE_SERVICE,     // Service state for provided interface
    } StateTypes;

    typedef enum {
        EDGE_DEPENDENCY, // Edge representing service state dependency
        EDGE_ERROR_PROP  // Edge representing error propagation
    } EdgeTypes;

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
    struct VertexProperty {
        std::string  Name;
        StateTypes   StateType;
        StateMachine * SM;
        VertexProperty(void) : Name(NONAME), StateType(STATE_INVALID), SM(0) {}
    };

    struct EdgeProperty {
        EdgeTypes EdgeType;
        EdgeProperty(EdgeTypes type): EdgeType(type) {}
        // TODO boolean flag can be added to seletively enable and disable state dependency
    };

    //typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                                  VertexProperty, EdgeProperty> GraphType;

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

    //! Initialize graph
    /*!
        Graph maintains three vertices by default:

        - s_ext: Extended component state (derived state)
        - s_F  : Framework state (state)
        - s_A  : Application state (state)
    */
    void InitGraph(void);

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

    //! Human readable output of this class
    void ToStream(std::ostream & os = std::cout, ExportFormatType format = EXPORT_HUMAN_READABLE) const;

    //! Export graph to GraphViz format file (.dot)
    /*!
        \param fileName Output file name without extension.  Extension (.dot) is added to file name.
               If file already exists, it is overwritten.
        \return true if success; false otherwise
    */
    bool ExportToGraphViz(const std::string & fileName) const;

    static std::string GetStateTypeString(StateTypes type);

};

inline std::ostream & operator << (std::ostream & os, const GCM & gcm) {
    gcm.ToStream(os);
    return os;
}

};

#endif // _gcm_h
