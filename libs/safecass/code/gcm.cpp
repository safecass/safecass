//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: May 30, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
//#include "config.h"
//#include "common/utils.h"
#include "safecass/gcm.h"

#include <fstream>
#include <boost/graph/graphviz.hpp>

#include "safecass/statemachine.h"

using namespace SC;

//! Reserved name of state machine for component state
std::string GCM::NameOfFrameworkStateMachine = "scFramework";

//! Reserved name of state machine for application state
std::string GCM::NameOfApplicationStateMachine = "scApplication";

//! Prefix of name of state machine for service state
std::string GCM::PrefixOfServiceStateMachine = "@";

GCM::GCM(void): CoordinatorName(NONAME), ComponentName(NONAME)
{}

GCM::GCM(const std::string & coordinatorName, const std::string & componentName)
    : CoordinatorName(coordinatorName), ComponentName(componentName)
{
    // Initialize graph
    InitGraph();
}

GCM::~GCM(void)
{
    VertexIter it, itEnd;
    boost::tie(it, itEnd) = boost::vertices(Graph);
    for (; it != itEnd; ++it)
        delete Graph[*it].SM;
}

void GCM::InitGraph(void)
{
    // If graph object already exists, clean up
    SCLOG_WARNING << "Removing all existing state machines, "
                    << "Coordinator: \"" << CoordinatorName << "\", "
                    << "Component: \"" << ComponentName << "\"" << std::endl;
    // Remove all vertices
    Graph.clear();
    SCASSERT(boost::num_vertices(Graph) == 0);
    SCASSERT(boost::num_edges(Graph) == 0);

    // Add vertices
    VertexFramework = boost::add_vertex(Graph);
    VertexApplication = boost::add_vertex(Graph);

    Graph[VertexFramework].Name = GCM::NameOfFrameworkStateMachine;
    Graph[VertexFramework].StateType = State::STATEMACHINE_FRAMEWORK;
    Graph[VertexFramework].SM = new StateMachine(ComponentName);

    Graph[VertexApplication].Name = GCM::NameOfApplicationStateMachine;
    Graph[VertexApplication].StateType = State::STATEMACHINE_APP;
    Graph[VertexApplication].SM = new StateMachine(ComponentName);

    SCASSERT(boost::num_vertices(Graph) == 2);
    SCASSERT(boost::num_edges(Graph) == 0);

    // TEMP Following nodes and edges are added only for testing purpose
#if 0
    EdgeDescriptor e;
    bool added;
    GCM::VertexDescriptor s_R1 = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_R2 = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_R3 = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_R4 = boost::add_vertex(Graph);

    GCM::VertexDescriptor s_P1 = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_P2 = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_P3 = boost::add_vertex(Graph);

    GCM::VertexDescriptor s_P1_derived = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_P2_derived = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_P3_derived = boost::add_vertex(Graph);

#define ADD_VERTEX(_v, _name, _type)\
    Graph[_v].Name = _name;\
    Graph[_v].StateType = _type;\
    Graph[_v].SM = new StateMachine(ComponentName);

    ADD_VERTEX(s_R1, "required1", State::STATEMACHINE_REQUIRED);
    ADD_VERTEX(s_R2, "required2", State::STATEMACHINE_REQUIRED);
    ADD_VERTEX(s_R3, "required3", State::STATEMACHINE_REQUIRED);
    ADD_VERTEX(s_R4, "required4", State::STATEMACHINE_REQUIRED);

    ADD_VERTEX(s_P1, "provided1", State::STATEMACHINE_PROVIDED);
    ADD_VERTEX(s_P2, "provided2", State::STATEMACHINE_PROVIDED);
    ADD_VERTEX(s_P3, "provided3", State::STATEMACHINE_PROVIDED);

    ADD_VERTEX(s_P1_derived, "~provided1", State::STATEMACHINE_SERVICE);
    ADD_VERTEX(s_P2_derived, "~provided2", State::STATEMACHINE_SERVICE);
    ADD_VERTEX(s_P3_derived, "~provided3", State::STATEMACHINE_SERVICE);

    boost::add_edge(s_P1, s_P1_derived, EdgeProperty(), Graph);
    boost::add_edge(s_P2, s_P2_derived, EdgeProperty(), Graph);
    boost::add_edge(s_P3, s_P3_derived, EdgeProperty(), Graph);
    boost::add_edge(VertexFramework, s_P1_derived, EdgeProperty(), Graph);
    boost::add_edge(VertexFramework, s_P2_derived, EdgeProperty(), Graph);
    boost::add_edge(VertexApplication, s_P2_derived, EdgeProperty(), Graph);
    boost::add_edge(VertexApplication, s_P3_derived, EdgeProperty(), Graph);

    boost::add_edge(s_R1, s_P1, EdgeProperty(), Graph);
    boost::add_edge(s_R1, s_P2, EdgeProperty(), Graph);
    boost::add_edge(s_R2, s_P3, EdgeProperty(), Graph);
    boost::add_edge(s_R4, s_P2, EdgeProperty(), Graph);

    // Change state
    Event eN2W("evt_NORMAL_TO_WARNING", 10, Event::TRANSITION_N2W);
    Event eN2E("evt_NORMAL_TO_ERROR", 10, Event::TRANSITION_N2E);

    Graph[s_R2].SM->ProcessEvent(eN2W);
    Graph[s_P3].SM->ProcessEvent(eN2E);
    Graph[s_P3_derived].SM->ProcessEvent(eN2E);
#endif
}

void GCM::ToStream(std::ostream & os, ExportFormatType format) const
{
    if (format == EXPORT_HUMAN_READABLE) {
        VertexIter itVertex, itVertexEnd;

        // Print list of vertices
        boost::tie(itVertex, itVertexEnd) = vertices(Graph);
        for (; itVertex != itVertexEnd; ++itVertex) {
            os << *itVertex << ": " << Graph[*itVertex].Name
               << " (" << State::GetString(Graph[*itVertex].StateType)
               << "), State: " << State::GetString(Graph[*itVertex].SM->GetCurrentState())
               << std::endl;
        }

        os << std::endl;

        // Print list of edges
        boost::tie(itVertex, itVertexEnd) = vertices(Graph);
        for (; itVertex != itVertexEnd; ++itVertex) {
            os << *itVertex << ": " << Graph[*itVertex].Name << std::endl;

            // Outgoing edges
            OutEdgeIter itEdgeOut, itEdgeOutEnd;
            boost::tie(itEdgeOut, itEdgeOutEnd) = boost::out_edges(*itVertex, Graph);
            for (; itEdgeOut != itEdgeOutEnd; ++itEdgeOut) {
                os << "\t--- affects --> ";
                os << Graph[boost::target(*itEdgeOut, Graph)].Name << std::endl;
            }

            // Incoming edges
            InEdgeIter itEdgeIn, itEdgeInEnd;
            boost::tie(itEdgeIn, itEdgeInEnd) = boost::in_edges(*itVertex, Graph);
            for (; itEdgeIn != itEdgeInEnd; ++itEdgeIn) {
                // Dependency
                os << "\t--- depends on ---> ";
                os << Graph[boost::source(*itEdgeIn, Graph)].Name << std::endl;
            }
        }
    }
    else if (format == EXPORT_GRAPHVIZ) {
#if 0
        boost::write_graphviz(os, Graph);
        boost::write_graphviz(os, Graph,
                              GCM::VertexPropertyWriter(Graph),
                              GCM::EdgePropertyWriter(Graph),
                              GCM::GraphPropertyWriter());
#endif
        // Graph
        os << "digraph " << ComponentName << " {" << std::endl
           << "node [shape=plaintext];" << std::endl
           << "edge [style=solid splines=curved];" << std::endl
           << "graph [rankdir=LR];" << std::endl;

        std::stringstream ss_required, ss_provided, ss_provided_service, ss_others;
        size_t n_required = 0, n_provided = 0, n_provided_service = 0;

        std::vector<size_t> vecRequired, vecProvided, vecProvidedService;

        ss_required << "required [label=<"
                    << "<table border=\"0\" cellborder=\"1\" cellpadding=\"3\" cellspacing=\"2\">";
        ss_provided << "provided [label=<"
                    << "<table border=\"0\" cellborder=\"1\" cellpadding=\"3\" cellspacing=\"2\">";
        ss_provided_service << "provided_service [label=<"
                    << "<table border=\"1\" bgcolor=\"#cecece\" cellborder=\"1\" cellpadding=\"3\" cellspacing=\"2\">";

        VertexIter itVertex, itVertexEnd;
        boost::tie(itVertex, itVertexEnd) = vertices(Graph);
        for (; itVertex != itVertexEnd; ++itVertex) {
            switch (Graph[*itVertex].StateType) {
            case State::STATEMACHINE_REQUIRED:
                ss_required << "<tr><td port=\"s" << n_required++ << "\" bgcolor=\""
                            << GetColorCode(Graph[*itVertex].SM->GetCurrentState())
                            << "\">" << Graph[*itVertex].Name << "</td></tr>";
                vecRequired.push_back(*itVertex);
                break;

            case State::STATEMACHINE_PROVIDED:
                ss_provided << "<tr><td port=\"s" << n_provided++ << "\" bgcolor=\""
                            << GetColorCode(Graph[*itVertex].SM->GetCurrentState())
                            << "\">" << Graph[*itVertex].Name << "</td></tr>";
                vecProvided.push_back(*itVertex);
                break;

            case State::STATEMACHINE_SERVICE:
                ss_provided_service << "<tr><td port=\"s" << n_provided_service++ << "\" bgcolor=\""
                                    << GetColorCode(Graph[*itVertex].SM->GetCurrentState())
                                    << "\">" << Graph[*itVertex].Name << "</td></tr>";
                vecProvidedService.push_back(*itVertex);
                break;

            case State::STATEMACHINE_FRAMEWORK:
            case State::STATEMACHINE_APP:
                ss_others << Graph[*itVertex].Name << " [fillcolor=\""
                          << GetColorCode(Graph[*itVertex].SM->GetCurrentState())
                          << "\", style=\"rounded,filled\", shape=rounded];" << std::endl;
                break;

            default:
                ;
            }
            // TODO add node color

            // os << *itVertex << ": " << Graph[*itVertex].Name << " (" << GetStateTypeString(Graph[*itVertex].StateType)
                // << "), State: " << State::GetStringState(Graph[*itVertex].SM->GetCurrentState()) << std::endl;
        }
        ss_required << "</table>>];";
        ss_provided << "</table>>];";
        //ss_provided_service << "\", style=filled, fillcolor=gray];";
        ss_provided_service << "</table>>];";

        if (!vecRequired.empty())
            os << ss_required.str() << std::endl;
        if (!vecProvided.empty())
            os << ss_provided.str() << std::endl;
        if (!vecProvidedService.empty())
            os << ss_provided_service.str() << std::endl;
        os << ss_others.str() << std::endl;
        // for testing
        //os << "title [label=<Birth of George Washington<BR /> <FONT POINT-SIZE=\"10\">See also: American Revolution</FONT>>];" << std::endl;

        // Print list of edges
        std::stringstream edges;

        const std::string arrowStyle = "[arrowhead=vee]";

        // 1. Outgoing edges from required interface state to provided interface state
        for (size_t i = 0; i < vecRequired.size(); ++i) {
            // Outgoing edges from source of which index is vecRequired[i] in Graph
            OutEdgeIter itEdgeOut, itEdgeOutEnd;
            boost::tie(itEdgeOut, itEdgeOutEnd) = boost::out_edges(vecRequired[i], Graph);
            for (; itEdgeOut != itEdgeOutEnd; ++itEdgeOut) {
                // Target
                edges << "required:s" << i << ":e -> ";
                // Look for target vertex in provided interface state vertices using its index
                const size_t idx = boost::target(*itEdgeOut, Graph);
                for (size_t j = 0; j < vecProvided.size(); ++j) {
                    if (vecProvided[j] == idx) {
                        edges << "provided:s" << j << ":w " << arrowStyle << ";" << std::endl;
                        break;
                    }
                }
            }
        }

        // 2. Incoming edges to provided interface service state
        for (size_t i = 0; i < vecProvidedService.size(); ++i) {
            InEdgeIter itEdgeIn, itEdgeInEnd;
            boost::tie(itEdgeIn, itEdgeInEnd) = boost::in_edges(vecProvidedService[i], Graph);
            for (; itEdgeIn != itEdgeInEnd; ++itEdgeIn) {
                // Get source vertex
                const size_t idx = boost::source(*itEdgeIn, Graph);
                // Check if source is provided interface. If yes, search for vecProvided
                // for its index.  If source is either s_A or s_F, edge needs to be defined
                // differently.
                if (Graph[idx].StateType == State::STATEMACHINE_PROVIDED) {
                    for (size_t j = 0; j < vecProvided.size(); ++j) {
                        if (vecProvided[j] == idx) {
                            edges << "provided:s" << j << ":e";
                            break;
                        }
                    }
                } else {
                    edges << Graph[idx].Name << ":e";
                }
                edges << " -> " << "provided_service:s" << i << ":w " << arrowStyle << ";" << std::endl;
            }
        }

        // export edges
        os << edges.str() << std::endl;

        os << "}" << std::endl;
    }
}

// NOTE: command line export and layout
// $ ccomps -x graphviz-test.dot | dot | gvpack -array | neato -Tpng -n2 -o graph.png
bool GCM::ExportToGraphViz(const std::string & fileName) const
{
    std::string fullFileName(fileName);
    fullFileName += ".dot";

    std::ofstream dotFile;
    dotFile.open(fullFileName.c_str());
    if (!dotFile.is_open()) {
        SCLOG_ERROR << "Unable to open file for graph export: " << fullFileName << std::endl;
        return false;
    }

    ToStream(dotFile, EXPORT_GRAPHVIZ);
    dotFile.close();

    return true;
}

std::string GCM::GetColorCode(State::StateType state)
{
    switch (state) {
    case State::NORMAL:  return "#ffffff"; // white
    case State::WARNING: return "#ffcc00"; // tangerine
    case State::ERROR:   return "#ff0000"; // red
    case State::FAILURE: return "#aa0000"; // dark red
    case State::INVALID: return "#000000"; // black
    }
}

#define PRINT_INTERFACE(_name, _type)\
    (_type == GCM::PROVIDED_INTERFACE ? "provided" : "required") << " interface \"" << _name << "\""

bool GCM::AddInterface(const std::string & name, InterfaceType type)
{
    if (FindInterface(name, type)) {
        SCLOG_ERROR << "Failed to add " << PRINT_INTERFACE(name, type)
                    << ": already exists" << std::endl;
        return false;
    }

    // Create vertex for new interface
    GCM::VertexDescriptor v = boost::add_vertex(Graph);
    Graph[v].Name = name;
    Graph[v].StateType =
        (type == GCM::PROVIDED_INTERFACE ? State::STATEMACHINE_PROVIDED : State::STATEMACHINE_REQUIRED);
    Graph[v].SM = new StateMachine(ComponentName);

    SCLOG_INFO << "Added " << PRINT_INTERFACE(name, type) << std::endl;

    // In case of provided interface
    if (type == GCM::PROVIDED_INTERFACE) {
        // Create another vertex for service state in case of provided interface
        GCM::VertexDescriptor v_service = boost::add_vertex(Graph);
        Graph[v_service].Name = NameOfServiceStateMachine(name);
        Graph[v_service].StateType = State::STATEMACHINE_SERVICE;
        Graph[v_service].SM = new StateMachine(ComponentName);

        // Add edge from provided interface state to service state vertex associated with
        // the provided interface
        GCM::EdgeDescriptor e;
        bool success = false;
        boost:tie(e, success) = boost::add_edge(v, v_service, EdgeProperty(), Graph);
        if (!success) {
            SCLOG_ERROR << "Failed to add edge from " << PRINT_INTERFACE(name, type)
                        << " to its service state vertex" << std::endl;
            return false;
        }

        SCLOG_INFO << "Added service state for " << PRINT_INTERFACE(name, type) << std::endl;
    }

    return true;
}

bool GCM::FindInterface(const std::string & name, InterfaceType type) const
{
    State::StateMachineType smType =
        (type == GCM::PROVIDED_INTERFACE ? State::STATEMACHINE_PROVIDED :
                                           State::STATEMACHINE_REQUIRED);

    return FindVertex(name, smType);
}

bool GCM::FindVertex(const std::string & name, State::StateMachineType type) const
{
    VertexIter it, itEnd;
    boost::tie(it, itEnd) = vertices(Graph);
    for (; it != itEnd; ++it) {
        if (Graph[*it].StateType != type)
            continue;
        if (name.compare(Graph[*it].Name) == 0)
            return true;
    }

    return false;
}

bool GCM::RemoveInterface(const std::string & name, InterfaceType type)
{
    const State::StateMachineType smType =
        (type == GCM::PROVIDED_INTERFACE ? State::STATEMACHINE_PROVIDED :
                                           State::STATEMACHINE_REQUIRED);

    bool found = false;
    VertexIter it, itEnd;

    boost::tie(it, itEnd) = vertices(Graph);
    for (; it != itEnd; ++it) {
        if (Graph[*it].StateType != smType)
            continue;
        if (name.compare(Graph[*it].Name) == 0) {
            // Found interface
            found = true;

            VertexDescriptor v = *it;
            // Remove all edges to and from the vertex before removing the vertex.
            // This invalidates descriptor and iterator.
            boost::clear_vertex(v, Graph);
            // Delete state machine object before removal
            delete Graph[v].SM;
            // Remove vertex from the vertex set of the graph with assumption that there
            // are no edges to or from this vertex.
            boost::remove_vertex(v, Graph);

            SCLOG_INFO << "Removed " << PRINT_INTERFACE(name, type) << std::endl;

            break;
        }
    }

    // In case of provided interface, remove service state as well
    if (type == PROVIDED_INTERFACE && found) {
        bool removedServiceState = false;
        boost::tie(it, itEnd) = vertices(Graph);
        for (; it != itEnd; ++it) {
            // Look for state machine for service state
            if (Graph[*it].StateType != State::STATEMACHINE_SERVICE)
                continue;
            if (Graph[*it].Name.compare(NameOfServiceStateMachine(name)) == 0) {
                delete Graph[*it].SM;
                boost::remove_vertex(*it, Graph);
                removedServiceState = true;

                SCLOG_INFO << "Removed service state for " << PRINT_INTERFACE(name, type) << std::endl;
                break;
            }
        }
        SCASSERT(removedServiceState);
    }

    if (!found) {
        SCLOG_WARNING << "Failed to remove " << PRINT_INTERFACE(name, type)
                      << ": no such interface is found" << std::endl;
        return false;
    }

    return true;
}

bool GCM::FindEdge(const std::string & vertexNameFrom, State::StateMachineType vertexTypeFrom,
                   const std::string & vertexNameTo, State::StateMachineType vertexTypeTo) const
{
    VertexIter it, itEnd;
    boost::tie(it, itEnd) = boost::vertices(Graph);

    GCM::VertexDescriptor v;
    for (; it != itEnd; ++it) {
        v = *it;
        // Look for vertex where edge is coming out of
        if ((Graph[v].Name.compare(vertexNameFrom) == 0) && (Graph[v].StateType == vertexTypeFrom)) {
            // Check outgoing edges
            OutEdgeIter itEdgeOut, itEdgeOutEnd;
            boost::tie(itEdgeOut, itEdgeOutEnd) = boost::out_edges(v, Graph);
            for (; itEdgeOut != itEdgeOutEnd; ++itEdgeOut) {
                if (vertexNameTo.compare(Graph[boost::target(*itEdgeOut, Graph)].Name) == 0) {
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

std::string GCM::NameOfServiceStateMachine(const std::string & nameOfProvidedInterface)
{
    std::string name = GCM::PrefixOfServiceStateMachine;
    name += nameOfProvidedInterface;

    return name;
}


State::StateType GCM::GetComponentState(ViewType view) const
{
    State::StateType stateFramework = Graph[VertexFramework].SM->GetCurrentState();

    if (view == GCM::FRAMEWORK_VIEW)
        return stateFramework;

    State stateApplication;
    GCM::VertexDescriptor v;

    BGL_FORALL_VERTICES(v, Graph, GCM::GraphType) {
        if (v == VertexFramework)
            continue;
        stateApplication *= Graph[v].SM->GetState();
    }

    if (view == GCM::APPLICATION_VIEW)
        return stateApplication.GetState();

    return (stateApplication * stateFramework).GetState();
}

//
// FIXME
//
// In case of Provided interface, service state should be also processed
//
// Also, after dependency info is introduced, error propagation should be
// handleded property.
bool GCM::DispatchEvent(Event & event, State::StateMachineType type, const std::string & target)
{
    switch (type) {
    default:
        SCLOG_WARNING << "Invalid state machine type: Event \"" << event.GetName() << "\" was not processed" << std::endl;
        return false;
    case State::STATEMACHINE_FRAMEWORK:
        // ProcessEvent() returns false if invalid transition was detected or event was ignored
        Graph[VertexFramework].SM->ProcessEvent(event);
        break;
    case State::STATEMACHINE_APP:
        Graph[VertexApplication].SM->ProcessEvent(event);
        break;
    case State::STATEMACHINE_PROVIDED:
    case State::STATEMACHINE_REQUIRED:
        {
            bool found = false;
            VertexIter it, itEnd;
            boost::tie(it, itEnd) = boost::vertices(Graph);
            for (; it != itEnd; ++it) {
                VertexDescriptor v = *it;
                if (Graph[v].StateType != type)
                    continue;
                if (Graph[v].Name.compare(target) == 0) {
                    found = true;
                    Graph[v].SM->ProcessEvent(event);
                    break;
                }
            }
            if (!found) {
                SCLOG_ERROR << "No state machine found to process event \"" << event.GetName() << "\": "
                            << "interface name \"" << target << "\" (" << State::GetString(type) << ")" << std::endl;
                return false;
            }
        }
        break;
    }

    return true;
}
