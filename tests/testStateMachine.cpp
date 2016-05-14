//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2014-2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2012
// Last revision: Mar 13, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "safecass/statemachine.h"

using namespace SC;

// Define state event handler for unit testing derived from StateEventHandler
class EventHandler: public StateEventHandler
{
public:
    size_t CountEntryExit[State::NUMBER_OF_ENTRY_EXIT];
    size_t CountTransition[State::NUMBER_OF_TRANSITIONS];

    virtual void OnEntry(const State::StateEntryExitType entryType) {
        ++CountEntryExit[static_cast<size_t>(entryType)];
    }
    virtual void OnExit(const State::StateEntryExitType exitType) {
        ++CountEntryExit[static_cast<size_t>(exitType)];
    }
    virtual void OnTransition(const State::TransitionType transition) {
        ++CountTransition[static_cast<size_t>(transition)];
    }
};

// Define custom state machine class derived from StateMachine for unit testing
class TestStateMachine: public StateMachine
{
public:
    TestStateMachine(void): StateMachine("TestStateMachine", new EventHandler())
    {}

    int GetCountEntryExit(const State::StateEntryExitType stateEntryExit) const {
        EventHandler * handler = dynamic_cast<EventHandler*>(this->GetStateEventHandler());
        SCASSERT(handler);
        return handler->CountEntryExit[static_cast<size_t>(stateEntryExit)];
    }

    int GetCountTransition(const State::TransitionType transition) const {
        EventHandler * handler = dynamic_cast<EventHandler*>(this->GetStateEventHandler());
        SCASSERT(handler);
        return handler->CountTransition[static_cast<size_t>(transition)];
    }
};

TEST(StateMachine, Constructor)
{
    StateMachine sm1;
    EXPECT_STREQ(NONAME, sm1.GetOwnerName().c_str());
    EXPECT_EQ(0, sm1.GetStateEventHandler());

    StateMachine sm2("sm_name2");
    EXPECT_STREQ("sm_name2", sm2.GetOwnerName().c_str());
    EXPECT_EQ(0, sm2.GetStateEventHandler());

    StateEventHandler * handler = new StateEventHandler;
    StateMachine sm3("sm_name3", handler);
    EXPECT_STREQ("sm_name3", sm3.GetOwnerName().c_str());
    EXPECT_EQ(handler, sm3.GetStateEventHandler());
}

// Tests for state transition management and polocy
TEST(StateMachine, ProcessEvent)
{
    TestStateMachine sm;

    // Initial state: Normal after entry
#define CHECK_STATE(v1, v2, v3, v4, v5, v6, v7, v8)\
    EXPECT_EQ(v1, sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY));\
    EXPECT_EQ(v2, sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT));\
    EXPECT_EQ(v3, sm.GetCountEntryExit(State::NORMAL_ON_ENTRY));\
    EXPECT_EQ(v4, sm.GetCountEntryExit(State::NORMAL_ON_EXIT));\
    EXPECT_EQ(v5, sm.GetCountEntryExit(State::WARNING_ON_ENTRY));\
    EXPECT_EQ(v6, sm.GetCountEntryExit(State::WARNING_ON_EXIT));\
    EXPECT_EQ(v7, sm.GetCountEntryExit(State::ERROR_ON_ENTRY));\
    EXPECT_EQ(v8, sm.GetCountEntryExit(State::ERROR_ON_EXIT));

    CHECK_STATE(1, 0, 1, 0, 0, 0, 0, 0);

    // No transition initially
#define CHECK_TRANSITION(v1, v2, v3, v4, v5, v6)\
    EXPECT_EQ(v1, sm.GetCountTransition(State::NORMAL_TO_WARNING));\
    EXPECT_EQ(v2, sm.GetCountTransition(State::WARNING_TO_NORMAL));\
    EXPECT_EQ(v3, sm.GetCountTransition(State::NORMAL_TO_ERROR));\
    EXPECT_EQ(v4, sm.GetCountTransition(State::ERROR_TO_NORMAL));\
    EXPECT_EQ(v5, sm.GetCountTransition(State::WARNING_TO_ERROR));\
    EXPECT_EQ(v6, sm.GetCountTransition(State::ERROR_TO_WARNING));

    CHECK_TRANSITION(0, 0, 0, 0, 0, 0);

    // Current state: Normal
    EXPECT_EQ(sm.GetCurrentState(), State::NORMAL);

    // Define events to test state transitions
    // onset events
    Event eN2W("evt_NORMAL_TO_WARNING", 10, Event::TRANSITION_N2W);
    Event eW2E("evt_WARNING_TO_ERROR", 10, Event::TRANSITION_W2E);
    Event eN2E("evt_NORMAL_TO_ERROR", 10, Event::TRANSITION_N2E);
    Event eNW2E("evt_NORMAL/WARNING_TO_ERROR", 10, Event::TRANSITION_NW2E);
    // completion events
    Event eW2N("evt_WARNING_TO_NORMAL", 10, Event::TRANSITION_W2N);
    Event eE2W("evt_ERROR_TO_WARNING", 10,  Event::TRANSITION_E2W);
    Event eE2N("evt_ERROR_TO_NORMAL", 10,  Event::TRANSITION_E2N);
    Event eEW2N("evt_ERROR/WARNING_TO_NORMAL", 10, Event::TRANSITION_EW2N);

    // Normal to Warning
    EXPECT_TRUE(sm.ProcessEvent(eN2W));
    EXPECT_EQ(State::WARNING, sm.GetCurrentState());
    CHECK_STATE(1, 0, // statemachine on enter/exit
                1, 1, // Normal state on enter/exit
                1, 0, // Warning state on enter/exit
                0, 0);// Error state on enter/exit
    CHECK_TRANSITION(1, 0, // Normal to/from Warning
                     0, 0, // Normal to/from Error
                     0, 0);// Warning to/from Error

    // Warning to Error
    EXPECT_TRUE(sm.ProcessEvent(eW2E));
    EXPECT_EQ(State::ERROR, sm.GetCurrentState());
    CHECK_STATE(1, 0,  // FSM
                1, 1,  // Normal
                1, 1,  // Warning
                1, 0); // Error
    CHECK_TRANSITION(1, 0,  // N to/from W
                     0, 0,  // N to/from E
                     1, 0); // W to/from E

    // Error to Warning
    EXPECT_TRUE(sm.ProcessEvent(eE2W));
    EXPECT_EQ(State::WARNING, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                1, 1,
                2, 1,
                1, 1);
    CHECK_TRANSITION(1, 0,
                     0, 0,
                     1, 1);

    // Warning to Normal
    EXPECT_TRUE(sm.ProcessEvent(eW2N));
    EXPECT_EQ(State::NORMAL, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                2, 1,
                2, 2,
                1, 1);
    CHECK_TRANSITION(1, 1,
                     0, 0,
                     1, 1);

    // Normal to Error
    EXPECT_TRUE(sm.ProcessEvent(eN2E));
    EXPECT_EQ(State::ERROR, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                2, 2,
                2, 2,
                2, 1);
    CHECK_TRANSITION(1, 1,
                     1, 0,
                     1, 1);

    // Error to Normal
    EXPECT_TRUE(sm.ProcessEvent(eE2N));
    EXPECT_EQ(State::NORMAL, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                3, 2,
                2, 2,
                2, 2);
    CHECK_TRANSITION(1, 1,
                     1, 1,
                     1, 1);

    // Normal to Error using NW2E
    EXPECT_TRUE(sm.ProcessEvent(eNW2E));
    EXPECT_EQ(State::ERROR, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                3, 3,
                2, 2,
                3, 2);
    CHECK_TRANSITION(1, 1,
                     2, 1,
                     1, 1);

    // Back to Warning
    EXPECT_TRUE(sm.ProcessEvent(eE2W));
    EXPECT_EQ(State::WARNING, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                3, 3,
                3, 2,
                3, 3);
    CHECK_TRANSITION(1, 1,
                     2, 1,
                     1, 2);

    // Warning to Error using NW2E
    EXPECT_TRUE(sm.ProcessEvent(eNW2E));
    EXPECT_EQ(State::ERROR, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                3, 3,
                3, 3,
                4, 3);
    CHECK_TRANSITION(1, 1,
                     2, 1,
                     2, 2);

    // Error to Normal using EW2N
    EXPECT_TRUE(sm.ProcessEvent(eEW2N));
    EXPECT_EQ(State::NORMAL, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                4, 3,
                3, 3,
                4, 4);
    CHECK_TRANSITION(1, 1,
                     2, 2,
                     2, 2);

    // Back to Warning
    EXPECT_TRUE(sm.ProcessEvent(eN2W));
    EXPECT_EQ(State::WARNING, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                4, 4,
                4, 3,
                4, 4);
    CHECK_TRANSITION(2, 1,
                     2, 2,
                     2, 2);

    // Warning to Normal using EW2N
    EXPECT_TRUE(sm.ProcessEvent(eEW2N));
    EXPECT_EQ(State::NORMAL, sm.GetCurrentState());
    CHECK_STATE(1, 0,
                5, 4,
                4, 4,
                4, 4);
    CHECK_TRANSITION(2, 2,
                     2, 2,
                     2, 2);
}

TEST(StateMachine, ToStream)
{
    StateMachine sm("state_machine_owner_name", new StateEventHandler());

    std::cout << sm << std::endl;

    Event eN2E("evt_NORMAL_TO_ERROR", 10, Event::TRANSITION_N2E);
    Event eE2W("evt_ERROR_TO_WARNING", 10,  Event::TRANSITION_E2W);
    Event eW2E("evt_WARNING_TO_ERROR", 10, Event::TRANSITION_W2E);

    EXPECT_TRUE(sm.ProcessEvent(eN2E));
    EXPECT_TRUE(sm.ProcessEvent(eE2W));
    EXPECT_TRUE(sm.ProcessEvent(eW2E));

    std::cout << sm << std::endl;
}
