//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2014
// Last revision: May 21, 2016
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

    Graph[s_A].Name = "s_A";
    Graph[s_A].StateType = GCM::STATE_APPLICATION;
    Graph[s_A].SM = new StateMachine(ComponentName);

    Graph[s_F].Name = "s_F";
    Graph[s_F].StateType = GCM::STATE_FRAMEWORK;
    Graph[s_F].SM = new StateMachine(ComponentName);

    // Add edges between vertices
    EdgeDescriptor e;
    bool added;
    // boost::tie(e, added) = boost::add_edge(s_A, s_ext, EdgeProperty(EDGE_DEPENDENCY), Graph);
    // SCASSERT(added);

    // TEMP Following nodes and edges are added only for testing purpose
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

    ADD_VERTEX(s_R1, "required1", GCM::STATE_REQUIRED);
    ADD_VERTEX(s_R2, "required2", GCM::STATE_REQUIRED);
    ADD_VERTEX(s_R3, "required3", GCM::STATE_REQUIRED);
    ADD_VERTEX(s_R4, "required4", GCM::STATE_REQUIRED);

    ADD_VERTEX(s_P1, "provided1", GCM::STATE_PROVIDED);
    ADD_VERTEX(s_P2, "provided2", GCM::STATE_PROVIDED);
    ADD_VERTEX(s_P3, "provided3", GCM::STATE_PROVIDED);

    ADD_VERTEX(s_P1_derived, "~provided1", GCM::STATE_SERVICE);
    ADD_VERTEX(s_P2_derived, "~provided2", GCM::STATE_SERVICE);
    ADD_VERTEX(s_P3_derived, "~provided3", GCM::STATE_SERVICE);

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
}

void GCM::ToStream(std::ostream & os, ExportFormatType format) const
{
    if (format == EXPORT_HUMAN_READABLE) {
        VertexIter itVertex, itVertexEnd;

        // Print list of vertices
        boost::tie(itVertex, itVertexEnd) = vertices(Graph);
        for (; itVertex != itVertexEnd; ++itVertex) {
            os << *itVertex << ": " << Graph[*itVertex].Name << " (" << GetStateTypeString(Graph[*itVertex].StateType)
                << "), State: " << State::GetStringState(Graph[*itVertex].SM->GetCurrentState()) << std::endl;
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
                os << boost::target(*itEdgeOut, Graph) << std::endl;
            }

            // Incoming edges
            InEdgeIter itEdgeIn, itEdgeInEnd;
            boost::tie(itEdgeIn, itEdgeInEnd) = boost::in_edges(*itVertex, Graph);
            for (; itEdgeIn != itEdgeInEnd; ++itEdgeIn) {
                //os << " <--( FIXME )-- " << boost::source(*itEdgeIn, Graph) << "\t";
                // Dependency
                if (Graph[*itEdgeIn].EdgeType == EDGE_DEPENDENCY) {
                    os << "\t<...depends on... ";
                }
                // Error propagation
                else {
                    os << "\t<--propagated from--- ";
                }
                os << boost::source(*itEdgeIn, Graph) << std::endl;
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
           << "node [shape=record];" << std::endl
           << "edge [style=solid splines=curved];" << std::endl
           << "rankdir=LR;" << std::endl;

        std::stringstream ss_required, ss_provided, ss_provided_service, ss_others;
        size_t n_required = 0, n_provided = 0, n_provided_service = 0;

        std::vector<size_t> vecRequired, vecProvided, vecProvidedService;

        ss_required << "required [label=\"";
        ss_provided << "provided [label=\"";
        ss_provided_service << "provided_service [label=\"";

        VertexIter itVertex, itVertexEnd;
        boost::tie(itVertex, itVertexEnd) = vertices(Graph);
        for (; itVertex != itVertexEnd; ++itVertex) {
            switch (Graph[*itVertex].StateType) {
            case STATE_REQUIRED:
                if (n_required > 0)
                    ss_required << "|";
                ss_required << "<s" << n_required++ << "> " << Graph[*itVertex].Name;
                vecRequired.push_back(*itVertex);
                break;

            case STATE_PROVIDED:
                if (n_provided > 0)
                    ss_provided << "|";
                ss_provided << "<s" << n_provided++ << "> " << Graph[*itVertex].Name;
                vecProvided.push_back(*itVertex);
                break;

            case STATE_SERVICE:
                if (n_provided_service > 0)
                    ss_provided_service << "|";
                ss_provided_service << "<s" << n_provided_service++ << "> " << Graph[*itVertex].Name;
                vecProvidedService.push_back(*itVertex);
                break;

            case STATE_FRAMEWORK:
            case STATE_APPLICATION:
                ss_others << Graph[*itVertex].Name << " [label=<s<font point-size=10>F</font>>, fillcolor=yellow, style=\"rounded,filled\", shape=rounded];" << std::endl;
                break;

            default:
                ;
            }
            // TODO add node color

            // os << *itVertex << ": " << Graph[*itVertex].Name << " (" << GetStateTypeString(Graph[*itVertex].StateType)
                // << "), State: " << State::GetStringState(Graph[*itVertex].SM->GetCurrentState()) << std::endl;
        }
        ss_required << "\"];";
        ss_provided << "\"];";
        //ss_provided_service << "\", style=filled, fillcolor=gray];";
        ss_provided_service << "\", style=dashed];";

        // FIXME check if n_required > 0 (??)
        os << ss_required.str() << std::endl;
        os << ss_provided.str() << std::endl;
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
                edges << "required:s" << i << " -> ";
                // Look for target vertex in provided interface state vertices using its index
                const size_t idx = boost::target(*itEdgeOut, Graph);
                for (size_t j = 0; j < vecProvided.size(); ++j) {
                    if (vecProvided[j] == idx) {
                        edges << "provided:s" << j << " " << arrowStyle << ";" << std::endl;
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
                if (Graph[idx].StateType == STATE_PROVIDED) {
                    for (size_t j = 0; j < vecProvided.size(); ++j) {
                        if (vecProvided[j] == idx) {
                            edges << "provided:s" << j;
                            break;
                        }
                    }
                } else {
                    edges << Graph[idx].Name;
                }
                edges << " -> " << "provided_service:s" << i << " " << arrowStyle << ";" << std::endl;
            }
        }

        // export edges
        os << edges.str() << std::endl;

        os << "}" << std::endl;
    }
}

std::string GCM::GetStateTypeString(StateTypes type)
{
    switch (type) {
    case STATE_INVALID:     return "INVALID_STATE";
    case STATE_FRAMEWORK:   return "FRAMEWORK_STATE";
    case STATE_APPLICATION: return "APPLICATION_STATE";
    case STATE_REQUIRED:    return "REQUIRED_STATE";
    case STATE_PROVIDED:    return "PROVIDED_STATE";
    case STATE_SERVICE:     return "SERVICE_STATE";
    }
}

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
