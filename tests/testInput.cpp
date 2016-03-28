//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Mar 16, 2016
// Last revision: Mar 16, 2016
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"

#include "historyBuffer.h"
#include "paramEigen.h"

using namespace SC;

// Tests basic features provided by boost::lockfree::spsc_queue
TEST(HistoryBuffer, spsc_basic)
{
    // queue size
    const size_t N = 5;

    // For testing, use int type
    boost::lockfree::spsc_queue<int> q(N);

    // Check initial conditions
    EXPECT_EQ(0, q.read_available());
    EXPECT_EQ(N, q.write_available());

    size_t nr = 0, nw = 0;

    // Push test
#define PUSH_CHECK( _x )\
    nr = q.read_available();\
    nw = q.write_available();\
    EXPECT_EQ(N, nr + nw);\
    EXPECT_TRUE(q.push(_x));\
    EXPECT_EQ(nr + 1, q.read_available());\
    EXPECT_EQ(nw - 1, q.write_available());

    PUSH_CHECK(1);
    EXPECT_EQ(1, q.front());

    PUSH_CHECK(2);
    EXPECT_EQ(1, q.front());

    PUSH_CHECK(3);
    EXPECT_EQ(1, q.front());

    PUSH_CHECK(4);
    EXPECT_EQ(1, q.front());

    PUSH_CHECK(5);
    EXPECT_EQ(1, q.front());

    EXPECT_FALSE(q.push(6));
    EXPECT_EQ(N, q.read_available());
    EXPECT_EQ(0, q.write_available());

    // Pop test
#define POP_CHECK\
    nr = q.read_available();\
    nw = q.write_available();\
    EXPECT_EQ(N, nr + nw);\
    EXPECT_TRUE(q.pop());\
    EXPECT_EQ(nr - 1, q.read_available());\
    EXPECT_EQ(nw + 1, q.write_available());

    POP_CHECK;
    EXPECT_EQ(2, q.front());

    POP_CHECK;
    EXPECT_EQ(3, q.front());

    POP_CHECK;
    EXPECT_EQ(4, q.front());

    POP_CHECK;
    EXPECT_EQ(5, q.front());

    EXPECT_TRUE(q.pop());

    EXPECT_FALSE(q.pop());
    EXPECT_EQ(0, q.read_available());
    EXPECT_EQ(N, q.write_available());

    // Reset test
    q.push(1);
    q.push(2);
    q.push(3);
    q.push(4);
    q.push(5);

    EXPECT_EQ(N, q.read_available());
    EXPECT_EQ(0, q.write_available());
    q.reset();
    EXPECT_EQ(0, q.read_available());
    EXPECT_EQ(N, q.write_available());

#undef PUSH_CHECK
#undef POP_CHECK
}

// Tests HistoryBuffer, which is essentially a set of timestamped spsc_queue
// instances
TEST(HistoryBuffer, spsc_array_basic)
{
    // TODO: integrate chrono
}

// Tests accessors that HistoryBuffer provides
TEST(HistoryBuffer, spsc_array_accessors)
{
}

// Tests Eigen parameter handling
TEST(HistoryBuffer, eigen_params)
{
#if 1
    Minimal<int>A;
    Minimal<short>B;
    Minimal<float>E(1.4f);
    Minimal<std::vector<int> >X;
    // calling setZero for a vector...?
    X.setZero();

    Minimal<Eigen::Vector2f>C;
    Minimal<Eigen::Vector2f>D(Eigen::Vector2f::Random());
    Minimal<Eigen::Array2i>F(Eigen::Array2i(2,8));
    C = D;

    std::cerr << " E " << E << " C " << C << " A " << A.val << " B " << B.val << "\n";

    A.increase();
    E.decreace();
    A.increase();
    C.setZero();
    C.increase();
    B.increase();
    B.increase();
    B.increase();

    std::cerr << " E " << E << " C " << C << " A " << A.val << " B " << B.val << "\n";
#endif
}
