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
    GCM::VertexDescriptor s_ext = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_A   = boost::add_vertex(Graph);
    GCM::VertexDescriptor s_F   = boost::add_vertex(Graph);

    Graph[s_ext].Name = "s_ext";
    Graph[s_ext].StateType = GCM::STATE_EXTENDED;
    Graph[s_ext].SM = new StateMachine(ComponentName);

    Graph[s_A].Name = "s_A";
    Graph[s_A].StateType = GCM::STATE_APPLICATION;
    Graph[s_A].SM = new StateMachine(ComponentName);

    Graph[s_F].Name = "s_F";
    Graph[s_F].StateType = GCM::STATE_FRAMEWORK;
    Graph[s_F].SM = new StateMachine(ComponentName);

    // Add edges between vertices
    EdgeDescriptor e;
    bool added;
    boost::tie(e, added) = boost::add_edge(s_A, s_ext, EdgeProperty(EDGE_DEPENDENCY), Graph);
    SCASSERT(added);
    boost::tie(e, added) = boost::add_edge(s_F, s_ext, EdgeProperty(EDGE_DEPENDENCY), Graph);
    SCASSERT(added);
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
        /*
template < typename VertexAndEdgeListGraph, typename VertexPropertyWriter,
           typename EdgePropertyWriter, typename GraphPropertyWriter >
void
write_graphviz(std::ostream& out, const VertexAndEdgeListGraph& g,
               VertexPropertyWriter vpw, EdgePropertyWriter epw,
               GraphPropertyWriter gpw);
        */
        boost::write_graphviz(os, Graph);//, make_label_writer(name));
    }
}

std::string GCM::GetStateTypeString(StateTypes type)
{
    switch (type) {
    case STATE_INVALID:     return "INVALID_STATE";
    case STATE_EXTENDED:    return "EXTENDED_STATE";
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
