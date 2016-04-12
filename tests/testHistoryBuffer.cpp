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

using namespace SC;

TEST(HistoryBuffer, Initialization)
{
    const size_t N = 256;

    HistoryBuffer hb(N);
    EXPECT_EQ(N, hb.GetBufferSize());

    std::cout << "History Buffer: " << hb << std::endl;
}
