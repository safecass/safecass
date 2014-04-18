//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 18, 2012
// Last revision: Apr 18, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "testGCM.h"

SFGCMTest::SFGCMTest()
{
    TEST_ADD(SFGCMTest::test);
    TEST_ADD(SFGCMTest::test2);
}

void SFGCMTest::setup()
{
    std::cout << "setup\n" << std::flush;
}

void SFGCMTest::tear_down()
{
    std::cout << "tear_down\n" << std::flush;
}

void SFGCMTest::test(void)
{
    TEST_ASSERT(true);
}

void SFGCMTest::test2(void)
{
    TEST_ASSERT(false);
}
