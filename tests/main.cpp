//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2015-2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Oct 28, 2015
// Last revision: Mar 15, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/casros
//
#include <stdio.h>
#include <stdlib.h>

#include "common/common.h"
#include "gtest/gtest.h"

int main(int argc, char * argv[])
{
    // Initialize Google test
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
