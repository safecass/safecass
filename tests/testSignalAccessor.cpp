//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 3, 2016
// Last revision: Apr 12, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

#include <typeinfo>
#include <numeric> // std::accumulate
#include <stdlib.h>

#include "gtest/gtest.h"
#include "safecass/paramEigen.h"
#include "safecass/signalAccessor.h"

#include <boost/circular_buffer.hpp>
#include <boost/call_traits.hpp>

using namespace SC;

// Check if boost circular buffer works correctly.  Adapted from tutorial
// example from boost.circular_buffer.  Note that this example uses
// ParamEigen<int> instead of int as parameter type.
TEST(SignalAccessor, BoostCircularBufferExample1)
{
    // Create a circular buffer of capacity 3
    boost::circular_buffer<ParamEigen<int> > cb(3);

    // Insert threee elements into the buffer.
    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    EXPECT_EQ(1, cb[0].Val);
    EXPECT_EQ(2, cb[1].Val);
    EXPECT_EQ(3, cb[2].Val);

    // The buffer is full now, so pushing subsequent
    // elements will overwrite the front-most elements.

    cb.push_back(4);  // Overwrite 1 with 4.
    cb.push_back(5);  // Overwrite 2 with 5.

    // The buffer now contains 3, 4 and 5.
    EXPECT_EQ(3, cb[0].Val);
    EXPECT_EQ(4, cb[1].Val);
    EXPECT_EQ(5, cb[2].Val);

    // Elements can be popped from either the front or the back.
    cb.pop_back();  // 5 is removed.
    cb.pop_front(); // 3 is removed.

    // Leaving only one element with value = 4.
    EXPECT_EQ(4, cb[0].Val);
}

TEST(SignalAccessor, BoostCircularBufferExample2)
{
    // Create a circular buffer of capacity 3.
    boost::circular_buffer<int> cb(3);
    EXPECT_TRUE(cb.capacity() == 3);
    // Check is empty.
    EXPECT_TRUE(cb.size() == 0);
    EXPECT_TRUE(cb.empty());

    // Insert some elements into the circular buffer.
    cb.push_back(1);
    cb.push_back(2);

    // Check push_backs have expected effect.
    EXPECT_TRUE(cb[0] == 1);
    EXPECT_TRUE(cb[1] == 2);
    EXPECT_TRUE(!cb.full());
    EXPECT_TRUE(cb.size() == 2);
    EXPECT_TRUE(cb.capacity() == 3);

    // Insert some other elements.
    cb.push_back(3);
    cb.push_back(4);

    // Evaluate the sum of all elements.
    int sum = std::accumulate(cb.begin(), cb.end(), 0);

    // Check state
    EXPECT_TRUE(sum == 9);
    EXPECT_TRUE(cb[0] == 2);
    EXPECT_TRUE(cb[1] == 3);
    EXPECT_TRUE(cb[2] == 4);
    EXPECT_TRUE(*cb.begin() == 2);
    EXPECT_TRUE(cb.front() == 2);
    EXPECT_TRUE(cb.back() == 4);
    EXPECT_TRUE(cb.full());
    EXPECT_TRUE(cb.size() == 3);
    EXPECT_TRUE(cb.capacity() == 3);
}

TEST(SignalAccessor, SignalAccessorTest)
{
    SignalAccessor<ParamEigen<double> > acc1(5);
    SignalAccessor<ParamEigen<std::vector<double> > > acc2(5);
    SignalAccessor<ParamEigen<Eigen::Array33d> > acc3(5);

    SignalAccessorBase::SignalsType accessorVec;
    accessorVec.push_back(&acc1);
    accessorVec.push_back(&acc2);
    accessorVec.push_back(&acc3);

    acc1.Push(1.0);

    std::vector<double> doubleVec;
    doubleVec.push_back(-1.0);
    doubleVec.push_back(-2.0);
    doubleVec.push_back(-3.0);
    acc2.Push(doubleVec);

    srand(time(0));

    Eigen::Array33d a = Eigen::Array33d::Random();
    acc3.Push(a);

    SignalAccessorBase::SignalsType::const_iterator it = accessorVec.begin();
    const SignalAccessorBase::SignalsType::const_iterator itEnd = accessorVec.end();

    for (; it != itEnd; ++it) {
        std::cout << "signal " << ": ";
        (*it)->ToStream(std::cout);
        std::cout << std::endl;
    }
}
