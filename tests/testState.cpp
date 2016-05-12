//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2014-2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2012
// Last revision: Mar 15, 2016
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "safecass/statemachine.h"
#include "safecass/event.h"

using namespace SC;

#if 0
TEST(StateMachine, state_transition)
{
    StateMachine sm;

    // Initial state: Normal after entry
#define CHECK_STATE(v1, v2, v3, v4, v5, v6, v7, v8)\
    EXPECT_EQ(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY), v1);\
    EXPECT_EQ(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT),  v2);\
    EXPECT_EQ(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY),       v3);\
    EXPECT_EQ(sm.GetCountEntryExit(State::NORMAL_ON_EXIT),        v4);\
    EXPECT_EQ(sm.GetCountEntryExit(State::WARNING_ON_ENTRY),      v5);\
    EXPECT_EQ(sm.GetCountEntryExit(State::WARNING_ON_EXIT),       v6);\
    EXPECT_EQ(sm.GetCountEntryExit(State::ERROR_ON_ENTRY),        v7);\
    EXPECT_EQ(sm.GetCountEntryExit(State::ERROR_ON_EXIT),         v8);

    CHECK_STATE(1, 0, 1, 0, 0, 0, 0, 0);

    // No transition initially
#define CHECK_TRANSITION(v1, v2, v3, v4, v5, v6)\
    EXPECT_EQ(sm.GetCountTransition(State::NORMAL_TO_WARNING), v1);\
    EXPECT_EQ(sm.GetCountTransition(State::WARNING_TO_NORMAL), v2);\
    EXPECT_EQ(sm.GetCountTransition(State::NORMAL_TO_ERROR),   v3);\
    EXPECT_EQ(sm.GetCountTransition(State::ERROR_TO_NORMAL),   v4);\
    EXPECT_EQ(sm.GetCountTransition(State::WARNING_TO_ERROR),  v5);\
    EXPECT_EQ(sm.GetCountTransition(State::ERROR_TO_WARNING),  v6);

    CHECK_TRANSITION(0, 0, 0, 0, 0, 0);

    // Current state: Normal
    EXPECT_EQ(sm.GetCurrentState(), State::NORMAL);

    // Define event object to test state transitions
    Event::TransitionsType transitions;
    transitions.push_back(State::NORMAL_TO_WARNING);
    const Event evtN2W("NORMAL_TO_WARNING", 10, transitions);

    transitions.clear();
    transitions.push_back(State::WARNING_TO_NORMAL);
    const Event evtW2N("WARNING_TO_NORMAL", 10, transitions);

    transitions.clear();
    transitions.push_back(State::NORMAL_TO_ERROR);
    const Event evtN2E("NORMAL_TO_ERROR", 10, transitions);

    transitions.clear();
    transitions.push_back(State::ERROR_TO_NORMAL);
    const Event evtE2N("ERROR_TO_NORMAL", 10, transitions);

    transitions.clear();
    transitions.push_back(State::WARNING_TO_ERROR);
    const Event evtW2E("WARNING_TO_ERROR", 10, transitions);

    transitions.clear();
    transitions.push_back(State::ERROR_TO_WARNING);
    const Event evtE2W("ERROR_TO_WARNING", 10, transitions);

    // Normal to Warning
    EXPECT_TRUE(sm.ProcessEvent(State::NORMAL_TO_WARNING, &evtN2W));
    EXPECT_EQ(sm.GetCurrentState(), State::WARNING);
    CHECK_STATE(1, 0, // statemachine on enter/exit
                1, 1, // Normal state on enter/exit
                1, 0, // Warning state on enter/exit
                0, 0);// Error state on enter/exit
    CHECK_TRANSITION(1, 0, // Normal to/from Warning
                     0, 0, // Normal to/from Error
                     0, 0);// Warning to/from Error

    // Warning to Error
    EXPECT_TRUE(sm.ProcessEvent(State::WARNING_TO_ERROR, &evtW2E));
    EXPECT_EQ(sm.GetCurrentState(), State::ERROR);
    CHECK_STATE(1, 0, 1, 1, 1, 1, 1, 0);
    CHECK_TRANSITION(1, 0, 0, 0, 1, 0);

    // Error to Warning
    EXPECT_TRUE(sm.ProcessEvent(State::ERROR_TO_WARNING, &evtE2W));
    EXPECT_EQ(sm.GetCurrentState(), State::WARNING);
    CHECK_STATE(1, 0, 1, 1, 2, 1, 1, 1);
    CHECK_TRANSITION(1, 0, 0, 0, 1, 1);

    // Warning to Normal
    EXPECT_TRUE(sm.ProcessEvent(State::WARNING_TO_NORMAL, &evtW2N));
    EXPECT_EQ(sm.GetCurrentState(), State::NORMAL);
    CHECK_STATE(1, 0, 2, 1, 2, 2, 1, 1);
    CHECK_TRANSITION(1, 1, 0, 0, 1, 1);

    // Normal to Error
    EXPECT_TRUE(sm.ProcessEvent(State::NORMAL_TO_ERROR, &evtN2E));
    EXPECT_EQ(sm.GetCurrentState(), State::ERROR);
    CHECK_STATE(1, 0, 2, 2, 2, 2, 2, 1);
    CHECK_TRANSITION(1, 1, 1, 0, 1, 1);

    // Error to Normal
    EXPECT_TRUE(sm.ProcessEvent(State::ERROR_TO_NORMAL, &evtE2N));
    EXPECT_EQ(sm.GetCurrentState(), State::NORMAL);
    CHECK_STATE(1, 0, 3, 2, 2, 2, 2, 2);
    CHECK_TRANSITION(1, 1, 1, 1, 1, 1);
}

TEST(StateMachine, state_operator_overloading)
{
    State s1, s2;

    // getter and setter
    s1.SetState(State::NORMAL);  EXPECT_TRUE(s1.GetState() == State::NORMAL);
    s1.SetState(State::WARNING); EXPECT_TRUE(s1.GetState() == State::WARNING);
    s1.SetState(State::ERROR);   EXPECT_TRUE(s1.GetState() == State::ERROR);

    // operator==
    s1.SetState(State::NORMAL);
    s2.SetState(State::NORMAL);  EXPECT_TRUE(s1 == s2);
    s2.SetState(State::WARNING); EXPECT_TRUE(!(s1 == s2));
    s2.SetState(State::ERROR);   EXPECT_TRUE(!(s1 == s2));

    // operator!=
    s1.SetState(State::NORMAL);
    s2.SetState(State::NORMAL);  EXPECT_TRUE(!(s1 != s2));
    s2.SetState(State::WARNING); EXPECT_TRUE(s1 != s2);
    s2.SetState(State::ERROR);   EXPECT_TRUE(s1 != s2);

    // operator>
    s1.SetState(State::NORMAL); s2.SetState(State::NORMAL);
    EXPECT_TRUE(!(s1 > s2));
    EXPECT_TRUE(!(s2 > s1));

    s1.SetState(State::NORMAL); s2.SetState(State::WARNING);
    EXPECT_TRUE(!(s1 > s2));
    EXPECT_TRUE(s2 > s1);

    s1.SetState(State::NORMAL); s2.SetState(State::ERROR);
    EXPECT_TRUE(!(s1 > s2));
    EXPECT_TRUE(s2 > s1);

    s1.SetState(State::WARNING); s2.SetState(State::WARNING);
    EXPECT_TRUE(!(s1 > s2));
    EXPECT_TRUE(!(s2 > s1));

    s1.SetState(State::WARNING); s2.SetState(State::ERROR);
    EXPECT_TRUE(!(s1 > s2));
    EXPECT_TRUE(s2 > s1);

    s1.SetState(State::ERROR); s2.SetState(State::ERROR);
    EXPECT_TRUE(!(s1 > s2));
    EXPECT_TRUE(!(s2 > s1));

    // operator<
    s1.SetState(State::NORMAL); s2.SetState(State::NORMAL);
    EXPECT_TRUE(!(s1 < s2));
    EXPECT_TRUE(!(s2 < s1));

    s1.SetState(State::NORMAL); s2.SetState(State::WARNING);
    EXPECT_TRUE(s1 < s2);
    EXPECT_TRUE(!(s2 < s1));

    s1.SetState(State::NORMAL); s2.SetState(State::ERROR);
    EXPECT_TRUE(s1 < s2);
    EXPECT_TRUE(!(s2 < s1));

    s1.SetState(State::WARNING); s2.SetState(State::WARNING);
    EXPECT_TRUE(!(s1 < s2));
    EXPECT_TRUE(!(s2 < s1));

    s1.SetState(State::WARNING); s2.SetState(State::ERROR);
    EXPECT_TRUE(s1 < s2);
    EXPECT_TRUE(!(s2 < s1));

    s1.SetState(State::ERROR); s2.SetState(State::ERROR);
    EXPECT_TRUE(!(s1 < s2));
    EXPECT_TRUE(!(s2 < s1));

    // operator*
    s1.SetState(State::NORMAL); s2.SetState(State::NORMAL);   EXPECT_TRUE((s1 * s2) == State(State::NORMAL));
    s1.SetState(State::NORMAL); s2.SetState(State::WARNING);  EXPECT_TRUE((s1 * s2) == State(State::WARNING));
    s1.SetState(State::NORMAL); s2.SetState(State::ERROR);    EXPECT_TRUE((s1 * s2) == State(State::ERROR));
    s1.SetState(State::WARNING); s2.SetState(State::NORMAL);  EXPECT_TRUE((s1 * s2) == State(State::WARNING));
    s1.SetState(State::WARNING); s2.SetState(State::WARNING); EXPECT_TRUE((s1 * s2) == State(State::WARNING));
    s1.SetState(State::WARNING); s2.SetState(State::ERROR);   EXPECT_TRUE((s1 * s2) == State(State::ERROR));
    s1.SetState(State::ERROR); s2.SetState(State::NORMAL);    EXPECT_TRUE((s1 * s2) == State(State::ERROR));
    s1.SetState(State::ERROR); s2.SetState(State::WARNING);   EXPECT_TRUE((s1 * s2) == State(State::ERROR));
    s1.SetState(State::ERROR); s2.SetState(State::ERROR);     EXPECT_TRUE((s1 * s2) == State(State::ERROR));

    // assignment operator
    s1.SetState(State::NORMAL);
    s1 = s2;
    EXPECT_TRUE(s1 == s2);
}
#endif
