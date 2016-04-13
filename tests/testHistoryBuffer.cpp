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

#include "gtest/gtest.h"
#include "safecass/historyBuffer.h"
#include "safecass/signalAccessor.h"

using namespace SC;

TEST(HistoryBuffer, Initialization)
{
    const size_t N = 256;

    HistoryBuffer hb(N);
    EXPECT_EQ(N, hb.GetBufferSize());
    EXPECT_EQ(N, hb.GetBufferSize());

    std::cout << "History Buffer: " << hb << std::endl;
}

TEST(HistoryBuffer, AddFindSignal)
{
    // history buffer without object ownership
    HistoryBuffer hb;
    EXPECT_EQ(HistoryBufferBase::INVALID_SIGNAL_INDEX, hb.AddSignal("", 0));
    EXPECT_EQ(hb.GetNumberOfSignals(), 0);

    // SignalAccessor<int> saInt(hb.GetBufferSize());
    // EXPECT_EQ(0, hb.AddSignal("saInt", &saInt));
    // EXPECT_EQ(hb.GetNumberOfSignals(), 1);
    // EXPECT_EQ(true, hb.FindSignal("saInt"));
    // EXPECT_EQ(false, hb.FindSignal("saInt1"));

    SignalAccessor<ParamEigen<int> > saInt2(hb.GetBufferSize());
    EXPECT_EQ(0, hb.AddSignal("saIntWrapper", &saInt2));
    EXPECT_EQ(hb.GetNumberOfSignals(), 1);
    EXPECT_EQ(true, hb.FindSignal("saIntWrapper"));
    EXPECT_EQ(false, hb.FindSignal("saIntWrapper1"));

    // history buffer with object ownership
    HistoryBuffer hb2(128, true);

    // SignalAccessor<double> * saDouble = new SignalAccessor<double>(hb.GetBufferSize());
    // EXPECT_EQ(0, hb2.AddSignal("saDouble", saDouble));
    // EXPECT_EQ(hb2.GetNumberOfSignals(), 1);
    // EXPECT_EQ(true, hb2.FindSignal("saDouble"));
    // EXPECT_EQ(false, hb2.FindSignal("saDouble1"));

    SignalAccessor<ParamEigen<double> > * saDouble2 = new SignalAccessor<ParamEigen<double> >(hb.GetBufferSize());
    EXPECT_EQ(0, hb2.AddSignal("saDoubleWrapper", saDouble2));
    EXPECT_EQ(hb2.GetNumberOfSignals(), 1);
    EXPECT_EQ(true, hb2.FindSignal("saDoubleWrapper"));
    EXPECT_EQ(false, hb2.FindSignal("saDoubleWrapper1"));
}

TEST(HistoryBuffer, Snapshot)
{
    // TODO: implement Snapshot()
    // TODO: Check if GetNewValue returns the same value after Snapshot()
}

TEST(HistoryBuffer, GetNewValue)
{
    HistoryBuffer hb;

    ParamEigen<int> paramInt;
    EXPECT_EQ(false, hb.GetNewValue("non-existent-signal-name", paramInt));
}
