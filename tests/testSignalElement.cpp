//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 3, 2016
// Last revision: Apr 3, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

#include <typeinfo> // typeid

#include "gtest/gtest.h"
#include "safecass/signalElement.h"
#include "safecass/historyBuffer.h"

using namespace SC;

// Tests for SignalElement
TEST(SignalElement, Accessors)
{
    HistoryBuffer hb;
    ParamEigen<double> param;
    param.Val = 3.0;

    SignalElement s("signal_name", param, &hb);

    // Name
    EXPECT_TRUE(s.GetName().compare("signal_name") == 0);
    // Param prototype
    ParamBase * paramClone = s.GetParamPrototype();
    EXPECT_TRUE(paramClone != 0);

    ParamEigen<double> * paramCloned = dynamic_cast<ParamEigen<double> *>(paramClone);
    EXPECT_TRUE(paramCloned != 0);
    EXPECT_EQ(3.0, paramCloned->Val);

    delete paramClone;

    // History buffer
    EXPECT_EQ(&hb, s.GetHistoryBufferInstance());

    // Signal index
    EXPECT_EQ(HistoryBufferBase::INVALID_HISTORY_BUFFER_INDEX, s.GetSignalIndex());

    // Timestamp of last sample fetch
    EXPECT_EQ(0, s.GetTimeLastSampleFetched());
}
