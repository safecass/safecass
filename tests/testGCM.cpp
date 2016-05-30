//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : May 15, 2016
// Last revision: May 30, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "config.h"
#include "safecass/gcm.h"
#include "safecass/state.h"
#include "safecass/event.h"

using namespace SC;

// Google fixture class for unit test
class GCMTest: public GCM, public ::testing::Test
{
public:
    GCMTest(void): GCM("testGCM", "GCMTest") {
    }

protected:
    virtual void SetUp(void) {
    }

    virtual void TearDown(void) {
    }
};

TEST(GCM, Constructor)
{
    const std::string str1("coordinator_name");
    const std::string str2("component_name");

    GCM gcm(str1, str2);
    EXPECT_STREQ(str1.c_str(), gcm.GetCoordinatorName().c_str());
    EXPECT_STREQ(str2.c_str(), gcm.GetComponentName().c_str());
}

TEST_F(GCMTest, InitGraph)
{
    // First call by constructor
    EXPECT_EQ(2, boost::num_vertices(Graph));
    EXPECT_EQ(0, boost::num_edges(Graph));

    // Add random vertex and edge
    GCM::VertexDescriptor v = boost::add_vertex(Graph);
    boost::add_edge(0, v, GCM::EdgeProperty(), Graph);
    boost::add_vertex(Graph);

    EXPECT_EQ(4, boost::num_vertices(Graph));
    EXPECT_EQ(1, boost::num_edges(Graph));

    // Second call by caller
    InitGraph();

    EXPECT_EQ(2, boost::num_vertices(Graph));
    EXPECT_EQ(0, boost::num_edges(Graph));
}

TEST_F(GCMTest, FindVertex)
{
    EXPECT_TRUE(FindVertex(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK));
    EXPECT_TRUE(FindVertex(GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP));

    // incorrect state machine type
    EXPECT_FALSE(FindVertex(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_APP));
    EXPECT_FALSE(FindVertex(GCM::NameOfApplicationStateMachine, State::STATEMACHINE_FRAMEWORK));

    // incorrect name
    EXPECT_FALSE(FindVertex(NONAME, State::STATEMACHINE_FRAMEWORK));
    EXPECT_FALSE(FindVertex(NONAME, State::STATEMACHINE_APP));
}

TEST_F(GCMTest, FindEdge)
{
    EXPECT_TRUE(FindVertex(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK));
    EXPECT_TRUE(FindVertex(GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP));

    EXPECT_FALSE(FindEdge(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK,
                          GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP));

    // manually add edge between s_F and s_A for testing
    EdgeDescriptor e;
    bool success;
    boost::tie(e, success) = boost::add_edge(0, 1, EdgeProperty(), Graph);
    EXPECT_TRUE(success);

    EXPECT_TRUE(FindEdge(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK,
                         GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP));
}

TEST_F(GCMTest, AddInterface)
{
    // Check initial graph
    EXPECT_EQ(2, boost::num_vertices(Graph));
    EXPECT_EQ(0, boost::num_edges(Graph));
    EXPECT_TRUE(FindVertex(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK));
    EXPECT_TRUE(FindVertex(GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP));
    EXPECT_FALSE(FindEdge(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK,
                          GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP));
    EXPECT_FALSE(FindEdge(GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP,
                          GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK));

    // 1. Add provided interface
    const std::string provName = "providedInterfaceName";
    EXPECT_TRUE(AddInterface(provName, GCM::PROVIDED_INTERFACE));

    // Adding provided interface should create:
    // 1) Two vertices for provided interface state and service state for the provided interface
    EXPECT_EQ(2 + 2, boost::num_vertices(Graph));
    EXPECT_EQ(0 + 1, boost::num_edges(Graph));
    // existing vertices
    EXPECT_TRUE(FindVertex(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK));
    EXPECT_TRUE(FindVertex(GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP));
    // new vertices
    EXPECT_TRUE(FindVertex(provName, State::STATEMACHINE_PROVIDED));
    EXPECT_TRUE(FindVertex(GCM::NameOfServiceStateMachine(provName), State::STATEMACHINE_SERVICE));
    // 2) One edge from provided interface state to service state
    EXPECT_TRUE(FindEdge(provName, State::STATEMACHINE_PROVIDED,
                         GCM::NameOfServiceStateMachine(provName), State::STATEMACHINE_SERVICE));

    // 2. Add required interface
    const std::string reqName = "requiredInterfaceName";
    EXPECT_TRUE(AddInterface(reqName, GCM::REQUIRED_INTERFACE));

    // Adding required interface should create:
    // 1) One vertex for required interface state
    EXPECT_EQ(2 + 2 + 1, boost::num_vertices(Graph));
    EXPECT_EQ(0 + 1, boost::num_edges(Graph));
    // existing vertices
    EXPECT_TRUE(FindVertex(GCM::NameOfFrameworkStateMachine, State::STATEMACHINE_FRAMEWORK));
    EXPECT_TRUE(FindVertex(GCM::NameOfApplicationStateMachine, State::STATEMACHINE_APP));
    EXPECT_TRUE(FindVertex(provName, State::STATEMACHINE_PROVIDED));
    EXPECT_TRUE(FindVertex(GCM::NameOfServiceStateMachine(provName), State::STATEMACHINE_SERVICE));
    // new vertices
    EXPECT_TRUE(FindVertex(reqName, State::STATEMACHINE_REQUIRED));
    // existing edges (no new edge)
    EXPECT_TRUE(FindEdge(provName, State::STATEMACHINE_PROVIDED,
                         GCM::NameOfServiceStateMachine(provName), State::STATEMACHINE_SERVICE));
}

TEST_F(GCMTest, FindInterface)
{
    EXPECT_FALSE(FindInterface("prvIntfcName", GCM::PROVIDED_INTERFACE));
    EXPECT_TRUE(AddInterface("prvIntfcName", GCM::PROVIDED_INTERFACE));
    EXPECT_TRUE(FindInterface("prvIntfcName", GCM::PROVIDED_INTERFACE));

    EXPECT_FALSE(FindInterface("reqIntfcName", GCM::REQUIRED_INTERFACE));
    EXPECT_TRUE(AddInterface("reqIntfcName", GCM::REQUIRED_INTERFACE));
    EXPECT_TRUE(FindInterface("reqIntfcName", GCM::REQUIRED_INTERFACE));
}

// Removing interface should undo what AddInterface() did
TEST_F(GCMTest, RemoveInterface)
{
    const std::string provName("prvIntfcName");
    const std::string reqName("prvIntfcName");

    // 1. Remove provided interface
    EXPECT_TRUE(AddInterface(provName, GCM::PROVIDED_INTERFACE));
    EXPECT_TRUE(FindInterface(provName, GCM::PROVIDED_INTERFACE));
    EXPECT_TRUE(RemoveInterface(provName, GCM::PROVIDED_INTERFACE));
    EXPECT_FALSE(FindInterface(provName, GCM::PROVIDED_INTERFACE));

    EXPECT_FALSE(FindVertex(provName, State::STATEMACHINE_PROVIDED));
    EXPECT_FALSE(FindVertex(GCM::NameOfServiceStateMachine(provName), State::STATEMACHINE_SERVICE));
    EXPECT_FALSE(FindEdge(provName, State::STATEMACHINE_PROVIDED,
                          GCM::NameOfServiceStateMachine(provName), State::STATEMACHINE_SERVICE));

    // 2. Remove required interface
    EXPECT_TRUE(AddInterface(reqName, GCM::REQUIRED_INTERFACE));
    EXPECT_TRUE(FindInterface(reqName, GCM::REQUIRED_INTERFACE));
    EXPECT_TRUE(RemoveInterface(reqName, GCM::REQUIRED_INTERFACE));
    EXPECT_FALSE(FindInterface(reqName, GCM::REQUIRED_INTERFACE));

    EXPECT_FALSE(FindVertex(reqName, State::STATEMACHINE_REQUIRED));
}

TEST_F(GCMTest, GetComponentState)
{
    // Add some interfaces for testing
    EXPECT_TRUE(AddInterface("prv1", GCM::PROVIDED_INTERFACE));
    EXPECT_TRUE(AddInterface("prv2", GCM::PROVIDED_INTERFACE));
    EXPECT_TRUE(AddInterface("req1", GCM::REQUIRED_INTERFACE));
    EXPECT_TRUE(AddInterface("req2", GCM::REQUIRED_INTERFACE));
    EXPECT_TRUE(AddInterface("req3", GCM::REQUIRED_INTERFACE));

    Event eN2W("evt_NORMAL_TO_WARNING", 10, Event::TRANSITION_N2W);
    Event eW2N("evt_WARNING_TO_NORMAL", 10, Event::TRANSITION_W2N);
    Event eN2E("evt_NORMAL_TO_ERROR", 10, Event::TRANSITION_N2E);
    Event eE2N("evt_ERROR_TO_NORMAL", 10, Event::TRANSITION_E2N);

    // 1. NORMAL to WARNING
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // Framework state to WARNING
    EXPECT_TRUE(DispatchEvent(eN2W, State::STATEMACHINE_FRAMEWORK));
    EXPECT_EQ(State::WARNING, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::WARNING, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));
    // Back to NORMAL
    EXPECT_TRUE(DispatchEvent(eW2N, State::STATEMACHINE_FRAMEWORK));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // Application state to WARNING
    EXPECT_TRUE(DispatchEvent(eN2W, State::STATEMACHINE_APP));
    EXPECT_EQ(State::WARNING, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::WARNING, GetComponentState(GCM::APPLICATION_VIEW));
    // Back to NORMAL
    EXPECT_TRUE(DispatchEvent(eW2N, State::STATEMACHINE_APP));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // Provided interface to WARNING
    EXPECT_TRUE(DispatchEvent(eN2W, State::STATEMACHINE_PROVIDED, "prv2"));
    EXPECT_EQ(State::WARNING, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::WARNING, GetComponentState(GCM::APPLICATION_VIEW));

    EXPECT_TRUE(ExportToGraphViz(SC_BUILD_ROOT_DIR "/warning-prov"));

    // Back to NORMAL
    EXPECT_TRUE(DispatchEvent(eW2N, State::STATEMACHINE_PROVIDED, "prv2"));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // Required interface to WARNING
    EXPECT_TRUE(DispatchEvent(eN2W, State::STATEMACHINE_REQUIRED, "req3"));
    EXPECT_EQ(State::WARNING, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::WARNING, GetComponentState(GCM::APPLICATION_VIEW));
    // Back to NORMAL
    EXPECT_TRUE(DispatchEvent(eW2N, State::STATEMACHINE_REQUIRED, "req3"));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // 2. NORMAL to ERROR
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // Framework state to ERROR
    EXPECT_TRUE(DispatchEvent(eN2E, State::STATEMACHINE_FRAMEWORK));
    EXPECT_EQ(State::ERROR, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::ERROR, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));
    // Back to NORMAL
    EXPECT_TRUE(DispatchEvent(eE2N, State::STATEMACHINE_FRAMEWORK));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // Application state to ERROR
    EXPECT_TRUE(DispatchEvent(eN2E, State::STATEMACHINE_APP));
    EXPECT_EQ(State::ERROR, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::ERROR, GetComponentState(GCM::APPLICATION_VIEW));
    // Back to NORMAL
    EXPECT_TRUE(DispatchEvent(eE2N, State::STATEMACHINE_APP));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // Provided interface to ERROR
    EXPECT_TRUE(DispatchEvent(eN2E, State::STATEMACHINE_PROVIDED, "prv2"));
    EXPECT_EQ(State::ERROR, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::ERROR, GetComponentState(GCM::APPLICATION_VIEW));
    // Back to NORMAL
    EXPECT_TRUE(DispatchEvent(eE2N, State::STATEMACHINE_PROVIDED, "prv2"));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));

    // Required interface to ERROR
    EXPECT_TRUE(DispatchEvent(eN2E, State::STATEMACHINE_REQUIRED, "req3"));
    EXPECT_EQ(State::ERROR, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::ERROR, GetComponentState(GCM::APPLICATION_VIEW));

    EXPECT_TRUE(ExportToGraphViz(SC_BUILD_ROOT_DIR "/error-req"));

    // Back to NORMAL
    EXPECT_TRUE(DispatchEvent(eE2N, State::STATEMACHINE_REQUIRED, "req3"));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::SYSTEM_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::FRAMEWORK_VIEW));
    EXPECT_EQ(State::NORMAL, GetComponentState(GCM::APPLICATION_VIEW));
}

TEST_F(GCMTest, Dispatchevent)
{
    // TODO
}

TEST(GCM, ToStream_HumanReadable)
{
    GCM gcm("coordinator_name", "component_name");
    std::cout << gcm << std::endl;
}

TEST(GCM, ToStream_GraphViz)
{
    GCM gcm("coordinator_name", "component_name");
    gcm.ToStream(std::cout, GCM::EXPORT_GRAPHVIZ);
}

TEST(GCM, ExportToGraphViz)
{
    GCM gcm("coordinator_name", "component_name");
    EXPECT_TRUE(gcm.ExportToGraphViz(SC_BUILD_ROOT_DIR "/graphviz-test"));
}

TEST(GCM, Accessors)
{
}

