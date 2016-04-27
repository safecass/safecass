//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 3, 2016
// Last revision: Apr 21, 2016
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
    paramDouble = 1.1;

    paramDoubleVec.SetValid();
    std::vector<double> doubleVec;
    doubleVec.push_back(-11.1);
    doubleVec.push_back(-22.2);
    doubleVec.push_back(-33.3);
    paramDoubleVec = doubleVec;

    paramIntList.SetValid();
    std::list<int> intList;
    intList.push_back(-7);
    intList.push_back(-8);
    intList.push_back(9);
    intList.push_back(10);
    paramIntList = intList;

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

// Tests for GetNewValue with signal id (signal name)
TEST(HistoryBuffer, GetNewValueById)
{
    HistoryBuffer hb;

    ParamEigen<int> aInt;
    EXPECT_TRUE(false == hb.GetNewValue("non-existent-signal-name", aInt));

    const std::string name = "aInt";
    HistoryBufferBase::IndexType index = hb.AddSignal(aInt, name);
    EXPECT_EQ(index, 0);

    hb.Snapshot();

    usleep(1000);

    ParamEigen<int> aIntFetched;
    EXPECT_TRUE(aIntFetched == 0);
    EXPECT_TRUE(aIntFetched.GetTimestamp() != aInt.GetTimestamp());

    EXPECT_TRUE(hb.GetNewValue(name, aIntFetched));
    EXPECT_TRUE(aIntFetched == aInt);
    EXPECT_TRUE(aIntFetched.Val == aInt.Val);
    EXPECT_TRUE(aIntFetched.GetTimestamp() == aInt.GetTimestamp());

    aInt = 1210;
    hb.Snapshot();

    usleep(1000);

    EXPECT_TRUE(hb.GetNewValue(name, aIntFetched));
    EXPECT_TRUE(aIntFetched == aInt);
    EXPECT_TRUE(aIntFetched.Val == aInt.Val);
    EXPECT_TRUE(aIntFetched.GetTimestamp() == aInt.GetTimestamp());
}

// Tests for GetNewValue with signal index
TEST(HistoryBuffer, GetNewValueByIndex)
{
    HistoryBuffer hb;

    ParamEigen<Eigen::Matrix2d> aEigen;
    EXPECT_TRUE(false == hb.GetNewValue(1234, aEigen));

    const std::string name = "aEigen";
    HistoryBufferBase::IndexType index = hb.AddSignal(aEigen, name);
    EXPECT_EQ(index, 0);

    hb.Snapshot();

    usleep(1000);

    ParamEigen<Eigen::Matrix2d> aEigenFetched;
    EXPECT_TRUE(aEigenFetched.Val.sum() == 0);
    EXPECT_TRUE(aEigenFetched.GetTimestamp() != aEigen.GetTimestamp());

    EXPECT_TRUE(hb.GetNewValue(index, aEigenFetched));
    EXPECT_TRUE(aEigenFetched.Val == aEigen.Val);
    EXPECT_TRUE(aEigenFetched.GetTimestamp() == aEigen.GetTimestamp());

    ParamEigen<Eigen::Matrix2d> aEigenRandom;
    aEigenRandom.Val = Eigen::Matrix2d::Random(2,2);
    aEigen = aEigenRandom;
    hb.Snapshot();

    usleep(1000);

    EXPECT_TRUE(hb.GetNewValue(index, aEigenFetched));
    EXPECT_TRUE(aEigenFetched.Val == aEigen.Val);
    EXPECT_TRUE(aEigenFetched.GetTimestamp() == aEigen.GetTimestamp());

    EXPECT_EQ(aEigenRandom.Val.sum(),  aEigenFetched.Val.sum());
    EXPECT_EQ(aEigenRandom.Val.prod(), aEigenFetched.Val.prod());
    EXPECT_EQ(aEigenRandom.Val.mean(), aEigenFetched.Val.mean());
}

// TODO: Test for PushNewValue
