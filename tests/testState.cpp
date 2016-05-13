//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2014-2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 22, 2012
// Last revision: May 12, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "safecass/state.h"

using namespace SC;

TEST(State, Constructors)
{
    State s;
    EXPECT_EQ(State::NORMAL, s.GetState());

    State ss(State::ERROR);
    EXPECT_EQ(State::ERROR, ss.GetState());
}

TEST(State, OperatorOverloading)
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

TEST(State, ToStream)
{
    State s(State::WARNING);

    std::cout << s << std::endl;
}
