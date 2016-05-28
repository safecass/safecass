//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: May 25, 2016
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
    VertexIter itVertex, itVertexEnd;
    boost::tie(itVertex, itVertexEnd) = vertices(Graph);
    for (; itVertex != itVertexEnd; ++itVertex)
        delete Graph[*itVertex].SM;
}

void GCM::InitGraph(void)
{
    static const size_t NumOfInitialVertices = 3;

    // If graph object already exists, clean up
    if (boost::num_vertices(Graph) > NumOfInitialVertices) {
        SCLOG_WARNING << "Existing GCM graph being reset (Coordinator: \"" << CoordinatorName << "\", "
                      << "Component: \"" << ComponentName << "\")" << std::endl;
        // FIXME Graceful deletion of state machine objects? May need to stop all
        // state machines first to avoid event-handling-while-statemachines-are-
        // destroyed cases.  Potentially, following line may cause crashes

        // FIXME Reset graph (empty out)
    }

    // Add vertices
    GCM::VertexDescriptor s_A   = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_F   = boost::add_vertex(Graph);

    Graph[s_A].Name = "Application";
    Graph[s_A].StateType = State::STATEMACHINE_APP;
    Graph[s_A].SM = new StateMachine(ComponentName);

    Graph[s_F].Name = "Framework";
    Graph[s_F].StateType = State::STATEMACHINE_FRAMEWORK;
    Graph[s_F].SM = new StateMachine(ComponentName);

    // Add edges between vertices
    EdgeDescriptor e;
    bool added;
    // boost::tie(e, added) = boost::add_edge(s_A, s_ext, EdgeProperty(EDGE_DEPENDENCY), Graph);
    // SCASSERT(added);

    // TEMP Following nodes and edges are added only for testing purpose
#if 0
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

    boost::add_edge(s_P1, s_P1_derived, EdgeProperty(EDGE_DEPENDENCY), Graph);
    boost::add_edge(s_P2, s_P2_derived, EdgeProperty(EDGE_DEPENDENCY), Graph);
    boost::add_edge(s_P3, s_P3_derived, EdgeProperty(EDGE_DEPENDENCY), Graph);
    boost::add_edge(s_F, s_P1_derived, EdgeProperty(EDGE_DEPENDENCY), Graph);
    boost::add_edge(s_F, s_P2_derived, EdgeProperty(EDGE_DEPENDENCY), Graph);
    boost::add_edge(s_A, s_P2_derived, EdgeProperty(EDGE_DEPENDENCY), Graph);
    boost::add_edge(s_A, s_P3_derived, EdgeProperty(EDGE_DEPENDENCY), Graph);

    boost::add_edge(s_R1, s_P1, EdgeProperty(EDGE_ERROR_PROP), Graph);
    boost::add_edge(s_R1, s_P2, EdgeProperty(EDGE_ERROR_PROP), Graph);
    boost::add_edge(s_R2, s_P3, EdgeProperty(EDGE_ERROR_PROP), Graph);
    boost::add_edge(s_R4, s_P2, EdgeProperty(EDGE_ERROR_PROP), Graph);

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
                // Dependency
                if (Graph[*itEdgeOut].EdgeType == EDGE_DEPENDENCY) {
                    os << "\t...affects...> ";
                }
                // Error propagation
                else {
                    os << "\t---propagates--> ";
                }
                os << Graph[boost::target(*itEdgeOut, Graph)].Name << std::endl;
            }

            // Incoming edges
            InEdgeIter itEdgeIn, itEdgeInEnd;
            boost::tie(itEdgeIn, itEdgeInEnd) = boost::in_edges(*itVertex, Graph);
            for (; itEdgeIn != itEdgeInEnd; ++itEdgeIn) {
                // Dependency
                if (Graph[*itEdgeIn].EdgeType == EDGE_DEPENDENCY) {
                    os << "\t<...depends on... ";
                }
                // Error propagation
                else {
                    os << "\t<--propagated from--- ";
                }
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

    GCM::VertexDescriptor v = boost::add_vertex(Graph);

    Graph[v].Name = name;
    Graph[v].StateType =
        (type == GCM::PROVIDED_INTERFACE ? State::STATEMACHINE_PROVIDED : State::STATEMACHINE_REQUIRED);
    Graph[v].SM = new StateMachine(ComponentName);

    SCLOG_INFO << "Add " << PRINT_INTERFACE(name, type) << std::endl;

    return true;
}

bool GCM::FindInterface(const std::string & name, InterfaceType type) const
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
    VertexIter it, itEnd;
    boost::tie(it, itEnd) = vertices(Graph);
    for (; it != itEnd; ++it) {
        if (Graph[*it].StateType != type)
            continue;
        if (name.compare(Graph[*it].Name) == 0) {
            VertexDescriptor v = *it;
            // Remove all edges to and from the vertex before removing the vertex.
            // This invalidates descriptor and iterator.
            boost::clear_vertex(v, Graph);
            // Remove vertex from the vertex set of the graph with assumption that there
            // are no edges to or from this vertex.
            boost::remove_vertex(v, Graph);

            SCLOG_INFO << "Removed " << PRINT_INTERFACE(name, type) << std::endl;

            return true;
        }
    }

    SCLOG_WARNING << "Failed to remove " << PRINT_INTERFACE(name, type)
                  << ": no such interface is found" << std::endl;

    return false;
}
