//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 18, 2012
// Last revision: Apr 19, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "testGCM.h"
#include "GCMHelper.h"

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>

using namespace SF;

SFGCMTest::SFGCMTest(void)
{
    // test registration
    TEST_ADD(SFGCMTest::test);

    GCMHelper::cisstInit();
}

SFGCMTest::~SFGCMTest(void)
{
    GCMHelper::cisstCleanup();
}

void SFGCMTest::setup()
{
}

void SFGCMTest::tear_down()
{
    std::cout << "tear_down\n" << std::flush;
}

void SFGCMTest::test(void)
{
    TEST_ASSERT(true);
}
