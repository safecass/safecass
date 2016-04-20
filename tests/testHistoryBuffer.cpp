//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 3, 2016
// Last revision: Apr 20, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

#include "gtest/gtest.h"
#include "safecass/historyBuffer.h"
#include "safecass/signalAccessor.h"

#include <vector>
#include <list>

using namespace SC;

TEST(HistoryBuffer, Initialization)
{
    const size_t N = 256;

    HistoryBuffer hb(N);
    EXPECT_EQ(N, hb.GetBufferSize());
    EXPECT_EQ(N, hb.GetBufferSize());

    std::cout << "History Buffer: " << hb << std::endl;
}

#define INVALID_INDEX HistoryBufferBase::INVALID_SIGNAL_INDEX

TEST(HistoryBuffer, AddSignal_FindSignal_GetSignal)
{
    HistoryBuffer hb;

    // Define different types of parameters
    ParamEigen<double>               paramDouble;
    ParamEigen<std::vector<double> > paramDoubleVec;
    ParamEigen<std::list<int> >      paramIntList;
    ParamEigen<Eigen::Array33d>      paramEigenArray33d;

    // Test for AddSignal()
    EXPECT_EQ(0, hb.AddSignal(paramDouble,    "Double"));
    EXPECT_EQ(1, hb.AddSignal(paramDoubleVec, "DoubleVec"));
    EXPECT_EQ(2, hb.AddSignal(paramIntList,   "IntList"));
    EXPECT_EQ(3, hb.AddSignal(paramEigenArray33d, "EigenArray33d"));

    // Failures due to duplicate name
    EXPECT_EQ(INVALID_INDEX, hb.AddSignal(paramDouble,    "Double"));
    EXPECT_EQ(INVALID_INDEX, hb.AddSignal(paramDoubleVec, "DoubleVec"));
    EXPECT_EQ(INVALID_INDEX, hb.AddSignal(paramIntList,   "IntList"));
    EXPECT_EQ(INVALID_INDEX, hb.AddSignal(paramEigenArray33d, "EigenArray33d"));

    // Test for FindSingal()
    EXPECT_TRUE(hb.FindSignal("Double"));
    EXPECT_TRUE(hb.FindSignal("DoubleVec"));
    EXPECT_TRUE(hb.FindSignal("IntList"));
    EXPECT_TRUE(hb.FindSignal("EigenArray33d"));
    EXPECT_FALSE(hb.FindSignal("Double-invalid"));

    // Test for GetSignalIndex()
    EXPECT_EQ(0, hb.GetSignalIndex("Double"));
    EXPECT_EQ(1, hb.GetSignalIndex("DoubleVec"));
    EXPECT_EQ(2, hb.GetSignalIndex("IntList"));
    EXPECT_EQ(3, hb.GetSignalIndex("EigenArray33d"));
    EXPECT_EQ(INVALID_INDEX, hb.GetSignalIndex("Double-invalid"));
}

TEST(HistoryBuffer, Snapshot)
{
    HistoryBuffer hb;

    // Add signals
    ParamEigen<double>               paramDouble;
    ParamEigen<std::vector<double> > paramDoubleVec;
    ParamEigen<std::list<int> >      paramIntList;
    ParamEigen<Eigen::Array33d>      paramEigenArray33d;

    // Test for AddSignal()
    EXPECT_EQ(0, hb.AddSignal(paramDouble,    "Double"));
    EXPECT_EQ(1, hb.AddSignal(paramDoubleVec, "DoubleVec"));
    EXPECT_EQ(2, hb.AddSignal(paramIntList,   "IntList"));
    EXPECT_EQ(3, hb.AddSignal(paramEigenArray33d, "EigenArray33d"));

    // History buffer's snapshot index still points to INVALID_SIGNAL_INDEX
    // (no Snapshot() is called before)
    EXPECT_EQ(INVALID_INDEX, hb.GetSnapshotIndex());
    std::cout << "Snapshot (init): " << hb << std::endl;

    // Snapshot
    hb.Snapshot();
    EXPECT_EQ(1, hb.GetSnapshotIndex());
    std::cout << "Snapshot (after first snapshot): " << hb << std::endl;

    // Change object values
    paramDouble.SetValid();
    paramDouble.Val = 1.1;

    paramDoubleVec.SetValid();
    std::vector<double> doubleVec;
    doubleVec.push_back(-11.1);
    doubleVec.push_back(-22.2);
    doubleVec.push_back(-33.3);
    paramDoubleVec.Val = doubleVec;

    paramIntList.SetValid();
    std::list<int> intList;
    intList.push_back(-7);
    intList.push_back(-8);
    intList.push_back(9);
    intList.push_back(10);
    paramIntList.Val = intList;

    srand(time(0));

    paramEigenArray33d.SetValid();
    Eigen::Array33d tmp = Eigen::Array33d::Random();
    paramEigenArray33d = tmp;

    // Snapshot and check if history buffer correctly maintains snapshots
    // of the objects
    hb.Snapshot();
    EXPECT_EQ(2, hb.GetSnapshotIndex());
    std::cout << "Snapshot (after second snapshot): " << hb << std::endl;
}

#if 0
TEST(HistoryBuffer, Snapshot)
{
    HistoryBuffer hb;

    // Define three variables of different types
    ParamEigen<double> paramDouble;
    ParamEigen<std::vector<double> > paramDoubleVec;
    ParamEigen<Eigen::Array33d> paramEigenArray33d;

    // Define signal accessor
    SignalAccessor<ParamEigen<double> > acc1(paramDouble, 5);
    SignalAccessor<ParamEigen<std::vector<double> > > acc2(paramDoubleVec, 5);
    SignalAccessor<ParamEigen<Eigen::Array33d> > acc3(paramEigenArray33d, 5);

    // Add signal accessors

    // TODO: implement Snapshot()

    // TODO: Check if GetNewValue returns the same value after Snapshot()

    SignalAccessorBase::SignalsType accessorVec;
}
#endif

TEST(HistoryBuffer, GetNewValue)
{
    HistoryBuffer hb;

    ParamEigen<int> paramInt;
    EXPECT_EQ(false, hb.GetNewValue("non-existent-signal-name", paramInt));
}
